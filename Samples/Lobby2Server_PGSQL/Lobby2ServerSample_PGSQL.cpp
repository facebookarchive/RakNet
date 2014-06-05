/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "Lobby2Message.h"
#include "RakPeerInterface.h"

#include "MessageIdentifiers.h"
#include "Kbhit.h"
#include "RakSleep.h"
#include "Lobby2Server_PGSQL.h"
#include "Lobby2Message_PGSQL.h"
#include "ProfanityFilter.h"
#include <ctype.h>
#include <stdlib.h>
#include "Gets.h"


#ifdef __INTEGRATE_LOBBY2_WITH_ROOMS_PLUGIN
#include "RoomsPlugin.h"
#endif

//#define _ALSO_ACT_AS_NAT_PUNCH_SERVER
#include "NatPunchthroughServer.h"
#include "UDPProxyCoordinator.h"
#include "UDPProxyServer.h"
#include "NatTypeDetectionServer.h"
#include "SocketLayer.h"
static const char *COORDINATOR_PASSWORD="Dummy Coordinator Password";
#ifdef _ALSO_ACT_AS_NAT_PUNCH_SERVER

#endif

void main(void)
{
	printf("The 2nd interation of the lobby server.\n");
	printf("Difficulty: Intermediate\n\n");

	char serverPort[30];
	RakNet::RakPeerInterface *rakPeer=RakNet::RakPeerInterface::GetInstance();
	rakPeer->SetTimeoutTime(5000,RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	//rakPeer->SetTimeoutTime(3000,RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	puts("Enter the rakPeer port to listen on");
	serverPort[0]=0;
	Gets(serverPort,sizeof(serverPort));
	if (serverPort[0]==0)
		strcpy(serverPort, "61111");

	RakNet::SocketDescriptor socketDescriptor(atoi(serverPort),0);
	rakPeer->SetMaximumIncomingConnections(32);
	if (rakPeer->Startup(32,&socketDescriptor, 1)!=RakNet::RAKNET_STARTED)
	{
		printf("Startup call failed\n");
		return;
	}
	else
		printf("Started on port %i\n", socketDescriptor.port);
	// Attach the plugin Lobby2Server
	// The class factory will create messages with server functionality
	RakNet::Lobby2Server_PGSQL lobby2Server;
	rakPeer->AttachPlugin(&lobby2Server);
	RakNet::Lobby2MessageFactory_PGSQL messageFactory;
	lobby2Server.SetMessageFactory(&messageFactory);

	// This is optional:
#ifdef __INTEGRATE_LOBBY2_WITH_ROOMS_PLUGIN
	RakNet::RoomsPlugin roomsPluginServer;
	rakPeer->AttachPlugin(&roomsPluginServer);
	lobby2Server.SetRoomsPlugin(&roomsPluginServer);
	RakNet::ProfanityFilter profanityFilter;
	profanityFilter.AddWord("Penis");
	roomsPluginServer.SetProfanityFilter(&profanityFilter);
	roomsPluginServer.roomsContainer.AddTitle("Test Title Name");
#endif

	printf("Enter database password:\n");
	char connectionString[256],password[128];
	char username[256];
	strcpy(username, "postgres");
	password[0]=0;
	Gets(password,sizeof(password));
	if (password[0]==0) strcpy(password, "aaaa");
	strcpy(connectionString, "user=");
	strcat(connectionString, username);
	strcat(connectionString, " password=");
	strcat(connectionString, password);

	if (lobby2Server.ConnectToDB(connectionString, 4)==false)
	{
		printf("Database connection failed\n");
		return;
	}

	printf("Lobby2Server started and waiting for connections\n");


	RakNet::Lobby2Server::ConfigurationProperties c;
	c.requiresEmailAddressValidationToLogin=false;
	c.requiresTitleToLogin=true;
	c.accountRegistrationRequiresCDKey=false;
	c.accountRegistrationRequiredAgeYears=0;
	lobby2Server.SetConfigurationProperties(c);

#ifdef _ALSO_ACT_AS_NAT_PUNCH_SERVER
	RakNet::NatPunchthroughServer natPunchthroughServer;
	RakNet::UDPProxyCoordinator udpProxyCoordinator;
	RakNet::UDPProxyServer udpProxyServer;
	RakNet::NatTypeDetectionServer natTypeDetectionServer;
	udpProxyCoordinator.SetRemoteLoginPassword(COORDINATOR_PASSWORD);
	rakPeer->AttachPlugin(&natPunchthroughServer);
	rakPeer->AttachPlugin(&udpProxyServer);
	rakPeer->AttachPlugin(&udpProxyCoordinator);
	rakPeer->AttachPlugin(&natTypeDetectionServer);
	char ipListStr[ MAXIMUM_NUMBER_OF_INTERNAL_IDS ][ 128 ];
	RakNet::SystemAddress ipList[ MAXIMUM_NUMBER_OF_INTERNAL_IDS ];
	for (int i=0; i < MAXIMUM_NUMBER_OF_INTERNAL_IDS; i++)
		ipList[i].ToString(false,ipListStr[i]);
	RakNet::SocketLayer::GetMyIP( ipList );
	natTypeDetectionServer.Startup(ipListStr[1], ipListStr[2], ipListStr[3]);
	// Login proxy server to proxy coordinator
	// Normally the proxy server is on a different computer. Here, we login to our own IP address since the plugin is on the same system

	// This makes it take high CPU usage, comment out of not wanted
	udpProxyServer.LoginToCoordinator(COORDINATOR_PASSWORD, rakPeer->GetMyBoundAddress());
#endif

	RakNet::Packet *packet;
	// Loop for input
	while (1)
	{
		for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet=rakPeer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_DISCONNECTION_NOTIFICATION:
				// Connection lost normally
				printf("ID_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_NEW_INCOMING_CONNECTION:
				// Connection lost normally
				printf("ID_NEW_INCOMING_CONNECTION\n");
				printf("Allowing all operations from remote client for testing (insecure)\n");
				printf("Use Lobby2Server::ExecuteCommand for local server operations\n");
				// For this test, allow all operations
				lobby2Server.AddAdminAddress(packet->systemAddress);
				lobby2Server.AddRankingAddress(packet->systemAddress);
				break;
			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				printf("ID_CONNECTION_LOST\n");
				break;
			}
		}

		// This sleep keeps RakNet responsive
		RakSleep(30);

		//printf("%i ", lobby2Server.GetUsers().Size());
	}

	RakNet::RakPeerInterface::DestroyInstance(rakPeer);
}
