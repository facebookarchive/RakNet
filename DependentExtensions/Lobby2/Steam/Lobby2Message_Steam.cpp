/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "Lobby2Message_Steam.h"
#include "steam_api.h"
#include "Lobby2Client_Steam_Impl.h"

using namespace RakNet;

bool Client_Login_Steam::ClientImpl( RakNet::Lobby2Plugin *client)
{
	(void) client;

	if ( !SteamAPI_Init() )
		resultCode=L2RC_GENERAL_ERROR;
	else
		resultCode=L2RC_SUCCESS;
	return true; // Done immediately
}
bool Client_Logoff_Steam::ClientImpl( RakNet::Lobby2Plugin *client)
{
	Lobby2Client_Steam_Impl *steam = (Lobby2Client_Steam_Impl *)client;
	steam->NotifyLeaveRoom();

	resultCode=L2RC_SUCCESS;
	SteamAPI_Shutdown();

	return true; // Done immediately
}
Console_SearchRooms_Steam::Console_SearchRooms_Steam()
{
	m_SteamCallResultLobbyMatchList = RakNet::OP_NEW<CCallResult<Lobby2Client_Steam_Impl, LobbyMatchList_t> > (_FILE_AND_LINE_);
}
Console_SearchRooms_Steam::~Console_SearchRooms_Steam()
{
	// Cast to make sure destructor gets called
	RakNet::OP_DELETE((CCallResult<Lobby2Client_Steam_Impl, LobbyMatchList_t>*)m_SteamCallResultLobbyMatchList, _FILE_AND_LINE_);
}
bool Console_SearchRooms_Steam::ClientImpl( RakNet::Lobby2Plugin *client)
{
	(void) client;

	requestId = SteamMatchmaking()->RequestLobbyList();
	((CCallResult<Lobby2Client_Steam_Impl, LobbyMatchList_t>*)m_SteamCallResultLobbyMatchList)->Set( requestId, (RakNet::Lobby2Client_Steam_Impl*) client, &Lobby2Client_Steam_Impl::OnLobbyMatchListCallback );
	return false; // Asynch
}
void Console_SearchRooms_Steam::DebugMsg(RakNet::RakString &out) const
{
	if (resultCode!=L2RC_SUCCESS)
	{
		Console_SearchRooms::DebugMsg(out);
		return;
	}
	out.Set("%i rooms found", roomNames.GetSize());
	for (DataStructures::DefaultIndexType i=0; i < roomNames.GetSize(); i++)
	{
		out += RakNet::RakString("\n%i. %s. ID=%" PRINTF_64_BIT_MODIFIER "u", i+1, roomNames[i].C_String(), roomIds[i]);
	}
}
bool Console_GetRoomDetails_Steam::ClientImpl( RakNet::Lobby2Plugin *client)
{
	(void) client;

	SteamMatchmaking()->RequestLobbyData( roomId );

	return false; // Asynch
}
Console_CreateRoom_Steam::Console_CreateRoom_Steam()
{
	m_SteamCallResultLobbyCreated = RakNet::OP_NEW<CCallResult<Lobby2Client_Steam_Impl, LobbyCreated_t> >(_FILE_AND_LINE_);
}
Console_CreateRoom_Steam::~Console_CreateRoom_Steam()
{
	// Cast to make sure destructor gets called
	RakNet::OP_DELETE((CCallResult<Lobby2Client_Steam_Impl, LobbyCreated_t>*)m_SteamCallResultLobbyCreated, _FILE_AND_LINE_);
}
bool Console_CreateRoom_Steam::ClientImpl( RakNet::Lobby2Plugin *client)
{
	if (roomIsPublic)
		requestId = SteamMatchmaking()->CreateLobby( k_ELobbyTypePublic, publicSlots  );
	else
		requestId = SteamMatchmaking()->CreateLobby( k_ELobbyTypeFriendsOnly, publicSlots  );

	// set the function to call when this completes
	((CCallResult<Lobby2Client_Steam_Impl, LobbyCreated_t>*)m_SteamCallResultLobbyCreated)->Set( requestId, (RakNet::Lobby2Client_Steam_Impl*) client, &Lobby2Client_Steam_Impl::OnLobbyCreated );

	return false; // Asynch
}
Console_JoinRoom_Steam::Console_JoinRoom_Steam()
{
	m_SteamCallResultLobbyEntered = RakNet::OP_NEW<CCallResult<Lobby2Client_Steam_Impl, LobbyEnter_t> > (_FILE_AND_LINE_);
}
Console_JoinRoom_Steam::~Console_JoinRoom_Steam()
{
	// Cast to make sure destructor gets called
	RakNet::OP_DELETE((CCallResult<Lobby2Client_Steam_Impl, LobbyEnter_t>*)m_SteamCallResultLobbyEntered, _FILE_AND_LINE_);
}
bool Console_JoinRoom_Steam::ClientImpl( RakNet::Lobby2Plugin *client)
{
	requestId = SteamMatchmaking()->JoinLobby( roomId  );

	// set the function to call when this completes
	((CCallResult<Lobby2Client_Steam_Impl, LobbyEnter_t>*)m_SteamCallResultLobbyEntered)->Set( requestId, (RakNet::Lobby2Client_Steam_Impl*) client, &Lobby2Client_Steam_Impl::OnLobbyJoined );

	return false; // Asynch
}
bool Console_LeaveRoom_Steam::ClientImpl( RakNet::Lobby2Plugin *client)
{
	SteamMatchmaking()->LeaveLobby( roomId );

	Lobby2Client_Steam_Impl *steam = (Lobby2Client_Steam_Impl *)client;
	steam->NotifyLeaveRoom();

	resultCode=L2RC_SUCCESS;
	return true; // Synchronous
}
bool Console_SendRoomChatMessage_Steam::ClientImpl( RakNet::Lobby2Plugin *client)
{
	(void) client;

	SteamMatchmaking()->SendLobbyChatMsg(roomId, message.C_String(), (int) message.GetLength()+1);

	// ISteamMatchmaking.h
	/*
	// Broadcasts a chat message to the all the users in the lobby
	// users in the lobby (including the local user) will receive a LobbyChatMsg_t callback
	// returns true if the message is successfully sent
	// pvMsgBody can be binary or text data, up to 4k
	// if pvMsgBody is text, cubMsgBody should be strlen( text ) + 1, to include the null terminator
	virtual bool SendLobbyChatMsg( CSteamID steamIDLobby, const void *pvMsgBody, int cubMsgBody ) = 0;
	*/

	resultCode=L2RC_SUCCESS;
	return true; // Synchronous
}
