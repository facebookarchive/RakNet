/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "RoomsBrowserGFx3_RakNet.h"
#include "Lobby2Client.h"
#include "XMLParser.h"
#include "MessageIdentifiers.h"
#include "RoomTypes.h"
#include "GetTime.h"

using namespace RakNet;

// ---------------------------------------------------------------------------------------

void ServerAndRoomBrowserData::Serialize(RakNet::BitStream *bitStream, bool writeToBitstream)
{
	bitStream->Serialize(writeToBitstream, numPlayers);
	bitStream->Serialize(writeToBitstream, maxPlayers);
	bitStream->Serialize(writeToBitstream, mapName);
	bitStream->Serialize(writeToBitstream, roomName);
	bitStream->Serialize(writeToBitstream, roomId);
	
}
void ServerAndRoomBrowserData::WriteToTable(DataStructures::Table *table)
{
	table->Clear();
	DataStructures::Table::Cell cells[4];
	table->AddColumn("numPlayers", DataStructures::Table::NUMERIC);
	cells[0].Set((unsigned int) numPlayers);
	table->AddColumn("maxPlayers", DataStructures::Table::NUMERIC);
	cells[1].Set((unsigned int) maxPlayers);
	table->AddColumn("mapName", DataStructures::Table::STRING);
	cells[2].Set(mapName.C_String());
	table->AddColumn("roomName", DataStructures::Table::STRING); // "Room name" from RoomTypes.cpp, write this here?
	cells[3].Set(roomName.C_String());
	// The rooms plugin uses this table, and the rooms plugin automatically assigns a room id
//	table->AddColumn(DefaultRoomColumns::GetColumnName(DefaultRoomColumns::TC_ROOM_ID), DataStructures::Table::NUMERIC);
//	cells[4].Set((double) roomId.g);
	RakAssert(table->GetColumnCount()==4);
	DataStructures::List<DataStructures::Table::Cell*> initialCellValues;
	for (int i=0; i < 4; i++)
		initialCellValues.Push(&cells[i],_FILE_AND_LINE_);
	table->AddRow(table->GetRowCount(), initialCellValues, true);
}
void ServerAndRoomBrowserData::SetAsOfflinePingResponse(RakNet::RakPeerInterface *rakPeer)
{
	RakNet::BitStream bs;
	Serialize(&bs,true);
	rakPeer->SetOfflinePingResponse((const char*) bs.GetData(),bs.GetNumberOfBytesUsed());
}

// ---------------------------------------------------------------------------------------

