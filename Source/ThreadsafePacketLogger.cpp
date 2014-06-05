/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "NativeFeatureIncludes.h"
#if _RAKNET_SUPPORT_PacketLogger==1

#include "ThreadsafePacketLogger.h"
#include <string.h>

using namespace RakNet;

ThreadsafePacketLogger::ThreadsafePacketLogger()
{

}
ThreadsafePacketLogger::~ThreadsafePacketLogger()
{
	char **msg;
	while ((msg = logMessages.ReadLock()) != 0)
	{
		rakFree_Ex((*msg), _FILE_AND_LINE_ );
	}
}
void ThreadsafePacketLogger::Update(void)
{
	char **msg;
	while ((msg = logMessages.ReadLock()) != 0)
	{
		WriteLog(*msg);
		rakFree_Ex((*msg), _FILE_AND_LINE_ );
	}
}
void ThreadsafePacketLogger::AddToLog(const char *str)
{
	char **msg = logMessages.WriteLock();
	*msg = (char*) rakMalloc_Ex( strlen(str)+1, _FILE_AND_LINE_ );
	strcpy(*msg, str);
	logMessages.WriteUnlock();
}

#endif // _RAKNET_SUPPORT_*
