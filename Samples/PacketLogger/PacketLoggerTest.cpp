/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include "GetTime.h"
#include "Rand.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "Gets.h"
#include "PacketLogger.h"
#include <assert.h>
#include "Kbhit.h"

#ifdef _WIN32
#include "WindowsIncludes.h" // Sleep
#else
#include <unistd.h> // usleep
#endif

static const int NUM_PEERS=2;
RakNet::RakPeerInterface *rakPeer[NUM_PEERS];
RakNet::PacketLogger messageHandler[NUM_PEERS];
void PrintConnections(void);

int main(void)
{
	int i;
	
	for (i=0; i < NUM_PEERS; i++)
		rakPeer[i]=RakNet::RakPeerInterface::GetInstance();

	printf("Packet Logger Test.\n");
	printf("Displays all packets being sent or received.\n");
	printf("Overwrite PacketLogger::Log to render output into your own program.\n");
	printf("Difficulty: Intermediate\n\n");

	printf("Comma delimited log format:\n");
	printf("1. Send or receive,\n");
	printf("2. Raw (direct socket send) OR Ack (Acknowledgement) OR\nTms (Timestamped packet),\n");
	printf("3. Message number,\n");
	printf("4. Packet Number (Independent for send & receive).\n(Each Packet may contain multiple messages),\n");
	printf("5. Packet ID (or a string for RPC calls),\n");
	printf("6. Bits used by the message (does not include 2-4 byte RakNet header),\n");
	printf("7. Time the message is sent,\n");
	printf("8. Local System (binary IP followed by port),\n");
	printf("9. Remote System (binary IP followed by port)\n\n");

	int peerIndex;

	// Initialize the message handlers
	for (peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
	{
		rakPeer[peerIndex]->AttachPlugin(&messageHandler[peerIndex]);
		rakPeer[peerIndex]->SetMaximumIncomingConnections(NUM_PEERS);
	}

	// Initialize the peers
	for (peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
	{
		RakNet::SocketDescriptor socketDescriptor(60000+peerIndex,0);
		rakPeer[peerIndex]->Startup(NUM_PEERS, &socketDescriptor, 1);
	}

	printf("Connecting two systems...\n\n");

	messageHandler[0].LogHeader();
	
	// Connect each peer to the prior peer
	for (peerIndex=1; peerIndex < NUM_PEERS; peerIndex++)
	{
        rakPeer[peerIndex]->Connect("127.0.0.1", 60000+peerIndex-1, 0, 0);
	}

#ifdef WIN32
	Sleep(5000);
#else
	usleep(5000*1000);
#endif

	for (i=0; i < NUM_PEERS; i++)
		RakNet::RakPeerInterface::DestroyInstance(rakPeer[i]);

	printf("Press enter to continue.\n");
	char temp[256];
	Gets(temp,sizeof(temp));

	return 1;
}
