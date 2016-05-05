/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "EmptyHeader.h"

#ifdef RAKNET_SOCKET_2_INLINE_FUNCTIONS

#ifndef RAKNETSOCKET2_WINDOWS_STORE_8
#define RAKNETSOCKET2_WINDOWS_STORE_8

#if defined(WINDOWS_STORE_RT)

#include <ppltasks.h>
#include <collection.h>
#include "RakString.h"

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Storage::Streams;
//using namespace Platform::Collections;
//using namespace Windows::Foundation::Collections;

namespace RakNet
{
    std::wstring StringUtf8ToWideChar(const std::string& strUtf8)
    {
        std::wstring ret;
        if (!strUtf8.empty())
        {
            int nNum = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, nullptr, 0);
            if (nNum)
            {
                WCHAR* wideCharString = new WCHAR[nNum + 1];
                wideCharString[0] = 0;

                nNum = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, wideCharString, nNum + 1);

                ret = wideCharString;
                delete[] wideCharString;
            }
            else
            {
                RakAssert("StringUtf8ToWideChar Wrong convert to WideChar" && 0);
            }
        }
        return ret;
    }

    std::string StringWideCharToUtf8(const std::wstring& strWideChar)
    {
        std::string ret;
        if (!strWideChar.empty())
        {
            int nNum = WideCharToMultiByte(CP_UTF8, 0, strWideChar.c_str(), -1, nullptr, 0, nullptr, FALSE);
            if (nNum)
            {
                char* utf8String = new char[nNum + 1];
                utf8String[0] = 0;

                nNum = WideCharToMultiByte(CP_UTF8, 0, strWideChar.c_str(), -1, utf8String, nNum + 1, nullptr, FALSE);

                ret = utf8String;
                delete[] utf8String;
            }
            else
            {
                RakAssert("StringUtf8ToWideChar Wrong convert to Utf8" && 0);
            }
        }

        return ret;
    }

    std::string PlatformStringToString(Platform::String^ s) {
        return StringWideCharToUtf8(std::wstring(s->Data()));
    }

    Platform::String^ PlatformStringFromString(const std::string& s)
    {
        std::wstring ws = StringUtf8ToWideChar(s);
        return ref new Platform::String(ws.data(), ws.length());
    }

public ref class OutputStreamAndDataWriter sealed
{
public:
	// http://msdn.microsoft.com/en-us/library/windows/apps/hh755807.aspx
	 property IOutputStream ^outputStream;
		 /*
            {
				IOutputStream ^ get() {return outputStream;}
                void set(IOutputStream ^ value) {outputStream = value;}
            }
			*/
	 property DataWriter ^dataWriter;
		 /*
            {
               DataWriter ^ get() {return dataWriter;}
                void set(DataWriter ^ value) {dataWriter = value;}
            }
	 */
private:
};

public ref class ListenerContext sealed
{
public:
    ListenerContext(Windows::Networking::Sockets::DatagramSocket^ listener);
    void OnMessage(Windows::Networking::Sockets::DatagramSocket^ socket, Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs^ eventArguments);
	OutputStreamAndDataWriter ^GetOutputStreamAndDataWriter(uint64_t s_addrVar);

private:
	~ListenerContext();
    // CRITICAL_SECTION lock;
	SimpleMutex outputStreamMapMutex;
    Windows::Networking::Sockets::DatagramSocket^ listener;
    //Windows::Storage::Streams::IOutputStream^ outputStream;
    //Windows::Networking::HostName^ hostName;
    //Platform::String^ port;
	// http://msdn.microsoft.com/en-us/library/windows/apps/hh700103.aspx
	Platform::Collections::Map<uint64_t,OutputStreamAndDataWriter^>^ outputStreamMap;

	void EchoMessage(Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs^ eventArguments);
};
}

using namespace RakNet;

ListenerContext::ListenerContext(Windows::Networking::Sockets::DatagramSocket^ listener)
{
    this->listener = listener;
//    InitializeCriticalSectionEx(&lock, 0, 0);
	outputStreamMap = ref new Platform::Collections::Map<uint64_t,OutputStreamAndDataWriter^>();
}

ListenerContext::~ListenerContext()
{
    // The listener can be closed in two ways:
    //  - explicit: by using delete operator (the listener is closed even if there are outstanding references to it).
    //  - implicit: by removing last reference to it (i.e. falling out-of-scope).
    // In this case this is the last reference to the listener so both will yield the same result.
    delete listener;
    listener = nullptr;
    
//    DeleteCriticalSection(&lock);
}

