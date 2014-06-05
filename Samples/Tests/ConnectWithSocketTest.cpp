/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "ConnectWithSocketTest.h"

/*
Description:
virtual bool RakPeerInterface::ConnectWithSocket  	(  	const char *   	 host, 		unsigned short  	remotePort, 		const char *  	passwordData, 		int  	passwordDataLength, 		RakNetSmartPtr< RakNetSocket >  	socket, 		unsigned  	sendConnectionAttemptCount = 7, 		unsigned  	timeBetweenSendConnectionAttemptsMS = 500, 		TimeMS  	timeoutTime = 0	  	) 	

virtual void RakPeerInterface::GetSockets  	(  	DataStructures::List< RakNetSmartPtr< RakNetSocket > > &   	 sockets  	 )   	 
virtual RakNetSmartPtr<RakNetSocket> RakPeerInterface::GetSocket  	(  	const SystemAddress   	 target  	 )   	 [pure virtual]

Success conditions:

Failure conditions:

RakPeerInterface Functions used, tested indirectly by its use:
Startup
SetMaximumIncomingConnections
Receive
DeallocatePacket
Send
IsConnected

RakPeerInterface Functions Explicitly Tested:
ConnectWithSocket
GetSockets
GetSocket

*/
int ConnectWithSocketTest::RunTest(DataStructures::List<RakString> params,bool isVerbose,bool noPauses)
{
	destroyList.Clear(false,_FILE_AND_LINE_);

	RakPeerInterface *server,*client;

	DataStructures::List< RakNetSmartPtr< RakNetSocket > > sockets;
	TestHelpers::StandardClientPrep(client,destroyList);
	TestHelpers::StandardServerPrep(server,destroyList);

	SystemAddress serverAddress;

	serverAddress.SetBinaryAddress("127.0.0.1");
	serverAddress.port=60000;

	printf("Testing normal connect before test\n");
	if (!TestHelpers::WaitAndConnectTwoPeersLocally(client,server,5000))
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[1-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 1;
	}

	TestHelpers::BroadCastTestPacket(client);

	if (!TestHelpers::WaitForTestPacket(server,5000))
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[2-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 2;
	}

	printf("Disconnecting client\n");
	CommonFunctions::DisconnectAndWait(client,"127.0.0.1",60000);

	RakNetSmartPtr<RakNetSocket> theSocket;

	client->GetSockets(sockets);

	theSocket=sockets[0];

	RakTimer timer2(5000);

	printf("Testing ConnectWithSocket using socket from GetSockets\n");
	while(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true)&&!timer2.IsExpired())
	{

		if(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))
		{
			client->ConnectWithSocket("127.0.0.1",serverAddress.port,0,0,theSocket);
		}

		RakSleep(100);

	}

	if (!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true))
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[3-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 3;
	}

	TestHelpers::BroadCastTestPacket(client);

	if (!TestHelpers::WaitForTestPacket(server,5000))
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[4-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 4;

	}

	printf("Disconnecting client\n");
	CommonFunctions::DisconnectAndWait(client,"127.0.0.1",60000);

	printf("Testing ConnectWithSocket using socket from GetSocket\n");
	theSocket=client->GetSocket(UNASSIGNED_SYSTEM_ADDRESS);//Get open Socket

	timer2.Start();

	while(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true)&&!timer2.IsExpired())
	{

		if(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))
		{
			client->ConnectWithSocket("127.0.0.1",serverAddress.port,0,0,theSocket);
		}

		RakSleep(100);

	}

	if (!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true))
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[5-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 5;
	}

	TestHelpers::BroadCastTestPacket(client);

	if (!TestHelpers::WaitForTestPacket(server,5000))
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[6-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 6;

	}

	return 0;

}

RakString ConnectWithSocketTest::GetTestName()
{

	return "ConnectWithSocketTest";

}

RakString ConnectWithSocketTest::ErrorCodeToString(int errorCode)
{

	if (errorCode>0&&(unsigned int)errorCode<=errorList.Size())
	{
		return errorList[errorCode-1];
	}
	else
	{
		return "Undefined Error";
	}	

}

ConnectWithSocketTest::ConnectWithSocketTest(void)
{
	errorList.Push("Client did not connect after 5 seconds",_FILE_AND_LINE_);
	errorList.Push("Control test send didn't work",_FILE_AND_LINE_);
	errorList.Push("Client did not connect after 5 secods Using ConnectWithSocket, could be GetSockets or ConnectWithSocket problem",_FILE_AND_LINE_);
	errorList.Push("Server did not recieve test packet from client",_FILE_AND_LINE_);
	errorList.Push("Client did not connect after 5 secods Using ConnectWithSocket, could be GetSocket or ConnectWithSocket problem",_FILE_AND_LINE_);
	errorList.Push("Server did not recieve test packet from client",_FILE_AND_LINE_);

}

ConnectWithSocketTest::~ConnectWithSocketTest(void)
{
}

void ConnectWithSocketTest::DestroyPeers()
{

	int theSize=destroyList.Size();

	for (int i=0; i < theSize; i++)
		RakPeerInterface::DestroyInstance(destroyList[i]);

}