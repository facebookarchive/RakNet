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
#include "RakNetStatistics.h"
#include <cstdio>
#include <memory.h>
#include <cstring>
#include <stdlib.h>
#include "Gets.h"

using namespace RakNet;

#ifdef WIN32
#include "Kbhit.h"
#else
#include "Kbhit.h"
#endif

int main(void)
{
	RakPeerInterface *rakServer;
	RakPeerInterface *rakClient;
	char ch;
	char str[255], remoteIP[255];
	char randomData[8192];
	int localPort, remotePort;
	int packetSize;
	int sendinterval;
	RakNet::TimeMS time;
	RakNet::Packet *p;
	RakNet::TimeMS lastPacketReceipt, lastNotification, lastSend;
	#ifndef _WIN32
	char buff[256];
	#endif
	
	memset(randomData, 255, sizeof(randomData));
	
	printf("This project is used to test two systems sending to each other with\n");
	printf("variable message sends rates and payload sizes\n");
	printf("Difficulty: Beginner\n\n");

	printf("Start relay (s)erver or start (c)lient?\n");
#ifndef _WIN32
	Gets(buff,sizeof(buff));
	ch=buff[0];
#else
	ch=getch();
#endif
	if (ch=='s' || ch=='S')
	{
		printf("Acting as server.\n");
		rakServer=RakNet::RakPeerInterface::GetInstance();
		rakClient=0;
	}
	else
	{
		printf("Acting as client.\n");
		rakClient=RakNet::RakPeerInterface::GetInstance();
		rakServer=0;
	}

	printf("Enter local port: ");
	Gets(str, sizeof(str));
	if (str[0]==0)
	{
		if (rakServer)
			localPort=60000;
		else
			localPort=0;
	}
	else
		localPort=atoi(str);

	if (rakServer)
	{
		RakNet::SocketDescriptor socketDescriptor(localPort,0);
		rakServer->Startup(100, &socketDescriptor, 1);
		rakServer->SetMaximumIncomingConnections(100);
	}
	else
	{
		printf("Enter remote IP: ");
		Gets(remoteIP,sizeof(remoteIP));
		if (remoteIP[0]==0)
			strcpy(remoteIP, "127.0.0.1");
		printf("Enter remote port: ");
		Gets(str, sizeof(str));
		if (str[0]==0)
			remotePort=60000;
		else
			remotePort=atoi(str);
		RakNet::SocketDescriptor socketDescriptor(localPort,0);
		rakClient->Startup(1, &socketDescriptor, 1);
		rakClient->Connect(remoteIP, remotePort, 0, 0);
	}

	printf("Entering loop.\nHit 'h' for help.\nHit 'q' to quit\n'i' to increase send interval\n'd' to decrease send interval\n'+' to increase packet size\n'-' to decrease packet size.\nSpace to show current statistics\n");

	sendinterval=128;
	packetSize=64;
	lastPacketReceipt=lastNotification=RakNet::GetTimeMS();
	lastSend=0;

	while (1)
	{
		time=RakNet::GetTimeMS();

		if (kbhit())
		{
#ifndef _WIN32
			Gets(buff,sizeof(buff));
			ch=buff[0];
#else
			ch=getch();
#endif
			if (ch=='q')
			{
				printf("Quitting\n");
				break;
			}
			else if (ch=='i')
			{
				sendinterval*=2;				
				printf("Send interval is now %i\n", sendinterval);
			}
			else if (ch=='d')
			{
				if (sendinterval>1)
					sendinterval/=2;
				printf("Send interval is now %i\n", sendinterval);
			}
			if (ch=='h')
				printf("Hit 'h' for help.\nHit 'q' to quit\n'i' to increase send interval\n'd' to decrease send interval\n'+' to increase packet size\n'-' to decrease packet size.\nSpace to show current statistics\n");
			else if (ch=='+')
			{
				if (packetSize < 8192)
					packetSize*=2;
				printf("Packet size is now %i\n", packetSize);
			}
			else if (ch=='-')
			{
				if (packetSize>1)
					packetSize/=2;
				printf("Packet size is now %i\n", packetSize);
			}
			else if (ch==' ')
			{
				if (rakServer)
				{
					StatisticsToString(rakServer->GetStatistics(rakServer->GetSystemAddressFromIndex(0)), randomData, 1);
					printf("%s", randomData);
				}
				else
				{
					StatisticsToString(rakClient->GetStatistics(rakClient->GetSystemAddressFromIndex(0)), randomData, 1);
					printf("%s", randomData);
				}

				printf("Send interval is %i\n", sendinterval);
				printf("Packet size is %i\n", packetSize);
				printf("\n");
			}

			
			ch=0;
		}

		// get packets
		if (rakServer)
			p = rakServer->Receive();
		else
			p=rakClient->Receive();

		if (p)
		{
			lastPacketReceipt=RakNet::GetTimeMS();

			switch (p->data[0])
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
				printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				// Connection lost normally
				printf("ID_DISCONNECTION_NOTIFICATION\n");
				break;

			case ID_NEW_INCOMING_CONNECTION:
				// Somebody connected.  We have their IP now
				printf("ID_NEW_INCOMING_CONNECTION\n");
				break;

			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				printf("ID_CONNECTION_LOST\n");
				break;
			default:
				// Relay
				if (rakServer)
					rakServer->Send((char*)p->data, p->length, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->systemAddress, true);
			}

			if (rakServer)
				rakServer->DeallocatePacket(p);
			else
				rakClient->DeallocatePacket(p);
		}

		if ((rakServer && rakServer->NumberOfConnections()>0) ||
			(rakClient && rakClient->NumberOfConnections()>0))
		{
			// Do sends
			if (lastSend + (RakNet::TimeMS)sendinterval < time)
			{
				if (rakServer)
				{
					rakServer->Send((char*)randomData, packetSize, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
				}
				else if (rakClient)
				{
					rakClient->Send((char*)randomData, packetSize, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
				}

				lastSend=time;
			}		

			if (lastPacketReceipt + 500 < (unsigned int)time && lastNotification + 500 < (unsigned int)time)
			{
				lastNotification=time;
				printf("Warning: No packets for %i ms.  Possibly a spike.\n", time - lastPacketReceipt);
			}
		}
	}

	if (rakServer)
		RakNet::RakPeerInterface::DestroyInstance(rakServer);
	else
		RakNet::RakPeerInterface::DestroyInstance(rakClient);

	return 1;
}
