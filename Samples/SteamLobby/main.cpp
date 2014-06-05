/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "Lobby2Client_Steam.h" // If Lobby2Client_Steam.h is included before SocketLayer.h, then it will use the steam send functions
#include "Lobby2Message_Steam.h"
#include "RakNetTime.h"
#include "RakSleep.h"
#include "RakNetTypes.h"
#include "RakPeerInterface.h"
#include "GetTime.h"
#include "MessageIdentifiers.h"
#include <windows.h>
#include <Kbhit.h>
#include "Gets.h"
#include "FullyConnectedMesh2.h"
#include "steam_api.h"

using namespace RakNet;

Lobby2MessageFactory_Steam *messageFactory;
Lobby2Client_Steam *lobby2Client;
RakNet::RakPeerInterface *rakPeer;
RakNet::FullyConnectedMesh2 *fcm2;
uint64_t lastRoom;

void PrintCommands(void)
{
	printf("a. GetRoomList\n");
	printf("b. LeaveRoom\n");
	printf("c. CreateRoom\n");
	printf("d. JoinRoom\n");
	printf("e. RefreshRoom\n");
	printf("f. SendRoomChatMessage\n");
	printf("g. ListRoomMembers\n");
	printf("?. Help\n");
	printf("(Escape). Quit\n");
}

struct SteamResults : public RakNet::Lobby2Callbacks
{
	virtual void MessageResult(RakNet::Notification_Console_MemberJoinedRoom *message)
	{
		RakNet::Notification_Console_MemberJoinedRoom_Steam *msgSteam = (RakNet::Notification_Console_MemberJoinedRoom_Steam *) message;
		RakNet::RakString msg;
		msgSteam->DebugMsg(msg);
		printf("%s\n", msg.C_String());
		// Guy with the lower ID connects to the guy with the higher ID
		uint64_t mySteamId=SteamUser()->GetSteamID().ConvertToUint64();
		if (mySteamId < msgSteam->srcMemberId)
		{
			// Steam's NAT punch is implicit, so it takes a long time to connect. Give it extra time
			unsigned int sendConnectionAttemptCount=24;
			unsigned int timeBetweenSendConnectionAttemptsMS=500;
			ConnectionAttemptResult car = rakPeer->Connect(msgSteam->remoteSystem.ToString(false), msgSteam->remoteSystem.GetPort(), 0, 0, 0, 0, sendConnectionAttemptCount, timeBetweenSendConnectionAttemptsMS);
			RakAssert(car==CONNECTION_ATTEMPT_STARTED);
		}
	}

	virtual void MessageResult(RakNet::Console_SearchRooms *message)
	{
		RakNet::Console_SearchRooms_Steam *msgSteam = (RakNet::Console_SearchRooms_Steam *) message;
		RakNet::RakString msg;
		msgSteam->DebugMsg(msg);
		printf("%s\n", msg.C_String());
		if (msgSteam->roomIds.GetSize()>0)
		{
			lastRoom=msgSteam->roomIds[0];
		}
	}

	virtual void ExecuteDefaultResult(RakNet::Lobby2Message *message)
	{
		RakNet::RakString out;
		message->DebugMsg(out);
		printf("%s\n", out.C_String());
	}
};

