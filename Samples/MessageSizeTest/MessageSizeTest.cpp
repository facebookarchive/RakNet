/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "RakPeerInterface.h"
#include "GetTime.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include <cstdio>
#include <memory.h>
#include <cstring>
#include <stdlib.h>
#include "Rand.h"
#include "RakNetStatistics.h"
#include "RakSleep.h"
#include "RakMemoryOverride.h"

using namespace RakNet;

int main(int argc, char **argv)
{
	RakPeerInterface *sender, *receiver;

	printf("This project tests sending messages of various sizes.\n");
	sender = RakNet::RakPeerInterface::GetInstance();
	receiver = RakNet::RakPeerInterface::GetInstance();
	RakNet::SocketDescriptor sd1(1234,0),sd2(1235,0);
	receiver->Startup(32, &sd1, 1);
	receiver->SetMaximumIncomingConnections(32);
	sender->Startup(1, &sd2, 1);
	sender->Connect("127.0.0.1", 1234, 0, 0);
	RakSleep(100);

	unsigned char data[4000];
	data[0]=ID_USER_PACKET_ENUM;
	for (unsigned int i=1; i < 4000; i++)
		data[i]=i%256;
	int stride, sum;
	int sendCount, receiveCount;
	for (stride=1; stride < 2000; stride++)
	{
		sendCount=0;
		receiveCount=0;
		for (sum=0; sum < 4000; sum+=stride)
		{
			sender->Send((const char*) data,stride,HIGH_PRIORITY,RELIABLE_ORDERED,0,RakNet::UNASSIGNED_SYSTEM_ADDRESS,true);
			sendCount++;
		}

		RakNet::Packet *p;
		for (p=sender->Receive(); p; sender->DeallocatePacket(p), p=sender->Receive())
			;

		RakNet::Time timeout=RakNet::GetTime()+1000;
		while (RakNet::GetTime()<timeout)
		{
			for (p=receiver->Receive(); p; receiver->DeallocatePacket(p), p=receiver->Receive())
			{
				if (p->data[0]==ID_USER_PACKET_ENUM)
				{
					receiveCount++;
				}
				for (unsigned int i=1; i < p->length; i++)
				{
					RakAssert(data[i]==i%256);
				}
			}
			RakSleep(30);
			if (receiveCount==sendCount)
				break;
		}

		if (sendCount==receiveCount)
			printf("Stride=%i Sends=%i Receives=%i\n", stride, sendCount, receiveCount);
		else
			printf("ERROR! Stride=%i Sends=%i Receives=%i\n", stride, sendCount, receiveCount);
	}

	if (sender)
		RakNet::RakPeerInterface::DestroyInstance(sender);
	if (receiver)
		RakNet::RakPeerInterface::DestroyInstance(receiver);

	return 1;
}