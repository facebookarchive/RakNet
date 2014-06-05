/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "PacketAndLowLevelTestsTest.h"


/*
Description:
Tests out the sunctions:
virtual int RakPeerInterface::GetSplitMessageProgressInterval  	(  	void   	  	 )   	 const "
virtual void RakPeerInterface::PushBackPacket  	(  	Packet *   	 packet, 		bool  	pushAtHead	  	) 			"
virtual bool RakPeerInterface::SendList  	(  	char **   	 data, 		const int *  	lengths, 		const int  	numParameters, 		PacketPriority  	priority, 		PacketReliability  	reliability, 		char  	orderingChannel, 		SystemAddress  	systemAddress, 		bool  	broadcast	  	) 			"						
virtual void RakPeerInterface::SetSplitMessageProgressInterval 	( 	int  	interval 	 )  	
virtual void RakPeerInterface::SetUnreliableTimeout  	(  	TimeMS   	 timeoutMS  	 )   	 
virtual Packet* RakPeerInterface::AllocatePacket  	(  	unsigned   	 dataSize  	 )   
AttachPlugin (PluginInterface2 *plugin)=0
DetachPlugin (PluginInterface2 *plugin)=0

Success conditions:

Failure conditions:

RakPeerInterface Functions used, tested indirectly by its use,list may not be complete:
Startup
SetMaximumIncomingConnections
Receive
DeallocatePacket
Send
IsConnected
RakPeerInterface Functions Explicitly Tested:
GetSplitMessageProgressInterval
PushBackPacket
SendList
SetSplitMessageProgressInterval
AllocatePacket
GetMTUSize
AttachPlugin
DetachPlugin

*/
int PacketAndLowLevelTestsTest::RunTest(DataStructures::List<RakString> params,bool isVerbose,bool noPauses)
{
	RakPeerInterface *server,*client;
	destroyList.Clear(false,_FILE_AND_LINE_);

	TestHelpers::StandardClientPrep(client,destroyList);
	TestHelpers::StandardServerPrep(server,destroyList);
	printf("Connecting to server\n");
	if (!TestHelpers::WaitAndConnectTwoPeersLocally(client,server,5000))
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[1-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 1;
	}

	printf("Testing SendList\n");

	char* dataList2[5];
	char **dataList=(char **)dataList2;
	int lengths[5];
	char curString1[]="AAAA";
	char curString2[]="ABBB";
	char curString3[]="ACCC";
	char curString4[]="ADDD";
	char curString5[]="AEEE";

	dataList[0]=curString1;
	dataList[1]=curString2;
	dataList[2]=curString3;
	dataList[3]=curString4;
	dataList[4]=curString5;

	for (int i=0;i<5;i++)
	{
		dataList[i][0]=ID_USER_PACKET_ENUM+1+i;
		lengths[i]=5;
	}

	client->SendList((const char**)dataList,lengths,5,HIGH_PRIORITY,RELIABLE_ORDERED,0, UNASSIGNED_SYSTEM_ADDRESS, true);

	Packet* packet;
	if (!(packet=CommonFunctions::WaitAndReturnMessageWithID(server,ID_USER_PACKET_ENUM+1,1000)))
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[9-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 9;
	}

	if (packet->length!=25)
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[13],!noPauses && isVerbose,__LINE__,__FILE__);

		return 14;

	}

	server->DeallocatePacket(packet);

	PluginInterface2* myPlug=new PacketChangerPlugin();

	printf("Test attach detach of plugins\n");
	client->AttachPlugin(myPlug); 
	TestHelpers::BroadCastTestPacket(client);
	if (TestHelpers::WaitForTestPacket(server,2000))
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[2-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 2;
	}

	client->DetachPlugin(myPlug); 
	TestHelpers::BroadCastTestPacket(client);
	if (!TestHelpers::WaitForTestPacket(server,2000))
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[3-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 3;
	}

	printf("Test AllocatePacket\n");
	Packet * hugePacket,*hugePacket2;
	const int dataSize=3000000;//around 30 meg didn't want to calculate the exact
	hugePacket=client->AllocatePacket(dataSize);
	hugePacket2=client->AllocatePacket(dataSize);

	/*//Couldn't find a good cross platform way for allocated memory so skipped this check
	if (somemalloccheck<3000000)
	{}
	*/

	printf("Assuming 3000000 allocation for splitpacket, testing setsplitpacket\n");

	hugePacket->data[0]=ID_USER_PACKET_ENUM+1;
	hugePacket2->data[0]=ID_USER_PACKET_ENUM+1;

	server->SetSplitMessageProgressInterval(1);

	if (server->GetSplitMessageProgressInterval()!=1)
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[4-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 4;

	}

	if (!client->Send((const char *)hugePacket->data,dataSize,HIGH_PRIORITY,RELIABLE_ORDERED,0, UNASSIGNED_SYSTEM_ADDRESS, true))
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[5-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 5;
	}

	if (!CommonFunctions::WaitForMessageWithID(server,ID_DOWNLOAD_PROGRESS,2000))
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[6-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 6;
	}

	while(CommonFunctions::WaitForMessageWithID(server,ID_DOWNLOAD_PROGRESS,500))//Clear out the rest before next test
	{
	}

	printf("Making sure still connected, if not connect\n");
	if (!TestHelpers::WaitAndConnectTwoPeersLocally(client,server,5000))//Make sure connected before test
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[11-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 11;
	}

	printf("Making sure standard send/recieve still functioning\n");
	TestHelpers::BroadCastTestPacket(client);
	if (!TestHelpers::WaitForTestPacket(server,5000))
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[12],!noPauses && isVerbose,__LINE__,__FILE__);

		return 13;
	}

	printf("Testing PushBackPacket\n");

	server->PushBackPacket(hugePacket,false);

	if (!TestHelpers::WaitForTestPacket(server,2000))
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[7-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 7;
	}

	printf("Making sure still connected, if not connect\n");
	if (!TestHelpers::WaitAndConnectTwoPeersLocally(client,server,5000))//Make sure connected before test
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[11-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 11;
	}

	printf("Making sure standard send/recieve still functioning\n");
	TestHelpers::BroadCastTestPacket(client);
	if (!TestHelpers::WaitForTestPacket(server,2000))
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[12-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 12;
	}

	printf("PushBackPacket head true test\n");
	server->PushBackPacket(hugePacket2,true);

	if (!TestHelpers::WaitForTestPacket(server,2000))
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[10-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 10;
	}

	printf("Making sure still connected, if not connect\n");
	if (!TestHelpers::WaitAndConnectTwoPeersLocally(client,server,5000))//Make sure connected before test
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[11-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 11;
	}

	printf("Run recieve test\n");
	TestHelpers::BroadCastTestPacket(client);
	if (!TestHelpers::WaitForTestPacket(server,2000))
	{

		if (isVerbose)
			DebugTools::ShowError(errorList[12-1],!noPauses && isVerbose,__LINE__,__FILE__);

		return 12;
	}

	return 0;

}

