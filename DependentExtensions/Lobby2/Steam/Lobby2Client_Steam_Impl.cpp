/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "Lobby2Client_Steam_Impl.h"
#include "Lobby2Message_Steam.h"
#include <stdlib.h>
#include "NativeTypes.h"
#include "MTUSize.h"
#include <windows.h>

using namespace RakNet;

STATIC_FACTORY_DEFINITIONS(Lobby2Client_Steam,Lobby2Client_Steam_Impl)

DEFINE_MULTILIST_PTR_TO_MEMBER_COMPARISONS(Lobby2Message,uint64_t,requestId);

int Lobby2Client_Steam_Impl::SystemAddressAndRoomMemberComp( const SystemAddress &key, const Lobby2Client_Steam_Impl::RoomMember &data )
{
	if (key<data.systemAddress)
		return -1;
	if (key==data.systemAddress)
		return 0;
	return 1;
}

int Lobby2Client_Steam_Impl::SteamIDAndRoomMemberComp( const uint64_t &key, const Lobby2Client_Steam_Impl::RoomMember &data )
{
	if (key<data.steamIDRemote)
		return -1;
	if (key==data.steamIDRemote)
		return 0;
	return 1;
}

Lobby2Client_Steam_Impl::Lobby2Client_Steam_Impl() :
m_CallbackLobbyDataUpdated( this, &Lobby2Client_Steam_Impl::OnLobbyDataUpdatedCallback ),
m_CallbackPersonaStateChange( this, &Lobby2Client_Steam_Impl::OnPersonaStateChange ),
m_CallbackLobbyDataUpdate( this, &Lobby2Client_Steam_Impl::OnLobbyDataUpdate ),
m_CallbackChatDataUpdate( this, &Lobby2Client_Steam_Impl::OnLobbyChatUpdate ),
m_CallbackChatMessageUpdate( this, &Lobby2Client_Steam_Impl::OnLobbyChatMessage ),
m_CallbackP2PSessionRequest( this, &Lobby2Client_Steam_Impl::OnP2PSessionRequest ),
m_CallbackP2PSessionConnectFail( this, &Lobby2Client_Steam_Impl::OnP2PSessionConnectFail )
{
	// Must recompile RakNet with MAXIMUM_MTU_SIZE set to 1200 in the preprocessor settings	
	RakAssert(MAXIMUM_MTU_SIZE<=1200);
	roomId=0;

}
Lobby2Client_Steam_Impl::~Lobby2Client_Steam_Impl()
{

}
void Lobby2Client_Steam_Impl::SendMsg(Lobby2Message *msg)
{
	if (msg->ClientImpl(this))
	{
		for (unsigned long i=0; i < callbacks.Size(); i++)
		{
			if (msg->callbackId==(uint32_t)-1 || msg->callbackId==callbacks[i]->callbackId)
				msg->CallCallback(callbacks[i]);
		}
	}
	else
	{
		// Won't be deleted by the user's call to Deref.
		msg->resultCode=L2RC_PROCESSING;
		msg->AddRef();
		PushDeferredCallback(msg);
	}
}
void Lobby2Client_Steam_Impl::Update(void)
{
	SteamAPI_RunCallbacks();

	/*
	// sending data
	// must be a handle to a connected socket
	// data is all sent via UDP, and thus send sizes are limited to 1200 bytes; after this, many routers will start dropping packets
	// use the reliable flag with caution; although the resend rate is pretty aggressive,
	// it can still cause stalls in receiving data (like TCP)
	virtual bool SendDataOnSocket( SNetSocket_t hSocket, void *pubData, uint32 cubData, bool bReliable ) = 0;

	// receiving data
	// returns false if there is no data remaining
	// fills out *pcubMsgSize with the size of the next message, in bytes
	virtual bool IsDataAvailableOnSocket( SNetSocket_t hSocket, uint32 *pcubMsgSize ) = 0; 

	// fills in pubDest with the contents of the message
	// messages are always complete, of the same size as was sent (i.e. packetized, not streaming)
	// if *pcubMsgSize < cubDest, only partial data is written
	// returns false if no data is available
	virtual bool RetrieveDataFromSocket( SNetSocket_t hSocket, void *pubDest, uint32 cubDest, uint32 *pcubMsgSize ) = 0; 
	*/
}

