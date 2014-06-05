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
#include "BitStream.h"
#include "TwoWayAuthentication.h"
#include "RakSleep.h"

static const int NUM_PEERS=2;
RakNet::RakPeerInterface *rakPeer[NUM_PEERS];
RakNet::TwoWayAuthentication *twoWayAuthenticationPlugin[NUM_PEERS];
int main(void)
{
	int i;

	for (i=0; i < NUM_PEERS; i++)
		rakPeer[i]=RakNet::RakPeerInterface::GetInstance();

	printf("This project tests and demonstrates the two way authentication plugin.\n");
	printf("Difficulty: Beginner\n\n");

	int peerIndex;

	// Initialize the message handlers
	for (peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
	{
		twoWayAuthenticationPlugin[peerIndex]=RakNet::OP_NEW<RakNet::TwoWayAuthentication>(_FILE_AND_LINE_);
		rakPeer[peerIndex]->AttachPlugin(twoWayAuthenticationPlugin[peerIndex]);
		rakPeer[peerIndex]->SetMaximumIncomingConnections(NUM_PEERS);
	}

	// Initialize the peers
	for (peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
	{
		RakNet::SocketDescriptor socketDescriptor(60000+peerIndex,0);
		rakPeer[peerIndex]->Startup(NUM_PEERS, &socketDescriptor, 1);
	}
	
	// Connect each peer to the prior peer
	for (peerIndex=1; peerIndex < NUM_PEERS; peerIndex++)
	{
        rakPeer[peerIndex]->Connect("127.0.0.1", 60000+peerIndex-1, 0, 0);
	}

	RakSleep(100);


	printf("Peers initialized and connected.\n");
	twoWayAuthenticationPlugin[0]->AddPassword("PWD0", "Password0");
	twoWayAuthenticationPlugin[0]->AddPassword("PWD1", "Password1");
	twoWayAuthenticationPlugin[1]->AddPassword("PWD0", "Password0");
	bool b = twoWayAuthenticationPlugin[0]->Challenge("PWD0", rakPeer[0]->GetGUIDFromIndex(0));
	RakAssert(b);
	printf("Stage 0, instance 0 challenges instance 1 (should pass)\n");
	int stage=0;
	int stage4FailureCount=0;
	int passCount=0;
	bool quit=false;

	while (!quit)
	{
		RakNet::Packet *packet;
		for (peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
		{
			packet=rakPeer[peerIndex]->Receive();
			if (packet)
			{
				switch (packet->data[0])
				{
					case ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_SUCCESS:
					case ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_SUCCESS:
					{
						RakNet::BitStream bs(packet->data, packet->length, false);
						bs.IgnoreBytes(sizeof(RakNet::MessageID));
						RakNet::RakString password;
						bs.Read(password);
						if (packet->data[0]==ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_SUCCESS)
							printf("ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_SUCCESS with %s from %s\n", password.C_String(), packet->systemAddress.ToString(true));
						else
							printf("ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_SUCCESS with %s from %s\n", password.C_String(), packet->systemAddress.ToString(true));
						if (++passCount==2)
						{
							if (stage<=2)
							{
								if (stage==0)
								{
									printf("Stage 1, instance 1 challenges instance 0 (should pass)\n");
									twoWayAuthenticationPlugin[1]->Challenge("PWD0", rakPeer[1]->GetGUIDFromIndex(0));
									passCount=0;
									// stage==1
								}
								else if (stage==1)
								{
									printf("Stage 2, instance 1 re-challenges instance 0 (should pass)\n");
									twoWayAuthenticationPlugin[1]->Challenge("PWD0", rakPeer[1]->GetGUIDFromIndex(0));
									passCount=0;
									// stage==2
								}
								else
								{
									printf("Stage 3, instance 0 challenges with bad password (call should fail)\n");
									if (twoWayAuthenticationPlugin[0]->Challenge("PWD3", rakPeer[0]->GetGUIDFromIndex(0))==false)
									{
										printf("Passed stage 3\n");
										stage++;
										printf("Stage 4, instance 0 challenges with unknown password (should fail twice)\n");
										twoWayAuthenticationPlugin[0]->Challenge("PWD1", rakPeer[0]->GetGUIDFromIndex(0));
									}
									else
									{
										printf("Failed stage 3, Challenge() did not return false\n");
									}
								}
								stage++;
							}
						}
						
					}
					break;
					case ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_FAILURE:
					{
						printf("ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_FAILED from %s\n", packet->systemAddress.ToString(true));
						if (stage!=4)
						{
							printf("Failed stage %i\n", stage);
						}
						else
						{
							if (++stage4FailureCount==2)
								quit=true;
						}
					}
					break;
					case ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_TIMEOUT:
					{
						RakNet::BitStream bs(packet->data, packet->length, false);
						bs.IgnoreBytes(sizeof(RakNet::MessageID));
						RakNet::RakString password;
						bs.Read(password);
						printf("ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_TIMEOUT with %s from %s\n", password.C_String(), packet->systemAddress.ToString(true));
						printf("Failed stage %i\n", stage);
					}
					break;
					case ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_FAILURE:
					{
						RakNet::BitStream bs(packet->data, packet->length, false);
						bs.IgnoreBytes(sizeof(RakNet::MessageID));
						RakNet::RakString password;
						bs.Read(password);
						printf("ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_FAILED with %s from %s\n", password.C_String(), packet->systemAddress.ToString(true));
						if (stage!=4)
						{
							printf("Failed stage %i\n", stage);
						}
						else
						{
							if (++stage4FailureCount==2)
								quit=true;
						}
					}
					break;
				}
				rakPeer[peerIndex]->DeallocatePacket(packet);
			}
		}

		RakSleep(30);
	}


	for (i=0; i < NUM_PEERS; i++)
		RakNet::RakPeerInterface::DestroyInstance(rakPeer[i]);

	for (peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
		RakNet::OP_DELETE(twoWayAuthenticationPlugin[peerIndex], _FILE_AND_LINE_);

	return 1;
}