void PacketAndLowLevelTestsTest::FloodWithHighPriority(RakPeerInterface* client)
{

	for (int i=0;i<60000;i++)
	{
		TestHelpers::BroadCastTestPacket(client,UNRELIABLE,HIGH_PRIORITY,ID_USER_PACKET_ENUM+2);
	}

}

RakString PacketAndLowLevelTestsTest::GetTestName()
{

	return "PacketAndLowLevelTestsTest";

}

RakString PacketAndLowLevelTestsTest::ErrorCodeToString(int errorCode)
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

void PacketAndLowLevelTestsTest::DestroyPeers()
{

	int theSize=destroyList.Size();

	for (int i=0; i < theSize; i++)
		RakPeerInterface::DestroyInstance(destroyList[i]);

}

PacketAndLowLevelTestsTest::PacketAndLowLevelTestsTest(void)
{

	errorList.Push("Client failed to connect to server",_FILE_AND_LINE_);
	errorList.Push("Attached plugin failed to modify packet",_FILE_AND_LINE_);
	errorList.Push("Plugin is still modifying packets after detach",_FILE_AND_LINE_);
	errorList.Push("GetSplitMessageProgressInterval returned wrong value",_FILE_AND_LINE_);
	errorList.Push("Send to server failed",_FILE_AND_LINE_);
	errorList.Push("Large packet did not split or did not properly get ID_DOWNLOAD_PROGRESS after SetSplitMessageProgressInterval is set to 1 millisecond",_FILE_AND_LINE_);
	errorList.Push("Did not recieve and put on packet made with AllocatePacket and put on recieve stack with PushBackPacket",_FILE_AND_LINE_);
	errorList.Push("Client failed to connect to server",_FILE_AND_LINE_);
	errorList.Push("Did not recieve all packets from SendList",_FILE_AND_LINE_);
	errorList.Push("Did not recieve and put on packet made with AllocatePacket and put on recieve stack with PushBackPacket",_FILE_AND_LINE_);
	errorList.Push("Client failed to connect to server",_FILE_AND_LINE_);
	errorList.Push("PushBackPacket messed up future communication",_FILE_AND_LINE_);
	errorList.Push("Send/Recieve failed",_FILE_AND_LINE_);
	errorList.Push("Recieved size incorrect",_FILE_AND_LINE_);

}

PacketAndLowLevelTestsTest::~PacketAndLowLevelTestsTest(void)
{
}