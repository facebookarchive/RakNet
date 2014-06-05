/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "PeerConnectDisconnectTest.h"

void PeerConnectDisconnectTest::WaitForConnectionRequestsToComplete(RakPeerInterface **peerList, int peerNum, bool isVerbose)
{
	SystemAddress currentSystem;
	bool msgWasPrinted=false;

	for (int i=0;i<peerNum;i++)
	{
		for (int j=i+1;j<peerNum;j++)//Start at i+1 so don't connect two of the same together.
		{
			currentSystem.SetBinaryAddress("127.0.0.1");
			currentSystem.port=60000+j;

			while (CommonFunctions::ConnectionStateMatchesOptions (peerList[i],currentSystem,false,true,true) )
			{
				if (msgWasPrinted==false)
				{
					printf("Waiting for connection requests to complete.\n");
					msgWasPrinted=true;
				}

				RakSleep(30);
			}
		}
	}
}
void PeerConnectDisconnectTest::WaitAndPrintResults(RakPeerInterface **peerList, int peerNum, bool isVerbose)
{
	WaitForConnectionRequestsToComplete(peerList,peerNum,isVerbose);

	Packet *packet;

	// Log all events per peer
	for (int i=0;i<peerNum;i++)//Receive for all peers
	{
		if (isVerbose)
			printf("For peer %i\n",i);

		for (packet=peerList[i]->Receive(); packet; peerList[i]->DeallocatePacket(packet), packet=peerList[i]->Receive())
		{
			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				if (isVerbose)
					printf("Another client has disconnected.\n");

				break;
			case ID_REMOTE_CONNECTION_LOST:
				if (isVerbose)
					printf("Another client has lost the connection.\n");

				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				if (isVerbose)              
					printf("Another client has connected.\n");
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				if (isVerbose)              
					printf("Our connection request has been accepted.\n");

				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				if (isVerbose)
					printf("A connection has failed.\n");

				break;

			case ID_NEW_INCOMING_CONNECTION:
				if (isVerbose)              
					printf("A connection is incoming.\n");

				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				if (isVerbose)              
					printf("The server is full.\n");

				break;

			case ID_ALREADY_CONNECTED:
				if (isVerbose)              
					printf("Already connected\n");

				break;

			case ID_DISCONNECTION_NOTIFICATION:
				if (isVerbose)
					printf("We have been disconnected.\n");
				break;
			case ID_CONNECTION_LOST:
				if (isVerbose)
					printf("Connection lost.\n");

				break;
			default:

				break;
			}
		}
	}

}

