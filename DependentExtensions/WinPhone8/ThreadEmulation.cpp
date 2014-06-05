// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

#include "ThreadEmulation.h"

#include <assert.h>
#include <vector>
#include <set>
#include <map>
#include <mutex>

using namespace std;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;


namespace ThreadEmulation
{
    // Stored data for CREATE_SUSPENDED and ResumeThread.
    struct PendingThreadInfo
    {
        LPTHREAD_START_ROUTINE lpStartAddress;
        LPVOID lpParameter;
        HANDLE completionEvent;
        int nPriority;
    };

    static map<HANDLE, PendingThreadInfo> pendingThreads;
    static mutex pendingThreadsLock;

    
    // Thread local storage.
    typedef vector<void*> ThreadLocalData;

    static __declspec(thread) ThreadLocalData* currentThreadData = nullptr;
    static set<ThreadLocalData*> allThreadData;
    static DWORD nextTlsIndex = 0;
    static vector<DWORD> freeTlsIndices;
    static mutex tlsAllocationLock;


    // Converts a Win32 thread priority to WinRT format.
    static WorkItemPriority GetWorkItemPriority(int nPriority)
    {
        if (nPriority < 0)
            return WorkItemPriority::Low;
        else if (nPriority > 0)
            return WorkItemPriority::High;
        else
            return WorkItemPriority::Normal;
    }


    // Helper shared between CreateThread and ResumeThread.
    static void StartThread(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, HANDLE completionEvent, int nPriority)
    {
        auto workItemHandler = ref new WorkItemHandler([=](IAsyncAction^)
        {
            // Run the user callback.
            try
            {
                lpStartAddress(lpParameter);
            }
			catch (Exception ^e)
			{
				Platform::String ^exceptionString = e->ToString();
				OutputDebugStringW(exceptionString->Data());
			}

            // Clean up any TLS allocations made by this thread.
            TlsShutdown();

            // Signal that the thread has completed.
            SetEvent(completionEvent);
            CloseHandle(completionEvent);

        }, CallbackContext::Any);

        ThreadPool::RunAsync(workItemHandler, GetWorkItemPriority(nPriority), WorkItemOptions::TimeSliced);
    }


    _Use_decl_annotations_ HANDLE WINAPI CreateThread(LPSECURITY_ATTRIBUTES unusedThreadAttributes, SIZE_T unusedStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD unusedThreadId)
    {
        // Validate parameters.
        assert(unusedThreadAttributes == nullptr);
        assert(unusedStackSize == 0);
        assert((dwCreationFlags & ~CREATE_SUSPENDED) == 0);
        assert(unusedThreadId == nullptr);

        // Create a handle that will be signalled when the thread has completed.
        HANDLE threadHandle = CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);

        if (!threadHandle)
            return nullptr;

        // Make a copy of the handle for internal use. This is necessary because
        // the caller is responsible for closing the handle returned by CreateThread,
        // and they may do that before or after the thread has finished running.
        HANDLE completionEvent;
        
        if (!DuplicateHandle(GetCurrentProcess(), threadHandle, GetCurrentProcess(), &completionEvent, 0, false, DUPLICATE_SAME_ACCESS))
        {
            CloseHandle(threadHandle);
            return nullptr;
        }