int main(int argc, char **argv)
{
	if (argc>1)
	{
		printf("Command arguments:\n");
		for (int i=1; i < argc; i++)
		{
			printf("%i. %s\n", i, argv[i]);
		}
	}

	SteamResults steamResults;
	rakPeer = RakNet::RakPeerInterface::GetInstance();
	fcm2 = RakNet::FullyConnectedMesh2::GetInstance();
	messageFactory = new Lobby2MessageFactory_Steam;
	lobby2Client = Lobby2Client_Steam::GetInstance();
	lobby2Client->AddCallbackInterface(&steamResults);
	lobby2Client->SetMessageFactory(messageFactory);
	SocketDescriptor sd(1234,0);
	rakPeer->Startup(32,&sd,1);
	rakPeer->SetMaximumIncomingConnections(32);
	rakPeer->AttachPlugin(fcm2);
	rakPeer->AttachPlugin(lobby2Client);
	// Connect manually in Notification_Console_MemberJoinedRoom
	fcm2->SetConnectOnNewRemoteConnection(false, "");
	RakNet::Lobby2Message* msg = messageFactory->Alloc(RakNet::L2MID_Client_Login);
	lobby2Client->SendMsg(msg);
	if (msg->resultCode!=L2RC_PROCESSING && msg->resultCode!=L2RC_SUCCESS)
	{
		printf("Steam must be running to play this game (SteamAPI_Init() failed).\n");
		printf("If this fails, steam_appid.txt was probably not in the working directory.\n");
		messageFactory->Dealloc(msg);
		return -1;
	}
	messageFactory->Dealloc(msg);
	
	PrintCommands();

	bool quit=false;
	char ch;
	while(!quit)
	{
		RakNet::Packet *packet;
		for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet=rakPeer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_DISCONNECTION_NOTIFICATION:
				// Connection lost normally
				printf("ID_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_ALREADY_CONNECTED:
				// Connection lost normally
				printf("ID_ALREADY_CONNECTED\n");
				break;
			case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_CONNECTION_LOST\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
				break;
			case ID_CONNECTION_BANNED: // Banned from this server
				printf("We are banned from this server.\n");
				break;			
			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("Connection attempt failed\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				// Sorry, the server is full.  I don't do anything here but
				// A real app should tell the user
				printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
				break;
			case ID_INVALID_PASSWORD:
				printf("ID_INVALID_PASSWORD\n");
				break;

			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				printf("ID_CONNECTION_LOST\n");
				break;

			case ID_CONNECTION_REQUEST_ACCEPTED:
				// This tells the client they have connected
				printf("ID_CONNECTION_REQUEST_ACCEPTED to %s with GUID %s\n", packet->systemAddress.ToString(), packet->guid.ToString());
				break;

			case ID_NEW_INCOMING_CONNECTION:
				printf("ID_NEW_INCOMING_CONNECTION\n");
				break;

			case ID_FCM2_NEW_HOST:
				{
					if (packet->systemAddress==RakNet::UNASSIGNED_SYSTEM_ADDRESS)
						printf("Got new host (ourselves)");
					else
						printf("Got new host %s, GUID=%s", packet->systemAddress.ToString(true), packet->guid.ToString());
					RakNet::BitStream bs(packet->data,packet->length,false);
					bs.IgnoreBytes(1);
					RakNetGUID oldHost;
					bs.Read(oldHost);
					// If the old host is different, then this message was due to losing connection to the host.
					if (oldHost!=packet->guid)
						printf(". Oldhost Guid=%s\n", oldHost.ToString());
					else
						printf("\n");
				}
				break;

			default:
				// It's a client, so just show the message
				printf("Unknown Message ID %i\n", packet->data[0]);
				break;
			}

		}
		if (kbhit())
		{
			ch=(char)getch();

			switch (ch)
			{
			case 'a':
				{

					RakNet::Lobby2Message* logoffMsg = messageFactory->Alloc(RakNet::L2MID_Console_SearchRooms);
					lobby2Client->SendMsg(logoffMsg);
					messageFactory->Dealloc(logoffMsg);
				}
				break;

			case 'b':
				{
					if (lobby2Client->GetRoomID()==0)
					{
						printf("Not in a room\n");
						break;
					}
					RakNet::Console_LeaveRoom_Steam* msg = (RakNet::Console_LeaveRoom_Steam*) messageFactory->Alloc(RakNet::L2MID_Console_LeaveRoom);
					msg->roomId=lobby2Client->GetRoomID();
					lobby2Client->SendMsg(msg);
					messageFactory->Dealloc(msg);
				}
				break;

			case 'c':
				{
					if (lobby2Client->GetRoomID()!=0)
					{
						printf("Already in a room\n");
						break;
					}

					RakNet::Console_CreateRoom_Steam* msg = (RakNet::Console_CreateRoom_Steam*) messageFactory->Alloc(RakNet::L2MID_Console_CreateRoom);
					// set the name of the lobby if it's ours
					char rgchLobbyName[256];
					msg->roomIsPublic=true;
					_snprintf( rgchLobbyName, sizeof( rgchLobbyName ), "%s's lobby", SteamFriends()->GetPersonaName() );
					msg->roomName=rgchLobbyName;
					msg->publicSlots=8;
					lobby2Client->SendMsg(msg);
					messageFactory->Dealloc(msg);

				}
				break;

			case 'd':
				{
					if (lobby2Client->GetRoomID()!=0)
					{
						printf("Already in a room\n");
						break;
					}

					RakNet::Console_JoinRoom_Steam* msg = (RakNet::Console_JoinRoom_Steam*) messageFactory->Alloc(RakNet::L2MID_Console_JoinRoom);
					printf("Enter room id, or enter for %" PRINTF_64_BIT_MODIFIER "u: ", lastRoom);
					char str[256];
					Gets(str, sizeof(str));
					if (str[0]==0)
					{
						msg->roomId=lastRoom;
					}
					else
					{
						msg->roomId=_atoi64(str);
					}
					lobby2Client->SendMsg(msg);
					messageFactory->Dealloc(msg);
				}
				break;

			case 'e':
				{
					if (lobby2Client->GetRoomID()==0)
					{
						printf("Not in a room\n");
						break;
					}

					RakNet::Console_GetRoomDetails_Steam* msg = (RakNet::Console_GetRoomDetails_Steam*) messageFactory->Alloc(RakNet::L2MID_Console_GetRoomDetails);
					msg->roomId=lobby2Client->GetRoomID();
					lobby2Client->SendMsg(msg);
					messageFactory->Dealloc(msg);
				}
				break;

			case 'f':
				{
					if (lobby2Client->GetRoomID()==0)
					{
						printf("Not in a room\n");
						break;

					}
					RakNet::Console_SendRoomChatMessage_Steam* msg = (RakNet::Console_SendRoomChatMessage_Steam*) messageFactory->Alloc(RakNet::L2MID_Console_SendRoomChatMessage);
					msg->message="Test chat message.";
					msg->roomId=lobby2Client->GetRoomID();
					lobby2Client->SendMsg(msg);
					messageFactory->Dealloc(msg);

				}
				break;

			case 'g':
				{
					DataStructures::OrderedList<uint64_t, uint64_t> roomMembers;
					lobby2Client->GetRoomMembers(roomMembers);
					for (unsigned int i=0; i < roomMembers.Size(); i++)
					{
						printf("%i. %s ID=%" PRINTF_64_BIT_MODIFIER "u\n", i+1, lobby2Client->GetRoomMemberName(roomMembers[i]), roomMembers[i]);
					}
				}
				break;


			case '?':
				{
					PrintCommands();

				}
				break;

			case 27:
				{
					quit=true;
				}
				break;
			}
		}

		RakSleep(30);
	}
	
	RakNet::Lobby2Message* logoffMsg = messageFactory->Alloc(RakNet::L2MID_Client_Logoff);
	lobby2Client->SendMsg(logoffMsg);
	messageFactory->Dealloc(logoffMsg);
	rakPeer->DetachPlugin(lobby2Client);
	rakPeer->DetachPlugin(fcm2);
	RakNet::RakPeerInterface::DestroyInstance(rakPeer);
	Lobby2Client_Steam::DestroyInstance(lobby2Client);
	RakNet::FullyConnectedMesh2::DestroyInstance(fcm2);

	return 1;
}