/*
What is being done here is having 8 peers all connect to eachother, disconnect, connect again.

Do this for about 10 seconds. Then allow them all to connect for one last time.

Good ideas for changes:
After the last check run a eightpeers like test an add the conditions
of that test as well.

Make sure that if we initiate the connection we get a proper message
and if not we get a proper message. Add proper conditions.

Randomize sending the disconnect notes

Success conditions:
All connected normally.

Failure conditions:
Doesn't reconnect normally.

During the very first connect loop any connect returns false.

Connect function returns false and peer is not connected to anything.

*/
int PeerConnectDisconnectTest::RunTest(DataStructures::List<RakString> params,bool isVerbose,bool noPauses)
{

	const int peerNum= 8;
	const int maxConnections=peerNum*3;//Max allowed connections for test set to times 3 to eliminate problem variables
	RakPeerInterface *peerList[peerNum];//A list of 8 peers

	SystemAddress currentSystem;

	destroyList.Clear(false,_FILE_AND_LINE_);

	//Initializations of the arrays
	for (int i=0;i<peerNum;i++)
	{
		peerList[i]=RakPeerInterface::GetInstance();
		destroyList.Push(peerList[i],_FILE_AND_LINE_);

		peerList[i]->Startup(maxConnections, &SocketDescriptor(60000+i,0), 1);
		peerList[i]->SetMaximumIncomingConnections(maxConnections);

	}

	//Connect all the peers together

	for (int i=0;i<peerNum;i++)
	{

		for (int j=i+1;j<peerNum;j++)//Start at i+1 so don't connect two of the same together.
		{

			if (peerList[i]->Connect("127.0.0.1", 60000+j, 0,0)!=CONNECTION_ATTEMPT_STARTED)
			{

				if (isVerbose)
					DebugTools::ShowError("Problem while calling connect.\n",!noPauses && isVerbose,__LINE__,__FILE__);

				return 1;//This fails the test, don't bother going on.

			}

		}	

	}

	TimeMS entryTime=GetTimeMS();//Loop entry time

	DataStructures::List< SystemAddress  > systemList;
	DataStructures::List< RakNetGUID > guidList;

	printf("Entering disconnect loop \n");

	while(GetTimeMS()-entryTime<10000)//Run for 10 Secoonds
	{

		//Disconnect all peers IF connected to any
		for (int i=0;i<peerNum;i++)
		{

			peerList[i]->GetSystemList(systemList,guidList);//Get connectionlist
			int len=systemList.Size();

			for (int j=0;j<len;j++)//Disconnect them all
			{

				peerList[i]->CloseConnection (systemList[j],true,0,LOW_PRIORITY); 	
			}

		}

		RakSleep(100);

		//Connect

		for (int i=0;i<peerNum;i++)
		{

			for (int j=i+1;j<peerNum;j++)//Start at i+1 so don't connect two of the same together.
			{

				currentSystem.SetBinaryAddress("127.0.0.1");
				currentSystem.port=60000+j;
				if(!CommonFunctions::ConnectionStateMatchesOptions (peerList[i],currentSystem,true,true,true,true) )//Are we connected or is there a pending operation ?
				{

					if (peerList[i]->Connect("127.0.0.1", 60000+j, 0,0)!=CONNECTION_ATTEMPT_STARTED)
					{

						if (isVerbose)
							DebugTools::ShowError("Problem while calling connect.\n",!noPauses && isVerbose,__LINE__,__FILE__);

						return 1;//This fails the test, don't bother going on.

					}
				}

			}	

		}

		WaitAndPrintResults(peerList,peerNum,isVerbose);

	}

	WaitAndPrintResults(peerList,peerNum,isVerbose);

	printf("Connecting peers\n");

	//Connect

	for (int i=0;i<peerNum;i++)
	{

		for (int j=i+1;j<peerNum;j++)//Start at i+1 so don't connect two of the same together.
		{
			currentSystem.SetBinaryAddress("127.0.0.1");
			currentSystem.port=60000+j;

			if(!CommonFunctions::ConnectionStateMatchesOptions (peerList[i],currentSystem,true,true,true,true) )//Are we connected or is there a pending operation ?
			{
				printf("Calling Connect() for peer %i to peer %i.\n",i,j);

				if (peerList[i]->Connect("127.0.0.1", 60000+j, 0,0)!=CONNECTION_ATTEMPT_STARTED)
				{
					peerList[i]->GetSystemList(systemList,guidList);//Get connectionlist
					int len=systemList.Size();

					if (isVerbose)
						DebugTools::ShowError("Problem while calling connect.\n",!noPauses && isVerbose,__LINE__,__FILE__);

					return 1;//This fails the test, don't bother going on.

				}
			}
			else
			{
				if (CommonFunctions::ConnectionStateMatchesOptions (peerList[i],currentSystem,false,false,false,true)==false)
					printf("Not calling Connect() for peer %i to peer %i because it is disconnecting.\n",i,j);
				else if (CommonFunctions::ConnectionStateMatchesOptions (peerList[i],currentSystem,false,true,true)==false)
					printf("Not calling Connect() for peer %i to peer %i because it is connecting.\n",i,j);
				else if (CommonFunctions::ConnectionStateMatchesOptions (peerList[i],currentSystem,true)==false)
					printf("Not calling Connect() for peer %i to peer %i because it is connected).\n",i,j);
			}
		}	

	}

	WaitAndPrintResults(peerList,peerNum,isVerbose);

	for (int i=0;i<peerNum;i++)
	{

		peerList[i]->GetSystemList(systemList,guidList);
		int connNum=guidList.Size();//Get the number of connections for the current peer
		if (connNum!=peerNum-1)//Did we connect to all?
		{

			if (isVerbose)
			{
				printf("Not all peers reconnected normally.\nFailed on peer number %i with %i peers\n",i,connNum);

				DebugTools::ShowError("",!noPauses && isVerbose,__LINE__,__FILE__);
			}

			return 2;

		}

	}

	

	if (isVerbose)
		printf("Pass\n");
	return 0;

}

RakString PeerConnectDisconnectTest::GetTestName()
{

	return "PeerConnectDisconnectTest";

}

RakString PeerConnectDisconnectTest::ErrorCodeToString(int errorCode)
{

	switch (errorCode)
	{

	case 0:
		return "No error";
		break;

	case 1:
		return "The connect function failed.";
		break;

	case 2:
		return "Peers did not connect normally.";
		break;

	default:
		return "Undefined Error";
	}

}

PeerConnectDisconnectTest::PeerConnectDisconnectTest(void)
{
}

PeerConnectDisconnectTest::~PeerConnectDisconnectTest(void)
{
}
void PeerConnectDisconnectTest::DestroyPeers()
{

	int theSize=destroyList.Size();

	for (int i=0; i < theSize; i++)
		RakPeerInterface::DestroyInstance(destroyList[i]);

}