RoomsBrowserGFx3_RakNet::RoomsBrowserGFx3_RakNet()
{
}
RoomsBrowserGFx3_RakNet::~RoomsBrowserGFx3_RakNet()
{
	Shutdown();
}
void RoomsBrowserGFx3_RakNet::Init(RakNet::Lobby2Client *_lobby2Client,
								   RakNet::Lobby2MessageFactory *_messageFactory,
								   RakPeerInterface *_rakPeer, 
								   RakNet::RoomsPlugin *_roomsPlugin,
								   RakNet::RakString _titleName,
								   RakNet::RakString _titleSecretKey,
								   RakNet::RakString _pathToXMLPropertyFile,
								   unsigned short _lanServerPort,
								   GPtr<FxDelegate> pDelegate,
								   GPtr<GFxMovieView> pMovie)
{
	RoomsBrowserGFx3::Init(pDelegate, pMovie);

	lobby2Client=_lobby2Client;
	msgFactory=_messageFactory;
	rakPeer=_rakPeer;
	roomsPlugin=_roomsPlugin;
	titleName=_titleName;
	titleSecretKey=_titleSecretKey;
	pathToXMLPropertyFile=_pathToXMLPropertyFile;
	lanServerPort=_lanServerPort;

	RoomsErrorCodeDescription::Validate();

	roomsPlugin->SetRoomsCallback(this);
}
void RoomsBrowserGFx3_RakNet::Update(void)
{

}
void RoomsBrowserGFx3_RakNet::Shutdown(void)
{
	RoomsBrowserGFx3::Shutdown();
}
void RoomsBrowserGFx3_RakNet::SaveProperty(const char *propertyId, const char *propertyValue)
{
	// this open and parse the XML file:
	XMLNode xMainNode=XMLNode::openFileHelper(pathToXMLPropertyFile.C_String(),"");
	if (xMainNode.isEmpty())
	{
		xMainNode=XMLNode::createXMLTopNode("");
	}
	XMLNode xNode=xMainNode.getChildNode(propertyId);
	if (xNode.isEmpty()==false)
	{
		xNode.deleteAttribute(0);
		xNode.addAttribute("value", propertyValue);
	}
	else
	{
		xNode = xMainNode.addChild(propertyId);
		xNode.addAttribute("value", propertyValue);
	}
	xMainNode.writeToFile(pathToXMLPropertyFile.C_String());
}
void RoomsBrowserGFx3_RakNet::LoadProperty(const char *propertyId, RakNet::RakString &propertyOut)
{
	XMLNode xMainNode=XMLNode::openFileHelper(pathToXMLPropertyFile.C_String(),"");
	if (xMainNode.isEmpty())
		return;
	XMLNode xNode=xMainNode.getChildNode(propertyId);
	LPCTSTR attr = xNode.getAttribute("value", 0);
	if (attr)
		propertyOut = attr;
	else
		propertyOut.Clear();
}
void RoomsBrowserGFx3_RakNet::OnClosedConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
	if (systemAddress==lobby2Client->GetServerAddress())
	{
		// lost connection
		FxResponseArgs<1> rargs;
		switch (lostConnectionReason)
		{
		case LCR_CLOSED_BY_USER:
			rargs.Add("LCR_CLOSED_BY_USER");
			break;
		case LCR_DISCONNECTION_NOTIFICATION:
			rargs.Add("LCR_DISCONNECTION_NOTIFICATION");
			break;
		case LCR_CONNECTION_LOST:
			rargs.Add("LCR_CONNECTION_LOST");
			break;
		}
		FxDelegate::Invoke2(movie, "c2f_NotifyServerConnectionLost", rargs);
	}
}
void RoomsBrowserGFx3_RakNet::OnNewConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, bool isIncoming)
{
	if (systemAddress==lobby2Client->GetServerAddress())
	{
		// is connected
		FxResponseArgs<0> rargs;
		FxDelegate::Invoke2(movie, "c2f_NotifyConnectionAttemptToServerSuccess", rargs);
	}
	else
	{
		FxResponseArgs<3> rargs;
		char saString[64];
		systemAddress.ToString(true,saString);
		char guidString[64];
		rakNetGUID.ToString(guidString);
		rargs.Add(saString);
		rargs.Add(guidString);
		rargs.Add(isIncoming);
		FxDelegate::Invoke2(movie, "c2f_NotifyNewConnection", rargs);
	}
}
void RoomsBrowserGFx3_RakNet::OnFailedConnectionAttempt(Packet *packet, PI2_FailedConnectionAttemptReason failedConnectionAttemptReason)
{
	// isn't connected
	FxResponseArgs<2> rargs;
	switch(failedConnectionAttemptReason)
	{
	case FCAR_CONNECTION_ATTEMPT_FAILED:
		rargs.Add("FCAR_CONNECTION_ATTEMPT_FAILED");
		break;
	case FCAR_ALREADY_CONNECTED:
		rargs.Add("FCAR_ALREADY_CONNECTED");
		break;
	case FCAR_NO_FREE_INCOMING_CONNECTIONS:
		rargs.Add("FCAR_NO_FREE_INCOMING_CONNECTIONS");
		break;
	case FCAR_SECURITY_PUBLIC_KEY_MISMATCH:
		rargs.Add("FCAR_SECURITY_PUBLIC_KEY_MISMATCH");
		break;
	case FCAR_CONNECTION_BANNED:
		rargs.Add("FCAR_CONNECTION_BANNED");
		break;
	case FCAR_INVALID_PASSWORD:
		rargs.Add("FCAR_INVALID_PASSWORD");
		break;
	case FCAR_INCOMPATIBLE_PROTOCOL:
		rargs.Add("FCAR_INCOMPATIBLE_PROTOCOL");
		break;
	case FCAR_IP_RECENTLY_CONNECTED:
		rargs.Add("FCAR_IP_RECENTLY_CONNECTED");
		break;
	case FCAR_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY:
		rargs.Add("FCAR_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY");
		break;
	case FCAR_OUR_SYSTEM_REQUIRES_SECURITY:
		rargs.Add("FCAR_OUR_SYSTEM_REQUIRES_SECURITY");
		break;
	case FCAR_PUBLIC_KEY_MISMATCH:
		rargs.Add("FCAR_PUBLIC_KEY_MISMATCH");
		break;
	}

	char saString[64];
	packet->systemAddress.ToString(true,saString);
	rargs.Add(saString);

	if (packet->systemAddress==lobby2Client->GetServerAddress())
		FxDelegate::Invoke2(movie, "c2f_NotifyConnectionAttemptToServerFailure", rargs);
	else
		FxDelegate::Invoke2(movie, "c2f_NotifyFailedConnectionAttempt", rargs);
}
PluginReceiveResult RoomsBrowserGFx3_RakNet::OnReceive(Packet *packet)
{
	switch (packet->data[0])
	{
	case ID_UNCONNECTED_PONG:
		if (packet->systemAddress.GetPort()>=lanServerPort && packet->systemAddress.GetPort()<lanServerPort+8 && packet->length>sizeof(RakNet::TimeMS)+sizeof(MessageID)
			// && packet->guid!=rakPeer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS)
			)
		{
			RakNet::BitStream bsIn(packet->data+1,packet->length-1,false);
			RakNet::TimeMS pingResponse;
			bsIn.Read(pingResponse);
			RakNet::TimeMS elapsedPing=RakNet::GetTimeMS()-pingResponse;
			RakNet::TimeMS elapsedPingMs;
#if __GET_TIME_64BIT==1
			elapsedPingMs=elapsedPing/1000;
#else
			elapsedPingMs=elapsedPing;
#endif

			// This was set in RakPeerInterface::SetOfflinePingResponse, by using ServerWANUpdate::SetAsOfflinePingResponse()
			ServerAndRoomBrowserData browserData;
			browserData.Serialize(&bsIn,false);

			FxResponseArgsList rargs;
			rargs.Add(false);
			rargs.Add((Double)browserData.roomId.g);
			char ipAddr[64];
			packet->systemAddress.ToString(true,ipAddr);
			rargs.Add(ipAddr);
			rargs.Add(browserData.roomName.C_String());
			rargs.Add((Double)browserData.numPlayers);
			rargs.Add((Double)browserData.maxPlayers);
			rargs.Add(browserData.mapName.C_String());

			FxDelegate::Invoke2(movie, "c2f_AddSingleRoom", rargs);
		}
		break;
	}

	return RR_CONTINUE_PROCESSING;
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_ConnectToServer)
{
	bool callSucceeded = rakPeer->Connect(pparams[0].GetString(), atoi(pparams[1].GetString()), 0, 0)==RakNet::CONNECTION_ATTEMPT_STARTED;
	FxResponseArgs<1> rargs;
	rargs.Add(callSucceeded);
	lobby2Client->SetServerAddress(SystemAddress(pparams[0].GetString(), atoi(pparams[1].GetString())));
	roomsPlugin->SetServerAddress(lobby2Client->GetServerAddress());
	// pparams.Respond(rargs);

	FxDelegate::Invoke2(movie, pparams[2].GetString(), rargs);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_Login)
{
	Client_Login *arg = (Client_Login *) msgFactory->Alloc(L2MID_Client_Login);
	arg->titleName=titleName;
	arg->titleSecretKey=titleSecretKey;
	arg->userName=pparams[0].GetString();
	arg->userPassword=pparams[1].GetString();
	loginUsername=arg->userName;

	lobby2Client->SendMsgAndDealloc(arg);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_RegisterAccount)
{
	RakNet::Client_RegisterAccount *arg = (RakNet::Client_RegisterAccount *) msgFactory->Alloc(L2MID_Client_RegisterAccount);
	arg->userName=pparams[0].GetString();
	loginUsername=arg->userName;
	arg->createAccountParameters.password=pparams[1].GetString();
	arg->titleName=titleName;
	lobby2Client->SendMsgAndDealloc(arg);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_UpdateRoomsList)
{
	bool isNetUpdate = pparams[0].GetBool();
	if (isNetUpdate)
	{
		SearchByFilter_Func func;
		func.roomQuery.queries=0;
		func.roomQuery.numQueries=0;
		func.onlyJoinable=false;
		func.gameIdentifier=titleName;
		func.userName=loginUsername;
		roomsPlugin->ExecuteFunc(&func);
	}
	else
	{
		// Ping out looking for game servers. We can't find servers unless they all use the same port
		// Up to 8 instances on the same machine is reasonable
		for (int i=0; i < 8; i++)
		{
			rakPeer->Ping("255.255.255.255", lanServerPort+i, false);
		}
	}
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_UpdateFriendsList)
{
	RakNet::Friends_GetFriends *arg = (RakNet::Friends_GetFriends *) msgFactory->Alloc(L2MID_Friends_GetFriends);
	lobby2Client->SendMsgAndDealloc(arg);
	RakNet::Friends_GetInvites *arg2 = (RakNet::Friends_GetInvites *) msgFactory->Alloc(L2MID_Friends_GetInvites);
	lobby2Client->SendMsgAndDealloc(arg2);	
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_JoinByFilter)
{
	if (pparams.GetArgCount()<1)
		return;

	RakNet::JoinByFilter_Func func;
	func.userName=loginUsername;
	func.gameIdentifier=titleName;
	func.roomMemberMode=RMM_ANY_PLAYABLE;

	bool roomIsFromServer=pparams[0].GetBool();
	double roomGuid=pparams[1].GetNumber();

	if (roomIsFromServer)
	{
		// See RoomTypes.h for other default columns
		func.query.AddQuery_NUMERIC( DefaultRoomColumns::GetColumnName(DefaultRoomColumns::TC_ROOM_ID), roomGuid);
		roomsPlugin->ExecuteFunc(&func);
	}
	else
	{
		SystemAddress sa;
		sa.FromString(pparams[2].GetString());
		char ipPart[32];
		sa.ToString(false,ipPart);
		rakPeer->Connect(ipPart,sa.GetPort(),0,0);
	}

}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_CreateRoom)
{
	/// Create a room for the WAN browser
	RakNet::CreateRoom_Func func;
	func.networkedRoomCreationParameters.slots.publicSlots=(unsigned int) pparams[2].GetNumber();
	func.networkedRoomCreationParameters.slots.reservedSlots=(unsigned int) pparams[3].GetNumber();
	func.networkedRoomCreationParameters.slots.spectatorSlots=0;
	func.networkedRoomCreationParameters.hiddenFromSearches=pparams[4].GetBool();
	func.networkedRoomCreationParameters.destroyOnModeratorLeave=false;
	func.networkedRoomCreationParameters.autoLockReadyStatus=true;
	if (pparams[4].GetBool()==true) // Room members can invite
		func.networkedRoomCreationParameters.inviteToRoomPermission=NetworkedRoomCreationParameters::INVITE_MODE_ANYONE_CAN_INVITE;
	else
		func.networkedRoomCreationParameters.inviteToRoomPermission=NetworkedRoomCreationParameters::INVITE_MODE_MODERATOR_CAN_INVITE;
	func.networkedRoomCreationParameters.inviteToSpectatorSlotPermission=NetworkedRoomCreationParameters::INVITE_MODE_ANYONE_CAN_INVITE;
	func.networkedRoomCreationParameters.clearInvitesOnNewModerator=true;
	func.networkedRoomCreationParameters.roomName=pparams[0].GetString();
	if (func.networkedRoomCreationParameters.roomName.IsEmpty())
		func.networkedRoomCreationParameters.roomName=loginUsername + "'s room";
	func.gameIdentifier=titleName;

	ServerAndRoomBrowserData browserData;
	browserData.roomId=rakPeer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	browserData.numPlayers=0;
	browserData.maxPlayers=func.networkedRoomCreationParameters.slots.publicSlots+func.networkedRoomCreationParameters.slots.reservedSlots;
	browserData.mapName=pparams[1].GetString();
	browserData.roomName=func.networkedRoomCreationParameters.roomName;
	browserData.SetAsOfflinePingResponse(rakPeer);

	browserData.WriteToTable(&func.initialRoomProperties);

	bool isLanGame = pparams[6].GetBool();
	if (isLanGame)
	{
		FxResponseArgsList rargs;
		rargs.Add(RoomsErrorCodeDescription::ToEnum(REC_SUCCESS));
		rargs.Add(true);
		FxDelegate::Invoke2(movie, "c2f_CreateRoom", rargs);
		return;
	}

	func.userName=loginUsername;
	roomsPlugin->ExecuteFunc(&func);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_Friends_SendInvite)
{
	RakNet::Friends_SendInvite *arg = (RakNet::Friends_SendInvite *) msgFactory->Alloc(L2MID_Friends_SendInvite);
	arg->targetHandle=pparams[0].GetString();
	lobby2Client->SendMsgAndDealloc(arg);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_Friends_Remove)
{
	RakNet::Friends_Remove *arg = (RakNet::Friends_Remove *) msgFactory->Alloc(L2MID_Friends_Remove);
	arg->targetHandle=pparams[0].GetString();
	lobby2Client->SendMsgAndDealloc(arg);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_Friends_AcceptInvite)
{
	RakNet::Friends_AcceptInvite *arg = (RakNet::Friends_AcceptInvite *) msgFactory->Alloc(L2MID_Friends_AcceptInvite);
	arg->targetHandle=pparams[0].GetString();
	lobby2Client->SendMsgAndDealloc(arg);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_Friends_RejectInvite)
{
	RakNet::Friends_RejectInvite *arg = (RakNet::Friends_RejectInvite *) msgFactory->Alloc(L2MID_Friends_RejectInvite);
	arg->targetHandle=pparams[0].GetString();
	lobby2Client->SendMsgAndDealloc(arg);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_Directed_Chat_Func)
{
	Chat_Func func;
	func.userName=loginUsername;
	func.chatMessage=pparams[1].GetString();
	func.privateMessageRecipient=pparams[0].GetString();
	func.chatDirectedToRoom=false;
	roomsPlugin->ExecuteFunc(&func);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_Room_Chat_Func)
{
	Chat_Func func;
	func.userName=loginUsername;
	func.chatMessage=pparams[0].GetString();
	func.chatDirectedToRoom=true;
	roomsPlugin->ExecuteFunc(&func);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_Logoff)
{
	RakNet::Client_Logoff *arg = (RakNet::Client_Logoff *) msgFactory->Alloc(L2MID_Client_Logoff);
	lobby2Client->SendMsgAndDealloc(arg);
}
//////////////////////////////////////////////////////////////////................................................
//////////////////////////////////////////////////////////////////................................................
//////////////////////////////////////////////////////////////////................................................
//////////////////////////////////////////////////////////////////................................................
//////////////////////////////////////////////////////////////////................................................
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_LeaveRoom)
{
	LeaveRoom_Func func;
	func.userName=loginUsername;
	roomsPlugin->ExecuteFunc(&func);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_SendInvite)
{
	SendInvite_Func func;
	func.userName=loginUsername;
	func.inviteeName=pparams[0].GetString();
	func.inviteToSpectatorSlot=pparams[1].GetBool();
	roomsPlugin->ExecuteFunc(&func);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_StartSpectating)
{
	StartSpectating_Func func;
	func.userName=loginUsername;
	roomsPlugin->ExecuteFunc(&func);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_StopSpectating)
{
	StopSpectating_Func func;
	func.userName=loginUsername;
	roomsPlugin->ExecuteFunc(&func);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_GrantModerator)
{
	GrantModerator_Func func;
	func.userName=loginUsername;
	func.newModerator=pparams[0].GetString();
	roomsPlugin->ExecuteFunc(&func);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_SetReadyStatus)
{
	SetReadyStatus_Func func;
	func.userName=loginUsername;
	func.isReady=pparams[0].GetBool();
	roomsPlugin->ExecuteFunc(&func);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_GetReadyStatus)
{
	GetReadyStatus_Func func;
	func.userName=loginUsername;
	roomsPlugin->ExecuteFunc(&func);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_SetRoomLockState)
{
	SetRoomLockState_Func func;
	func.userName=loginUsername;
	RakNet::RakString lockState=pparams[0].GetString();
	if (lockState=="NOT_LOCKED")
		func.roomLockState=RLS_NOT_LOCKED;
	else if (lockState=="PLAYERS_LOCKED")
		func.roomLockState=RLS_PLAYERS_LOCKED;
	else if (lockState=="ALL_LOCKED")
		func.roomLockState=RLS_ALL_LOCKED;
	else
	{
		RakAssert("Unknown lock state in RoomsBrowserGFx3_RakNet" && 0);
	}
	roomsPlugin->ExecuteFunc(&func);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_GetRoomLockState)
{
	GetRoomLockState_Func func;
	func.userName=loginUsername;
	roomsPlugin->ExecuteFunc(&func);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_AreAllMembersReady)
{
	AreAllMembersReady_Func func;
	func.userName=loginUsername;
	roomsPlugin->ExecuteFunc(&func);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_KickMember)
{
	KickMember_Func func;
	func.userName=loginUsername;
	func.kickedMember=pparams[0].GetString();
	func.reason=pparams[1].GetString();
	roomsPlugin->ExecuteFunc(&func);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_GetRoomProperties)
{
	GetRoomProperties_Func func;
	func.userName=loginUsername;
	roomsPlugin->ExecuteFunc(&func);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(RoomsBrowserGFx3_RakNet, f2c_StartGame)
{
	// User should override this to do start game functionality, if desired

	FxResponseArgs<1> rargs;
	rargs.Add("User must override RoomsBrowserGFx3_RakNet");
	FxDelegate::Invoke2(movie, "c2f_StartGame", rargs);

}
void RoomsBrowserGFx3_RakNet::Accept(CallbackProcessor* cbreg)
{
	RoomsBrowserGFx3::Accept(cbreg);

	cbreg->Process( "f2c_ConnectToServer", &::f2c_ConnectToServer );
	cbreg->Process( "f2c_Login", &::f2c_Login );
	cbreg->Process( "f2c_RegisterAccount", &::f2c_RegisterAccount );

	cbreg->Process( "f2c_UpdateRoomsList", &::f2c_UpdateRoomsList );
	cbreg->Process( "f2c_UpdateFriendsList", &::f2c_UpdateFriendsList );
	cbreg->Process( "f2c_JoinByFilter", &::f2c_JoinByFilter );
	cbreg->Process( "f2c_CreateRoom", &::f2c_CreateRoom );
	cbreg->Process( "f2c_Friends_SendInvite", &::f2c_Friends_SendInvite );
	cbreg->Process( "f2c_Friends_Remove", &::f2c_Friends_Remove );
	cbreg->Process( "f2c_Friends_AcceptInvite", &::f2c_Friends_AcceptInvite );
	cbreg->Process( "f2c_Friends_RejectInvite", &::f2c_Friends_RejectInvite );
	cbreg->Process( "f2c_Directed_Chat_Func", &::f2c_Directed_Chat_Func );
	cbreg->Process( "f2c_Room_Chat_Func", &::f2c_Room_Chat_Func );
	cbreg->Process( "f2c_Logoff", &::f2c_Logoff );

	cbreg->Process( "f2c_LeaveRoom", &::f2c_LeaveRoom );
	cbreg->Process( "f2c_SendInvite", &::f2c_SendInvite );
	cbreg->Process( "f2c_StartSpectating", &::f2c_StartSpectating );
	cbreg->Process( "f2c_StopSpectating", &::f2c_StopSpectating );
	cbreg->Process( "f2c_GrantModerator", &::f2c_GrantModerator );
	cbreg->Process( "f2c_SetReadyStatus", &::f2c_SetReadyStatus );
	cbreg->Process( "f2c_GetReadyStatus", &::f2c_GetReadyStatus );
	cbreg->Process( "f2c_SetRoomLockState", &::f2c_SetRoomLockState );
	cbreg->Process( "f2c_GetRoomLockState", &::f2c_GetRoomLockState );
	cbreg->Process( "f2c_AreAllMembersReady", &::f2c_AreAllMembersReady );
	cbreg->Process( "f2c_KickMember", &::f2c_KickMember );
	cbreg->Process( "f2c_GetRoomProperties", &::f2c_GetRoomProperties );
	cbreg->Process( "f2c_StartGame", &::f2c_StartGame );

}
void RoomsBrowserGFx3_RakNet::MessageResult(Client_Login *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnum(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Client_Login", rargs);
}
void RoomsBrowserGFx3_RakNet::MessageResult(RakNet::Client_Logoff *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnum(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Client_Logoff", rargs);

	rakPeer->CloseConnection(lobby2Client->GetServerAddress(), true);
}
void RoomsBrowserGFx3_RakNet::MessageResult(Client_RegisterAccount *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnum(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Client_RegisterAccount", rargs);
}
void RoomsBrowserGFx3_RakNet::CreateRoom_Callback( const SystemAddress &senderAddress, RakNet::CreateRoom_Func *callResult)
{
	if (senderAddress!=lobby2Client->GetServerAddress())
		return;

	FxResponseArgsList rargs;
	rargs.Add(RoomsErrorCodeDescription::ToEnum(callResult->resultCode));
	rargs.Add(false);
	FxDelegate::Invoke2(movie, "c2f_CreateRoom", rargs);
}
void RoomsBrowserGFx3_RakNet::SearchByFilter_Callback( const SystemAddress &senderAddress, RakNet::SearchByFilter_Func *callResult)
{
	if (senderAddress!=lobby2Client->GetServerAddress())
		return;

	FxResponseArgsList rargs;
	rargs.Add(RoomsErrorCodeDescription::ToEnum(callResult->resultCode));

	if (callResult->resultCode==REC_SUCCESS)
	{
		rargs.Add((Double)callResult->roomsOutput.Size());

		unsigned int i;
		for (i=0; i < callResult->roomsOutput.Size(); i++)
		{
			rargs.Add(true);
			rargs.Add((Double)callResult->roomsOutput[i]->GetProperty(DefaultRoomColumns::GetColumnName(DefaultRoomColumns::TC_ROOM_ID))->i);
			char ipAddr[64];
			callResult->roomsOutput[i]->roomMemberList[0].systemAddress.ToString(true,ipAddr);
			rargs.Add(ipAddr);
			rargs.Add(callResult->roomsOutput[i]->roomMemberList[0].name.C_String());
			rargs.Add((Double)callResult->roomsOutput[i]->roomMemberList.Size());
			rargs.Add((Double)callResult->roomsOutput[i]->GetProperty(DefaultRoomColumns::TC_TOTAL_SLOTS)->i);
			rargs.Add(callResult->roomsOutput[i]->GetProperty("mapName")->c);
		}
	}

	FxDelegate::Invoke2(movie, "c2f_SearchByFilter_Callback", rargs);
}
void RoomsBrowserGFx3_RakNet::JoinByFilter_Callback( const SystemAddress &senderAddress, RakNet::JoinByFilter_Func *callResult)
{
	if (senderAddress!=lobby2Client->GetServerAddress())
		return;

	FxResponseArgsList rargs;
	rargs.Add(RoomsErrorCodeDescription::ToEnum(callResult->resultCode));

	FxDelegate::Invoke2(movie, "c2f_JoinByFilter", rargs);
}
void RoomsBrowserGFx3_RakNet::Chat_Callback( const SystemAddress &senderAddress, Chat_Func *callResult)
{
	if (senderAddress!=lobby2Client->GetServerAddress())
		return;

	FxResponseArgs<3> rargs;
	rargs.Add(RoomsErrorCodeDescription::ToEnum(callResult->resultCode));
	rargs.Add(callResult->privateMessageRecipient.C_String());
	rargs.Add(callResult->chatMessage.C_String());

	FxDelegate::Invoke2(movie, "c2f_Chat_Callback", rargs);
}
void RoomsBrowserGFx3_RakNet::Chat_Callback( const SystemAddress &senderAddress, Chat_Notification *notification)
{
	if (senderAddress!=lobby2Client->GetServerAddress())
		return;

	FxResponseArgs<4> rargs;

	rargs.Add(notification->sender.C_String());
	rargs.Add(notification->privateMessageRecipient.C_String());
	rargs.Add(notification->chatMessage.C_String());
	rargs.Add(notification->filteredChatMessage.C_String());

	FxDelegate::Invoke2(movie, "c2f_Chat_Notification", rargs);
}
void WriteUsernameAndStatus(UsernameAndOnlineStatus &uaos, FxResponseArgsList &rargs)
{
	rargs.Add(uaos.handle.C_String());
	rargs.Add(uaos.isOnline);
	rargs.Add(uaos.presence.isVisible);
	rargs.Add(uaos.presence.titleNameOrID.C_String());
	rargs.Add(uaos.presence.statusString.C_String());
}
void RoomsBrowserGFx3_RakNet::MessageResult(RakNet::Friends_GetFriends *message)
{
	FxResponseArgsList rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnum(message->resultCode));

	unsigned int i;
	rargs.Add((Double) message->myFriends.Size());
	for (i=0; i < message->myFriends.Size(); i++)
	{
		WriteUsernameAndStatus(message->myFriends[i].usernameAndStatus, rargs);
	}

	FxDelegate::Invoke2(movie, "c2f_Friends_GetFriends", rargs);
}
void RoomsBrowserGFx3_RakNet::MessageResult(RakNet::Friends_GetInvites *message)
{
	FxResponseArgsList rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnum(message->resultCode));

	unsigned int i;
	rargs.Add((Double) message->invitesReceived.Size());
	for (i=0; i < message->invitesReceived.Size(); i++)
	{
		WriteUsernameAndStatus(message->invitesReceived[i].usernameAndStatus, rargs);
	}

	rargs.Add((Double) message->invitesSent.Size());
	for (i=0; i < message->invitesSent.Size(); i++)
	{
		WriteUsernameAndStatus(message->invitesSent[i].usernameAndStatus, rargs);
	}

	FxDelegate::Invoke2(movie, "c2f_Friends_GetInvites", rargs);
}
void RoomsBrowserGFx3_RakNet::MessageResult(RakNet::Friends_SendInvite *message)
{
	FxResponseArgs<2> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnum(message->resultCode));
	rargs.Add(message->targetHandle.C_String());

	FxDelegate::Invoke2(movie, "c2f_Friends_SendInvite", rargs);
}
void RoomsBrowserGFx3_RakNet::MessageResult(RakNet::Friends_AcceptInvite *message)
{
	FxResponseArgs<5> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnum(message->resultCode));
	rargs.Add(message->targetHandle.C_String());
	rargs.Add(message->presence.isVisible);
	rargs.Add(message->presence.titleNameOrID.C_String());
	rargs.Add(message->presence.statusString.C_String());

	FxDelegate::Invoke2(movie, "c2f_Friends_AcceptInvite", rargs);
}
void RoomsBrowserGFx3_RakNet::MessageResult(RakNet::Friends_RejectInvite *message)
{
	FxResponseArgs<2> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnum(message->resultCode));
	rargs.Add(message->targetHandle.C_String());

	FxDelegate::Invoke2(movie, "c2f_Friends_RejectInvite", rargs);
}
void RoomsBrowserGFx3_RakNet::MessageResult(RakNet::Friends_Remove *message)
{
	FxResponseArgs<2> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnum(message->resultCode));
	rargs.Add(message->targetHandle.C_String());

	FxDelegate::Invoke2(movie, "c2f_Friends_Remove", rargs);
}
void RoomsBrowserGFx3_RakNet::MessageResult(RakNet::Notification_Friends_PresenceUpdate *message)
{
	FxResponseArgs<4> rargs;
	rargs.Add(message->otherHandle.C_String());
	rargs.Add(message->newPresence.isVisible);
	rargs.Add(message->newPresence.titleNameOrID.C_String());
	rargs.Add(message->newPresence.statusString.C_String());

	FxDelegate::Invoke2(movie, "c2f_Notification_Friends_PresenceUpdate", rargs);
}
void RoomsBrowserGFx3_RakNet::MessageResult(RakNet::Notification_Friends_StatusChange *message)
{
	FxResponseArgs<5> rargs;
	rargs.Add(message->OpToString());
	rargs.Add(message->otherHandle.C_String());
	rargs.Add(message->presence.isVisible);
	rargs.Add(message->presence.titleNameOrID.C_String());
	rargs.Add(message->presence.statusString.C_String());

	FxDelegate::Invoke2(movie, "c2f_Notification_Friends_StatusChange", rargs);
}
//////////////////////////////////////////////////////////////////................................................
//////////////////////////////////////////////////////////////////................................................
//////////////////////////////////////////////////////////////////................................................
//////////////////////////////////////////////////////////////////................................................
//////////////////////////////////////////////////////////////////................................................
void RoomsBrowserGFx3_RakNet::LeaveRoom_Callback( const SystemAddress &senderAddress, RakNet::LeaveRoom_Func *callResult)
{
	FxResponseArgs<1> rargs;
	rargs.Add(RoomsErrorCodeDescription::ToEnum(callResult->resultCode));
	FxDelegate::Invoke2(movie, "c2f_LeaveRoom", rargs);
}
void RoomsBrowserGFx3_RakNet::SendInvite_Callback( const SystemAddress &senderAddress, RakNet::SendInvite_Func *callResult)
{
	FxResponseArgs<2> rargs;
	rargs.Add(RoomsErrorCodeDescription::ToEnum(callResult->resultCode));
	rargs.Add(callResult->inviteeName.C_String());
	rargs.Add(callResult->inviteToSpectatorSlot);
	FxDelegate::Invoke2(movie, "c2f_SendInvite", rargs);
}
void RoomsBrowserGFx3_RakNet::StartSpectating_Callback( const SystemAddress &senderAddress, RakNet::StartSpectating_Func *callResult)
{
	FxResponseArgs<1> rargs;
	rargs.Add(RoomsErrorCodeDescription::ToEnum(callResult->resultCode));
	FxDelegate::Invoke2(movie, "c2f_StartSpectating", rargs);
}
void RoomsBrowserGFx3_RakNet::StopSpectating_Callback( const SystemAddress &senderAddress, RakNet::StopSpectating_Func *callResult)
{
	FxResponseArgs<1> rargs;
	rargs.Add(RoomsErrorCodeDescription::ToEnum(callResult->resultCode));
	FxDelegate::Invoke2(movie, "c2f_StopSpectating", rargs);
}
void RoomsBrowserGFx3_RakNet::GrantModerator_Callback( const SystemAddress &senderAddress, RakNet::GrantModerator_Func *callResult)
{
	FxResponseArgs<2> rargs;
	rargs.Add(RoomsErrorCodeDescription::ToEnum(callResult->resultCode));
	rargs.Add(callResult->newModerator.C_String());
	FxDelegate::Invoke2(movie, "c2f_AreAllMembersReady", rargs);
}
void RoomsBrowserGFx3_RakNet::SetReadyStatus_Callback( const SystemAddress &senderAddress, RakNet::SetReadyStatus_Func *callResult)
{
	FxResponseArgsList rargs;
	rargs.Add(RoomsErrorCodeDescription::ToEnum(callResult->resultCode));
	rargs.Add(callResult->isReady);
	rargs.Add((Double) callResult->readyUsers.Size());
	for (unsigned int i=0; i < callResult->readyUsers.Size(); i++)
		rargs.Add(callResult->readyUsers[i].C_String());
	rargs.Add((Double) callResult->unreadyUsers.Size());
	for (unsigned int i=0; i < callResult->unreadyUsers.Size(); i++)
		rargs.Add(callResult->unreadyUsers[i].C_String());
	FxDelegate::Invoke2(movie, "c2f_SetReadyStatus", rargs);
}
void RoomsBrowserGFx3_RakNet::GetReadyStatus_Callback( const SystemAddress &senderAddress, RakNet::GetReadyStatus_Func *callResult)
{
	FxResponseArgsList rargs;
	rargs.Add(RoomsErrorCodeDescription::ToEnum(callResult->resultCode));
	for (unsigned int i=0; i < callResult->readyUsers.Size(); i++)
		rargs.Add(callResult->readyUsers[i].C_String());
	rargs.Add((Double) callResult->unreadyUsers.Size());
	for (unsigned int i=0; i < callResult->unreadyUsers.Size(); i++)
		rargs.Add(callResult->unreadyUsers[i].C_String());
	FxDelegate::Invoke2(movie, "c2f_GetReadyStatus", rargs);
}
void RoomsBrowserGFx3_RakNet::SetRoomLockState_Callback( const SystemAddress &senderAddress, RakNet::SetRoomLockState_Func *callResult)
{
	FxResponseArgs<2> rargs;
	rargs.Add(RoomsErrorCodeDescription::ToEnum(callResult->resultCode));
	if (callResult->roomLockState==RLS_NOT_LOCKED)
		rargs.Add("NOT_LOCKED"); // Anyone can join or leave
	else if (callResult->roomLockState==RLS_PLAYERS_LOCKED)
		rargs.Add("PLAYERS_LOCKED"); // Anyone can join as spectator or become spectator. New players are not allowed. You cannot leave spectator.
	else
		rargs.Add("ALL_LOCKED"); // No new players are allowed, and you cannot toggle spectator
	FxDelegate::Invoke2(movie, "c2f_SetRoomLockState", rargs);
}
void RoomsBrowserGFx3_RakNet::GetRoomLockState_Callback( const SystemAddress &senderAddress, RakNet::GetRoomLockState_Func *callResult)
{
	FxResponseArgs<2> rargs;
	rargs.Add(RoomsErrorCodeDescription::ToEnum(callResult->resultCode));
	if (callResult->roomLockState==RLS_NOT_LOCKED)
		rargs.Add("NOT_LOCKED"); // Anyone can join or leave
	else if (callResult->roomLockState==RLS_PLAYERS_LOCKED)
		rargs.Add("PLAYERS_LOCKED"); // Anyone can join as spectator or become spectator. New players are not allowed. You cannot leave spectator.
	else
		rargs.Add("ALL_LOCKED"); // No new players are allowed, and you cannot toggle spectator
	FxDelegate::Invoke2(movie, "c2f_GetRoomLockState", rargs);
}
void RoomsBrowserGFx3_RakNet::AreAllMembersReady_Callback( const SystemAddress &senderAddress, RakNet::AreAllMembersReady_Func *callResult)
{
	FxResponseArgs<2> rargs;
	rargs.Add(RoomsErrorCodeDescription::ToEnum(callResult->resultCode));
	rargs.Add(callResult->allReady);
	FxDelegate::Invoke2(movie, "c2f_AreAllMembersReady", rargs);
}
void RoomsBrowserGFx3_RakNet::KickMember_Callback( const SystemAddress &senderAddress, RakNet::KickMember_Func *callResult)
{
	FxResponseArgs<3> rargs;
	rargs.Add(RoomsErrorCodeDescription::ToEnum(callResult->resultCode));
	rargs.Add(callResult->kickedMember.C_String());
	rargs.Add(callResult->reason.C_String());
	FxDelegate::Invoke2(movie, "c2f_KickMember", rargs);
}
void RoomsBrowserGFx3_RakNet::GetRoomProperties_Callback( const SystemAddress &senderAddress, RakNet::GetRoomProperties_Func *callResult)
{
	char saString[32];
	char guidString[64];
	FxResponseArgsList rargs;
	rargs.Add(RoomsErrorCodeDescription::ToEnum(callResult->resultCode));
	rargs.Add(callResult->roomName.C_String());
	rargs.Add((Double)callResult->roomDescriptor.roomMemberList.Size());
	for (unsigned int i=0; i < callResult->roomDescriptor.roomMemberList.Size(); i++)
	{
		rargs.Add(callResult->roomDescriptor.roomMemberList[i].name.C_String());
		const char *c = RoomMemberModeToEnum(callResult->roomDescriptor.roomMemberList[i].roomMemberMode);
		rargs.Add(c);
		rargs.Add(callResult->roomDescriptor.roomMemberList[i].isReady);
		callResult->roomDescriptor.roomMemberList[i].systemAddress.ToString(true,saString);
		rargs.Add(saString);
		callResult->roomDescriptor.roomMemberList[i].guid.ToString(guidString);
		rargs.Add(guidString);
	}

	rargs.Add((Double)callResult->roomDescriptor.banList.Size());
	for (unsigned int i=0; i < callResult->roomDescriptor.banList.Size(); i++)
	{
		rargs.Add(callResult->roomDescriptor.banList[i].target.C_String());
		rargs.Add(callResult->roomDescriptor.banList[i].reason.C_String());
	}

	if (callResult->roomDescriptor.roomLockState==RLS_NOT_LOCKED)
		rargs.Add("NOT_LOCKED"); // Anyone can join or leave
	else if (callResult->roomDescriptor.roomLockState==RLS_PLAYERS_LOCKED)
		rargs.Add("PLAYERS_LOCKED"); // Anyone can join as spectator or become spectator. New players are not allowed. You cannot leave spectator.
	else
		rargs.Add("ALL_LOCKED"); // No new players are allowed, and you cannot toggle spectator

	rargs.Add((Double) callResult->roomDescriptor.lobbyRoomId);
	rargs.Add((Double) callResult->roomDescriptor.autoLockReadyStatus);
	rargs.Add((Double) callResult->roomDescriptor.hiddenFromSearches);

	rargs.Add(NetworkedRoomCreationParameters::SendInvitePermissionToEnum(callResult->roomDescriptor.inviteToRoomPermission));
	rargs.Add(NetworkedRoomCreationParameters::SendInvitePermissionToEnum(callResult->roomDescriptor.inviteToSpectatorSlotPermission));

	rargs.Add(callResult->roomDescriptor.GetProperty("mapName")->c);

	FxDelegate::Invoke2(movie, "c2f_GetRoomProperties", rargs);
}
void RoomsBrowserGFx3_RakNet::RoomMemberStartedSpectating_Callback( const SystemAddress &senderAddress, RakNet::RoomMemberStartedSpectating_Notification *notification)
{
	FxResponseArgs<1> rargs;
	rargs.Add(notification->userName.C_String());
	FxDelegate::Invoke2(movie, "c2f_RoomMemberStartedSpectating_Callback", rargs);
}
void RoomsBrowserGFx3_RakNet::RoomMemberStoppedSpectating_Callback( const SystemAddress &senderAddress, RakNet::RoomMemberStoppedSpectating_Notification *notification)
{
	FxResponseArgs<1> rargs;
	rargs.Add(notification->userName.C_String());
	FxDelegate::Invoke2(movie, "c2f_RoomMemberStoppedSpectating_Callback", rargs);
}
void RoomsBrowserGFx3_RakNet::ModeratorChanged_Callback( const SystemAddress &senderAddress, RakNet::ModeratorChanged_Notification *notification)
{
	FxResponseArgs<2> rargs;
	rargs.Add(notification->newModerator.C_String());
	rargs.Add(notification->oldModerator.C_String());
	FxDelegate::Invoke2(movie, "c2f_ModeratorChanged_Callback", rargs);
}
void RoomsBrowserGFx3_RakNet::RoomMemberReadyStatusSet_Callback( const SystemAddress &senderAddress, RakNet::RoomMemberReadyStatusSet_Notification *notification)
{
	FxResponseArgsList rargs;
	rargs.Add(notification->isReady);
	rargs.Add(notification->roomMember.C_String());
	rargs.Add((Double) notification->readyUsers.Size());
	for (unsigned int i=0; i < notification->readyUsers.Size(); i++)
		rargs.Add(notification->readyUsers[i].C_String());
	rargs.Add((Double) notification->unreadyUsers.Size());
	for (unsigned int i=0; i < notification->unreadyUsers.Size(); i++)
		rargs.Add(notification->unreadyUsers[i].C_String());
	FxDelegate::Invoke2(movie, "c2f_RoomMemberReadyStatusSet_Callback", rargs);
}
void RoomsBrowserGFx3_RakNet::RoomLockStateSet_Callback( const SystemAddress &senderAddress, RakNet::RoomLockStateSet_Notification *notification)
{
	FxResponseArgs<1> rargs;
	if (notification->roomLockState==RLS_NOT_LOCKED)
		rargs.Add("NOT_LOCKED"); // Anyone can join or leave
	else if (notification->roomLockState==RLS_PLAYERS_LOCKED)
		rargs.Add("PLAYERS_LOCKED"); // Anyone can join as spectator or become spectator. New players are not allowed. You cannot leave spectator.
	else
		rargs.Add("ALL_LOCKED"); // No new players are allowed, and you cannot toggle spectator
	FxDelegate::Invoke2(movie, "c2f_RoomLockStateSet_Callback", rargs);
}
void RoomsBrowserGFx3_RakNet::RoomMemberKicked_Callback( const SystemAddress &senderAddress, RakNet::RoomMemberKicked_Notification *notification)
{
	FxResponseArgs<3> rargs;
	rargs.Add(notification->kickedMember.C_String());
	rargs.Add(notification->moderator.C_String());
	rargs.Add(notification->reason.C_String());
	FxDelegate::Invoke2(movie, "c2f_RoomMemberKicked_Callback", rargs);
}
void RoomsBrowserGFx3_RakNet::RoomMemberLeftRoom_Callback( const SystemAddress &senderAddress, RakNet::RoomMemberLeftRoom_Notification *notification)
{
	FxResponseArgs<1> rargs;
	rargs.Add(notification->roomMember.C_String());
	FxDelegate::Invoke2(movie, "c2f_RoomMemberLeftRoom_Callback", rargs);
}
void RoomsBrowserGFx3_RakNet::RoomMemberJoinedRoom_Callback( const SystemAddress &senderAddress, RakNet::RoomMemberJoinedRoom_Notification *notification)
{
	char saString[32];
	FxResponseArgs<4> rargs;
	rargs.Add(notification->joinedRoomResult->acceptedInvitorName.C_String());
	notification->joinedRoomResult->acceptedInvitorAddress.ToString(true,saString);
	rargs.Add(saString);
	rargs.Add(notification->joinedRoomResult->joiningMemberName.C_String());
	notification->joinedRoomResult->joiningMemberAddress.ToString(true,saString);
	rargs.Add(saString);
	FxDelegate::Invoke2(movie, "c2f_RoomMemberJoinedRoom_Callback", rargs);
}
void RoomsBrowserGFx3_RakNet::RoomInvitationSent_Callback( const SystemAddress &senderAddress, RakNet::RoomInvitationSent_Notification *notification)
{
	FxResponseArgs<2> rargs;
	rargs.Add(notification->invitorName.C_String());
	rargs.Add(notification->inviteToSpectatorSlot);
	FxDelegate::Invoke2(movie, "c2f_RoomInvitationSent_Callback", rargs);
}
void RoomsBrowserGFx3_RakNet::RoomInvitationWithdrawn_Callback( const SystemAddress &senderAddress, RakNet::RoomInvitationWithdrawn_Notification *notification)
{
	FxResponseArgs<2> rargs;
	rargs.Add(notification->invitedUser.invitorName.C_String());
	char saString[32];
	notification->invitedUser.invitorSystemAddress.ToString(true,saString);
	rargs.Add(saString);
	FxDelegate::Invoke2(movie, "c2f_RoomInvitationWithdrawn_Callback", rargs);
}
void RoomsBrowserGFx3_RakNet::RoomDestroyedOnModeratorLeft_Callback( const SystemAddress &senderAddress, RakNet::RoomDestroyedOnModeratorLeft_Notification *notification)
{
	FxResponseArgs<1> rargs;
	rargs.Add(notification->oldModerator.C_String());
	FxDelegate::Invoke2(movie, "c2f_RoomDestroyedOnModeratorLeft_Callback", rargs);
}