void Lobby2Client_Steam_Impl::PushDeferredCallback(Lobby2Message *msg)
{
	deferredCallbacks.Push(msg, msg->requestId, _FILE_AND_LINE_ );
}
void Lobby2Client_Steam_Impl::CallCBWithResultCode(Lobby2Message *msg, Lobby2ResultCode rc)
{
	if (msg)
	{
		msg->resultCode=rc;
		for (unsigned long i=0; i < callbacks.Size(); i++)
		{
			if (msg->callbackId==(uint32_t)-1 || msg->callbackId==callbacks[i]->callbackId)
				msg->CallCallback(callbacks[i]);
		}
	}	
}
void Lobby2Client_Steam_Impl::OnLobbyMatchListCallback( LobbyMatchList_t *pCallback, bool bIOFailure )
{
	(void) bIOFailure;

	uint32_t i;
	for (i=0; i < deferredCallbacks.GetSize(); i++)
	{
		// Get any instance of Console_SearchRooms
		if (deferredCallbacks[i]->GetID()==L2MID_Console_SearchRooms)
		{
			Console_SearchRooms_Steam *callbackResult = (Console_SearchRooms_Steam *) deferredCallbacks[i];
			//			iterate the returned lobbies with GetLobbyByIndex(), from values 0 to m_nLobbiesMatching-1
			// lobbies are returned in order of closeness to the user, so add them to the list in that order
			for ( uint32 iLobby = 0; iLobby < pCallback->m_nLobbiesMatching; iLobby++ )
			{
				CSteamID steamId = SteamMatchmaking()->GetLobbyByIndex( iLobby );
				callbackResult->roomIds.Push(steamId.ConvertToUint64(), _FILE_AND_LINE_ );
				RakNet::RakString s = SteamMatchmaking()->GetLobbyData( steamId, "name" );
				callbackResult->roomNames.Push(s, _FILE_AND_LINE_ );
			}

			CallCBWithResultCode(callbackResult, L2RC_SUCCESS);
			msgFactory->Dealloc(callbackResult);
			deferredCallbacks.RemoveAtIndex(i);
			break;
		}
	}
}
void Lobby2Client_Steam_Impl::OnLobbyDataUpdatedCallback( LobbyDataUpdate_t *pCallback )
{
	uint32_t i;
	for (i=0; i < deferredCallbacks.GetSize(); i++)
	{
		if (deferredCallbacks[i]->GetID()==L2MID_Console_GetRoomDetails)
		{
			Console_GetRoomDetails_Steam *callbackResult = (Console_GetRoomDetails_Steam *) deferredCallbacks[i];
			if (callbackResult->roomId==pCallback->m_ulSteamIDLobby)
			{
				const char *pchLobbyName = SteamMatchmaking()->GetLobbyData( pCallback->m_ulSteamIDLobby, "name" );
				if ( pchLobbyName[0] )
				{
					callbackResult->roomName=pchLobbyName;
				}
				if (pCallback->m_bSuccess)
					CallCBWithResultCode(callbackResult, L2RC_SUCCESS);
				else
					CallCBWithResultCode(callbackResult, L2RC_Console_GetRoomDetails_NO_ROOMS_FOUND);
				msgFactory->Dealloc(callbackResult);
				deferredCallbacks.RemoveAtIndex(i);
				break;
			}
		}
	}

	Console_GetRoomDetails_Steam notification;
	const char *pchLobbyName = SteamMatchmaking()->GetLobbyData( pCallback->m_ulSteamIDLobby, "name" );
	if ( pchLobbyName[0] )
	{
		notification.roomName=pchLobbyName;
	}
	notification.roomId=pCallback->m_ulSteamIDLobby;
	CallCBWithResultCode(&notification, L2RC_SUCCESS);
}
void Lobby2Client_Steam_Impl::OnLobbyCreated( LobbyCreated_t *pCallback, bool bIOFailure )
{
	(void) bIOFailure;

	uint32_t i;
	for (i=0; i < deferredCallbacks.GetSize(); i++)
	{
		if (deferredCallbacks[i]->GetID()==L2MID_Console_CreateRoom)
		{
			Console_CreateRoom_Steam *callbackResult = (Console_CreateRoom_Steam *) deferredCallbacks[i];
			callbackResult->roomId=pCallback->m_ulSteamIDLobby;
			SteamMatchmaking()->SetLobbyData( callbackResult->roomId, "name", callbackResult->roomName.C_String() );
			roomId=pCallback->m_ulSteamIDLobby;

			printf("\nNumber of Steam Lobby Members:%i in Lobby Name:%s\n", SteamMatchmaking()->GetNumLobbyMembers(roomId), callbackResult->roomName.C_String());
			RoomMember roomMember;
			roomMember.steamIDRemote=SteamMatchmaking()->GetLobbyOwner(roomId).ConvertToUint64();
			roomMember.systemAddress.address.addr4.sin_addr.s_addr=nextFreeSystemAddress++;
			roomMember.systemAddress.SetPortHostOrder(STEAM_UNUSED_PORT);
			roomMembersByAddr.Insert(roomMember.systemAddress,roomMember,true,_FILE_AND_LINE_);
			roomMembersById.Insert(roomMember.steamIDRemote,roomMember,true,_FILE_AND_LINE_);

			callbackResult->extendedResultCode=pCallback->m_eResult;
			if (pCallback->m_eResult==k_EResultOK)
			{
				CallCBWithResultCode(callbackResult, L2RC_SUCCESS);
			}
			else
			{
				CallCBWithResultCode(callbackResult, L2RC_GENERAL_ERROR);
			}
			msgFactory->Dealloc(callbackResult);
			deferredCallbacks.RemoveAtIndex(i);

			// Commented out: Do not send the notification for yourself
			// CallRoomCallbacks();
			break;
		}
	}
}
void Lobby2Client_Steam_Impl::OnLobbyJoined( LobbyEnter_t *pCallback, bool bIOFailure )
{
	(void) bIOFailure;

	uint32_t i;
	for (i=0; i < deferredCallbacks.GetSize(); i++)
	{
		if (deferredCallbacks[i]->GetID()==L2MID_Console_JoinRoom)
		{
			Console_JoinRoom_Steam *callbackResult = (Console_JoinRoom_Steam *) deferredCallbacks[i];

			if (pCallback->m_EChatRoomEnterResponse==k_EChatRoomEnterResponseSuccess)
			{
				roomId=pCallback->m_ulSteamIDLobby;

				CallCBWithResultCode(callbackResult, L2RC_SUCCESS);

				// First push to prevent being notified of ourselves
				RoomMember roomMember;
				roomMember.steamIDRemote=SteamUser()->GetSteamID().ConvertToUint64();
				roomMember.systemAddress.address.addr4.sin_addr.s_addr=nextFreeSystemAddress++;
				roomMember.systemAddress.SetPortHostOrder(STEAM_UNUSED_PORT);
				roomMembersByAddr.Insert(roomMember.systemAddress,roomMember,true,_FILE_AND_LINE_);
				roomMembersById.Insert(roomMember.steamIDRemote,roomMember,true,_FILE_AND_LINE_);

				CallRoomCallbacks();

				// In case the asynch lobby update didn't get it fast enough
				uint64_t myId64=SteamUser()->GetSteamID().ConvertToUint64();
				if (roomMembersById.HasData(myId64)==false)
				{
					roomMember.steamIDRemote=SteamMatchmaking()->GetLobbyOwner(roomId).ConvertToUint64();
					roomMember.systemAddress.address.addr4.sin_addr.s_addr=nextFreeSystemAddress++;
					roomMember.systemAddress.SetPortHostOrder(STEAM_UNUSED_PORT);
					roomMembersByAddr.Insert(roomMember.systemAddress,roomMember,true,_FILE_AND_LINE_);
					roomMembersById.Insert(roomMember.steamIDRemote,roomMember,true,_FILE_AND_LINE_);
				}
			}
			else
			{
				CallCBWithResultCode(callbackResult, L2RC_Console_JoinRoom_NO_SUCH_ROOM);
			}

			msgFactory->Dealloc(callbackResult);
			deferredCallbacks.RemoveAtIndex(i);
			break;
		}
	}
}
bool Lobby2Client_Steam_Impl::IsCommandRunning( Lobby2MessageID msgId )
{
	uint32_t i;
	for (i=0; i < deferredCallbacks.GetSize(); i++)
	{
		if (deferredCallbacks[i]->GetID()==msgId)
		{
			return true;
		}
	}
	return false;
}

