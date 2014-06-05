/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "DroppedConnectionConvertTest.h"

/*
Description:

Tests silently dropping multiple instances of RakNet. This is used to test that lost connections are detected properly.

Randomly tests the timout detections to see if the connections are dropped.

Success conditions:
Clients connect and reconnect normally and do not have an extra connection.
Random timout detection passes.

Failure conditions:
Client has more than one connection.
Client unable to reconnect.
Connect function fails and there is no pending operation and there is no current connection with server.
Random timout detection fails.

*/

static const int NUMBER_OF_CLIENTS=9;

int DroppedConnectionConvertTest::RunTest(DataStructures::List<RakString> params,bool isVerbose,bool noPauses)
{

	RakPeerInterface *server;
	RakPeerInterface *clients[NUMBER_OF_CLIENTS];
	unsigned index, connectionCount;
	SystemAddress serverID;
	Packet *p;
	unsigned short numberOfSystems;
	unsigned short numberOfSystems2;
	int sender;

	// Buffer for input (an ugly hack to keep *nix happy)
	//	char buff[256];

	// Used to refer to systems.  We already know the IP
	unsigned short serverPort = 20000;
	serverID.binaryAddress=inet_addr("127.0.0.1");
	serverID.port=serverPort;

	server=RakPeerInterface::GetInstance();
	destroyList.Clear(false,_FILE_AND_LINE_);
	destroyList.Push(server,_FILE_AND_LINE_);
	//	server->InitializeSecurity(0,0,0,0);
	SocketDescriptor socketDescriptor(serverPort,0);
	server->Startup(NUMBER_OF_CLIENTS, &socketDescriptor, 1);
	server->SetMaximumIncomingConnections(NUMBER_OF_CLIENTS);
	server->SetTimeoutTime(2000,UNASSIGNED_SYSTEM_ADDRESS);

	for (index=0; index < NUMBER_OF_CLIENTS; index++)
	{
		clients[index]=RakPeerInterface::GetInstance();
		destroyList.Push(clients[index],_FILE_AND_LINE_);
		SocketDescriptor socketDescriptor2(serverPort+1+index,0);
		clients[index]->Startup(1, &socketDescriptor2, 1);
		if (clients[index]->Connect("127.0.0.1", serverPort, 0, 0)!=CONNECTION_ATTEMPT_STARTED)
		{
			DebugTools::ShowError("Connect function failed.",!noPauses && isVerbose,__LINE__,__FILE__);
			return 2;

		}
		clients[index]->SetTimeoutTime(5000,UNASSIGNED_SYSTEM_ADDRESS);

		RakSleep(1000);
		if (isVerbose)
			printf("%i. ", index);
	}

	TimeMS entryTime=GetTimeMS();//Loop entry time

	int seed = 12345;
	if (isVerbose)
		printf("Using seed %i\n", seed);
	seedMT(seed);//specify seed to keep execution path the same.

	int randomTest;

	bool dropTest=false;
	RakTimer timeoutWaitTimer(1000);

	while (GetTimeMS()-entryTime<30000)//run for 30 seconds.
	{
		// User input

		randomTest=randomMT() %4;

		if(dropTest)
		{

			server->GetConnectionList(0, &numberOfSystems);
			numberOfSystems2=numberOfSystems;

			connectionCount=0;
			for (index=0; index < NUMBER_OF_CLIENTS; index++)
			{
				clients[index]->GetConnectionList(0, &numberOfSystems);
				if (numberOfSystems>1)
				{
					if (isVerbose)
					{
						printf("Client %i has %i connections\n", index, numberOfSystems);
						DebugTools::ShowError("Client has more than one connection",!noPauses && isVerbose,__LINE__,__FILE__);
						return 1;
					}

				}
				if (numberOfSystems==1)
				{
					connectionCount++;
				}
			}

			if (connectionCount!=numberOfSystems2)
			{
				if (isVerbose)
					DebugTools::ShowError("Timeout on dropped clients not detected",!noPauses && isVerbose,__LINE__,__FILE__);
				return 3;
			}

		}
		dropTest=false;

		switch(randomTest)
		{

		case 0:
			{
				index = randomMT() % NUMBER_OF_CLIENTS;

				clients[index]->GetConnectionList(0, &numberOfSystems);
				clients[index]->CloseConnection(serverID, false,0);
				if (numberOfSystems==0)
				{
					if (isVerbose)
						printf("Client %i silently closing inactive connection.\n",index);
				}
				else
				{
					if (isVerbose)
						printf("Client %i silently closing active connection.\n",index);
				}
			}

			break;
		case 1:
			{
				index = randomMT() % NUMBER_OF_CLIENTS;

				clients[index]->GetConnectionList(0, &numberOfSystems);

				if(!CommonFunctions::ConnectionStateMatchesOptions (clients[index],serverID,true,true,true,true) )//Are we connected or is there a pending operation ?
				{
					if (clients[index]->Connect("127.0.0.1", serverPort, 0, 0)!=CONNECTION_ATTEMPT_STARTED)
					{

						DebugTools::ShowError("Connect function failed.",!noPauses && isVerbose,__LINE__,__FILE__);
						return 2;

					}
				}
				if (numberOfSystems==0)
				{
					if (isVerbose)
						printf("Client %i connecting to same existing connection.\n",index);

				}
				else
				{
					if (isVerbose)
						printf("Client %i connecting to closed connection.\n",index);
				}
			}

			break;
		case 2:
			{

				if (isVerbose)
					printf("Randomly connecting and disconnecting each client\n");
				for (index=0; index < NUMBER_OF_CLIENTS; index++)
				{
					if (NUMBER_OF_CLIENTS==1 || (randomMT()%2)==0)
					{
						if (clients[index]->IsActive())
						{

							int randomTest2=randomMT() %2;
							if (randomTest2)
								clients[index]->CloseConnection(serverID, false, 0);
							else
								clients[index]->CloseConnection(serverID, true, 0);
						}
					}
					else
					{
						if(!CommonFunctions::ConnectionStateMatchesOptions (clients[index],serverID,true,true,true,true) )//Are we connected or is there a pending operation ?
						{
							if (clients[index]->Connect("127.0.0.1", serverPort, 0, 0)!=CONNECTION_ATTEMPT_STARTED)
							{
								DebugTools::ShowError("Connect function failed.",!noPauses && isVerbose,__LINE__,__FILE__);
								return 2;

							}
						}
					}
				}
			}
			break;

		case 3:
			{

				if (isVerbose)
					printf("Testing if clients dropped after timeout.\n");
				timeoutWaitTimer.Start();
						//Wait half the timeout time, the other half after receive so we don't drop all connections only missing ones, Active ait so the threads run on linux
				while (!timeoutWaitTimer.IsExpired())
				{
				RakSleep(50);
				}
				dropTest=true;

			}
			break;
		default:
			// Ignore anything else
			break;
		}

		server->GetConnectionList(0, &numberOfSystems);
		numberOfSystems2=numberOfSystems;
		if (isVerbose)
			printf("The server thinks %i clients are connected.\n", numberOfSystems);
		connectionCount=0;
		for (index=0; index < NUMBER_OF_CLIENTS; index++)
		{
			clients[index]->GetConnectionList(0, &numberOfSystems);
			if (numberOfSystems>1)
			{
				if (isVerbose)
				{
					printf("Client %i has %i connections\n", index, numberOfSystems);
					DebugTools::ShowError("Client has more than one connection",!noPauses && isVerbose,__LINE__,__FILE__);
					return 1;
				}

			}
			if (numberOfSystems==1)
			{
				connectionCount++;
			}
		}

		if (isVerbose)
			printf("%i clients are actually connected.\n", connectionCount);
		if (isVerbose)
			printf("server->NumberOfConnections==%i.\n", server->NumberOfConnections());

		//}

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
					if (isVerbose)
						printf("%i: %ID_CONNECTION_REQUEST_ACCEPTED from %i.\n",sender, p->systemAddress.port);
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					// Connection lost normally
					if (isVerbose)
						printf("%i: ID_DISCONNECTION_NOTIFICATION from %i.\n",sender, p->systemAddress.port);
					break;

				case ID_NEW_INCOMING_CONNECTION:
					// Somebody connected.  We have their IP now
					if (isVerbose)
						printf("%i: ID_NEW_INCOMING_CONNECTION from %i.\n",sender, p->systemAddress.port);
					break;


				case ID_CONNECTION_LOST:
					// Couldn't deliver a reliable packet - i.e. the other system was abnormally
					// terminated
					if (isVerbose)
						printf("%i: ID_CONNECTION_LOST from %i.\n",sender, p->systemAddress.port);
					break;

				case ID_NO_FREE_INCOMING_CONNECTIONS:
					if (isVerbose)
						printf("%i: ID_NO_FREE_INCOMING_CONNECTIONS from %i.\n",sender, p->systemAddress.port);
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
		if (dropTest)
		{
			//Trigger the timeout if no recieve
			timeoutWaitTimer.Start();
			while (!timeoutWaitTimer.IsExpired())
			{
			RakSleep(50);
			}
		}
		// 11/29/05 - No longer necessary since I added the keepalive
		/*
		// Have everyone send a reliable packet so dropped connections are noticed.
		ch=255;
		server->Send((char*)&ch, 1, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);

		for (index=0; index < NUMBER_OF_CLIENTS; index++)
		clients[index]->Send((char*)&ch, 1, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
		*/

		// Sleep so this loop doesn't take up all the CPU time

		RakSleep(10);

	}

	return 0;
}

RakString DroppedConnectionConvertTest::GetTestName()
{
	return "DroppedConnectionConvertTest";

}

RakString DroppedConnectionConvertTest::ErrorCodeToString(int errorCode)
{

	switch (errorCode)
	{

	case 0:
		return "No error";
		break;

	case 1:
		return "Client has more than one connection";
		break;

	case 2:
		return "Connect failed";
		break;

	case 3:
		return "Timeout not detected";
		break;

	default:
		return "Undefined Error";
	}

}

void DroppedConnectionConvertTest::DestroyPeers()
{

	int theSize=destroyList.Size();

	for (int i=0; i < theSize; i++)
		RakPeerInterface::DestroyInstance(destroyList[i]);

}

DroppedConnectionConvertTest::DroppedConnectionConvertTest(void)
{
}

DroppedConnectionConvertTest::~DroppedConnectionConvertTest(void)
{
}