        try
        {
            if (dwCreationFlags & CREATE_SUSPENDED)
            {
                // Store info about a suspended thread.
                PendingThreadInfo info;

                info.lpStartAddress = lpStartAddress;
                info.lpParameter = lpParameter;
                info.completionEvent = completionEvent;
                info.nPriority = 0;

                lock_guard<mutex> lock(pendingThreadsLock);

                pendingThreads[threadHandle] = info;
            }
            else
            {
                // Start the thread immediately.
                StartThread(lpStartAddress, lpParameter, completionEvent, 0);
            }
    
            return threadHandle;
        }
        catch (...)
        {
            // Clean up if thread creation fails.
            CloseHandle(threadHandle);
            CloseHandle(completionEvent);

            return nullptr;
        }
    }


    _Use_decl_annotations_ DWORD WINAPI ResumeThread(HANDLE hThread)
    {
        lock_guard<mutex> lock(pendingThreadsLock);

        // Look up the requested thread.
        auto threadInfo = pendingThreads.find(hThread);

        if (threadInfo == pendingThreads.end())
        {
            // Can only resume threads while they are in CREATE_SUSPENDED state.
            assert(false);
            return (DWORD)-1;
        }

        // Start the thread.
        try
        {
            PendingThreadInfo& info = threadInfo->second;

            StartThread(info.lpStartAddress, info.lpParameter, info.completionEvent, info.nPriority);
        }
        catch (...)
        {
            return (DWORD)-1;
        }

        // Remove this thread from the pending list.
        pendingThreads.erase(threadInfo);

        return 0;
    }


    _Use_decl_annotations_ BOOL WINAPI SetThreadPriority(HANDLE hThread, int nPriority)
    {
        lock_guard<mutex> lock(pendingThreadsLock);

        // Look up the requested thread.
        auto threadInfo = pendingThreads.find(hThread);

        if (threadInfo == pendingThreads.end())
        {
            // Can only set priority on threads while they are in CREATE_SUSPENDED state.
            assert(false);
            return false;
        }

        // Store the new priority.
        threadInfo->second.nPriority = nPriority;

        return true;
    }


    _Use_decl_annotations_ VOID WINAPI Sleep(DWORD dwMilliseconds)
    {
        static HANDLE singletonEvent = nullptr;

        HANDLE sleepEvent = singletonEvent;

        // Demand create the event.
        if (!sleepEvent)
        {
            sleepEvent = CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);

            if (!sleepEvent)
                return;

            HANDLE previousEvent = InterlockedCompareExchangePointerRelease(&singletonEvent, sleepEvent, nullptr);
            
            if (previousEvent)
            {
                // Back out if multiple threads try to demand create at the same time.
                CloseHandle(sleepEvent);
                sleepEvent = previousEvent;
            }
        }

        // Emulate sleep by waiting with timeout on an event that is never signalled.
        WaitForSingleObjectEx(sleepEvent, dwMilliseconds, false);
    }


    DWORD WINAPI TlsAlloc()
    {
        lock_guard<mutex> lock(tlsAllocationLock);
        
        // Can we reuse a previously freed TLS slot?
        if (!freeTlsIndices.empty())
        {
            DWORD result = freeTlsIndices.back();
            freeTlsIndices.pop_back();
            return result;
        }

        // Allocate a new TLS slot.
        return nextTlsIndex++;
    }


    _Use_decl_annotations_ BOOL WINAPI TlsFree(DWORD dwTlsIndex)
    {
        lock_guard<mutex> lock(tlsAllocationLock);

        assert(dwTlsIndex < nextTlsIndex);
        assert(find(freeTlsIndices.begin(), freeTlsIndices.end(), dwTlsIndex) == freeTlsIndices.end());

        // Store this slot for reuse by TlsAlloc.
        try
        {
            freeTlsIndices.push_back(dwTlsIndex);
        }
        catch (...)
        {
            return false;
        }

        // Zero the value for all threads that might be using this now freed slot.
        for each (auto threadData in allThreadData)
        {
            if (threadData->size() > dwTlsIndex)
            {
                threadData->at(dwTlsIndex) = nullptr;
            }
        }

        return true;
    }


    _Use_decl_annotations_ LPVOID WINAPI TlsGetValue(DWORD dwTlsIndex)
    {
        ThreadLocalData* threadData = currentThreadData;

        if (threadData && threadData->size() > dwTlsIndex)
        {
            // Return the value of an allocated TLS slot.
            return threadData->at(dwTlsIndex);
        }
        else
        {
            // Default value for unallocated slots.
            return nullptr;
        }
    }


    _Use_decl_annotations_ BOOL WINAPI TlsSetValue(DWORD dwTlsIndex, LPVOID lpTlsValue)
    {
        ThreadLocalData* threadData = currentThreadData;

        if (!threadData)
        {
            // First time allocation of TLS data for this thread.
            try
            {
                threadData = new ThreadLocalData(dwTlsIndex + 1, nullptr);
                
                lock_guard<mutex> lock(tlsAllocationLock);

                allThreadData.insert(threadData);

                currentThreadData = threadData;
            }
            catch (...)
            {
                if (threadData)
                    delete threadData;

                return false;
            }
        }
        else if (threadData->size() <= dwTlsIndex)
        {
            // This thread already has a TLS data block, but it must be expanded to fit the specified slot.
            try
            {
                lock_guard<mutex> lock(tlsAllocationLock);

                threadData->resize(dwTlsIndex + 1, nullptr);
            }
            catch (...)
            {
                return false;
            }
        }

        // Store the new value for this slot.
        threadData->at(dwTlsIndex) = lpTlsValue;

        return true;
    }


    // Called at thread exit to clean up TLS allocations.
    void WINAPI TlsShutdown()
    {
        ThreadLocalData* threadData = currentThreadData;

        if (threadData)
        {
            {
                lock_guard<mutex> lock(tlsAllocationLock);

                allThreadData.erase(threadData);
            }

            currentThreadData = nullptr;

            delete threadData;
        }
    }
}
