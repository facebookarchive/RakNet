/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "ReliableOrderedConvertedTest.h"

FILE *fp;
int memoryUsage=0;

char lastError[512];

void* ReliableOrderedConvertedTest::LoggedMalloc(size_t size, const char *file, unsigned int line)
{
	memoryUsage+=(int)size;
	if (fp)
		fprintf(fp,"Alloc %s:%i %i bytes %i total\n", file,line,size,memoryUsage);
	char *p = (char*) malloc(size+sizeof(size));
	memcpy(p,&size,sizeof(size));
	return p+sizeof(size);
}
void ReliableOrderedConvertedTest::LoggedFree(void *p, const char *file, unsigned int line)
{
	char *realP=(char*)p-sizeof(size_t);
	size_t allocatedSize;
	memcpy(&allocatedSize,realP,sizeof(size_t));
	memoryUsage-=(int)allocatedSize;
	if (fp)
		fprintf(fp,"Free %s:%i %i bytes %i total\n", file,line,allocatedSize,memoryUsage);
	free(realP);
}
void* ReliableOrderedConvertedTest::LoggedRealloc(void *p, size_t size, const char *file, unsigned int line)
{
	char *realP=(char*)p-sizeof(size_t);
	size_t allocatedSize;
	memcpy(&allocatedSize,realP,sizeof(size_t));
	memoryUsage-=(int)allocatedSize;
	memoryUsage+=(int)size;
	p = realloc(realP,size+sizeof(size));
	memcpy(p,&size,sizeof(size));
	if (fp)
		fprintf(fp,"Realloc %s:%i %i to %i bytes %i total\n", file,line,allocatedSize,size,memoryUsage);
	return (char*)p+sizeof(size);
}

/*
What is being done here is having a server connect to a client.

Packets are sent at 30 millisecond intervals for 12 seconds.

Length and sequence are checked for each packet.

Success conditions:
All packets are correctly recieved in order.

Failure conditions:

All packets are not  correctly recieved.
All packets are not in order.

*/