void Lobby2Client_Steam_Impl::OnPersonaStateChange( PersonaStateChange_t *pCallback )
{
	// callbacks are broadcast to all listeners, so we'll get this for every friend who changes state
	// so make sure the user is in the lobby before acting
	if ( !SteamFriends()->IsUserInSource( pCallback->m_ulSteamID, roomId ) )
		return;

	if ((pCallback->m_nChangeFlags & k_EPersonaChangeNameFirstSet) ||
		(pCallback->m_nChangeFlags & k_EPersonaChangeName))
	{
		Notification_Friends_StatusChange_Steam notification;
		notification.friendId=pCallback->m_ulSteamID;
		const char *pchName = SteamFriends()->GetFriendPersonaName( notification.friendId );
		notification.friendNewName=pchName;
		CallCBWithResultCode(&notification, L2RC_SUCCESS);
	}
}
void Lobby2Client_Steam_Impl::OnLobbyDataUpdate( LobbyDataUpdate_t *pCallback )
{
	// callbacks are broadcast to all listeners, so we'll get this for every lobby we're requesting
	if ( roomId != pCallback->m_ulSteamIDLobby )
		return;

	Notification_Console_UpdateRoomParameters_Steam notification;
	notification.roomId=roomId;
	notification.roomNewName=SteamMatchmaking()->GetLobbyData( roomId, "name" );
	if (pCallback->m_bSuccess)
		CallCBWithResultCode(&notification, L2RC_SUCCESS);
	else
		CallCBWithResultCode(&notification, L2RC_Console_GetRoomDetails_NO_ROOMS_FOUND);
}
void Lobby2Client_Steam_Impl::OnLobbyChatUpdate( LobbyChatUpdate_t *pCallback )
{
	// callbacks are broadcast to all listeners, so we'll get this for every lobby we're requesting
	if ( roomId != pCallback->m_ulSteamIDLobby )
		return;

	// Purpose: Handles users in the lobby joining or leaving ??????
	CallRoomCallbacks();	
}
void Lobby2Client_Steam_Impl::OnLobbyChatMessage( LobbyChatMsg_t *pCallback )
{
	CSteamID speaker;
	EChatEntryType entryType;
	char data[2048];
	int cubData=sizeof(data);
	SteamMatchmaking()->GetLobbyChatEntry( roomId, pCallback->m_iChatID, &speaker, data, cubData, &entryType);
	if (entryType==k_EChatEntryTypeChatMsg)
	{
		Notification_Console_RoomChatMessage_Steam notification;
		notification.message=data;
		CallCBWithResultCode(&notification, L2RC_SUCCESS);
	}

}
void Lobby2Client_Steam_Impl::GetRoomMembers(DataStructures::OrderedList<uint64_t, uint64_t> &_roomMembers)
{
	_roomMembers.Clear(true,_FILE_AND_LINE_);
	int cLobbyMembers = SteamMatchmaking()->GetNumLobbyMembers( roomId );
	for ( int i = 0; i < cLobbyMembers; i++ )
	{
		CSteamID steamIDLobbyMember = SteamMatchmaking()->GetLobbyMemberByIndex( roomId, i ) ;
		uint64_t memberid=steamIDLobbyMember.ConvertToUint64();
		_roomMembers.Insert(memberid,memberid,true,_FILE_AND_LINE_);
	}
}

