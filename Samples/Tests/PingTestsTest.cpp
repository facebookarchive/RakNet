/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "PingTestsTest.h"

/*
Description:
Tests out:
virtual int 	GetAveragePing (const SystemAddress systemAddress)=0
virtual int 	GetLastPing (const SystemAddress systemAddress) const =0
virtual int 	GetLowestPing (const SystemAddress systemAddress) const =0
virtual void 	SetOccasionalPing (bool doPing)=0

Ping is tested in CrossConnectionConvertTest,SetOfflinePingResponse and GetOfflinePingResponse tested in OfflineMessagesConvertTest

Success conditions:
Currently is that GetAveragePing and SetOccasionalPing works

Failure conditions:

RakPeerInterface Functions used, tested indirectly by its use, not all encompassing list:
Startup
SetMaximumIncomingConnections
Receive
DeallocatePacket

RakPeerInterface Functions Explicitly Tested:
GetAveragePing 
GetLastPing 
GetLowestPing 
SetOccasionalPing 
*/

int PingTestsTest::RunTest(DataStructures::List<RakString> params,bool isVerbose,bool noPauses)
{

	RakPeerInterface *sender,*sender2, *receiver;
	destroyList.Clear(false,_FILE_AND_LINE_);

	TestHelpers::StandardClientPrep(sender,destroyList);

	TestHelpers::StandardClientPrep(sender2,destroyList);

	receiver=RakPeerInterface::GetInstance();
	destroyList.Push(receiver,_FILE_AND_LINE_);
	receiver->Startup(2, &SocketDescriptor(60000,0), 1);
	receiver->SetMaximumIncomingConnections(2);
	Packet * packet;

	SystemAddress currentSystem;

	currentSystem.SetBinaryAddress("127.0.0.1");
	currentSystem.port=60000;

	printf("Connecting sender2\n");
	if (!TestHelpers::WaitAndConnectTwoPeersLocally(sender2,receiver,5000))
	{

		if (isVerbose)
			DebugTools::ShowError("Could not connect after 5 seconds\n",!noPauses && isVerbose,__LINE__,__FILE__);

		return 2;

	}

	printf("Getting ping data for lastping and lowestping\n");
	sender2->SetOccasionalPing(false);//Test the lowest ping and such without  occassionalping,occasional ping comes later
	RakTimer timer(1500);

	int lastPing=0;
	int lowestPing=0;
	TimeMS nextPing=0;

	while(!timer.IsExpired())
	{
		for (packet=receiver->Receive();packet;receiver->DeallocatePacket(packet),packet=receiver->Receive())
		{
			if (isVerbose)
				printf("Receive packet id %i\n",packet->data[0]);
		}

		for (packet=sender2->Receive();packet;sender2->DeallocatePacket(packet),packet=sender2->Receive())
		{
			if (isVerbose)
				printf("Send packet id %i\n",packet->data[0]);

		}

		if (GetTimeMS()>nextPing)
		{
			sender2->Ping(currentSystem);
			nextPing=GetTimeMS()+30;
		}

		RakSleep(3);
	}

	int averagePing=sender2->GetAveragePing(currentSystem);
	if (isVerbose)
		printf("Average Ping time %i\n",averagePing);

	lastPing=sender2->GetLastPing(currentSystem);
	lowestPing=sender2->GetLowestPing(currentSystem);

	if (isVerbose)
		printf("Last Ping time %i\n",lastPing);

	if (isVerbose)
		printf("Lowest Ping time %i\n",lowestPing);

	int returnVal=TestAverageValue(averagePing,__LINE__, noPauses, isVerbose);

	if (returnVal!=0)
	{

		return returnVal;
	}

	if (lastPing>100)//100 MS for localhost?
	{
		if (isVerbose)
			DebugTools::ShowError("Problem with the last ping time,greater then 100MS for localhost\n",!noPauses && isVerbose,__LINE__,__FILE__);

		return 3;
	}

	if (lowestPing>10)//The lowest ping for localhost should drop below 10MS at least once
	{

		if (isVerbose)
			DebugTools::ShowError("The lowest ping for localhost should drop below 10MS at least once\n",!noPauses && isVerbose,__LINE__,__FILE__);

		return 4;
	}

	if (lastPing<lowestPing)
	{

		if (isVerbose)
			DebugTools::ShowError("There is a problem if the lastping is lower than the lowestping stat\n",!noPauses && isVerbose,__LINE__,__FILE__);

		return 5;
	}

	CommonFunctions::DisconnectAndWait(sender2,"127.0.0.1",60000);//Eliminate variables.

	printf("Connecting sender\n");
	if (!TestHelpers::WaitAndConnectTwoPeersLocally(sender,receiver,5000))
	{

		if (isVerbose)
			DebugTools::ShowError("Could not connect after 5 seconds\n",!noPauses && isVerbose,__LINE__,__FILE__);

		return 2;

	}

	lastPing=0;
	lowestPing=0;
	sender->SetOccasionalPing(true);

	printf("Testing SetOccasionalPing\n");

	timer.Start();
	while(!timer.IsExpired())
	{
		for (packet=receiver->Receive();packet;receiver->DeallocatePacket(packet),packet=receiver->Receive())
		{
			if (isVerbose)
				printf("Receive packet id %i\n",packet->data[0]);
		}

		for (packet=sender->Receive();packet;sender->DeallocatePacket(packet),packet=sender->Receive())
		{
			if (isVerbose)
				printf("Send packet id %i\n",packet->data[0]);

		}

		RakSleep(3);
	}

	averagePing=sender->GetAveragePing(currentSystem);
	if (isVerbose)
		printf("Average Ping time %i\n",averagePing);

	returnVal=TestAverageValue(averagePing,__LINE__, noPauses, isVerbose);

	if (returnVal!=0)
	{

		return returnVal;
	}

	return 0;

}

int PingTestsTest::TestAverageValue(int averagePing,int line,bool noPauses,bool isVerbose)
{

	if (averagePing<0)
	{

		if (isVerbose)
			DebugTools::ShowError("Problem with the average ping time,should never be less than zero in this test\n",!noPauses && isVerbose,line,__FILE__);

		return 1;

	}

	if (averagePing>10)//Average Ping should not be greater than 10MS for localhost. Command line pings typically give < 1ms
	{

		if (isVerbose)
			DebugTools::ShowError("Average Ping should not be greater than 10MS for localhost. Command line pings typically give < 1ms\n",!noPauses && isVerbose,line,__FILE__);

		return 5;

	}

	return 0;

}

RakString PingTestsTest::GetTestName()
{

	return "PingTestsTest";

}

RakString PingTestsTest::ErrorCodeToString(int errorCode)
{

	switch (errorCode)
	{

	case 0:
		return "No error";
		break;

	case 1:
		return "Problem with the average ping time,should never be less than 0 in this test";
		break;

	case 2:
		return "Could not connect after 5 seconds";
		break;

	case 3:
		return "Problem with the last ping time,greater then 100MS for localhost";
		break;

	case 4:
		return "The lowest ping for localhost should drop below 10MS at least once";
		break;

	case 5:
		return "There is a problem if the lastping is lower than the lowestping stat";
		break;

	case 6:
		return "Average Ping should not be greater than 10MS for localhost. Command line pings typically give < 1ms";
		break;

	default:
		return "Undefined Error";
	}

}

PingTestsTest::PingTestsTest(void)
{
}

PingTestsTest::~PingTestsTest(void)
{
}

void PingTestsTest::DestroyPeers()
{

	int theSize=destroyList.Size();

	for (int i=0; i < theSize; i++)
		RakPeerInterface::DestroyInstance(destroyList[i]);

}