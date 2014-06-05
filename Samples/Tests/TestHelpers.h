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


#include "RakString.h"

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "RakPeer.h"
#include "RakSleep.h"
#include "DebugTools.h"
#include "CommonFunctions.h"
#include "RakTimer.h"

using namespace RakNet;
class TestHelpers
{
public:
	TestHelpers(void);
	~TestHelpers(void);

	static void StandardServerPrep(RakPeerInterface *&server);
	static void StandardClientPrep(RakPeerInterface *&client);
		static void StandardServerPrep(RakPeerInterface *&server,DataStructures::List <RakPeerInterface *> &destroyList);
	static void StandardClientPrep(RakPeerInterface *&client,DataStructures::List <RakPeerInterface *> &destroyList);

	static bool WaitAndConnectTwoPeersLocally(RakPeerInterface *connector,RakPeerInterface *connectee,int millisecondsToWait);
	static bool ConnectTwoPeersLocally(RakPeerInterface *connector,RakPeerInterface *connectee);
	///static bool BroadCastTestPacket(RakPeerInterface *sender);
	static bool BroadCastTestPacket(RakPeerInterface *sender,PacketReliability rel=RELIABLE_ORDERED,PacketPriority pr=HIGH_PRIORITY,int typeNum=ID_USER_PACKET_ENUM+1);
	static bool WaitForTestPacket(RakPeerInterface *reciever,int millisecondsToWait);
	static void RecieveForXTime(RakPeerInterface *reciever,int millisecondsToWait);
	static bool SendTestPacketDirected(RakPeerInterface *sender,char * ip,int port,PacketReliability rel=RELIABLE_ORDERED,PacketPriority pr=HIGH_PRIORITY,int typeNum=ID_USER_PACKET_ENUM+1);

};
