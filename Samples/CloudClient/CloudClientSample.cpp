/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "CloudClient.h"
#include "RakPeerInterface.h"
#include "RakSleep.h"
#include <stdlib.h>

void PrintHelp(void)
{
	printf("CloudClient as a directory server.\n");
	printf("Usage:\n");
	printf("CloudClient.exe ServerAddress [Port]\n\n");
	printf("Parameters:\n");
	printf("ServerAddress - Any server in the cloud.\n");
	printf("Port - Server listen port. Default is 60000\n");
	printf("Example:\n");
	printf("CloudServer.exe test.dnsalias.net 60000\n\n");
}

#define CLOUD_CLIENT_PRIMARY_KEY "CC_Sample_PK"

void UploadInstanceToCloud(RakNet::CloudClient *cloudClient, RakNet::RakNetGUID serverGuid);
void GetClientSubscription(RakNet::CloudClient *cloudClient, RakNet::RakNetGUID serverGuid);
void GetServers(RakNet::CloudClient *cloudClient, RakNet::RakNetGUID serverGuid);

int main(int argc, char **argv)
{
	const char *DEFAULT_SERVER_ADDRESS="test.dnsalias.net";
	const unsigned short DEFAULT_SERVER_PORT=60000;

	const char *serverAddress;
	unsigned short serverPort;


#ifndef _DEBUG
	// Only use DEFAULT_SERVER_ADDRESS for debugging
	if (argc<2)
	{
		PrintHelp();
		return false;
	}
#endif

	if (argc<2) serverAddress=DEFAULT_SERVER_ADDRESS;
	else serverAddress=argv[1];

	if (argc<3) serverPort=DEFAULT_SERVER_PORT;
	else serverPort=atoi(argv[2]);

	// ---- RAKPEER -----
	RakNet::RakPeerInterface *rakPeer;
	rakPeer=RakNet::RakPeerInterface::GetInstance();
	static const unsigned short clientLocalPort=0;
	RakNet::SocketDescriptor sd(clientLocalPort,0); // Change this if you want
	RakNet::StartupResult sr = rakPeer->Startup(1,&sd,1); // Change this if you want
	rakPeer->SetMaximumIncomingConnections(0); // Change this if you want
	if (sr!=RakNet::RAKNET_STARTED)
	{
		printf("Startup failed. Reason=%i\n", (int) sr);
		return 1;
	}

	RakNet::CloudClient cloudClient;
	rakPeer->AttachPlugin(&cloudClient);

	RakNet::ConnectionAttemptResult car = rakPeer->Connect(serverAddress, serverPort, 0, 0);
	if (car==RakNet::CANNOT_RESOLVE_DOMAIN_NAME)
	{
		printf("Cannot resolve domain name\n");
		return 1;
	}

	printf("Connecting to %s...\n", serverAddress);
	bool didRebalance=false; // So we only reconnect to a lower load server once, for load balancing
	RakNet::Packet *packet;
	while (1)
	{
		for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet=rakPeer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_CONNECTION_LOST:
				printf("Lost connection to server.\n");
				return 1;
			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("Failed to connect to server at %s.\n", packet->systemAddress.ToString(true));
				return 1;
			case ID_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY:
			case ID_OUR_SYSTEM_REQUIRES_SECURITY:
			case ID_PUBLIC_KEY_MISMATCH:
			case ID_INVALID_PASSWORD:
			case ID_CONNECTION_BANNED:
				// You won't see these unless you modified CloudServer
				printf("Server rejected the connection.\n");
				return 1;
			case ID_INCOMPATIBLE_PROTOCOL_VERSION:
				printf("Server is running an incompatible RakNet version.\n");
				return 1;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("Server has no free connections\n");
				return 1;
			case ID_IP_RECENTLY_CONNECTED:
				printf("Recently connected. Retrying.");
				rakPeer->Connect(serverAddress, serverPort, 0, 0);
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				printf("Connected to server.\n");
				UploadInstanceToCloud(&cloudClient, packet->guid);
				GetClientSubscription(&cloudClient, packet->guid);
				GetServers(&cloudClient, packet->guid);
				break;
			case ID_CLOUD_GET_RESPONSE:
				{
					RakNet::CloudQueryResult cloudQueryResult;
					cloudClient.OnGetReponse(&cloudQueryResult, packet);
					unsigned int rowIndex;
					const bool wasCallToGetServers=cloudQueryResult.cloudQuery.keys[0].primaryKey=="CloudConnCount";
					printf("\n");
					if (wasCallToGetServers)
						printf("Downloaded server list. %i servers.\n", cloudQueryResult.rowsReturned.Size());
					else
						printf("Downloaded client list. %i clients.\n", cloudQueryResult.rowsReturned.Size());

					unsigned short connectionsOnOurServer=65535;
					unsigned short lowestConnectionsServer=65535;
					RakNet::SystemAddress lowestConnectionAddress;

					for (rowIndex=0; rowIndex < cloudQueryResult.rowsReturned.Size(); rowIndex++)
					{
						RakNet::CloudQueryRow *row = cloudQueryResult.rowsReturned[rowIndex];
						if (wasCallToGetServers)
						{
							unsigned short connCount;
							RakNet::BitStream bsIn(row->data, row->length, false);
							bsIn.Read(connCount);
							printf("%i. Server found at %s with %i connections\n", rowIndex+1, row->serverSystemAddress.ToString(true), connCount);

							unsigned short connectionsExcludingOurselves;
							if (row->serverGUID==packet->guid)
								connectionsExcludingOurselves=connCount-1;
							else
								connectionsExcludingOurselves=connCount;


							// Find the lowest load server (optional)
							if (packet->guid==row->serverGUID)
							{
								connectionsOnOurServer=connectionsExcludingOurselves;
							}
							else if (connectionsExcludingOurselves < lowestConnectionsServer)
							{
								lowestConnectionsServer=connectionsExcludingOurselves;
								lowestConnectionAddress=row->serverSystemAddress;
							}
						}
						else
						{
							printf("%i. Client found at %s", rowIndex+1, row->clientSystemAddress.ToString(true));
							if (row->clientGUID==rakPeer->GetMyGUID())
								printf(" (Ourselves)");
							RakNet::BitStream bsIn(row->data, row->length, false);
							RakNet::RakString clientData;
							bsIn.Read(clientData);
							printf(" Data: %s", clientData.C_String());
							printf("\n");
						}
					}


					// Do load balancing by reconnecting to lowest load server (optional)
					if (didRebalance==false && wasCallToGetServers && cloudQueryResult.rowsReturned.Size()>0 && connectionsOnOurServer>lowestConnectionsServer)
					{
						printf("Reconnecting to lower load server %s\n", lowestConnectionAddress.ToString(false));

						rakPeer->CloseConnection(packet->guid, true);
						// Wait for the thread to close, otherwise will immediately get back ID_CONNECTION_ATTEMPT_FAILED because no free outgoing connection slots
						// Alternatively, just call Startup() with 2 slots instead of 1
						RakSleep(500);

						rakPeer->Connect(lowestConnectionAddress.ToString(false), lowestConnectionAddress.GetPort(), 0, 0);
						didRebalance=true;
					}

					cloudClient.DeallocateWithDefaultAllocator(&cloudQueryResult);
				}

				break;
			case ID_CLOUD_SUBSCRIPTION_NOTIFICATION:
				{
					bool wasUpdated;
					RakNet::CloudQueryRow cloudQueryRow;
					cloudClient.OnSubscriptionNotification(&wasUpdated, &cloudQueryRow, packet, 0 );
					if (wasUpdated)
						printf("New client at %s\n", cloudQueryRow.clientSystemAddress.ToString(true));
					else
						printf("Lost client at %s\n", cloudQueryRow.clientSystemAddress.ToString(true));
					cloudClient.DeallocateWithDefaultAllocator(&cloudQueryRow);
				}

				break;
			}
		}

		// Any additional client processing can go here
		RakSleep(30);
	}

	RakNet::RakPeerInterface::DestroyInstance(rakPeer);
	return 0;
}

