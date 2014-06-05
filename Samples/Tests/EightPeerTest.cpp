/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "EightPeerTest.h"

/*
What is being done here is having 8 peers all connect to eachother and be
connected. Then it check if they all connect. If so send data in ordered reliable mode for 100
loops.

Possible ideas for changes:
Possibly use rakpeerinterfaces GetSystemList() for number of 
connected peers instead of manually tracking. Would be slower though,
shouldn't be significant at this number but the recieve speed it part of the test.

Success conditions:
Peers connect and receive all packets in order.
No disconnections allowed in this version of the test.

Failure conditions:

If cannot connect to all peers for 20 seconds.
All packets are not recieved.
All packets are not in order.
Disconnection.
*/
int EightPeerTest::RunTest(DataStructures::List<RakString> params,bool isVerbose,bool noPauses)
{
	const int peerNum= 8;
	RakPeerInterface *peerList[peerNum];//A list of 8 peers
	int connectionAmount[peerNum];//Counter for me to keep track of connection requests and accepts
	int recievedFromList[peerNum][peerNum];//Counter for me to keep track of packets received
	int lastNumberReceivedFromList[peerNum][peerNum];//Counter for me to keep track of last recieved sequence number
	const int numPackets=100;
	Packet *packet;
	BitStream bitStream;
	destroyList.Clear(false,_FILE_AND_LINE_);

	//Initializations of the arrays
	for (int i=0;i<peerNum;i++)
	{
		peerList[i]=RakPeerInterface::GetInstance();
		destroyList.Push(peerList[i],_FILE_AND_LINE_);
		connectionAmount[i]=0;

		for (int j=0;j<peerNum;j++)
		{
			recievedFromList[i][j]=0;
			lastNumberReceivedFromList[i][j]=0;
		}

		peerList[i]->Startup(peerNum*2, &SocketDescriptor(60000+i,0), 1);
		peerList[i]->SetMaximumIncomingConnections(peerNum);

	}

	//Connect all the peers together
	for (int i=0;i<peerNum;i++)
	{
		for (int j=i+1;j<peerNum;j++)//Start at i+1 so don't connect two of the same together.
		{
			if (peerList[i]->Connect("127.0.0.1", 60000+j, 0,0)!=CONNECTION_ATTEMPT_STARTED)
			{
				if (isVerbose)
				{
					DebugTools::ShowError("Problem while calling connect. \n",!noPauses && isVerbose,__LINE__,__FILE__);

				}
				return 1;//This fails the test, don't bother going on.
			}
		}

	}

	TimeMS entryTime=GetTimeMS();//Loop entry time
	TimeMS finishTimer=GetTimeMS();
	bool initialConnectOver=false;//Our initial connect all has been done.

	for (int k=0;k<numPackets||GetTimeMS()-finishTimer<5000;)//Quit after we send 100 messages while connected, if not all connected and not failure, otherwise fail after 20 seconds and exit
	{
		bool allConnected=true;//Start true, only one failed case makes it all fail
		for (int i=0;i<peerNum;i++)//Make sure all peers are connected to eachother
		{
			if (connectionAmount[i]<peerNum-1)
			{
				allConnected=false;
			}
		}

		if (GetTimeMS()-entryTime>20000 &&!initialConnectOver &&!allConnected)//failed for 20 seconds
		{

			if (isVerbose)
				DebugTools::ShowError("Failed to connect to all peers after 20 seconds",!noPauses && isVerbose,__LINE__,__FILE__);
			return 2;
			break;
		}

		if (allConnected)
		{
			if(!initialConnectOver)
				initialConnectOver=true;
			if (k<numPackets)
			{
			for (int i=0;i<peerNum;i++)//Have all peers send a message to all peers
			{

				bitStream.Reset();

				bitStream.Write((unsigned char) (ID_USER_PACKET_ENUM+1));

				bitStream.Write(k);
				bitStream.Write(i);

				peerList[i]->Send(&bitStream, HIGH_PRIORITY, RELIABLE_ORDERED ,0, UNASSIGNED_SYSTEM_ADDRESS, true);

			}
			}
			k++;
		}

		if (k>=numPackets-3)//This is our last 3 packets, give it time to send packet and arrive on interface, 2 seconds is more than enough 
		{
			RakSleep(300);
			if (k==numPackets)
			{
				finishTimer=GetTimeMS();
			}
		}

		for (int i=0;i<peerNum;i++)//Receive for all peers
		{
			if (allConnected)//If all connected try to make the data more visually appealing by bunching it in one receive
			{
				int waittime=0;
				do
				{
					packet=peerList[i]->Receive();
					waittime++;

					if (!packet)
					{
						RakSleep(1);

					}

					if (waittime>1000)//Check for packet every millisec and if one second has passed move on, don't block execution
					{
						break;
					}
				}
				while(!packet);//For testing purposes wait for packet a little while, go if not recieved
			}
			else//Otherwise just keep recieving quickly until connected
			{
				packet=peerList[i]->Receive();
			}
			if (isVerbose)
				printf("For peer %i with %i connected peers.\n",i,connectionAmount[i]);
			while(packet)
			{
				switch (packet->data[0])
				{
				case ID_REMOTE_DISCONNECTION_NOTIFICATION:
					if (isVerbose)
					{
						printf("Another client has disconnected.\n");
						DebugTools::ShowError("Test failed.\n",!noPauses && isVerbose,__LINE__,__FILE__);
					}
					return 3;
					break;
				case ID_REMOTE_CONNECTION_LOST:
					if (isVerbose)
					{
						printf("Another client has lost the connection.\n");
						DebugTools::ShowError("Test failed.\n",!noPauses && isVerbose,__LINE__,__FILE__);           
					}
					return 3;
					break;
				case ID_REMOTE_NEW_INCOMING_CONNECTION:
					if (isVerbose)              
						printf("Another client has connected.\n");
					break;
				case ID_CONNECTION_REQUEST_ACCEPTED:
					if (isVerbose)              
						printf("Our connection request has been accepted.\n");
					connectionAmount[i]++;

					break;
				case ID_CONNECTION_ATTEMPT_FAILED:

					if (isVerbose)
						DebugTools::ShowError("A connection has failed.\n Test failed.\n",!noPauses && isVerbose,__LINE__,__FILE__);
					return 2;
					break;

				case ID_NEW_INCOMING_CONNECTION:
					if (isVerbose)              
						printf("A connection is incoming.\n");
					connectionAmount[i]++;//For this test assume connection. Test will fail if connection fails.
					break;
				case ID_NO_FREE_INCOMING_CONNECTIONS://Should not happend
					if (isVerbose)    
					{
						printf("The server is full. This shouldn't happen in this test ever.\n");

						DebugTools::ShowError("Test failed.\n",!noPauses && isVerbose,__LINE__,__FILE__);
					}
					return 2;
					break;

				case ID_ALREADY_CONNECTED:
					if (isVerbose)              
						printf("Already connected\n");//Shouldn't happen

					break;

				case ID_DISCONNECTION_NOTIFICATION:
					if (isVerbose)
					{
						printf("We have been disconnected.\n");

						DebugTools::ShowError("Test failed.\n",!noPauses && isVerbose,__LINE__,__FILE__);
					}
					return 3;
					break;
				case ID_CONNECTION_LOST:
					allConnected=false;
					connectionAmount[i]--;
					if (isVerbose)
					{
						printf("Connection lost.\n");

						DebugTools::ShowError("Test failed.\n",!noPauses && isVerbose,__LINE__,__FILE__);
					}

					return 3;

					break;
				default:

					if (packet->data[0]==ID_USER_PACKET_ENUM+1)
					{
						int thePeerNum;
						int sequenceNum;
						bitStream.Reset();
						bitStream.Write((char*)packet->data, packet->length);
						bitStream.IgnoreBits(8);
						bitStream.Read(sequenceNum);
						bitStream.Read(thePeerNum);
						if (isVerbose)
							printf("Message %i from %i\n",sequenceNum,thePeerNum );

						if (thePeerNum>=0&&thePeerNum<peerNum)
						{
							if (lastNumberReceivedFromList[i][thePeerNum]==sequenceNum)
							{
								lastNumberReceivedFromList[i][thePeerNum]++;
							}
							else
							{
								if (isVerbose)
								{
									printf("Packets out of order");
									DebugTools::ShowError("Test failed.\n",!noPauses && isVerbose,__LINE__,__FILE__);

								}
								return 4;
							}
							recievedFromList[i][thePeerNum]++;}
					}
					break;
				}
				peerList[i]->DeallocatePacket(packet);
				// Stay in the loop as long as there are more packets.
				packet = peerList[i]->Receive();
			}
		}
		RakSleep(0);//If needed for testing
	}

	for (int i=0;i<peerNum;i++)
	{

		for (int j=0;j<peerNum;j++)
		{
			if (i!=j)
			{
				if (isVerbose)
					printf("%i recieved %i packets from %i\n",i,recievedFromList[i][j],j);
				if  (recievedFromList[i][j]!=numPackets)
				{
					if (isVerbose)
					{
						printf("Not all packets recieved. it was in reliable ordered mode so that means test failed or wait time needs increasing\n");

						DebugTools::ShowError("Test failed.\n",!noPauses && isVerbose,__LINE__,__FILE__);
					}
					return 5;

				}

			}
		}
	}

	printf("All packets recieved in order,pass\n");
	return 0;

}

RakString EightPeerTest::GetTestName()
{

	return "EightPeerTest";

}

RakString EightPeerTest::ErrorCodeToString(int errorCode)
{

	switch (errorCode)
	{

	case 0:
		return "No error";
		break;

	case 1:
		return "Connect function returned failure.";
		break;

	case 2:
		return "Peers failed to connect.";
		break;

	case 3:
		return "There was a disconnection.";
		break;

	case 4:
		return "Not ordered.";
		break;

	case 5:
		return "Not reliable.";
		break;

	default:
		return "Undefined Error";
	}

}

EightPeerTest::EightPeerTest(void)
{
}

EightPeerTest::~EightPeerTest(void)
{
}

void EightPeerTest::DestroyPeers()
{

	int theSize=destroyList.Size();

	for (int i=0; i < theSize; i++)
		RakPeerInterface::DestroyInstance(destroyList[i]);

}