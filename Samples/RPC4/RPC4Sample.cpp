/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "RPC4Plugin.h"
#include "RakPeerInterface.h"
#include <stdio.h>
#include "Kbhit.h"
#include <string.h>
#include <stdlib.h>
#include "RakSleep.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "Gets.h"

using namespace RakNet;

RakPeerInterface *rakPeer;

void CFunc1( RakNet::BitStream *bitStream, Packet *packet )
{
	printf("CFunc1 ");
	RakNet::RakString data;
	int offset=bitStream->GetReadOffset();
	bool read = bitStream->ReadCompressed(data);
	RakAssert(read);
	printf("%s\n", data.C_String());
};

void CFunc2( RakNet::BitStream *bitStream, Packet *packet )
{
	printf("CFunc2 ");
	RakNet::RakString data;
	int offset=bitStream->GetReadOffset();
	bool read = bitStream->ReadCompressed(data);
	RakAssert(read);
	printf("%s\n", data.C_String());
};

void CFunc3( RakNet::BitStream *bitStream, RakNet::BitStream *returnData, Packet *packet )
{
	printf("CFunc3 ");
	RakNet::RakString data;
	int offset=bitStream->GetReadOffset();
	bool read = bitStream->ReadCompressed(data);
	RakAssert(read);
	printf("%s\n", data.C_String());
	returnData->WriteCompressed("CFunc3 returnData");
};

int main(void)
{
	printf("Demonstration of the RPC4 plugin.\n");
	printf("Difficulty: Beginner\n\n");

	rakPeer=RakNet::RakPeerInterface::GetInstance();
	
	// Holds user data
	char ip[64], serverPort[30], clientPort[30];

	// Get our input
	puts("Enter the port to listen on");
	Gets(clientPort,sizeof(clientPort));
	if (clientPort[0]==0)
		strcpy(clientPort, "0");
	
	RakNet::SocketDescriptor sd1(atoi(clientPort),0);
	rakPeer->Startup(8,&sd1,1);
	rakPeer->SetMaximumIncomingConnections(8);

	puts("Enter IP to connect to, or enter for none");
	Gets(ip, sizeof(ip));
	rakPeer->AllowConnectionResponseIPMigration(false);

	RPC4 rpc;
	rakPeer->AttachPlugin(&rpc);
	rpc.RegisterSlot("Event1", CFunc1, 0);
	rpc.RegisterSlot("Event1", CFunc2, 0);
	rpc.RegisterBlockingFunction("Blocking", CFunc3);

	RakNet::Packet *packet;
	if (ip[0])
	{
		puts("Enter the port to connect to");
		Gets(serverPort,sizeof(serverPort));
		rakPeer->Connect(ip, atoi(serverPort), 0, 0);

		RakSleep(1000);

		for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet=rakPeer->Receive())
			;

		RakNet::BitStream testBs;
		testBs.WriteCompressed("testData");
	//	rpc.Signal("Event1", &testBs, HIGH_PRIORITY,RELIABLE_ORDERED,0,rakPeer->GetSystemAddressFromIndex(0),false, true);

		RakSleep(100);
		for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), rakPeer->Receive())
			;

		// Needs 2 program instances, because while the call is blocking rakPeer2->Receive() isn't getting called
		RakNet::BitStream testBlockingReturn;
		rpc.CallBlocking("Blocking", &testBs, HIGH_PRIORITY,RELIABLE_ORDERED,0,rakPeer->GetSystemAddressFromIndex(0),&testBlockingReturn);

		RakNet::RakString data;
		bool read = testBlockingReturn.ReadCompressed(data);
		printf("%s\n", data.C_String());
	}
	else
	{
		printf("Waiting for connections.\n");

		while (1)
		{
			RakSleep(100);
			for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet=rakPeer->Receive())
				;
		}
	}
	

	rakPeer->Shutdown(100,0);
	RakNet::RakPeerInterface::DestroyInstance(rakPeer);

	return 1;
}