const char * Lobby2Client_Steam_Impl::GetRoomMemberName(uint64_t memberId)
{
	return SteamFriends()->GetFriendPersonaName( memberId );
}

bool Lobby2Client_Steam_Impl::IsRoomOwner(const uint64_t cSteamID)
{
	if(SteamUser()->GetSteamID() == SteamMatchmaking()->GetLobbyOwner(cSteamID))
		return true;

	return false;
}

bool Lobby2Client_Steam_Impl::IsInRoom(void) const
{
	return roomMembersById.Size() > 0;
}

void Lobby2Client_Steam_Impl::CallRoomCallbacks()
{
	DataStructures::OrderedList<uint64_t,uint64_t> currentMembers;
	GetRoomMembers(currentMembers);
	DataStructures::OrderedList<uint64_t, RoomMember, SteamIDAndRoomMemberComp> updatedRoomMembers;
	bool anyChanges=false;

	unsigned int currentMemberIndex=0, oldMemberIndex=0;
	while (currentMemberIndex < currentMembers.Size() && oldMemberIndex < roomMembersById.Size())
	{
		if (currentMembers[currentMemberIndex]<roomMembersById[oldMemberIndex].steamIDRemote)
		{
			RoomMember roomMember;
			roomMember.steamIDRemote=currentMembers[currentMemberIndex];
			roomMember.systemAddress.address.addr4.sin_addr.s_addr=nextFreeSystemAddress++;
			roomMember.systemAddress.SetPortHostOrder(STEAM_UNUSED_PORT);
			updatedRoomMembers.Insert(roomMember.steamIDRemote,roomMember,true,_FILE_AND_LINE_);

			anyChanges=true;

			// new member
			NotifyNewMember(currentMembers[currentMemberIndex], roomMember.systemAddress);
			currentMemberIndex++;
		}
		else if (currentMembers[currentMemberIndex]>roomMembersById[oldMemberIndex].steamIDRemote)
		{
			anyChanges=true;

			// dropped member
			NotifyDroppedMember(roomMembersById[oldMemberIndex].steamIDRemote, roomMembersById[oldMemberIndex].systemAddress);
			oldMemberIndex++;
		}
		else
		{
			updatedRoomMembers.Insert(roomMembersById[oldMemberIndex].steamIDRemote,roomMembersById[oldMemberIndex],true,_FILE_AND_LINE_);

			currentMemberIndex++;
			oldMemberIndex++;
		}
	}

	while (oldMemberIndex < roomMembersById.Size())
	{
		anyChanges=true;

		// dropped member
		NotifyDroppedMember(roomMembersById[oldMemberIndex].steamIDRemote, roomMembersById[oldMemberIndex].systemAddress);

		oldMemberIndex++;
	}
	while (currentMemberIndex < currentMembers.Size())
	{
		RoomMember roomMember;
		roomMember.steamIDRemote=currentMembers[currentMemberIndex];
		roomMember.systemAddress.address.addr4.sin_addr.s_addr=nextFreeSystemAddress++;
		roomMember.systemAddress.SetPortHostOrder(STEAM_UNUSED_PORT);
		updatedRoomMembers.Insert(roomMember.steamIDRemote,roomMember,true,_FILE_AND_LINE_);

		anyChanges=true;

		// new member
		NotifyNewMember(currentMembers[currentMemberIndex], roomMember.systemAddress);

		currentMemberIndex++;
	}

	if (anyChanges)
	{
		roomMembersById=updatedRoomMembers;
		roomMembersByAddr.Clear(true, _FILE_AND_LINE_);
		for (currentMemberIndex=0; currentMemberIndex < roomMembersById.Size(); currentMemberIndex++)
		{
			roomMembersByAddr.Insert(roomMembersById[currentMemberIndex].systemAddress, roomMembersById[currentMemberIndex], true, _FILE_AND_LINE_);
		}
	}
}
void Lobby2Client_Steam_Impl::NotifyNewMember(uint64_t memberId, SystemAddress remoteSystem)
{
	// const char *pchName = SteamFriends()->GetFriendPersonaName( memberId );

	Notification_Console_MemberJoinedRoom_Steam notification;
	notification.roomId=roomId;
	notification.srcMemberId=memberId;
	notification.memberName=SteamFriends()->GetFriendPersonaName( memberId );
	notification.remoteSystem=remoteSystem;

	
	CallCBWithResultCode(&notification, L2RC_SUCCESS);
}
void Lobby2Client_Steam_Impl::NotifyDroppedMember(uint64_t memberId, SystemAddress remoteSystem)
{
	/// const char *pchName = SteamFriends()->GetFriendPersonaName( memberId );

	Notification_Console_MemberLeftRoom_Steam notification;
	notification.roomId=roomId;
	notification.srcMemberId=memberId;
	notification.memberName=SteamFriends()->GetFriendPersonaName( memberId );
	notification.remoteSystem=remoteSystem;
	CallCBWithResultCode(&notification, L2RC_SUCCESS);

	unsigned int i;
	bool objectExists;
	i = roomMembersById.GetIndexFromKey(memberId, &objectExists);
	if (objectExists)
	{
		rakPeerInterface->CloseConnection(roomMembersById[i].systemAddress,false);
		// Is this necessary?
		SteamNetworking()->CloseP2PSessionWithUser( memberId );
	}
}
void Lobby2Client_Steam_Impl::ClearRoom(void)
{
	roomId=0;
	if (SteamNetworking())
	{
		for (unsigned int i=0; i < roomMembersById.Size(); i++)
		{
			SteamNetworking()->CloseP2PSessionWithUser( roomMembersById[i].steamIDRemote );
		}
	}
	roomMembersById.Clear(true,_FILE_AND_LINE_);
	roomMembersByAddr.Clear(true,_FILE_AND_LINE_);
}
void Lobby2Client_Steam_Impl::OnP2PSessionRequest( P2PSessionRequest_t *pCallback )
{
	// we'll accept a connection from anyone
	SteamNetworking()->AcceptP2PSessionWithUser( pCallback->m_steamIDRemote );
}
void Lobby2Client_Steam_Impl::OnP2PSessionConnectFail( P2PSessionConnectFail_t *pCallback )
{
	(void) pCallback;

	// we've sent a packet to the user, but it never got through
	// we can just use the normal timeout
}
void Lobby2Client_Steam_Impl::NotifyLeaveRoom(void)
{
	ClearRoom();
}

