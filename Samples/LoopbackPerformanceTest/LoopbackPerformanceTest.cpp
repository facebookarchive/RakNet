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
#include "MessageIdentifiers.h" // Enumerations
#include "GetTime.h"
#include "RakNetStatistics.h"
#include <cstdio>
#include <stdlib.h>
#include "Gets.h"

#ifdef _WIN32
#include "WindowsIncludes.h" // Sleep
#define SLEEP(arg) ( Sleep( (arg) ) )
#else
#include <unistd.h> // usleep
#define SLEEP(arg) ( usleep( (arg) *1000 ) )
#endif

static const int DESTINATION_SYSTEM_PORT=60000;
static const int RELAY_SYSTEM_PORT=60001;
static const int SOURCE_SYSTEM_PORT=60002;

int main(void)
{
	RakNet::RakPeerInterface *localSystem;
	RakNet::Packet *p;
	int systemType;
	unsigned char byteBlock[4096];
	RakNet::TimeMS time, quitTime, nextStatsTime;
	unsigned int packetsPerSecond, bytesPerPacket, num,index, bytesInPackets;
	RakNet::TimeMS lastSendTime;
	int sendMode;
	int verbosityLevel;
	unsigned int showStatsInterval;
	bool connectionCompleted, incomingConnectionCompleted;
	RakNet::RakNetStatistics *rss;

	printf("Loopback performance test.\n");
	printf("This test measures the effective transfer rate of RakNet.\n\n");
	printf("Instructions:\nStart 3 instances of this program.\n");
	printf("Press\n1. for the first instance (destination)\n2. for the second instance (relay)\n3. for the third instance (source).\n");
	printf("When the third instance is started the test will start.\n\n");
	printf("Difficulty: Intermediate\n\n");
	printf("Which instance is this?  Enter 1, 2, or 3: ");
	
	Gets((char*)byteBlock, sizeof(byteBlock));
	systemType=byteBlock[0]-'0'-1;
	if (systemType < 0 || systemType > 2)
	{
		printf("Error, you must enter 1, 2, or 3.\nQuitting.\n");
		return 1;
	}

	localSystem=RakNet::RakPeerInterface::GetInstance();
	
	printf("How many seconds do you want to run the test for?\n");
	Gets((char*)byteBlock, sizeof(byteBlock));
	if (byteBlock[0]==0)
	{
		printf("Defaulting to 90 seconds\n");
		quitTime=90;
	}
	else
		quitTime=atoi((char*)byteBlock);

	printf("Enter statistics verbosity level, 0=lowest, 2=highest\n");
	Gets((char*)byteBlock, sizeof(byteBlock));
	if (byteBlock[0]==0)
	{
		printf("Defaulting to verbosity level 1\n");
		verbosityLevel=1;
	}
	else
		verbosityLevel=atoi((char*)byteBlock);

	printf("How frequently to show statistics, in seconds?\n");
	Gets((char*)byteBlock, sizeof(byteBlock));
	if (byteBlock[0]==0)
	{
		printf("Defaulting to 5 seconds\n");
		showStatsInterval=5*1000;
	}
	else
		showStatsInterval=atoi((char*)byteBlock)*1000;

	if (systemType==0)
	{
		printf("Initializing Raknet...\n");
		// Destination.  Accept one connection and wait for further instructions.
		RakNet::SocketDescriptor socketDescriptor(DESTINATION_SYSTEM_PORT,0);
		if (localSystem->Startup(1, &socketDescriptor, 1)!=RakNet::RAKNET_STARTED)
		{
			printf("Failed to initialize RakNet!.\nQuitting\n");
			return 1;
		}	
		localSystem->SetMaximumIncomingConnections(1);
		printf("Initialization complete. Destination system started and waiting...\n");
	}
	else if (systemType==1)
	{
		printf("What send mode to use for relays?\n");
		printf("(0). UNRELIABLE\n");
		printf("(1). UNRELIABLE_SEQUENCED\n");
		printf("(2). RELIABLE\n");
		printf("(3). RELIABLE_ORDERED\n");
		printf("(4). RELIABLE_SEQUENCED\n");
		Gets((char*)byteBlock, sizeof(byteBlock));
		if (byteBlock[0]==0)
		{
			printf("Defaulting to RELIABLE\n");
			sendMode=2;
		}
		else
		{
			sendMode=atoi((char*)byteBlock);
			if (sendMode < 0 || sendMode > 4)
			{
				printf("Invalid send mode.  Using UNRELIABLE\n");
				sendMode=0;
			}
		}

		printf("Initializing Raknet...\n");
		// Relay.  Accept one connection, initiate outgoing connection, wait for further instructions.
		RakNet::SocketDescriptor socketDescriptor(RELAY_SYSTEM_PORT,0);
		if (localSystem->Startup(2, &socketDescriptor, 1)!=RakNet::RAKNET_STARTED)
		{
			printf("Failed to initialize RakNet!.\nQuitting\n");
			return 1;
		}
		localSystem->SetMaximumIncomingConnections(1);
		socketDescriptor.port=DESTINATION_SYSTEM_PORT;
		if (localSystem->Connect("127.0.0.1", DESTINATION_SYSTEM_PORT, 0, 0)!=RakNet::CONNECTION_ATTEMPT_STARTED)
		{
			printf("Connect call failed!.\nQuitting\n");
			return 1;
		}

		printf("Initialization complete. Relay system started.\nConnecting to destination and waiting for sender...\n");
	}
	else
	{
		printf("How many packets do you wish to send per second?\n");
		Gets((char*)byteBlock, sizeof(byteBlock));
		if (byteBlock[0]==0)
		{
#ifdef _DEBUG
			printf("Defaulting to 1000\n");
			packetsPerSecond=1000;
#else
			printf("Defaulting to 10000\n");
			packetsPerSecond=10000;
#endif
		}
		else
			packetsPerSecond=atoi((char*)byteBlock);
		printf("How many bytes per packet?\n");
		Gets((char*)byteBlock, sizeof(byteBlock));
		if (byteBlock[0]==0)
		{
			printf("Defaulting to 400\n");
			bytesPerPacket=400;
		}
		else
		{
			bytesPerPacket=atoi((char*)byteBlock);
			if (bytesPerPacket > 4096)
			{
				printf("Increase the array size of byteBlock to send more than 4096 bytes.\n");
				bytesPerPacket=4096;
			}
		}
		
		printf("What send mode?\n");
		printf("(0). UNRELIABLE\n");
		printf("(1). UNRELIABLE_SEQUENCED\n");
		printf("(2). RELIABLE\n");
		printf("(3). RELIABLE_ORDERED\n");
		printf("(4). RELIABLE_SEQUENCED\n");
		Gets((char*)byteBlock, sizeof(byteBlock));
		if (byteBlock[0]==0)
		{
			printf("Defaulting to RELIABLE\n");
			sendMode=2;
		}
		else
		{
			sendMode=atoi((char*)byteBlock);
			if (sendMode < 0 || sendMode > 4)
			{
				printf("Invalid send mode.  Using UNRELIABLE\n");
				sendMode=0;
			}
		}

		printf("Initializing RakNet...\n");
		// Sender.  Initiate outgoing connection to relay.
		RakNet::SocketDescriptor socketDescriptor(SOURCE_SYSTEM_PORT,0);
		if (localSystem->Startup(1, &socketDescriptor, 1)!=RakNet::RAKNET_STARTED)
		{
			printf("Failed to initialize RakNet!.\nQuitting\n");
			return 1;
		}
		if (localSystem->Connect("127.0.0.1", RELAY_SYSTEM_PORT, 0, 0)!=RakNet::CONNECTION_ATTEMPT_STARTED)
		{
			printf("Connect call failed!.\nQuitting\n");
			return 1;
		}

		printf("Initialization complete. Sender system started. Connecting to relay...\n");
	}

	connectionCompleted=false;
	incomingConnectionCompleted=false;
	time = RakNet::GetTimeMS();
	lastSendTime=time;
	nextStatsTime=time+2000; // First stat shows up in 2 seconds
	bytesInPackets=0;

	while (time < quitTime || (connectionCompleted==false && incomingConnectionCompleted==false))
	{
		time = RakNet::GetTimeMS();
		// Parse messages
		while (1)
		{
			p = localSystem->Receive();

			if (p)
			{
				bytesInPackets+=p->length;
				switch (p->data[0])
				{
				case ID_CONNECTION_REQUEST_ACCEPTED:
					printf("ID_CONNECTION_REQUEST_ACCEPTED.\n");
					connectionCompleted=true;
					// Timer starts when a connection has completed
					if (systemType==1 || systemType==2)
						quitTime=quitTime*1000 + time;
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					// Connection lost normally
					printf("ID_DISCONNECTION_NOTIFICATION.\n");
			//		connectionCompleted=false;
					break;
				case ID_NEW_INCOMING_CONNECTION:
					// Somebody connected.  We have their IP now
					printf("ID_NEW_INCOMING_CONNECTION.\n");
					incomingConnectionCompleted=true;
					// Timer starts when a new connection has come in
					if (systemType==0)
						quitTime=quitTime*1000 + time;
					if (systemType==1 && connectionCompleted==false)
						printf("Warning, relay connection to destination has not completed yet.\n");
					break;

				case ID_CONNECTION_LOST:
					// Couldn't deliver a reliable packet - i.e. the other system was abnormally
					// terminated
					printf("ID_CONNECTION_LOST.\n");
				//	connectionCompleted=false;
					break;
				case ID_NO_FREE_INCOMING_CONNECTIONS:
					printf("ID_NO_FREE_INCOMING_CONNECTIONS.\n");
					break;
				default:
					// The relay system will relay all data with 255 as the first byte
					if (systemType==1)
					{
						if (p->data[0]==255)
						{
							if (localSystem->Send((char*)p->data, p->length, HIGH_PRIORITY, (PacketReliability)sendMode, 0, p->systemAddress, true)==false)
							{
								printf("Relay failed!\n");
							}
						}
						else
							printf("Got packet with ID %u\n", p->data[0]);
					}
						
					break;
				}
			}
			else
				break;

			localSystem->DeallocatePacket(p);
		}

		// Show stats.
		if (time > nextStatsTime && (connectionCompleted || incomingConnectionCompleted))
		{
			printf("\n* First connected system statistics:\n");
			rss=localSystem->GetStatistics(localSystem->GetSystemAddressFromIndex(0));
			StatisticsToString(rss, (char*)byteBlock, verbosityLevel);
			printf("%s", byteBlock);
			if (systemType==1)
			{
				rss=localSystem->GetStatistics(localSystem->GetSystemAddressFromIndex(1));
				if (rss)
				{
					printf("* Second connected system statistics:\n");
					StatisticsToString(rss, (char*)byteBlock, verbosityLevel);
					printf("%s", byteBlock);
				}				
			}

			nextStatsTime = time + showStatsInterval;
		}

		// As the destination, we don't care if the connection is completed.  Do nothing
		// As the relay, we relay packets if the connection is completed.
		// That is done when the packet arrives.
		// As the source, we start sending packets when the connection is completed.
		if (systemType==2 && connectionCompleted)
		{
			
			// Number of packets to send is (float)(packetsPerSecond * (time - lastSendTime)) / 1000.0f;
			num=(packetsPerSecond * (unsigned int) (time - lastSendTime)) / 1000;
			byteBlock[0]=255; // Relay all data with an identifier of 255
			for (index=0; index < num; index++)
			{
				localSystem->Send((char*)byteBlock, bytesPerPacket, HIGH_PRIORITY, (PacketReliability)sendMode, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
			}
            
			lastSendTime+= (1000 * num) / packetsPerSecond;
		}

		SLEEP(100);
	}

	printf("Test duration elapsed.  Final Stats:\n");
	printf("\n* First connected system statistics:\n");
	rss=localSystem->GetStatistics(localSystem->GetSystemAddressFromIndex(0));
	StatisticsToString(rss, (char*)byteBlock, 2);
	printf("%s", byteBlock);
	if (systemType==1)
	{
		rss=localSystem->GetStatistics(localSystem->GetSystemAddressFromIndex(1));
		if (rss)
		{
			printf("* Second connected system statistics:\n");
			StatisticsToString(rss, (char*)byteBlock, 2);
			printf("%s", byteBlock);
		}				
	}

	printf("Hit enter to continue.\n");

	char buff[100];
	Gets(buff,sizeof(buff));

	RakNet::RakPeerInterface::DestroyInstance(localSystem);
	return 0;
}
