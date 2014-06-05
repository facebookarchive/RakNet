/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "RakPeerInterface.h"
#include "GetTime.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include <cstdio>
#include <memory.h>
#include <cstring>
#include <stdlib.h>
#include "Rand.h"
#include "RakNetStatistics.h"
#include "RakSleep.h"
#include "RakMemoryOverride.h"
#include <stdio.h>
#include "Gets.h"
#include "Kbhit.h"
#include "RakSleep.h"

using namespace RakNet;

int main(int argc, char **argv)
{
	RakPeerInterface *rakPeer;
	char str[256];
	char ip[32];
	unsigned short remotePort, localPort;
	RakNet::Packet *packet;

	printf("This project tests sending a burst of messages to a remote system.\n");
	printf("Difficulty: Beginner\n\n");
	
	rakPeer = RakNet::RakPeerInterface::GetInstance();
	
	printf("Enter remote IP (enter to not connect): ");
	Gets(ip, sizeof(ip));
	if (ip[0])
	{
		printf("Enter remote port: ");
		Gets(str, sizeof(str));
		if (str[0]==0)
			strcpy(str, "60000");
		remotePort=atoi(str);
		
		printf("Enter local port: ");
		Gets(str, sizeof(str));
		if (str[0]==0)
			strcpy(str, "0");
		localPort=atoi(str);
		
		RakNet::SocketDescriptor socketDescriptor(localPort,0);
		rakPeer->Startup(32, &socketDescriptor, 1);
		
		printf("Connecting...\n");
		rakPeer->Connect(ip, remotePort, 0, 0);
	}
	else
	{
		printf("Enter local port: ");
		Gets(str, sizeof(str));
		if (str[0]==0)
			strcpy(str, "60000");
		localPort=atoi(str);
		
		RakNet::SocketDescriptor socketDescriptor(localPort,0);
		rakPeer->Startup(32, &socketDescriptor, 1);
	}
	rakPeer->SetMaximumIncomingConnections(32);
	
	printf("'s' to send. ' ' for statistics. 'q' to quit.\n");

	while (1)
	{
		if (kbhit())
		{
			char ch=getch();
			if (ch=='q')
				return 1;
			else if (ch==' ')
			{
				RakNetStatistics *rss;
				char message[2048];
					rss=rakPeer->GetStatistics(rakPeer->GetSystemAddressFromIndex(0));
				StatisticsToString(rss, message, 2);
				printf("%s", message);
			}
			else if (ch=='s')
			{
				char msgSizeStr[128], msgCountStr[128];
				uint32_t msgSize, msgCount,index;
				printf("Enter message size in bytes: ");
				Gets(msgSizeStr, sizeof(msgSizeStr));
				if (msgSizeStr[0]==0)
					msgSize=4096;
				else
					msgSize=atoi(msgSizeStr);
				printf("Enter times to repeatedly send message: ");
				Gets(msgCountStr, sizeof(msgCountStr));
				if (msgCountStr[0]==0)
					msgCount=128;
				else
					msgCount=atoi(msgCountStr);
				RakNet::BitStream bitStream;
				for (index=0; index < msgCount; index++)
				{
					bitStream.Reset();
					bitStream.Write((MessageID)ID_USER_PACKET_ENUM);
					bitStream.Write(msgSize);
					bitStream.Write(index);
					bitStream.Write(msgCount);
					bitStream.PadWithZeroToByteLength(msgSize);
					rakPeer->Send(&bitStream, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
				}
				printf("Sent\n");
					
			}
		}
		
		for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet=rakPeer->Receive())
		{
			switch(packet->data[0])
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
				printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
				break;
			case ID_NEW_INCOMING_CONNECTION:
				printf("ID_NEW_INCOMING_CONNECTION\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				printf("ID_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_CONNECTION_LOST:
				printf("ID_CONNECTION_LOST\n");
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("Connection attempt failed\n");
				break;
			case ID_USER_PACKET_ENUM:
				{
					uint32_t msgSize, msgCount, index;
					RakNet::BitStream bitStream(packet->data, packet->length, false);
					bitStream.IgnoreBytes(sizeof(MessageID));
					bitStream.Read(msgSize);
					bitStream.Read(index);
					bitStream.Read(msgCount);
					printf("%i/%i len=%i", index+1, msgCount, packet->length);
					if (msgSize > BITS_TO_BYTES(bitStream.GetReadOffset()) && packet->length!=msgSize)
						printf("UNDERLENGTH!\n");
					else
						printf("\n");
					break;
				}
			default:
				printf("Unknown message type %i\n", packet->data[0]);
			}
		}
	
		RakSleep(30);
	}

	rakPeer->Shutdown(100);
	RakNet::RakPeerInterface::DestroyInstance(rakPeer);

	return 1;
}
