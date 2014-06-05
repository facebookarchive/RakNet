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
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "RakNetTypes.h"
#include "RakSleep.h"
#include "FullyConnectedMesh2.h"
#include "ConnectionGraph2.h"
#include <assert.h>
#include "SocketLayer.h"
#include "Kbhit.h"
#include "PacketLogger.h"
#include "Gets.h"
#include "BitStream.h"

using namespace RakNet;

#define NUM_PEERS 4
RakNet::RakPeerInterface *rakPeer[NUM_PEERS];

class FullyConnectedMesh2_UserData : public FullyConnectedMesh2
{
	virtual void WriteVJCUserData(RakNet::BitStream *bsOut) {bsOut->Write(RakString("WriteVJCUserData test"));}
	virtual void WriteVJSUserData(RakNet::BitStream *bsOut, RakNetGUID userGuid) {bsOut->Write(RakString("WriteVJSUserData test, userGuid=%s", userGuid.ToString()));}
};

int main()
{
	FullyConnectedMesh2_UserData fcm2[NUM_PEERS];

	for (int i=0; i < NUM_PEERS; i++)
	{
		rakPeer[i]=RakNet::RakPeerInterface::GetInstance();
		rakPeer[i]->AttachPlugin(&fcm2[i]);
		fcm2[i].SetAutoparticipateConnections(false);
		fcm2[i].SetConnectOnNewRemoteConnection(false, "");
		RakNet::SocketDescriptor sd;
		sd.port=60000+i;
		StartupResult sr = rakPeer[i]->Startup(NUM_PEERS,&sd,1);
		RakAssert(sr==RAKNET_STARTED);
		rakPeer[i]->SetMaximumIncomingConnections(NUM_PEERS);
		rakPeer[i]->SetTimeoutTime(1000,RakNet::UNASSIGNED_SYSTEM_ADDRESS);
		printf("%i. Our guid is %s\n", i, rakPeer[i]->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString());
	}
	
	RakSleep(100);

	for (int i=1; i < NUM_PEERS; i++)
	{
		ConnectionAttemptResult car = rakPeer[i]->Connect("127.0.0.1", 60000, 0, 0 );
		RakAssert(car==CONNECTION_ATTEMPT_STARTED);
	}
	
	RakSleep(100);
	
	for (int i=1; i < NUM_PEERS; i++)
	{
		fcm2[0].StartVerifiedJoin(rakPeer[i]->GetMyGUID());
	}
	

	bool quit=false;
	RakNet::Packet *packet;
	char ch;
	while (!quit)
	{
		for (int peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
		{
			for (packet = rakPeer[peerIndex]->Receive(); packet; rakPeer[peerIndex]->DeallocatePacket(packet), packet = rakPeer[peerIndex]->Receive())
			{
				switch (packet->data[0])
				{
				case ID_FCM2_VERIFIED_JOIN_START:
					{
						printf("%s: Got ID_FCM2_VERIFIED_JOIN_START from %s. address=", rakPeer[peerIndex]->GetMyGUID().ToString(), packet->guid.ToString());
						DataStructures::List<SystemAddress> addresses;
						DataStructures::List<RakNetGUID> guids;
						DataStructures::List<BitStream*> userData;
						fcm2[peerIndex].GetVerifiedJoinRequiredProcessingList(packet->guid, addresses, guids, userData);
						for (unsigned int i=0; i < guids.Size(); i++)
						{
							printf("%s:", guids[i].ToString());
							ConnectionAttemptResult car = rakPeer[peerIndex]->Connect(addresses[i].ToString(false), addresses[i].GetPort(), 0, 0);
							switch (car)
							{
							case CONNECTION_ATTEMPT_STARTED:
								printf("CONNECTION_ATTEMPT_STARTED");
								break;
							case ALREADY_CONNECTED_TO_ENDPOINT:
								printf("ALREADY_CONNECTED_TO_ENDPOINT");
								break;
							case CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS:
								printf("CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS");
								break;
							default:
								printf("Other");
							}
							printf(" ");

							RakString userDataAsStr;
							userData[i]->Read(userDataAsStr);
							printf(userDataAsStr.C_String());
						}
						printf("\n");
					}
					break;

				case ID_FCM2_VERIFIED_JOIN_FAILED:
					printf("%s: ID_FCM2_VERIFIED_JOIN_FAILED from %s\n", rakPeer[peerIndex]->GetMyGUID().ToString(), packet->guid.ToString());
					break;

				case ID_FCM2_VERIFIED_JOIN_CAPABLE:
					{
						RakNet::BitStream bs(packet->data,packet->length,false);
						FullyConnectedMesh2::SkipToVJCUserData(&bs);
						RakString testStr;
						bs.Read(testStr);

						printf("%s: ID_FCM2_VERIFIED_JOIN_CAPABLE from %s\n", rakPeer[peerIndex]->GetMyGUID().ToString(), packet->guid.ToString());
						printf("STR: %s\n", testStr.C_String());
						fcm2[peerIndex].RespondOnVerifiedJoinCapable(packet, true, 0);
					}
					break;

				case ID_FCM2_VERIFIED_JOIN_ACCEPTED:
					{
						bool thisSystemAccepted;
						DataStructures::List<RakNetGUID> systemsAccepted;
						RakNet::BitStream additionalData;
						fcm2[peerIndex].GetVerifiedJoinAcceptedAdditionalData(packet, &thisSystemAccepted, systemsAccepted, &additionalData);
						if (thisSystemAccepted)
						{
							printf("%s: ID_FCM2_VERIFIED_JOIN_ACCEPTED from %s. systemsAccepted=", rakPeer[peerIndex]->GetMyGUID().ToString(), packet->guid.ToString());
							for (unsigned int i=0; i < systemsAccepted.Size(); i++)
								printf("%s ", systemsAccepted[i].ToString());
							printf("\n");
						}
						break;
					}

				case ID_FCM2_VERIFIED_JOIN_REJECTED:
					printf("%s: ID_FCM2_VERIFIED_JOIN_REJECTED from %s\n", rakPeer[peerIndex]->GetMyGUID().ToString(), packet->guid.ToString());
					rakPeer[peerIndex]->CloseConnection(packet->guid, true);
					break;

				default:
					printf("%s: %s from %s\n", rakPeer[peerIndex]->GetMyGUID().ToString(), PacketLogger::BaseIDTOString(packet->data[0]), packet->guid.ToString());
				}
			}
		}

		if (kbhit())
		{
			ch=getch();
			if (ch=='q' || ch=='Q')
			{
				printf("Quitting.\n");
				quit=true;
			}
		}


		RakSleep(30);
	}

	for (int i=0; i < NUM_PEERS; i++)
	{
		RakNet::RakPeerInterface::DestroyInstance(rakPeer[i]);
	}
	return 0;
}
