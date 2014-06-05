/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

/// \file
/// \brief Tests multiple readers and writers on the same instance of RakPeer.


#include "RakPeerInterface.h"

#include "GetTime.h"
#include "RakNetStatistics.h"
#include "MessageIdentifiers.h"
#include "Kbhit.h"
#include <stdio.h> // Printf
#include "WindowsIncludes.h" // Sleep
//#include <process.h>
#include "RakThread.h"
#include "RakSleep.h"
using namespace RakNet;

RakPeerInterface *peer1, *peer2;
bool endThreads;

RAK_THREAD_DECLARATION(ProducerThread)
{
	char i = *((char *) arguments);
	char out[2];
	out[0]=(char) ID_USER_PACKET_ENUM;
	out[1]=i;

	while (endThreads==false)
	{
//		printf("Thread %i writing...\n", i);
		if (i&1)
			peer1->Send(out, 2, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
		else
			peer2->Send(out, 2, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

//		printf("Thread %i done writing\n", i);
		RakSleep(30);
	}
	
	return 0;
}

RAK_THREAD_DECLARATION(ConsumerThread)
{
	char i = *((char *) arguments);
	RakNet::Packet *p;
	while (endThreads==false)
	{
//		printf("Thread %i reading...\n", i);
		if (i&1)
			p=peer1->Receive();
		else
			p=peer2->Receive();
	//	printf("Thread %i done reading...\n", i);

		if (p)
		{
			if (p->data[0]==ID_USER_PACKET_ENUM)
				printf("Got data from thread %i\n", p->data[1]);
			if (i&1)
				peer1->DeallocatePacket(p);
			else
				peer2->DeallocatePacket(p);
		}

        RakSleep(30);		
	}

	return 0;
}

int main()
{
	peer1=RakNet::RakPeerInterface::GetInstance();
	peer2=RakNet::RakPeerInterface::GetInstance();
	peer1->SetMaximumIncomingConnections(1);
	peer2->SetMaximumIncomingConnections(1);
	RakNet::SocketDescriptor socketDescriptor(1234,0);
	peer1->Startup(1,&socketDescriptor, 1);
	socketDescriptor.port=1235;
	peer2->Startup(1,&socketDescriptor, 1);
	RakSleep(500);
	peer1->Connect("127.0.0.1", 1235, 0, 0);
	peer2->Connect("127.0.0.1", 1234, 0, 0);

	printf("Tests multiple threads sharing the same instance of RakPeer\n");
	printf("Difficulty: Beginner\n\n");



	endThreads=false;
	unsigned i;
	char count[20];
	printf("Starting threads\n");
	for (i=0; i< 10; i++)
	{
		count[i]=i;
		RakNet::RakThread::Create(&ProducerThread, count+i);
	}
	for (; i < 20; i++)
	{
		count[i]=i;
		RakNet::RakThread::Create(&ConsumerThread, count+i );
	}

	printf("Running test\n");
	RakNet::TimeMS endTime = 60 * 1000 + RakNet::GetTimeMS();
	while (RakNet::GetTimeMS() < endTime)
	{

		RakSleep(0);
	}
	endThreads=true;
	printf("Test done!\n");

	RakNet::RakPeerInterface::DestroyInstance(peer1);
	RakNet::RakPeerInterface::DestroyInstance(peer2);

	return 0;
}
