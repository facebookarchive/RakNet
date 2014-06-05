/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "RakTimer.h"

RakTimer::RakTimer(void)
{
	timerLength=1000;
	Start();
}

RakTimer::RakTimer(int lengthInMilliseconds)
{
	timerLength=lengthInMilliseconds;
	Start();
}

RakTimer::~RakTimer(void)
{
}

    void RakTimer::SetTimerLength(int lengthInMilliseconds)
	{
	timerLength=lengthInMilliseconds;
	}
	void RakTimer::Start()
	{
		startTime=RakNet::GetTimeMS();

	}
    void RakTimer::Pause()
	{
	pauseOffset=(int)(RakNet::GetTimeMS()-startTime);
	}
	void RakTimer::Resume()
	{
	startTime=RakNet::GetTimeMS()-pauseOffset;
	}

	bool RakTimer::IsExpired()
	{

	return (RakNet::GetTimeMS()-startTime>timerLength);
	
	}
