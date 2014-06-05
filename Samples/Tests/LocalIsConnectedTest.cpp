/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "LocalIsConnectedTest.h"

/*
Description:
Tests

IsLocalIP
SendLoopback
GetConnectionState
GetLocalIP
GetInternalID

Success conditions:
All tests pass

Failure conditions:
Any test fails

RakPeerInterface Functions used, tested indirectly by its use:
Startup
SetMaximumIncomingConnections
Receive
DeallocatePacket
Send

RakPeerInterface Functions Explicitly Tested:
IsLocalIP
SendLoopback
GetConnectionState
GetLocalIP
GetInternalID
*/
int LocalIsConnectedTest::RunTest(DataStructures::List<RakString> params,bool isVerbose,bool noPauses)
{

	RakPeerInterface *server,*client;
	destroyList.Clear(false,_FILE_AND_LINE_);

	server=RakPeerInterface::GetInstance();
	destroyList.Push(server,_FILE_AND_LINE_);
	client=RakPeerInterface::GetInstance();
	destroyList.Push(client,_FILE_AND_LINE_);

	client->Startup(1,&SocketDescriptor(),1);
	server->Startup(1,&SocketDescriptor(60000,0),1);
	server->SetMaximumIncomingConnections(1);

	SystemAddress serverAddress;

	serverAddress.SetBinaryAddress("127.0.0.1");
	serverAddress.port=60000;
	TimeMS entryTime=GetTimeMS();
	bool lastConnect=false;
	if (isVerbose)
		printf("Testing GetConnectionState\n");

	while(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true)&&GetTimeMS()-entryTime<5000)
	{

		if(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))
		{
			lastConnect=client->Connect("127.0.0.1",serverAddress.port,0,0)==CONNECTION_ATTEMPT_STARTED;
		}

		RakSleep(100);

	}

	if (!lastConnect)//Use thise method to only check if the connect function fails, detecting connected client is done next
	{
		if (isVerbose)
			DebugTools::ShowError("Client could not connect after 5 seconds\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 1;
	}

	if(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true))
	{
		if (isVerbose)
			DebugTools::ShowError("IsConnected did not detect connected client",!noPauses && isVerbose,__LINE__,__FILE__);
		return 2;
	}
	client->CloseConnection (serverAddress,true,0,LOW_PRIORITY); 

	if(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,false,false,true))
	{
		DebugTools::ShowError("IsConnected did not detect disconnecting client",!noPauses && isVerbose,__LINE__,__FILE__);
		return 3;
	}

	RakSleep(1000);
	client->Connect("127.0.0.1",serverAddress.port,0,0);

	if(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true))
	{
		DebugTools::ShowError("IsConnected did not detect connecting client",!noPauses && isVerbose,__LINE__,__FILE__);

		return 4;

	}

	entryTime=GetTimeMS();

	while(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true)&&GetTimeMS()-entryTime<5000)
	{

		if(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))
		{
			client->Connect("127.0.0.1",serverAddress.port,0,0);
		}

		RakSleep(100);

	}

	if (!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true))
	{
		if (isVerbose)
			DebugTools::ShowError("Client could not connect after 5 seconds\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 1;
	}

	if (isVerbose)
		printf("Testing IsLocalIP\n");

	if (!client->IsLocalIP("127.0.0.1"))
	{
		if (isVerbose)
			DebugTools::ShowError("IsLocalIP failed test\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 5;
	}

	if (isVerbose)
		printf("Testing SendLoopback\n");
	char str[]="AAAAAAAAAA";
	str[0]=(char)(ID_USER_PACKET_ENUM+1);
	client->SendLoopback(str, (int) strlen(str)+1);
	client->SendLoopback(str, (int) strlen(str)+1);
	client->SendLoopback(str, (int) strlen(str)+1);
	client->SendLoopback(str, (int) strlen(str)+1);
	client->SendLoopback(str, (int) strlen(str)+1);
	client->SendLoopback(str, (int) strlen(str)+1);
	client->SendLoopback(str, (int) strlen(str)+1);

	bool recievedPacket=false;
	Packet *packet;

	TimeMS	stopWaiting = GetTimeMS() + 1000;
	while (GetTimeMS()<stopWaiting)
	{

		for (packet=client->Receive(); packet; client->DeallocatePacket(packet), packet=client->Receive())
		{

			if (packet->data[0]==ID_USER_PACKET_ENUM+1)
			{

				recievedPacket=true;

			}
		}

	}

	if (!recievedPacket)
	{
		if (isVerbose)
			DebugTools::ShowError("SendLoopback failed test\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 6;
	}

	if (isVerbose)
		printf("Testing GetLocalIP\n");
	const char * localIp=client->GetLocalIP(0);

	if (!client->IsLocalIP(localIp))
	{
		if (isVerbose)
			DebugTools::ShowError("GetLocalIP failed test\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 7;
	}

	if (isVerbose)
		printf("Testing GetInternalID\n");

	SystemAddress localAddress=client->GetInternalID();

	char convertedIp[39];

	sprintf(convertedIp,"%d.%d.%d.%d",  ((localAddress.binaryAddress >> (24 - 8 * 3)) & 0xFF),((localAddress.binaryAddress >> (24 - 16)) & 0xFF),((localAddress.binaryAddress >> (24 - 8 )) & 0xFF),((localAddress.binaryAddress >> (24)) & 0xFF));

	printf("GetInternalID returned %s\n",convertedIp);

	if (!client->IsLocalIP(convertedIp))
	{
		if (isVerbose)
			DebugTools::ShowError("GetInternalID failed test\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 8;
	}

	return 0;

}

RakString LocalIsConnectedTest::GetTestName()
{

	return "LocalIsConnectedTest";

}

RakString LocalIsConnectedTest::ErrorCodeToString(int errorCode)
{

	switch (errorCode)
	{

	case 0:
		return "No error";
		break;

	case 1:
		return "Client could not connect after 5 seconds";
		break;
	case 2:
		return "IsConnected did not detect connected client";
		break;
	case 3:
		return "IsConnected did not detect disconnecting client";
		break;
	case 4:
		return "IsConnected did not detect connecting client";
		break;

	case 5:
		return "IsLocalIP failed test";
		break;

	case 6:
		return "Sendloopback failed test";
		break;

	case 7:
		return "GetLocalIP failed test";
		break;

	case 8:
		return "GetInternalID failed test";
		break;

	default:
		return "Undefined Error";
	}

}

LocalIsConnectedTest::LocalIsConnectedTest(void)
{
}

LocalIsConnectedTest::~LocalIsConnectedTest(void)
{
}

void LocalIsConnectedTest::DestroyPeers()
{

	int theSize=destroyList.Size();

	for (int i=0; i < theSize; i++)
		RakPeerInterface::DestroyInstance(destroyList[i]);

}