OutputStreamAndDataWriter ^ListenerContext::GetOutputStreamAndDataWriter(uint64_t s_addrVar)
{
	outputStreamMapMutex.Lock();
	if (outputStreamMap->HasKey(s_addrVar))
	{
		OutputStreamAndDataWriter ^o = outputStreamMap->Lookup(s_addrVar);
		outputStreamMapMutex.Unlock();
		return o;
	}
	outputStreamMapMutex.Unlock();
	uint16_t port = s_addrVar & 0xFFFF;
	uint32_t addr = (uint32_t) (s_addrVar >> 32);
	//addr = ntohl(addr);
	char bufIP[64];
	unsigned char *ucp = (unsigned char *)&addr;
	sprintf(bufIP, "%d.%d.%d.%d",
		ucp[0] & 0xff,
		ucp[1] & 0xff,
		ucp[2] & 0xff,
		ucp[3] & 0xff);
	char portStr[32];
	_itoa(port, portStr, 10);

    
	HostName ^hostName = ref new HostName(PlatformStringFromString(bufIP));
	task< IOutputStream^ > op(listener->GetOutputStreamAsync(hostName, PlatformStringFromString(portStr)));
	op.wait();
	OutputStreamAndDataWriter ^outputStreamAndDataWriter = ref new OutputStreamAndDataWriter;
	outputStreamAndDataWriter->outputStream = op.get();
	outputStreamAndDataWriter->dataWriter = ref new DataWriter(outputStreamAndDataWriter->outputStream);
	
	outputStreamMapMutex.Lock();
	if (outputStreamMap->HasKey(s_addrVar)==false)
	{
		outputStreamMap->Insert(s_addrVar, outputStreamAndDataWriter);
		outputStreamMapMutex.Unlock();
		return outputStreamAndDataWriter;
	}
	else
	{
		// Just use the one that was inserted from another thread
		OutputStreamAndDataWriter ^o = outputStreamMap->Lookup(s_addrVar);
		outputStreamMapMutex.Unlock();
		return o;
	}
}