int Lobby2Client_Steam_Impl::RakNetSendTo( const char *data, int length, const SystemAddress &systemAddress )
{
	bool objectExists;
	unsigned int i = roomMembersByAddr.GetIndexFromKey(systemAddress, &objectExists);
	if (objectExists)
	{
		if (SteamNetworking()->SendP2PPacket(roomMembersByAddr[i].steamIDRemote, data, length, k_EP2PSendUnreliable))
			return length;
		else
			return 0;
	}
	else if (systemAddress.GetPort()!=STEAM_UNUSED_PORT)
	{
	//	return SocketLayer::SendTo_PC(s,data,length,systemAddress,_FILE_AND_LINE_);
		return -1;
	}
	return 0;
}

int Lobby2Client_Steam_Impl::RakNetRecvFrom( char dataOut[ MAXIMUM_MTU_SIZE ], SystemAddress *senderOut, bool calledFromMainThread)
{
	(void) calledFromMainThread;

	uint32 pcubMsgSize;
	if (SteamNetworking() && SteamNetworking()->IsP2PPacketAvailable(&pcubMsgSize))
	{
		CSteamID psteamIDRemote;
		if (SteamNetworking()->ReadP2PPacket(dataOut, MAXIMUM_MTU_SIZE, &pcubMsgSize, &psteamIDRemote))
		{
			uint64_t steamIDRemote64=psteamIDRemote.ConvertToUint64();
			unsigned int i;
			bool objectExists;
			i = roomMembersById.GetIndexFromKey(steamIDRemote64, &objectExists);
			if (objectExists)
			{
				*senderOut=roomMembersById[i].systemAddress;
			}
			return pcubMsgSize;
		}
	}
	return 0;
}

