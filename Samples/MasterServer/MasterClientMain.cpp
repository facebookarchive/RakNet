/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

// This is my own internal test program for the master client but serves as a good example.
// Right now it is hardcoded to connect to 127.0.0.1.  You would run the master server first, then run this.

#include "MasterCommon.h"
#include "MasterClient.h"
#include "StringCompressor.h"
#include "BitStream.h"
#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"

#include <cstdio>
#include <cstring>
#include <stdlib.h>

#ifdef WIN32
#include <conio.h>
#else
#include "../Unix/kbhit.h"
#endif

#ifdef _WIN32
#include <conio.h>
#include <windows.h> // Sleep
#else
#include "../Unix/kbhit.h"
#include <unistd.h> // usleep
#endif

#define READCHAR(arg) gets(arg); ch=arg[0];

// remove this
#include "PacketEnumerations.h"

int main(void)
{
	printf("DEPRECIATED - Use LightweightDatabase instead\n");

	BitStream bitStream;
	char str[256];
	char ch;
	MasterClient masterClient;
	RakPeerInterface *testGameServerOrClient;
	unsigned int serverListSize, index;
	bool identiferFound;
	const char *outputString;
	int outputInt;
	Packet *p;

	// Create a fake game
	testGameServerOrClient = RakNetworkFactory::GetRakPeerInterface();
	testGameServerOrClient->Initialize(8, 60003, 0);
	testGameServerOrClient->SetMaximumIncomingConnections(8);
	testGameServerOrClient->AttachPlugin(&masterClient);

	printf("This project shows how to use the master client.\n");
	printf("The master client is used by game servers to advertise themselves.\n");
	printf("On the master server and by game clients to download a list of game servers\n");
	printf("Difficulty: Intermediate\n\n");

	if (masterClient.Connect("127.0.0.1", 60000))
		printf("Master client connecting...\n");
	else
		printf("Master client failed to start or connect.\n");

	printf("(Q)uit\n(q)uery master server\n(l)ist server\n(d)elist server\n(D)isconnect from the master server.\n(a)dd rule\n(r)emove rule\n(p)ing server list\n(C)onnect to the master server.\n(c)onnect to another game, using NAT punch-through with master server, bypassing most NATs\n(SPACE) print server list\n");
	char buff[256];
	while (1)
	{
		if (kbhit())
		{
			READCHAR(buff);
			if (ch=='Q')
				break;
			if (ch=='q')
			{
				masterClient.ClearQueryRules();
				printf("Enter query key 1/2 or enter for none: ");
				gets(str);
				masterClient.AddQueryRule(str);
				printf("Enter query key 2/2 or enter for none: ");
				gets(str);
				masterClient.AddQueryRule(str);
				masterClient.QueryMasterServer();
				printf("Server queried.  Press space to see server list.\n");
			}
			else if (ch=='l')
			{
				printf("Uploading game server.  Query to see it.\n");
				masterClient.ListServer();
			}
			else if (ch=='d')
			{
				printf("Server delisted.  Query to update our own list.\n");
				masterClient.DelistServer();
			}
			else if (ch=='D')
			{
				printf("Disconnected.\n");
				PlayerID playerId;
				testGameServerOrClient->IPToPlayerID("127.0.0.1", 60000, &playerId);
				testGameServerOrClient->CloseConnection(playerId, true, 0);
			}
			else if (ch=='C')
			{
				if (masterClient.Connect("127.0.0.1", 60000))
					printf("Master client connecting...\n");
				else
					printf("Master client failed to start or connect.\n");
			}
			else if (ch=='a')
			{
				printf("Adding sample rules.  Query to update our own list.\n");
				masterClient.PostRule("Game name", "My big game o' death.", 0);
				masterClient.PostRule("Game type", "Death match", 0);
				masterClient.PostRule("Score",0, 100);
			}
			else if (ch=='r')
			{
				printf("Removing rules. Query to update our own list.\n");
				masterClient.RemoveRule("Game type");
				masterClient.RemoveRule("Game name");
				masterClient.RemoveRule("Score");
			}
			else if (ch=='p')
			{
				printf("Pinging any servers in our list\n");
				masterClient.PingServers();
			}
			else if (ch=='c')
			{
				char ip[22];
				printf("Sending connection attempt notification to master server\n");
				printf("Enter IP of server from game list: ");
				gets(ip);
				printf("Enter port: ");
				gets(str);
				if (ip[0]!=0 && str[0]!=0)
				{
					masterClient.ConnectionAttemptNotification(ip, atoi(str));
					printf("Sent connection attempt notification to the server the master server\n");
				}
				else
				{
					printf("Aborting...");
				}				
			}
			else if (ch==' ')
			{
				serverListSize=masterClient.GetServerListSize();
				if (serverListSize==0)
				{
					printf("No servers in list\n");
				}
				else
				{
					for (index=0; index < serverListSize; index++)
					{
						printf("%i. ", index);
						outputString=masterClient.GetServerListRuleAsString(index, "IP", &identiferFound);
						if (identiferFound)
							printf("%s:", outputString);
						else
							printf("NO_IP:");
						outputInt=masterClient.GetServerListRuleAsInt(index, "Port", &identiferFound);
						if (identiferFound)
							printf("%i ", outputInt);
						else
							printf("NO_PORT ");
						outputInt=masterClient.GetServerListRuleAsInt(index, "Ping", &identiferFound);
						if (identiferFound)
							printf("%i ", outputInt);
						else
							printf("NO_PING ");
						outputString=masterClient.GetServerListRuleAsString(index, "Game type", &identiferFound);
						if (identiferFound)
							printf("%s ", outputString);
						else
							printf("NO_GT ");
						outputString=masterClient.GetServerListRuleAsString(index, "Game name", &identiferFound);
						if (identiferFound)
							printf("%s ", outputString);
						else
							printf("NO_GN ");
						outputInt=masterClient.GetServerListRuleAsInt(index, "Score", &identiferFound);
						if (identiferFound)
							printf("%i\n", outputInt);
						else
							printf("NO_SCORE\n");
					}
				}
			}
			ch=0;
		}

		p = testGameServerOrClient->Receive();
		while (p)
		{
			// Ignore any game packets.  The master server plugin handles everything.
			testGameServerOrClient->DeallocatePacket(p);
			// Call Receive every update to keep the plugin going
			p = testGameServerOrClient->Receive();
		}

#ifdef _WIN32
		Sleep(30);
#else
		usleep(30 * 1000);
#endif
	}

	masterClient.Disconnect();
	RakNetworkFactory::DestroyRakPeerInterface(testGameServerOrClient);

	return 0;
}
