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
#include "RakSleep.h"
#include "RelayPlugin.h"
#include "Gets.h"
#include "Kbhit.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"

using namespace RakNet;

int main(void)
{
	printf("Tests the RelayPlugin as a server.\n");
	printf("Difficulty: Beginner\n\n");

	char str[64], str2[64];

	RakNet::RakPeerInterface *peer=RakNet::RakPeerInterface::GetInstance();
	RelayPlugin *relayPlugin = RelayPlugin::GetInstance();
	peer->AttachPlugin(relayPlugin);

	// Get our input
	char ip[64], serverPort[30], listenPort[30];
	puts("Enter the port to listen on");
	Gets(listenPort,sizeof(listenPort));
	if (listenPort[0]==0)
		strcpy(listenPort, "1234");

	relayPlugin->SetAcceptAddParticipantRequests(true);

	// Connecting the client is very simple.  0 means we don't care about
	// a connectionValidationInteger, and false for low priority threads
	RakNet::SocketDescriptor socketDescriptor(atoi(listenPort),0);
	socketDescriptor.socketFamily=AF_INET;
	peer->Startup(8,&socketDescriptor, 1);
	peer->SetMaximumIncomingConnections(8);
	peer->SetOccasionalPing(true);

	puts("Enter IP to connect to, or enter for none");
	Gets(ip, sizeof(ip));
	peer->AllowConnectionResponseIPMigration(false);
	if (ip[0])
	{
		puts("Enter the port to connect to");
		Gets(serverPort,sizeof(serverPort));
		if (serverPort[0]==0)
			strcpy(serverPort, "1234");

		RakNet::ConnectionAttemptResult car = peer->Connect(ip, atoi(serverPort), 0, 0);
		RakAssert(car==RakNet::CONNECTION_ATTEMPT_STARTED);
	}

	peer->SetTimeoutTime(30000, UNASSIGNED_SYSTEM_ADDRESS);
	peer->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS).ToString(str);
	printf("My GUID is %s\n", str);

	printf("(A)ddParticipantRequestFromClient\n");
	printf("(R)emoveParticipantRequestFromClient\n");
	printf("SendTo(P)articipant\n");
	printf("(S)endGroupMessage\n");
	printf("(J)oinGroupRequest\n");
	printf("(L)eaveGroup\n");
	printf("(G)etGroupList\n");
	printf("(Q)uit\n");

	char name[128];
	while (1)
	{
		if (kbhit())
		{
			char ch = getch();
			if (ch=='a' || ch=='A')
			{
				printf("Enter name of participant: ");
				Gets(name, sizeof(name));
				if (name[0])
				{
					relayPlugin->AddParticipantRequestFromClient(name,peer->GetGUIDFromIndex(0));
					printf("Done\n");
				}
				else
				{
					printf("Operation aborted\n");
				}

			}
			else if (ch=='r' || ch=='R')
			{
				relayPlugin->RemoveParticipantRequestFromClient(peer->GetGUIDFromIndex(0));
				printf("Done\n");
			}
			else if (ch=='p' || ch=='P')
			{
				char name[128];
				printf("Enter name of participant: ");
				Gets(name, sizeof(name));
				if (name[0])
				{
					printf("Enter message to send: ");
					char msg[256];
					Gets(msg, sizeof(msg));
					RakString msgRs = msg;
					BitStream msgBs;
					msgBs.WriteCompressed(msgRs);
					relayPlugin->SendToParticipant(peer->GetGUIDFromIndex(0), name, &msgBs, HIGH_PRIORITY, RELIABLE_ORDERED, 0 );
					printf("Done\n");
				}
				else
				{
					printf("Operation aborted\n");
				}
			}
			else if (ch=='s' || ch=='S')
			{
				printf("Enter message to send: ");
				char msg[256];
				Gets(msg, sizeof(msg));
				RakString msgRs = msg;
				BitStream msgBs;
				msgBs.Write(msgRs);
				relayPlugin->SendGroupMessage(peer->GetGUIDFromIndex(0), &msgBs, HIGH_PRIORITY, RELIABLE_ORDERED, 0 );
				printf("Done\n");
			}
			else if (ch=='j' || ch=='J')
			{
				printf("Enter group name to join: ");
				char msg[256];
				Gets(msg, sizeof(msg));
				relayPlugin->JoinGroupRequest(peer->GetGUIDFromIndex(0), msg);
				printf("Done\n");
			}
			else if (ch=='l' || ch=='l')
			{
				relayPlugin->LeaveGroup(peer->GetGUIDFromIndex(0));
				printf("Done\n");
			}
			else if (ch=='g' || ch=='G')
			{
				relayPlugin->GetGroupList(peer->GetGUIDFromIndex(0));
				printf("Done\n");
			}
			else if (ch=='q')
			{
				break;
			}
		}

		Packet *packet;
		for (packet=peer->Receive(); packet; peer->DeallocatePacket(packet), packet=peer->Receive())
		{
			packet->guid.ToString(str);
			packet->systemAddress.ToString(true,str2);
			if (packet->data[0]==ID_NEW_INCOMING_CONNECTION)
			{
				printf("ID_NEW_INCOMING_CONNECTION from %s on %s\n", str, str2);
			}
			else if (packet->data[0]==ID_CONNECTION_REQUEST_ACCEPTED)
			{
				printf("ID_CONNECTION_REQUEST_ACCEPTED from %s on %s\n", str, str2);
			}
			else if (packet->data[0]==ID_CONNECTION_LOST)
			{
				printf("ID_CONNECTION_LOST from %s on %s\n", str, str2);
			}
			else if (packet->data[0]==ID_RELAY_PLUGIN)
			{
				BitStream msgRs;
				RakString senderRs;
				BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(MessageID));
				RelayPluginEnums rpe;
				bsIn.ReadCasted<MessageID>(rpe);
				switch (rpe)
				{
				case RPE_MESSAGE_TO_CLIENT_FROM_SERVER:
					{
						RakString senderName;
						bsIn.ReadCompressed(senderName);
						bsIn.AlignReadToByteBoundary();
						RakString dataInAsStr;
						bsIn.ReadCompressed(dataInAsStr);
						printf("RPE_MESSAGE_TO_CLIENT_FROM_SERVER from %s, data=%s\n", senderName.C_String(), dataInAsStr.C_String());
					}
					break;
				case RPE_ADD_CLIENT_NOT_ALLOWED:
					{
						RakString senderName;
						bsIn.ReadCompressed(senderName);
						printf("RPE_ADD_CLIENT_NOT_ALLOWED for %s\n", senderName.C_String());
					}
					break;
				case RPE_ADD_CLIENT_TARGET_NOT_CONNECTED:
					{
						RakString senderName;
						bsIn.ReadCompressed(senderName);
						printf("RPE_ADD_CLIENT_TARGET_NOT_CONNECTED for %s\n", senderName.C_String());
					}
					break;
				case RPE_ADD_CLIENT_NAME_ALREADY_IN_USE:
					{
						RakString senderName;
						bsIn.ReadCompressed(senderName);
						printf("RPE_ADD_CLIENT_NAME_ALREADY_IN_USE for %s\n", senderName.C_String());
					}
					break;
				case RPE_ADD_CLIENT_SUCCESS:
					{
						RakString senderName;
						bsIn.ReadCompressed(senderName);
						printf("RPE_ADD_CLIENT_SUCCESS for %s\n", senderName.C_String());
					}
					break;
				case RPE_USER_ENTERED_ROOM:
					{
						RakString whichUser;
						bsIn.ReadCompressed(whichUser);
						printf("RPE_USER_ENTERED_ROOM user=%s\n", whichUser.C_String());
					}
					break;
				case RPE_USER_LEFT_ROOM:
					{
						RakString whichUser;
						bsIn.ReadCompressed(whichUser);
						printf("RPE_USER_LEFT_ROOM user=%s\n", whichUser.C_String());
					}
					break;
				case RPE_GROUP_MSG_FROM_SERVER:
					{
						RakString senderName;
						bsIn.ReadCompressed(senderName);
						bsIn.AlignReadToByteBoundary();
						RakString dataInAsStr;
						bsIn.Read(dataInAsStr);
						printf("RPE_GROUP_MSG_FROM_SERVER from %s, data=%s\n", senderName.C_String(), dataInAsStr.C_String());
					}
					break;
				case RPE_GET_GROUP_LIST_REPLY_FROM_SERVER:
					{
						uint16_t chatRoomsSize, usersInRoomSize;
						bsIn.Read(chatRoomsSize);
						RakString roomName;
						printf("RPE_GET_GROUP_LIST_REPLY_FROM_SERVER %i rooms\n", chatRoomsSize);
						for (uint16_t chatRoomsIdx=0; chatRoomsIdx < chatRoomsSize; chatRoomsIdx++)
						{
							bsIn.ReadCompressed(roomName);
							bsIn.Read(usersInRoomSize);
							printf("%i. %s %i users\n", chatRoomsIdx+1, roomName.C_String(), usersInRoomSize);
						}
					}
					break;
				default:
					{
						RakAssert(0);
					}
				}
			}
		}

		RakSleep(30);
	}
	

	return 1;
}

