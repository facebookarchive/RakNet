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

#if defined(UNICODE)
#include "RakWString.h"
#endif

#include "PacketOutputWindowLogger.h"
#include "RakString.h"
#if defined(_WIN32)
#include "WindowsIncludes.h"
#endif

using namespace RakNet;

PacketOutputWindowLogger::PacketOutputWindowLogger()
{
}
PacketOutputWindowLogger::~PacketOutputWindowLogger()
{
}
void PacketOutputWindowLogger::WriteLog(const char *str)
{
#if defined(_WIN32)

	#if defined(UNICODE)
		RakNet::RakWString str2 = str;
		str2+="\n";
		OutputDebugString(str2.C_String());
	#else
		RakNet::RakString str2 = str;
		str2+="\n";
		OutputDebugString(str2.C_String());
	#endif
// DS_APR
#elif defined(__native_client__)
	fprintf(stderr, "%s\n", str);
// /DS_APR
#endif
}

#endif // _RAKNET_SUPPORT_*