void UploadInstanceToCloud(RakNet::CloudClient *cloudClient, RakNet::RakNetGUID serverGuid)
{
	RakNet::CloudKey cloudKey(CLOUD_CLIENT_PRIMARY_KEY,0);
	RakNet::BitStream bs;
	bs.Write("Hello World"); // This could be anything such as player list, game name, etc.
	cloudClient->Post(&cloudKey, bs.GetData(), bs.GetNumberOfBytesUsed(), serverGuid);
}
void GetClientSubscription(RakNet::CloudClient *cloudClient, RakNet::RakNetGUID serverGuid)
{
	RakNet::CloudQuery cloudQuery;
	cloudQuery.keys.Push(RakNet::CloudKey(CLOUD_CLIENT_PRIMARY_KEY,0),_FILE_AND_LINE_);
	cloudQuery.subscribeToResults=true; // Causes ID_CLOUD_SUBSCRIPTION_NOTIFICATION
	cloudClient->Get(&cloudQuery, serverGuid);
}
void GetServers(RakNet::CloudClient *cloudClient, RakNet::RakNetGUID serverGuid)
{
	RakNet::CloudQuery cloudQuery;
	cloudQuery.keys.Push(RakNet::CloudKey("CloudConnCount",0),_FILE_AND_LINE_); // CloudConnCount is defined at the top of CloudServerHelper.cpp
	cloudQuery.subscribeToResults=false;
	cloudClient->Get(&cloudQuery, serverGuid);
}