void ListenerContext::OnMessage(Windows::Networking::Sockets::DatagramSocket^ socket, Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs^ eventArguments)
{
	HostName ^remoteHost=eventArguments->RemoteAddress;
	eventArguments->RemoteAddress->DisplayName;
	eventArguments->RemotePort;
	
	RNS2_WindowsStore8 *rns2 = RNS2_WindowsStore8::GetRNS2FromDatagramSocket(socket);
	if (rns2==0)
		return;
		
	//auto platformBuffer = ref new Platform::Array<BYTE>((unsigned char*) sendParameters->data, (UINT)sendParameters->length);
	RNS2EventHandler *eventHandler = rns2->GetEventHandler();
	RNS2RecvStruct *recvFromStruct = eventHandler->AllocRNS2RecvStruct(_FILE_AND_LINE_);

	// http://stackoverflow.com/questions/11853838/getting-an-array-of-bytes-out-of-windowsstoragestreamsibuffer
	IBuffer ^uselessBuffer = eventArguments->GetDataReader()->DetachBuffer();
	Windows::Storage::Streams::DataReader^ uselessReader =  Windows::Storage::Streams::DataReader::FromBuffer(uselessBuffer);
    Platform::Array<unsigned char>^ managedBytes = ref new Platform::Array<unsigned char>(uselessBuffer->Length);
    uselessReader->ReadBytes( managedBytes );
    for(unsigned int i = 0; i < uselessBuffer->Length; i++)
        recvFromStruct->data[i] = managedBytes[i];
	recvFromStruct->bytesRead = uselessBuffer->Length;
	recvFromStruct->systemAddress.address.addr4.sin_addr.s_addr = RNS2_WindowsStore8::WinRTInet_Addr(PlatformStringToString(eventArguments->RemoteAddress->DisplayName).c_str());
	recvFromStruct->systemAddress.SetPortHostOrder(atoi(PlatformStringToString(eventArguments->RemotePort).c_str()));
	recvFromStruct->timeRead=RakNet::GetTimeUS();
	recvFromStruct->socket = rns2;
	eventHandler->OnRNS2Recv(recvFromStruct);

	/*
    if (outputStream != nullptr)
    {
        EchoMessage(eventArguments);
        return;
    }


    // We do not have an output stream yet so create one.
    task<IOutputStream^>(socket->GetOutputStreamAsync(eventArguments->RemoteAddress, eventArguments->RemotePort)).then([this, socket, eventArguments] (IOutputStream^ stream)
    {
        // It might happen that the OnMessage was invoked more than once before the GetOutputStreamAsync completed.
        // In this case we will end up with multiple streams - make sure we have just one of it.
        EnterCriticalSection(&lock);

        if (outputStream == nullptr)
        {
            outputStream = stream;
            hostName = eventArguments->RemoteAddress;
            port = eventArguments->RemotePort;
        }

        LeaveCriticalSection(&lock);

        EchoMessage(eventArguments);
    }).then([this, socket, eventArguments] (task<void> previousTask)
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point.
            previousTask.get();
        }
        catch (Exception^ exception)
        {
        //    NotifyUserFromAsyncThread("Getting an output stream failed with error: " + exception->Message, NotifyType::ErrorMessage);
        }
    });
	*/
}
void ListenerContext::EchoMessage(DatagramSocketMessageReceivedEventArgs^ eventArguments)
{
}
DataStructures::List<RakNet::RNS2_WindowsStore8*> RakNet::RNS2_WindowsStore8::rns2List;
SimpleMutex RNS2_WindowsStore8::rns2ListMutex;
RNS2_WindowsStore8::RNS2_WindowsStore8()
{
	rns2ListMutex.Lock();
    rns2List.Insert(this, _FILE_AND_LINE_);
	rns2ListMutex.Unlock();
}
RNS2_WindowsStore8::~RNS2_WindowsStore8()
{
	unsigned int i;
	rns2ListMutex.Lock();
	for (i=0; i < rns2List.Size(); i++)
	{
		if (rns2List[i]==this)
		{
			rns2List.RemoveAtIndexFast(i);
			break;
		}
	}
	rns2ListMutex.Unlock();
}
RNS2_WindowsStore8 *RNS2_WindowsStore8::GetRNS2FromDatagramSocket(Windows::Networking::Sockets::DatagramSocket^ s)
{
	RNS2_WindowsStore8 *out=0;
	unsigned int i;
	rns2ListMutex.Lock();
	for (i=0; i < rns2List.Size(); i++)
	{
		if (rns2List[i]->listener==s)
		{
			out=rns2List[i];
			break;
		}
	}
	rns2ListMutex.Unlock();
	return out;
}
RNS2BindResult RNS2_WindowsStore8::Bind( Platform::String ^localServiceName ) {

	listener = ref new DatagramSocket();
    listenerContext = ref new ListenerContext(listener);
    listener->MessageReceived += ref new TypedEventHandler<DatagramSocket^, DatagramSocketMessageReceivedEventArgs^>(listenerContext, &ListenerContext::OnMessage);
	// http://msdn.microsoft.com/en-us/library/dd492427.aspx
	task<void> bindOp(listener->BindServiceNameAsync(localServiceName));
	try
	{
		bindOp.wait();
	}
	catch (Exception^ exception)
    {
		return BR_FAILED_TO_BIND_SOCKET;
    }

	return BR_SUCCESS;
}
void RNS2_WindowsStore8::GetMyIP( SystemAddress addresses[MAXIMUM_NUMBER_OF_INTERNAL_IDS] ) {RakAssert("GetMyIP Unsupported" && 0);}
RNS2SendResult RNS2_WindowsStore8::Send( RNS2_SendParameters *sendParameters, const char *file, unsigned int line ) {
	uint64_t s_addrVar = sendParameters->systemAddress.address.addr4.sin_addr.s_addr;
	s_addrVar <<=32;
	s_addrVar |= sendParameters->systemAddress.debugPort;
	OutputStreamAndDataWriter ^outputStreamAndDataWriter = listenerContext->GetOutputStreamAndDataWriter(s_addrVar);
	// DataWriter ^dataWriter = ref new DataWriter(outputStream);

	auto platformBuffer = ref new Platform::Array<BYTE>((unsigned char*) sendParameters->data, (UINT)sendParameters->length);
    outputStreamAndDataWriter->dataWriter->WriteBytes(platformBuffer);
	

	 // Write the locally buffered data to the network. Please note that write operation will succeed
    // even if the server is not listening.
    task<unsigned int>(outputStreamAndDataWriter->dataWriter->StoreAsync()).then([&] (task<unsigned int> writeTask)
    {
        try
        {
            // Try getting an excpetion.
     //       writeTask.get();
     //       SendOutput->Style = dynamic_cast<Windows::UI::Xaml::Style^>(rootPage->Resources->Lookup("StatusStyle"));
     //       SendOutput->Text = "\"" + stringToSend + "\" sent successfully";
        }
        catch (Exception^ exception)
        {
         //   rootPage->NotifyUser("Send failed with error: " + exception->Message, NotifyType::ErrorMessage);
			OutputDebugStringW(exception->Message->Data());
        }
    });

	return 0;
}
void RNS2_WindowsStore8::DomainNameToIP( const char *domainName, char ip[65] ) {
	ip[0]=0;

//	RakAssert("DomainNameToIP Unsupported" && 0);

//	std::string s_str = std::string(domainName);
//	std::wstring wid_str = std::wstring(s_str.begin(), s_str.end());
//	const wchar_t* w_char = wid_str.c_str();
	
//	DatagramSocket ^listener = ref new DatagramSocket();


//	task<void> bindOp(listener->BindServiceNameAsync(ref new Platform::String()));
	//bindOp.wait();

	HostName ^hostName = ref new HostName(PlatformStringFromString(domainName));
	//HostName ^hostName = ref new HostName(ref new Platform::String(L"microsoft.com"));
	//HostName ^hostName = ref new HostName(ref new Platform::String(L"127.0.0.1"));
	//task< Windows::Foundation::Collections::IVectorView<EndpointPair^>^ > op(listener->GetEndpointPairsAsync(hostName, L"42"));
	//listener->GetEndpointPairsAsync(hostName, L"42");
	// task< Windows::Foundation::Collections::IVectorView<EndpointPair^>^ > op(DatagramSocket::GetEndpointPairsAsync(hostName, L"42"));

	IAsyncOperation<Windows::Foundation::Collections::IVectorView<Windows::Networking::EndpointPair^>^>^ op = DatagramSocket::GetEndpointPairsAsync(hostName, L"0");
	
	bool completed=false;
	op->Completed = ref new AsyncOperationCompletedHandler< Windows::Foundation::Collections::IVectorView<Windows::Networking::EndpointPair^>^ >(
		[&] (
		IAsyncOperation< Windows::Foundation::Collections::IVectorView<Windows::Networking::EndpointPair^>^>^ result,
		AsyncStatus status
		) {
		Windows::Foundation::Collections::IVectorView<Windows::Networking::EndpointPair^>^ result2 = result->GetResults();
		if (result2->Size>0)
		{
			strcpy(ip, PlatformStringToString(result2->GetAt(0)->RemoteHostName->DisplayName).c_str());
		}
		else
		{
			ip[0]=0;
		}

		result->Close();
		completed=true;
	} 
	) ;
	
	while (!completed)
		RakSleep(0);
	/*
	op->Completed = ref new AsyncOperationCompletedHandler< Windows::Foundation::Collections::IVectorView<EndpointPair^>^ >(
		[](IAsyncOperation< Windows::Foundation::Collections::IVectorView<EndpointPair^>^ >^ operation)
             {
                operation->GetResults();

             });
			 */

	
//	RakSleep(10000);
	/*
	try
	{
		op.wait();
	}
	catch (Exception^ exception)
    {
		int a=5;
    }
	Windows::Foundation::Collections::IVectorView<EndpointPair^>^view = op.get();
	if (view->Size>0)
	{
		Platform::String ^name = view->GetAt(0)->RemoteHostName->DisplayName;
		RakString rs2;
		rs2.FromWideChar(name->Data());
		strcpy(ip, rs2.C_String());
	}
	else
	{
		ip[0]=0;
	}
	*/

}


