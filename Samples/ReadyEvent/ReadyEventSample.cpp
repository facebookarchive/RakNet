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
#include "Rand.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "ReadyEvent.h"
#include <assert.h>
#include "Kbhit.h"
#include "RakSleep.h"
#include "SocketLayer.h"
#include "FullyConnectedMesh2.h"
#include "ConnectionGraph2.h"

void PrintConnections();

using namespace RakNet;

RakPeerInterface *rakPeer;
ReadyEvent readyEventPlugin;

// These two plugins are just to automatically create a fully connected mesh so I don't have to call connect more than once
FullyConnectedMesh2 fcm2;
ConnectionGraph2 cg2;

int main(void)
{
	rakPeer=RakPeerInterface::GetInstance();

	printf("This project tests and demonstrates the ready event plugin.\n");
	printf("It is used in a peer to peer environment to have a group of\nsystems signal an event.\n");
	printf("It is useful for changing turns in a turn based game,\nor for lobby systems where everyone has to set ready before the game starts\n");
	printf("Difficulty: Beginner\n\n");

	rakPeer->AttachPlugin(&readyEventPlugin);
	rakPeer->AttachPlugin(&fcm2);
	rakPeer->AttachPlugin(&cg2);
	rakPeer->SetMaximumIncomingConnections(8);

	fcm2.SetAutoparticipateConnections(true);
	fcm2.SetConnectOnNewRemoteConnection(true, "");
	cg2.SetAutoProcessNewConnections(true);

	// Initialize the peers
	SocketDescriptor sd(60000,0);
	while (IRNS2_Berkley::IsPortInUse(sd.port,sd.hostAddress,sd.socketFamily,SOCK_DGRAM)==true)
		sd.port++;
	StartupResult sr = rakPeer->Startup(8, &sd, 1);
	RakAssert(sr==RAKNET_STARTED);
	printf("Started on port %i\n", sd.port);

	// Give the threads time to properly start
	RakSleep(200);

	printf("Peers initialized.\n");

	printf("'C' to connect\n");
	printf("'D' to disconnect\n");
	printf("'S' to signal\n");
	printf("'U' to unsignal\n");
	printf("'F' to force all systems to be completed (cannot be unset)\n");
	printf("'Q' to quit\n");
	printf("' ' to print wait status\n");

	char str[128];
	char ch=0;
	while (1)
	{
		if (kbhit())
			ch=getch();

		if (ch=='s' || ch=='S')
		{
			ch=0;
			if (readyEventPlugin.SetEvent(0,true))
				printf("This system is signaled\n");
			else
				printf("This system is signaled FAILED\n");
		}
		if (ch=='u' || ch=='U')
		{
			ch=0;
			if (readyEventPlugin.SetEvent(0,false))
				printf("This system is unsignaled\n");
			else
				printf("This system is unsignaled FAILED\n");
		}
		if (ch=='c' || ch=='C')
		{
			ch=0;
			printf("Which IP? (Press enter for 127.0.0.1)");
			gets(str);
			if (str[0]==0)
				strcpy(str, "127.0.0.1");
			char port[64];
			printf("Which port? (Press enter for 60000)");
			gets(port);
			if (port[0]==0)
				strcpy(port, "60000");
			ConnectionAttemptResult car = rakPeer->Connect(str, atoi(port), 0, 0, 0);
			RakAssert(car==CONNECTION_ATTEMPT_STARTED);
			printf("Connecting.\n");
		}
		if (ch=='d' || ch=='D')
		{
			ch=0;
			rakPeer->Shutdown(100,0);
			sr = rakPeer->Startup(8, &sd, 1);
			RakAssert(sr==RAKNET_STARTED);
			printf("Restarting RakPeerInterface.\n");
		}
		if (ch=='f' || ch=='F')
		{
			ch=0;
			readyEventPlugin.ForceCompletion(0);
			printf("Called ForceCompletion()\nIsEventCompleted() will be fixed at true for all systems.\n");
		}
		if (ch==' ')
		{
			ch=0;

			printf("\n");
			PrintConnections();
			if (readyEventPlugin.IsEventSet(0))
				printf("Signaled=True, ");
			else
				printf("Signaled=False, ");

			if (readyEventPlugin.IsEventCompleted(0))
				printf("Completed=True\n");
			else if (readyEventPlugin.IsEventCompletionProcessing(0))
				printf("Completed=InProgress\n");
			else
				printf("Completed=False\n");

			DataStructures::List<SystemAddress> addresses;
			DataStructures::List<RakNetGUID> guids;
			rakPeer->GetSystemList(addresses, guids);
			for (unsigned short i=0; i < guids.Size(); i++)
			{
				ReadyEventSystemStatus ress = readyEventPlugin.GetReadyStatus(0, guids[i]);
				printf("  Remote system %i, status = ", i);

				switch (ress)
				{
				case RES_NOT_WAITING:
					printf("RES_NOT_WAITING\n");
					break;
				case RES_WAITING:
					printf("RES_WAITING\n");
					break;
				case RES_READY:
					printf("RES_READY\n");
					break;
				case RES_ALL_READY:
					printf("RES_ALL_READY\n");
					break;
				case RES_UNKNOWN_EVENT:
					printf("RES_UNKNOWN_EVENT\n");
					break;
				}
			}
		}
		if (ch=='Q' || ch=='q')
		{
			break;
		}

		Packet *p = rakPeer->Receive();
		if (p)
		{
			switch (p->data[0])
			{
			case ID_NEW_INCOMING_CONNECTION:
				printf("ID_NEW_INCOMING_CONNECTION\n");
				readyEventPlugin.AddToWaitList(0, p->guid);
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
				readyEventPlugin.AddToWaitList(0, p->guid);
				break;
			case ID_READY_EVENT_ALL_SET:
				printf("Got ID_READY_EVENT_ALL_SET from %s\n", p->guid.ToString());
				break;

			case ID_READY_EVENT_SET:
				printf("Got ID_READY_EVENT_SET from %s\n", p->guid.ToString());
				break;

			case ID_READY_EVENT_UNSET:
				printf("Got ID_READY_EVENT_UNSET from %s\n", p->guid.ToString());
				break;

			case ID_DISCONNECTION_NOTIFICATION:
				// Connection lost normally
				printf("ID_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_ALREADY_CONNECTED:
				// Connection lost normally
				printf("ID_ALREADY_CONNECTED with guid %" PRINTF_64_BIT_MODIFIER "u\n", p->guid);
				break;
			case ID_INCOMPATIBLE_PROTOCOL_VERSION:
				printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
				break;
			case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_DISCONNECTION_NOTIFICATION\n"); 
				break;
			case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_CONNECTION_LOST\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
				break;
			case ID_CONNECTION_BANNED: // Banned from this server
				printf("We are banned from this server.\n");
				break;			
			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("Connection attempt failed\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				// Sorry, the server is full.  I don't do anything here but
				// A real app should tell the user
				printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
				break;
			case ID_INVALID_PASSWORD:
				printf("ID_INVALID_PASSWORD\n");
				break;
			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				printf("ID_CONNECTION_LOST\n");
				break;
			case ID_CONNECTED_PING:
			case ID_UNCONNECTED_PING:
				printf("Ping from %s\n", p->systemAddress.ToString(true));
				break;
			}

			rakPeer->DeallocatePacket(p);
		}		

		// Keep raknet threads responsive
		RakSleep(30);
	}
	
	RakPeerInterface::DestroyInstance(rakPeer);

	return 1;
}

void PrintConnections()
{
	int i,j;
	char ch=0;
	SystemAddress systemAddress;
	
	printf("--------------------------------\n");

	SystemAddress remoteSystems[8];
	unsigned short numberOfSystems;
	rakPeer->GetConnectionList(remoteSystems, &numberOfSystems);

	for (i=0; i < numberOfSystems; i++)
	{
		printf("%i (Conn): ", 60000+i);
		for (j=0; j < numberOfSystems; j++)
		{
			systemAddress=rakPeer->GetSystemAddressFromIndex(j);
			if (systemAddress!=UNASSIGNED_SYSTEM_ADDRESS)
				printf("%i ", systemAddress.GetPort());
		}

		printf("\n");
	}
	printf("\n");

	printf("--------------------------------\n");
	
}