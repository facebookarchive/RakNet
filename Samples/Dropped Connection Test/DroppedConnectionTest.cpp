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
#include "Rand.h" // randomMT
#include "MessageIdentifiers.h" // Enumerations
#include "RakNetTypes.h" // SystemAddress
#include <cstdio>
using namespace RakNet;

#ifdef _WIN32
#include "Kbhit.h"
#include "WindowsIncludes.h" // Sleep
#else
#include "Kbhit.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#ifdef _DEBUG
static const int NUMBER_OF_CLIENTS=9;
#else
static const int NUMBER_OF_CLIENTS=100;
#endif

void ShowHelp(void)
{
	printf("System started.\n(D)isconnect a random client silently\n(C)onnect a random client\n(R)andom silent disconnects and connects for all clients.\n(N)otify server of random disconnections.\nSpace to verify connection list.\n(H)elp\n(Q)uit\n");
}

int main(void)
{
	RakPeerInterface *server;
	RakPeerInterface *clients[NUMBER_OF_CLIENTS];
	unsigned index, connectionCount;
	unsigned char ch;
	SystemAddress serverID;
	RakNet::Packet *p;
	unsigned short numberOfSystems;
	int sender;
	
	// Buffer for input (an ugly hack to keep *nix happy)
	#ifndef _WIN32
	char buff[256];
	#endif

	printf("This is a project I use internally to test if dropped connections are detected\n");
	printf("Difficulty: Intermediate\n\n");

	printf("Dropped Connection Test.\n");

	unsigned short serverPort = 20000;
	server=RakNet::RakPeerInterface::GetInstance();
//	server->InitializeSecurity(0,0,0,0);
	RakNet::SocketDescriptor socketDescriptor(serverPort,0);
	server->Startup(NUMBER_OF_CLIENTS, &socketDescriptor, 1);
	server->SetMaximumIncomingConnections(NUMBER_OF_CLIENTS);
	server->SetTimeoutTime(10000,UNASSIGNED_SYSTEM_ADDRESS);

	for (index=0; index < NUMBER_OF_CLIENTS; index++)
	{
		clients[index]=RakNet::RakPeerInterface::GetInstance();
		RakNet::SocketDescriptor socketDescriptor2(serverPort+1+index,0);
		clients[index]->Startup(1, &socketDescriptor2, 1);
		clients[index]->Connect("127.0.0.1", serverPort, 0, 0);
		clients[index]->SetTimeoutTime(5000,RakNet::UNASSIGNED_SYSTEM_ADDRESS);

		#ifdef _WIN32
				Sleep(10);
		#else
				usleep(10 * 1000);
		#endif
		printf("%i. ", index);
	}

	ShowHelp();

	while (1)
	{
		// User input
		if (kbhit())
		{
#ifndef _WIN32
			Gets(buff,sizeof(buff));
			ch=buff[0];
#else
			ch=getch();
#endif			

			if (ch=='d' || ch=='D')
			{
				index = randomMT() % NUMBER_OF_CLIENTS;
				
				clients[index]->GetConnectionList(0, &numberOfSystems);
				clients[index]->CloseConnection(serverID, false,0);
				if (numberOfSystems==0)
					printf("Client %i silently closing inactive connection.\n",index);
				else
					printf("Client %i silently closing active connection.\n",index);
			}
			else if (ch=='c' || ch=='C')
			{
				index = randomMT() % NUMBER_OF_CLIENTS;

				clients[index]->GetConnectionList(0, &numberOfSystems);
				clients[index]->Connect("127.0.0.1", serverPort, 0, 0);
				if (numberOfSystems==0)
					printf("Client %i connecting to same existing connection.\n",index);
				else
					printf("Client %i connecting to closed connection.\n",index);
			}
			else if (ch=='r' || ch=='R' || ch=='n' || ch=='N')
			{
				printf("Randomly connecting and disconnecting each client\n");
				for (index=0; index < NUMBER_OF_CLIENTS; index++)
				{
					if (NUMBER_OF_CLIENTS==1 || (randomMT()%2)==0)
					{
						if (clients[index]->IsActive())
						{
							if (ch=='r' || ch=='R')
								clients[index]->CloseConnection(serverID, false, 0);
							else
								clients[index]->CloseConnection(serverID, true, 0);
						}
					}
					else
					{
						clients[index]->Connect("127.0.0.1", serverPort, 0, 0);
					}
				}
			}
			else if (ch==' ')
			{
				server->GetConnectionList(0, &numberOfSystems);
				printf("The server thinks %i clients are connected.\n", numberOfSystems);
				connectionCount=0;
				for (index=0; index < NUMBER_OF_CLIENTS; index++)
				{
					clients[index]->GetConnectionList(0, &numberOfSystems);
					if (numberOfSystems>1)
						printf("Bug: Client %i has %i connections\n", index, numberOfSystems);
					if (numberOfSystems==1)
					{
						connectionCount++;
					}
				}
				printf("%i clients are actually connected.\n", connectionCount);
				printf("server->NumberOfConnections==%i.\n", server->NumberOfConnections());
			}
			else if (ch=='h' || ch=='H')
			{
				ShowHelp();
			}
			else if (ch=='q' || ch=='Q')
			{
				break;
			}
			ch=0;
		}

		// Parse messages
		
		while (1)
		{
			p = server->Receive();
			sender=NUMBER_OF_CLIENTS;
			if (p==0)
			{
				for (index=0; index < NUMBER_OF_CLIENTS; index++)
				{
					p = clients[index]->Receive();
					if (p!=0)
					{
						sender=index;
						break;						
					}
				}
			}

			if (p)
			{
				switch (p->data[0])
				{
				case ID_CONNECTION_REQUEST_ACCEPTED:
					printf("%i: %ID_CONNECTION_REQUEST_ACCEPTED from %i.\n",sender, p->systemAddress.GetPort());
					serverID=p->systemAddress;
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					// Connection lost normally
					printf("%i: ID_DISCONNECTION_NOTIFICATION from %i.\n",sender, p->systemAddress.GetPort());
					break;

				case ID_NEW_INCOMING_CONNECTION:
					// Somebody connected.  We have their IP now
					printf("%i: ID_NEW_INCOMING_CONNECTION from %i.\n",sender, p->systemAddress.GetPort());
					break;

				case ID_CONNECTION_LOST:
					// Couldn't deliver a reliable packet - i.e. the other system was abnormally
					// terminated
					printf("%i: ID_CONNECTION_LOST from %i.\n",sender, p->systemAddress.GetPort());
					break;

				case ID_NO_FREE_INCOMING_CONNECTIONS:
					printf("%i: ID_NO_FREE_INCOMING_CONNECTIONS from %i.\n",sender, p->systemAddress.GetPort());
					break;

				default:
					// Ignore anything else
					break;
				}
			}
			else
				break;

			if (sender==NUMBER_OF_CLIENTS)
				server->DeallocatePacket(p);
			else
				clients[sender]->DeallocatePacket(p);
		}

		// 11/29/05 - No longer necessary since I added the keepalive
		/*
		// Have everyone send a reliable packet so dropped connections are noticed.
		ch=255;
		server->Send((char*)&ch, 1, HIGH_PRIORITY, RELIABLE, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

		for (index=0; index < NUMBER_OF_CLIENTS; index++)
			clients[index]->Send((char*)&ch, 1, HIGH_PRIORITY, RELIABLE, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
			*/

		// Sleep so this loop doesn't take up all the CPU time

#ifdef _WIN32
			Sleep(30);
#else
			usleep(30 * 1000);
#endif

	}

	RakNet::RakPeerInterface::DestroyInstance(server);
	for (index=0; index < NUMBER_OF_CLIENTS; index++)
		RakNet::RakPeerInterface::DestroyInstance(clients[index]);
	return 1;
}
