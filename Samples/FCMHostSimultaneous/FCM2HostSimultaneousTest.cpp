/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include "GetTime.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"

#include "RakNetTypes.h"
#include "RakSleep.h"
#include "FullyConnectedMesh2.h"
#include "ConnectionGraph2.h"
#include <assert.h>
#include "SocketLayer.h"
#include "Kbhit.h"
#include "PacketLogger.h"
#include "Gets.h"

using namespace RakNet;

#define NUM_PEERS 8
RakNet::RakPeerInterface *rakPeer[NUM_PEERS];

int main()
{
	FullyConnectedMesh2 fcm2[NUM_PEERS];
	ConnectionGraph2 cg2[NUM_PEERS];

	for (int i=0; i < NUM_PEERS; i++)
	{
		rakPeer[i]=RakNet::RakPeerInterface::GetInstance();
		rakPeer[i]->AttachPlugin(&fcm2[i]);
		rakPeer[i]->AttachPlugin(&cg2[i]);
		fcm2[i].SetAutoparticipateConnections(true);
		RakNet::SocketDescriptor sd;
		sd.port=60000+i;
		StartupResult sr = rakPeer[i]->Startup(NUM_PEERS,&sd,1);
		RakAssert(sr==RAKNET_STARTED);
		rakPeer[i]->SetMaximumIncomingConnections(NUM_PEERS);
		rakPeer[i]->SetTimeoutTime(1000,RakNet::UNASSIGNED_SYSTEM_ADDRESS);
		printf("Our guid is %s\n", rakPeer[i]->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString());
	}

	for (int i=0; i < NUM_PEERS; i++)
	{
		for (int j=0; j < NUM_PEERS; j++)
		{
			if (i==j)
				continue;
			ConnectionAttemptResult car = rakPeer[i]->Connect("127.0.0.1", 60000+j, 0, 0 );
			RakAssert(car==CONNECTION_ATTEMPT_STARTED);
		}
	}

	bool quit=false;
	RakNet::Packet *packet;
	char ch;
	while (!quit)
	{
		for (int i=0; i < NUM_PEERS; i++)
		{
			for (packet = rakPeer[i]->Receive(); packet; rakPeer[i]->DeallocatePacket(packet), packet = rakPeer[i]->Receive())
			{
				switch (packet->data[0])
				{
				case ID_DISCONNECTION_NOTIFICATION:
					// Connection lost normally
					printf("%i. ID_DISCONNECTION_NOTIFICATION\n", i);
					break;
				case ID_CONNECTION_ATTEMPT_FAILED:
					printf("%i. ID_CONNECTION_ATTEMPT_FAILED\n", i);
					break;

				case ID_NEW_INCOMING_CONNECTION:
					// Somebody connected.  We have their IP now
					printf("%i. ID_NEW_INCOMING_CONNECTION from %s. guid=%s.\n", i, packet->systemAddress.ToString(true), packet->guid.ToString());
					break;

				case ID_CONNECTION_REQUEST_ACCEPTED:
					// Somebody connected.  We have their IP now
					printf("%i. ID_CONNECTION_REQUEST_ACCEPTED from %s. guid=%s.\n", i, packet->systemAddress.ToString(true), packet->guid.ToString());
					break;


				case ID_CONNECTION_LOST:
					// Couldn't deliver a reliable packet - i.e. the other system was abnormally
					// terminated
					printf("%i. ID_CONNECTION_LOST\n", i);
					break;


				case ID_FCM2_NEW_HOST:
					if (packet->systemAddress==RakNet::UNASSIGNED_SYSTEM_ADDRESS)
						printf("%i. Got new host (ourselves)\n", i);
					else
						printf("%i. Got new host %s, %s\n", i, packet->systemAddress.ToString(true), packet->guid.ToString());
					break;
				}
			}
		}

		if (kbhit())
		{
			ch=getch();
			if (ch==' ')
			{
				unsigned int participantList;
				RakNetGUID hostGuid;
				bool weAreHost;
				for (int i=0; i < NUM_PEERS; i++)
				{
					if (rakPeer[i]->IsActive()==false)
						continue;

					fcm2[i].GetParticipantCount(&participantList);
					weAreHost=fcm2[i].IsHostSystem();
					hostGuid=fcm2[i].GetHostSystem();
					
					if (weAreHost)
						printf("%i. %iP myGuid=%s, hostGuid=%s tcc=%i (Y)\n",i, participantList, rakPeer[i]->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString(), hostGuid.ToString(), fcm2[i].GetTotalConnectionCount());
					else
						printf("%i. %iP myGuid=%s, hostGuid=%s tcc=%i (N)\n",i, participantList, rakPeer[i]->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString(), hostGuid.ToString(), fcm2[i].GetTotalConnectionCount());
				}
			}
			if (ch=='d' || ch=='D')
			{
				char str[32];
				printf("Enter system index to disconnect.\n");
				Gets(str, sizeof(str));
				rakPeer[atoi(str)]->Shutdown(100);
				printf("Done.\n");
			}
			if (ch=='q' || ch=='Q')
			{
				printf("Quitting.\n");
				quit=true;
			}
		}


		RakSleep(30);
	}

	for (int i=0; i < NUM_PEERS; i++)
	{
		RakNet::RakPeerInterface::DestroyInstance(rakPeer[i]);
	}
	return 0;
}
