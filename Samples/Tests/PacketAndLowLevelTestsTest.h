/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#pragma once


#include "TestInterface.h"

#include "RakString.h"

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "RakNetTime.h"
#include "GetTime.h"
#include "DebugTools.h"
#include "CommonFunctions.h"
#include "TestHelpers.h"
#include "PacketChangerPlugin.h"

using namespace RakNet;
class PacketAndLowLevelTestsTest : public TestInterface
{
public:
	PacketAndLowLevelTestsTest(void);
	~PacketAndLowLevelTestsTest(void);
	int RunTest(DataStructures::List<RakString> params,bool isVerbose,bool noPauses);//should return 0 if no error, or the error number
	RakString GetTestName();
	RakString ErrorCodeToString(int errorCode);
	void DestroyPeers();
protected:
	void FloodWithHighPriority(RakPeerInterface* client);
		
private:
	DataStructures::List <RakString> errorList;
	DataStructures::List <RakPeerInterface *> destroyList;

};