int ReliableOrderedConvertedTest::RunTest(DataStructures::List<RakString> params,bool isVerbose,bool noPauses)
{

	RakPeerInterface *sender, *receiver;
	unsigned int packetNumberSender[32],packetNumberReceiver[32], receivedPacketNumberReceiver, receivedTimeReceiver;
	char str[256];
	char ip[32];
	TimeMS sendInterval, nextSend, currentTime, quitTime;
	unsigned short remotePort, localPort;
	unsigned char streamNumberSender,streamNumberReceiver;
	BitStream bitStream;
	Packet *packet;
	bool doSend=false;

	for (int i=0; i < 32; i++)
	{
		packetNumberSender[i]=0;
		packetNumberReceiver[i]=0;

	}

	/*
	if (argc==2)
	{
	fp = fopen(argv[1],"wt");
	SetMalloc_Ex(LoggedMalloc);
	SetRealloc_Ex(LoggedRealloc);
	SetFree_Ex(LoggedFree);
	}
	else
	*/
	fp=0;
	destroyList.Clear(false,_FILE_AND_LINE_);

	sender =RakPeerInterface::GetInstance();
	destroyList.Push(	sender ,_FILE_AND_LINE_);
	//sender->ApplyNetworkSimulator(.02, 100, 50);

	/*
	if (str[0]==0)
	sendInterval=30;
	else
	sendInterval=atoi(str);*///possible future params

	sendInterval=30;

	/*
	printf("Enter remote IP: ");
	Gets(ip, sizeof(ip));
	if (ip[0]==0)*/
	strcpy(ip, "127.0.0.1");

	/*
	printf("Enter remote port: ");
	Gets(str, sizeof(str));
	if (str[0]==0)*/
	strcpy(str, "60000");
	remotePort=atoi(str);
	/*
	printf("Enter local port: ");
	Gets(str, sizeof(str));
	if (str[0]==0)*/
	strcpy(str, "0");
	localPort=atoi(str);

	if (isVerbose)
		printf("Connecting...\n");

	sender->Startup(1, &SocketDescriptor(localPort,0), 1);
	sender->Connect(ip, remotePort, 0, 0);

	receiver =RakPeerInterface::GetInstance();
	destroyList.Push(	receiver ,_FILE_AND_LINE_);

	/*
	printf("Enter local port: ");
	Gets(str, sizeof(str));
	if (str[0]==0)*/
	strcpy(str, "60000");
	localPort=atoi(str);

	if (isVerbose)
		printf("Waiting for connections...\n");

	receiver->Startup(32, &SocketDescriptor(localPort,0), 1);
	receiver->SetMaximumIncomingConnections(32);

	//	if (sender)
	//		sender->ApplyNetworkSimulator(128000, 50, 100);
	//	if (receiver)
	//		receiver->ApplyNetworkSimulator(128000, 50, 100);

	/*printf("How long to run this test for, in seconds?\n");
	Gets(str, sizeof(str));
	if (str[0]==0)*/
	strcpy(str, "12");

	currentTime = GetTimeMS();
	quitTime = atoi(str) * 1000 + currentTime;

	nextSend=currentTime;

	while (currentTime < quitTime)
		//while (1)
	{

		packet = sender->Receive();
		while (packet)
		{
			// PARSE TYPES
			switch(packet->data[0])
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
				if (isVerbose)
					printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
				doSend=true;
				nextSend=currentTime;
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				if (isVerbose)
					printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				if (isVerbose)
					printf("ID_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_CONNECTION_LOST:
				if (isVerbose)
					printf("ID_CONNECTION_LOST\n");
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				if (isVerbose)
					printf("Connection attempt failed\n");
				break;
			}

			sender->DeallocatePacket(packet);
			packet = sender->Receive();
		}

		while (doSend && currentTime > nextSend)
		{
			streamNumberSender=0;
			//	streamNumber = randomMT() % 32;
			// Do the send
			bitStream.Reset();
			bitStream.Write((unsigned char) (ID_USER_PACKET_ENUM+1));
			bitStream.Write(packetNumberSender[streamNumberSender]++);
			bitStream.Write(streamNumberSender);
			bitStream.Write(currentTime);
			char *pad;
			int padLength = (randomMT() % 5000) + 1;
			pad = new char [padLength];
			bitStream.Write(pad, padLength);
			delete [] pad;
			// Send on a random priority with a random stream
			// if (sender->Send(&bitStream, HIGH_PRIORITY, (PacketReliability) (RELIABLE + (randomMT() %2)) ,streamNumber, UNASSIGNED_SYSTEM_ADDRESS, true)==false)
			if (sender->Send(&bitStream, HIGH_PRIORITY, RELIABLE_ORDERED ,streamNumberSender, UNASSIGNED_SYSTEM_ADDRESS, true)==false)
				packetNumberSender[streamNumberSender]--; // Didn't finish connecting yet?

			RakNetStatistics *rssSender;
			rssSender=sender->GetStatistics(sender->GetSystemAddressFromIndex(0));
			if (isVerbose)
				printf("Snd: %i.\n", packetNumberSender[streamNumberSender]);

			nextSend+=sendInterval;

			// Test halting
			//	if (rand()%20==0)
			//		nextSend+=1000;
		}

		packet = receiver->Receive();
		while (packet)
		{
			switch(packet->data[0])
			{
			case ID_NEW_INCOMING_CONNECTION:
				if (isVerbose)
					printf("ID_NEW_INCOMING_CONNECTION\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				if (isVerbose)
					printf("ID_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_CONNECTION_LOST:
				if (isVerbose)
					printf("ID_CONNECTION_LOST\n");
				break;
			case ID_USER_PACKET_ENUM+1:
				bitStream.Reset();
				bitStream.Write((char*)packet->data, packet->length);
				bitStream.IgnoreBits(8); // Ignore ID_USER_PACKET_ENUM+1
				bitStream.Read(receivedPacketNumberReceiver);
				bitStream.Read(streamNumberReceiver);
				bitStream.Read(receivedTimeReceiver);

				if (receivedPacketNumberReceiver!=packetNumberReceiver[streamNumberReceiver])
				{

					//WARNING: If you modify the below code make sure the whole string remains in bounds, sprintf will NOT do it for you. 
					//The error string is 512 in length

					//Note: Removed buffer checking because chance is insignificant, left code if wanted in future. Needs limits.h ISO C standard.

					/*
					int maxIntWorkingCopy= INT_MAX;

					int maxIntCharLen =0; 

					while (maxIntWorkingCopy>0)
					{maxIntCharLen++;
					maxIntWorkingCopy/=10;
					}

					if (strlen(lastError)>maxIntCharLen* 3 +27)//512 should be a good len for now
					{*/

					sprintf(lastError,"Expecting %i got %i (channel %i).",packetNumberReceiver[streamNumberReceiver], receivedPacketNumberReceiver, streamNumberReceiver);

					/*
					}
					else
					{
					sprintf(lastError,"Did not get what was expected. More details can be given if the error string buffer size is increased.");

					}*/

					if (isVerbose)
					{

						RakNetStatistics *rssSender,*rssReceiver;
						char message[2048];

						rssSender=sender->GetStatistics(sender->GetSystemAddressFromIndex(0));

						rssReceiver=receiver->GetStatistics(receiver->GetSystemAddressFromIndex(0));
						StatisticsToString(rssSender, message, 2);
						printf("Server stats %s\n", message);
						StatisticsToString(rssReceiver, message, 2);
						printf("Client stats%s", message);

						DebugTools::ShowError(lastError,!noPauses && isVerbose,__LINE__,__FILE__);
					}

					return 1;
				}
				else
					if (isVerbose)
					{
						printf("Got %i.Channel %i.Len %i.", packetNumberReceiver[streamNumberReceiver], streamNumberReceiver, packet->length);

						printf("Sent=%u Received=%u Diff=%i.\n", receivedTimeReceiver, currentTime, (int)currentTime - (int) receivedTimeReceiver);
					}

					packetNumberReceiver[streamNumberReceiver]++;
					break;
			}

			receiver->DeallocatePacket(packet);
			packet = receiver->Receive();
		}

		RakSleep(0);

		currentTime=GetTimeMS();
	}

	if (isVerbose)
	{

		RakNetStatistics *rssSender,*rssReceiver;
		char message[2048];

		rssSender=sender->GetStatistics(sender->GetSystemAddressFromIndex(0));

		rssReceiver=receiver->GetStatistics(receiver->GetSystemAddressFromIndex(0));
		StatisticsToString(rssSender, message, 2);
		printf("Server stats %s\n", message);
		StatisticsToString(rssReceiver, message, 2);
		printf("Client stats%s", message);
	}

	if (fp)
		fclose(fp);

	return 0;
}

RakString ReliableOrderedConvertedTest::GetTestName()
{

	return "ReliableOrderedConvertedTest";

}

RakString ReliableOrderedConvertedTest::ErrorCodeToString(int errorCode)
{

	RakString returnString;

	switch (errorCode)
	{

	case 0:
		return "No error";
		break;

	case 1:
		returnString= "The very last error for this object was ";
		returnString+=lastError;
		return returnString;
		break;

	default:
		return "Undefined Error";
	}
}

ReliableOrderedConvertedTest::ReliableOrderedConvertedTest(void)
{
}

ReliableOrderedConvertedTest::~ReliableOrderedConvertedTest(void)
{
}
void ReliableOrderedConvertedTest::DestroyPeers()
{

	int theSize=destroyList.Size();

	for (int i=0; i < theSize; i++)
		RakPeerInterface::DestroyInstance(destroyList[i]);

}