bool Lobby2Client_Steam_Impl::IsOverrideAddress(const SystemAddress &systemAddress) const
{
	return (systemAddress.GetPort() == STEAM_UNUSED_PORT);
}

void Lobby2Client_Steam_Impl::OnRakPeerShutdown(void)
{
	ClearRoom();
}
void Lobby2Client_Steam_Impl::OnClosedConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
	(void) lostConnectionReason;
	(void) rakNetGUID;

	bool objectExists;
	unsigned int i = roomMembersByAddr.GetIndexFromKey(systemAddress, &objectExists);
	if (objectExists)
	{
		uint64_t steamIDRemote = roomMembersByAddr[i].steamIDRemote;

		SteamNetworking()->CloseP2PSessionWithUser( steamIDRemote );
		roomMembersByAddr.RemoveAtIndex(i);

		i = roomMembersById.GetIndexFromKey(steamIDRemote, &objectExists);
		RakAssert(objectExists);
		roomMembersById.RemoveAtIndex(i);
	}
}
void Lobby2Client_Steam_Impl::OnFailedConnectionAttempt(Packet *packet, PI2_FailedConnectionAttemptReason failedConnectionAttemptReason)
{
	(void) failedConnectionAttemptReason;

	bool objectExists;
	unsigned int i = roomMembersByAddr.GetIndexFromKey(packet->systemAddress, &objectExists);
	if (objectExists)
	{
		uint64_t steamIDRemote = roomMembersByAddr[i].steamIDRemote;

		SteamNetworking()->CloseP2PSessionWithUser( steamIDRemote );
		roomMembersByAddr.RemoveAtIndex(i);

		i = roomMembersById.GetIndexFromKey(steamIDRemote, &objectExists);
		RakAssert(objectExists);
		roomMembersById.RemoveAtIndex(i);
	}
}
void Lobby2Client_Steam_Impl::OnAttach(void)
{
	nextFreeSystemAddress=(uint32_t) rakPeerInterface->GetMyGUID().g;

	// If this asserts, call RakPeer::Startup() before attaching Lobby2Client_Steam
	DataStructures::List<RakNetSocket2* > sockets;
	rakPeerInterface->GetSockets(sockets);
	((RNS2_Windows*)sockets[0])->SetSocketLayerOverride(this);
}
void Lobby2Client_Steam_Impl::OnDetach(void)
{
	DataStructures::List<RakNetSocket2* > sockets;
	rakPeerInterface->GetSockets(sockets);
	((RNS2_Windows*)sockets[0])->SetSocketLayerOverride(this);
}