int RNS2_WindowsStore8::WinRTInet_Addr(const char *str) {
	int parts[4];
	unsigned char curVal;

	const char *strIndex=str;
	int partsIndex;

	for (partsIndex=0; partsIndex < 4; partsIndex++)
		parts[partsIndex]=0;

	partsIndex=0;
	curVal=0;
	while (partsIndex < 4)
	{
		if (*strIndex < '0' || *strIndex > '9')
		{
			parts[partsIndex]=curVal;
			partsIndex++;
			curVal=0;

			if (*strIndex==0)
				break;
		}
		else
		{
			curVal*=10;
			curVal+=*strIndex-'0';
		}
		strIndex++;
	}

	return htonl((parts[0]<<24) | (parts[1]<<16) | (parts[2]<<8) | (parts[3]<<0) );
}

int RNS2_WindowsStore8::WinRTSetSockOpt(Windows::Networking::Sockets::DatagramSocket ^s,
   int level,
   int optname,
   const char * optval,
   socklen_t optlen) {RakAssert("WinRTSetSockOpt Unsupported" && 0); return 0;}
   
int RNS2_WindowsStore8::WinRTIOCTLSocket(Windows::Networking::Sockets::DatagramSocket ^s,
	long cmd,
	unsigned long *argp) {RakAssert("WinRTIOCTLSocket Unsupported" && 0); return 0;}
	
int RNS2_WindowsStore8::WinRTGetSockName(Windows::Networking::Sockets::DatagramSocket ^s,
	struct sockaddr *name,
	socklen_t* namelen) {RakAssert("WinRTGetSockName Unsupported" && 0); return 0;}

#endif // WINDOWS_STORE_RT

#endif // file header

#endif // #ifdef RAKNET_SOCKET_2_INLINE_FUNCTIONS
