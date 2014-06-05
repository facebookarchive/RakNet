/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

/// \file
///


#if defined(_WIN32)
#include "WindowsIncludes.h"

 #if !defined(WINDOWS_PHONE_8)
		// To call timeGetTime
		// on Code::Blocks, this needs to be libwinmm.a instead
		#pragma comment(lib, "Winmm.lib")
	#endif

#endif

#include "GetTime.h"




#if defined(_WIN32)
//DWORD mProcMask;
//DWORD mSysMask;
//HANDLE mThread;









#else
#include <sys/time.h>
#include <unistd.h>
RakNet::TimeUS initialTime;
#endif

static bool initialized=false;

#if defined(GET_TIME_SPIKE_LIMIT) && GET_TIME_SPIKE_LIMIT>0
#include "SimpleMutex.h"
RakNet::TimeUS lastNormalizedReturnedValue=0;
RakNet::TimeUS lastNormalizedInputValue=0;
/// This constraints timer forward jumps to 1 second, and does not let it jump backwards
/// See http://support.microsoft.com/kb/274323 where the timer can sometimes jump forward by hours or days
/// This also has the effect where debugging a sending system won't treat the time spent halted past 1 second as elapsed network time
RakNet::TimeUS NormalizeTime(RakNet::TimeUS timeIn)
{
	RakNet::TimeUS diff, lastNormalizedReturnedValueCopy;
	static RakNet::SimpleMutex mutex;
	
	mutex.Lock();
	if (timeIn>=lastNormalizedInputValue)
	{
		diff = timeIn-lastNormalizedInputValue;
		if (diff > GET_TIME_SPIKE_LIMIT)
			lastNormalizedReturnedValue+=GET_TIME_SPIKE_LIMIT;
		else
			lastNormalizedReturnedValue+=diff;
	}
	else
		lastNormalizedReturnedValue+=GET_TIME_SPIKE_LIMIT;

	lastNormalizedInputValue=timeIn;
	lastNormalizedReturnedValueCopy=lastNormalizedReturnedValue;
	mutex.Unlock();

	return lastNormalizedReturnedValueCopy;
}
#endif // #if defined(GET_TIME_SPIKE_LIMIT) && GET_TIME_SPIKE_LIMIT>0
RakNet::Time RakNet::GetTime( void )
{
	return (RakNet::Time)(GetTimeUS()/1000);
}
RakNet::TimeMS RakNet::GetTimeMS( void )
{
	return (RakNet::TimeMS)(GetTimeUS()/1000);
}
















































#if   defined(_WIN32)
RakNet::TimeUS GetTimeUS_Windows( void )
{
	if ( initialized == false)
	{
		initialized = true;

		// Save the current process
#if !defined(_WIN32_WCE)
//		HANDLE mProc = GetCurrentProcess();

		// Get the current Affinity
#if _MSC_VER >= 1400 && defined (_M_X64)
//		GetProcessAffinityMask(mProc, (PDWORD_PTR)&mProcMask, (PDWORD_PTR)&mSysMask);
#else
//		GetProcessAffinityMask(mProc, &mProcMask, &mSysMask);
#endif
//		mThread = GetCurrentThread();

#endif // _WIN32_WCE
	}	

	// 9/26/2010 In China running LuDaShi, QueryPerformanceFrequency has to be called every time because CPU clock speeds can be different
	RakNet::TimeUS curTime;
	LARGE_INTEGER PerfVal;
	LARGE_INTEGER yo1;

	QueryPerformanceFrequency( &yo1 );
	QueryPerformanceCounter( &PerfVal );

	__int64 quotient, remainder;
	quotient=((PerfVal.QuadPart) / yo1.QuadPart);
	remainder=((PerfVal.QuadPart) % yo1.QuadPart);
	curTime = (RakNet::TimeUS) quotient*(RakNet::TimeUS)1000000 + (remainder*(RakNet::TimeUS)1000000 / yo1.QuadPart);

#if defined(GET_TIME_SPIKE_LIMIT) && GET_TIME_SPIKE_LIMIT>0
	return NormalizeTime(curTime);
#else
	return curTime;
#endif // #if defined(GET_TIME_SPIKE_LIMIT) && GET_TIME_SPIKE_LIMIT>0
}
#elif defined(__GNUC__)  || defined(__GCCXML__) || defined(__S3E__)
RakNet::TimeUS GetTimeUS_Linux( void )
{
	timeval tp;
	if ( initialized == false)
	{
		gettimeofday( &tp, 0 );
		initialized=true;
		// I do this because otherwise RakNet::Time in milliseconds won't work as it will underflow when dividing by 1000 to do the conversion
		initialTime = ( tp.tv_sec ) * (RakNet::TimeUS) 1000000 + ( tp.tv_usec );
	}

	// GCC
	RakNet::TimeUS curTime;
	gettimeofday( &tp, 0 );

	curTime = ( tp.tv_sec ) * (RakNet::TimeUS) 1000000 + ( tp.tv_usec );

#if defined(GET_TIME_SPIKE_LIMIT) && GET_TIME_SPIKE_LIMIT>0
	return NormalizeTime(curTime - initialTime);
#else
	return curTime - initialTime;
#endif // #if defined(GET_TIME_SPIKE_LIMIT) && GET_TIME_SPIKE_LIMIT>0
}
#endif

RakNet::TimeUS RakNet::GetTimeUS( void )
{






#if   defined(_WIN32)
	return GetTimeUS_Windows();
#else
	return GetTimeUS_Linux();
#endif
}
bool RakNet::GreaterThan(RakNet::Time a, RakNet::Time b)
{
	// a > b?
	const RakNet::Time halfSpan =(RakNet::Time) (((RakNet::Time)(const RakNet::Time)-1)/(RakNet::Time)2);
	return b!=a && b-a>halfSpan;
}
bool RakNet::LessThan(RakNet::Time a, RakNet::Time b)
{
	// a < b?
	const RakNet::Time halfSpan = ((RakNet::Time)(const RakNet::Time)-1)/(RakNet::Time)2;
	return b!=a && b-a<halfSpan;
}
