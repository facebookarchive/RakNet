/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __LOBBY_2_MESSAGE_H
#define __LOBBY_2_MESSAGE_H

#include "BitStream.h"
#include "Lobby2ResultCode.h"
#include "RakString.h"
#include "RakAssert.h"
#include "RakNetSmartPtr.h"
#include "SimpleMutex.h"
#include "Lobby2Presence.h"





#pragma once

namespace RakNet
{

struct Lobby2Callbacks;
class Lobby2Client;
class Lobby2Server;
struct BinaryDataBlock;
struct Lobby2ServerCommand;
class Lobby2Plugin;

const unsigned int L2_MAX_BINARY_DATA_LENGTH=1000000;


// --------------------------------------------- Enumeration list of all message IDs --------------------------------------------

/// All enumerations for Lobby2Message. Each Lobby2Message has one enumeration
/// \ingroup LOBBY_2_COMMANDS
enum Lobby2MessageID
{
	L2MID_Platform_Startup,
	L2MID_Platform_Shutdown,
	L2MID_System_CreateDatabase,
	L2MID_System_DestroyDatabase,
	L2MID_System_CreateTitle,
	L2MID_System_DestroyTitle,
	L2MID_System_GetTitleRequiredAge,
	L2MID_System_GetTitleBinaryData,
	L2MID_System_RegisterProfanity,
	L2MID_System_BanUser,
	L2MID_System_UnbanUser,
	L2MID_CDKey_Add,
	L2MID_CDKey_GetStatus,
	L2MID_CDKey_Use,
	L2MID_CDKey_FlagStolen,
	L2MID_Client_Login,
	L2MID_Client_Logoff,
	L2MID_Client_RegisterAccount,
	L2MID_System_SetEmailAddressValidated,
	L2MID_Client_ValidateHandle,
	L2MID_System_DeleteAccount,
	L2MID_System_PruneAccounts,
	L2MID_Client_GetEmailAddress,
	L2MID_Client_GetPasswordRecoveryQuestionByHandle,
	L2MID_Client_GetPasswordByPasswordRecoveryAnswer,
	L2MID_Client_ChangeHandle,
	L2MID_Client_UpdateAccount,
	L2MID_Client_GetAccountDetails,
	L2MID_Client_StartIgnore,
	L2MID_Client_StopIgnore,
	L2MID_Client_GetIgnoreList,
	L2MID_Client_PerTitleIntegerStorage,
	L2MID_Client_PerTitleBinaryStorage,
	L2MID_Client_SetPresence,
	L2MID_Client_GetPresence,
	L2MID_Friends_SendInvite,
	L2MID_Friends_AcceptInvite,
	L2MID_Friends_RejectInvite,
	L2MID_Friends_GetInvites,
	L2MID_Friends_GetFriends,
	L2MID_Friends_Remove,
	L2MID_BookmarkedUsers_Add,
	L2MID_BookmarkedUsers_Remove,
	L2MID_BookmarkedUsers_Get,
	L2MID_Emails_Send,
	L2MID_Emails_Get,
	L2MID_Emails_Delete,
	L2MID_Emails_SetStatus,
	L2MID_Ranking_SubmitMatch,
	L2MID_Ranking_GetMatches,
	L2MID_Ranking_GetMatchBinaryData,
	L2MID_Ranking_GetTotalScore,
	L2MID_Ranking_WipeScoresForPlayer,
	L2MID_Ranking_WipeMatches,
	L2MID_Ranking_PruneMatches,
	L2MID_Ranking_UpdateRating,
	L2MID_Ranking_WipeRatings,
	L2MID_Ranking_GetRating,
	L2MID_Clans_Create,
	L2MID_Clans_SetProperties,
	L2MID_Clans_GetProperties,
	L2MID_Clans_SetMyMemberProperties,
	L2MID_Clans_GrantLeader,
	L2MID_Clans_SetSubleaderStatus,
	L2MID_Clans_SetMemberRank,
	L2MID_Clans_GetMemberProperties,
	L2MID_Clans_ChangeHandle,
	L2MID_Clans_Leave,
	L2MID_Clans_Get,
	L2MID_Clans_SendJoinInvitation,
	L2MID_Clans_WithdrawJoinInvitation,
	L2MID_Clans_AcceptJoinInvitation,
	L2MID_Clans_RejectJoinInvitation,
	L2MID_Clans_DownloadInvitationList,
	L2MID_Clans_SendJoinRequest,
	L2MID_Clans_WithdrawJoinRequest,
	L2MID_Clans_AcceptJoinRequest,
	L2MID_Clans_RejectJoinRequest,
	L2MID_Clans_DownloadRequestList,
	L2MID_Clans_KickAndBlacklistUser,
	L2MID_Clans_UnblacklistUser,
	L2MID_Clans_GetBlacklist,
	L2MID_Clans_GetMembers,
	L2MID_Clans_GetList,
	L2MID_Clans_CreateBoard,
	L2MID_Clans_DestroyBoard,
	L2MID_Clans_CreateNewTopic,
	L2MID_Clans_ReplyToTopic,
	L2MID_Clans_RemovePost,
	L2MID_Clans_GetBoards,
	L2MID_Clans_GetTopics,
	L2MID_Clans_GetPosts,
	L2MID_Console_GameBootCheck,
	L2MID_Console_GetGameBootInviteDetails,
	L2MID_Console_GetServerStatus,
	L2MID_Console_GetWorldListFromServer,
	L2MID_Console_GetLobbyListFromWorld,
	L2MID_Console_JoinLobby,
	L2MID_Console_LeaveLobby,
	L2MID_Console_SendLobbyChatMessage,
	L2MID_Console_SearchRooms,
	L2MID_Console_GetRoomDetails,
	L2MID_Console_GetLobbyMemberData,
	L2MID_Console_CreateRoom,
	L2MID_Console_SignIntoRoom,
	L2MID_Console_SetRoomSearchProperties,
	L2MID_Console_UpdateRoomParameters,
	L2MID_Console_JoinRoom,
	L2MID_Console_LeaveRoom,
	L2MID_Console_SendLobbyInvitationToRoom,
	L2MID_Console_SendGUIInvitationToRoom,
	L2MID_Console_SendDataMessageToUser,
	L2MID_Console_SendRoomChatMessage,
	L2MID_Console_ShowFriendsUI,
	L2MID_Console_EndGame,
	L2MID_Console_StartGame,
	L2MID_Console_ShowPartyUI,
	L2MID_Console_ShowMessagesUI,
	L2MID_Console_ShowGUIInvitationsToRooms,
	L2MID_Console_EnableDisableRoomVoiceChat,
	L2MID_Notification_Client_RemoteLogin,
	L2MID_Notification_Client_IgnoreStatus,
	L2MID_Notification_Friends_StatusChange,
	L2MID_Notification_Friends_PresenceUpdate,
	L2MID_Notification_User_ChangedHandle,
	L2MID_Notification_Friends_CreatedClan,
	L2MID_Notification_Emails_Received,
	L2MID_Notification_Clans_GrantLeader,
	L2MID_Notification_Clans_SetSubleaderStatus,
	L2MID_Notification_Clans_SetMemberRank,
	L2MID_Notification_Clans_ChangeHandle,
	L2MID_Notification_Clans_Leave,
	L2MID_Notification_Clans_PendingJoinStatus,
	L2MID_Notification_Clans_NewClanMember,
	L2MID_Notification_Clans_KickAndBlacklistUser,
	L2MID_Notification_Clans_UnblacklistUser,
	L2MID_Notification_Clans_Destroyed,
	L2MID_Notification_Console_CableDisconnected,
	L2MID_Notification_Console_ContextError,
	L2MID_Notification_Console_MemberJoinedLobby,
	L2MID_Notification_Console_MemberLeftLobby,
	L2MID_Notification_Console_LobbyDestroyed,
	L2MID_Notification_Console_LobbyMemberDataUpdated,
	L2MID_Notification_Console_LobbyGotChatMessage,
	L2MID_Notification_Console_LobbyGotRoomInvitation,
	L2MID_Notification_Console_MemberJoinedRoom,
	L2MID_Notification_Console_MemberLeftRoom,
	L2MID_Notification_Console_KickedOutOfRoom,
	L2MID_Notification_Console_RoomWasDestroyed,
	L2MID_Notification_Console_UpdateRoomParameters,
	L2MID_Notification_Console_RoomOwnerChanged,
	L2MID_Notification_Console_RoomChatMessage,
	L2MID_Notification_Console_RoomMessage,
	L2MID_Notification_Console_ChatEvent,
	L2MID_Notification_Console_MuteListChanged,
	L2MID_Notification_Console_Local_Users_Changed,
	L2MID_Notification_ReceivedDataMessageFromUser,
	L2MID_Notification_Console_MemberJoinedParty,
	L2MID_Notification_Console_MemberLeftParty,
	L2MID_Notification_Console_Game_Started, // XBOX only
	L2MID_Notification_Console_Game_Ending, // XBOX only
	L2MID_Notification_Console_Game_Ended, // XBOX only
	L2MID_Notification_Console_Got_Room_Invite,
	L2MID_Notification_Console_Accepted_Room_Invite,

	L2MID_COUNT,
};

// Should match tab;e lobby2.clanMemberStates
/// \ingroup LOBBY_2_COMMANDS
enum ClanMemberState
{
	CMD_UNDEFINED=0,
	CMD_ACTIVE,
	CMD_BANNED,
	CMD_JOIN_INVITED,
	CMD_JOIN_REQUESTED,
};
// --------------------------------------------- Base class for all messages (functions and notifications --------------------------------------------

/// \brief A Lobby2Message encapsulates a networked function call from the client.
/// \details The client should fill in the input parameters, call Lobby2Client::SendMsg(), and wait for the reply in the callback passed to Lobby2Client::SetCallbackInterface()
/// The input parameters are always serialized back from the server.
/// See resultCode for the result of the operation. L2RC_SUCCESS means success. Anything else means failure.
/// Any message may return between L2RC_NOT_LOGGED_IN and L2RC_EMAIL_ADDRESS_IS_INVALID, which indices formatting errors in the input.
/// All other return codes have the name of the message in the enumeration.
/// The system can be extended by deriving from Lobby2Message, adding your own input and output parameters, and deriving from Lobby2MessageFactory register your own class factory with RakNet::Lobby2Plugin::SetMessageFactory()
/// \ingroup LOBBY_2_COMMANDS
struct Lobby2Message
{
	Lobby2Message();
	virtual ~Lobby2Message() {}

	/// Every message has an ID identifying it across the network
	virtual Lobby2MessageID GetID(void) const=0;

	/// Is this message something that should only be run by a system with admin privileges?
	/// Set admin privileges with Lobby2Server::AddAdminAddress()
	virtual bool RequiresAdmin(void) const=0;

	/// Is this message something that should only be run by a system with ranking upload priviledges?
	/// Set ranking privileges with Lobby2Server::AddRankingAddress()
	virtual bool RequiresRankingPermission(void) const=0;

	/// Should this message not be processed on the server if the requesting user disconnects before it completes?
	/// This should be true for functions that only return data. False for functions that affect other users, or change the database
	virtual bool CancelOnDisconnect(void) const=0;

	/// Does this function require logging into the server before it can be executed?
	/// If true, the user id and user handle will be automatically inferred by the last login by looking up the sender's system address.
	/// If false, the message should include the username so the database query can lookup which user is performing this operation.
	virtual bool RequiresLogin(void) const=0;
	
	// Serialize data in this class. Currently just the resultCode
	void SerializeBase(bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream);
	
	/// Overridable serialization of the contents of this message. Defaults to SerializeBase()
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	
	/// If data members can be validated for correctness in the server's main thread, override this function and do those checks here.
	/// \return True for input OK. False if the input is bad and does not need to be further processed in the database threads.
	virtual bool PrevalidateInput(void);
	
	/// Override to do any Lobby2Client functionality when the message is returned from the server (usually nothing).
	/// \return True to call CallCallback immediately. False to defer for some reason (always true on the PC)
	virtual bool ClientImpl( RakNet::Lobby2Plugin *client);
	
	/// This message has been processed by the server and has arrived back on the client.
	/// Call the client informing the user of this event.
	virtual void CallCallback(Lobby2Callbacks *cb)=0;
	
	/// Do any Lobby2Server	functionality when the message first arrives on the server, and after it has returned true from PrevalidateInput()
	/// If it returns true, the message has been handled, and the result is sent to the client
	/// If it returns false, the message continues to ServerDBImpl
	virtual bool ServerPreDBMemoryImpl( Lobby2Server *server, RakString userHandle );

	/// Do any Lobby2Server	functionality after the message has been processed by the database, in the server thread.
	virtual void ServerPostDBMemoryImpl( Lobby2Server *server, RakString userHandle );
	
	/// Do any Lobby2Server	functionality when the message is processed in a database thread on the server.
	/// It is safe to do slow database calls in this function.
	/// If it returns true, the message has been handled, and the result is sent to the client
	/// If it returns false, that means ignore the message
	virtual bool ServerDBImpl( Lobby2ServerCommand *command, void *databaseInterface );

	/// The only non-printable character is space
	/// Cannot start with space
	/// Cannot end with space
	/// Cannot have two spaces in a row
	bool ValidateHandle( RakNet::RakString *handle );

	/// Binary data cannot be longer than L2_MAX_BINARY_DATA_LENGTH
	bool ValidateBinary( RakNetSmartPtr<BinaryDataBlock>binaryDataBlock);

	/// Required text cannot be empty.
	bool ValidateRequiredText( RakNet::RakString *text );

	/// Passwords must contain at least 5 characters
	bool ValidatePassword( RakNet::RakString *text );

	/// Check email address format
	bool ValidateEmailAddress( RakNet::RakString *text );

	/// Convert the enumeration representing this message to a string, and return it. Done automatically by macros.
	virtual const char *GetName(void) const=0;
	
	/// Write the result of this message to out(). Done automatically by macros.
	virtual void DebugMsg(RakNet::RakString &out) const=0;

	/// Print the result of DebugMsg
	virtual void DebugPrintf(void) const
	{
		RakNet::RakString out; DebugMsg(out); printf(out.C_String());
	}
	
	/// Result of the operation. L2RC_SUCCESS means the result completed. Anything else means an error
	RakNet::Lobby2ResultCode resultCode;

	// For polling, when necessary
	virtual bool WasCompleted( RakNet::Lobby2Plugin *client ) {(void) client; return false;}

	// Is this message a notification / callback?
	virtual bool IsNotification(void) const {return false;}

	/// Just a number, representing which instance of Lobby2Callbacks should process the result of this operation
	/// -1 means all
	uint32_t callbackId;

	/// Used for consoles
	int extendedResultCode;

	void AddRef(void) {refCountMutex.Lock(); refCount++; refCountMutex.Unlock(); }
	void Deref(void) {refCountMutex.Lock(); refCount--; refCountMutex.Unlock();}
	int GetRefCount(void) {int r; refCountMutex.Lock(); r = refCount; refCountMutex.Unlock(); return r;}

	/// Just a number, uniquely identifying each allocation of Lobby2Message.
	/// Use it if you need to lookup queries on the callback reply



	uint64_t requestId;






private:

	SimpleMutex refCountMutex;
	/// Reference count
	int refCount;
};

// --------------------------------------------- Prototypes --------------------------------------------

struct Platform_Startup;
struct Platform_Shutdown;
struct System_CreateDatabase;
struct System_DestroyDatabase;
struct System_CreateTitle;
struct System_DestroyTitle;
struct System_GetTitleRequiredAge;
struct System_GetTitleBinaryData;
struct System_RegisterProfanity;
struct System_BanUser;
struct System_UnbanUser;
struct CDKey_Add;
struct CDKey_GetStatus;
struct CDKey_Use;
struct CDKey_FlagStolen;
struct Client_Login;
struct Client_Logoff;
struct Client_RegisterAccount;
struct System_SetEmailAddressValidated;
struct Client_ValidateHandle;
struct System_DeleteAccount;
struct System_PruneAccounts;
struct Client_GetEmailAddress;
struct Client_GetPasswordRecoveryQuestionByHandle;
struct Client_GetPasswordByPasswordRecoveryAnswer;
struct Client_ChangeHandle;
struct Client_UpdateAccount;
struct Client_GetAccountDetails;
struct Client_StartIgnore;
struct Client_StopIgnore;
struct Client_GetIgnoreList;
struct Client_PerTitleIntegerStorage;
struct Client_PerTitleBinaryStorage;
struct Client_SetPresence;
struct Client_GetPresence;
struct Friends_SendInvite;
struct Friends_AcceptInvite;
struct Friends_RejectInvite;
struct Friends_GetInvites;
struct Friends_GetFriends;
struct Friends_Remove;
struct BookmarkedUsers_Add;
struct BookmarkedUsers_Remove;
struct BookmarkedUsers_Get;
struct Emails_Send;
struct Emails_Get;
struct Emails_Delete;
struct Emails_SetStatus;
struct Ranking_SubmitMatch;
struct Ranking_GetMatches;
struct Ranking_GetMatchBinaryData;
struct Ranking_GetTotalScore;
struct Ranking_WipeScoresForPlayer;
struct Ranking_WipeMatches;
struct Ranking_PruneMatches;
struct Ranking_UpdateRating;
struct Ranking_WipeRatings;
struct Ranking_GetRating;
struct Clans_Create;
struct Clans_SetProperties;
struct Clans_GetProperties;
struct Clans_SetMyMemberProperties;
struct Clans_GrantLeader;
struct Clans_SetSubleaderStatus;
struct Clans_SetMemberRank;
struct Clans_GetMemberProperties;
struct Clans_ChangeHandle;
struct Clans_Leave;
struct Clans_Get;
struct Clans_SendJoinInvitation;
struct Clans_WithdrawJoinInvitation;
struct Clans_AcceptJoinInvitation;
struct Clans_RejectJoinInvitation;
struct Clans_DownloadInvitationList;
struct Clans_SendJoinRequest;
struct Clans_WithdrawJoinRequest;
struct Clans_AcceptJoinRequest;
struct Clans_RejectJoinRequest;
struct Clans_DownloadRequestList;
struct Clans_KickAndBlacklistUser;
struct Clans_UnblacklistUser;
struct Clans_GetBlacklist;
struct Clans_GetMembers;
struct Clans_GetList;
struct Clans_CreateBoard;
struct Clans_DestroyBoard;
struct Clans_CreateNewTopic;
struct Clans_ReplyToTopic;
struct Clans_RemovePost;
struct Clans_GetBoards;
struct Clans_GetTopics;
struct Clans_GetPosts;
struct Console_GameBootCheck;
struct Console_GetGameBootInviteDetails;
struct Console_GetServerStatus;
struct Console_GetWorldListFromServer;
struct Console_GetLobbyListFromWorld;
struct Console_JoinLobby;
struct Console_LeaveLobby;
struct Console_SendLobbyChatMessage;
struct Console_SearchRooms;
struct Console_GetRoomDetails;
struct Console_GetLobbyMemberData;
struct Console_CreateRoom;
struct Console_SignIntoRoom;
struct Console_SetRoomSearchProperties;
struct Console_UpdateRoomParameters;
struct Console_JoinRoom;
struct Console_LeaveRoom;
struct Console_SendLobbyInvitationToRoom;
struct Console_SendGUIInvitationToRoom;
struct Console_SendDataMessageToUser;
struct Console_SendRoomChatMessage;
struct Console_ShowFriendsUI;
struct Console_EndGame;
struct Console_StartGame;
struct Console_ShowPartyUI;
struct Console_ShowMessagesUI;
struct Console_ShowGUIInvitationsToRooms;
struct Console_EnableDisableRoomVoiceChat;
struct Notification_Client_RemoteLogin;
struct Notification_Client_IgnoreStatus;
struct Notification_Friends_StatusChange;
struct Notification_Friends_PresenceUpdate;
struct Notification_User_ChangedHandle;
struct Notification_Friends_CreatedClan;
struct Notification_Emails_Received;
struct Notification_Clans_GrantLeader;
struct Notification_Clans_SetSubleaderStatus;
struct Notification_Clans_SetMemberRank;
struct Notification_Clans_ChangeHandle;
struct Notification_Clans_Leave;
struct Notification_Clans_PendingJoinStatus;
struct Notification_Clans_NewClanMember;
struct Notification_Clans_KickAndBlacklistUser;
struct Notification_Clans_UnblacklistUser;
struct Notification_Clans_Destroyed;
struct Notification_Console_CableDisconnected;
struct Notification_Console_ContextError;
struct Notification_Console_MemberJoinedLobby;
struct Notification_Console_MemberLeftLobby;
struct Notification_Console_LobbyDestroyed;
struct Notification_Console_LobbyMemberDataUpdated;
struct Notification_Console_LobbyGotChatMessage;
struct Notification_Console_LobbyGotRoomInvitation;
struct Notification_Console_MemberJoinedRoom;
struct Notification_Console_MemberLeftRoom;
struct Notification_Console_KickedOutOfRoom;
struct Notification_Console_RoomWasDestroyed;
struct Notification_Console_UpdateRoomParameters;
struct Notification_Console_RoomOwnerChanged;
struct Notification_Console_RoomChatMessage;
struct Notification_Console_RoomMessage;
//struct Notification_Console_RoomMemberConnectivityUpdate;
struct Notification_Console_ChatEvent;
struct Notification_Console_MuteListChanged;
struct Notification_Console_Local_Users_Changed;
struct Notification_ReceivedDataMessageFromUser;
struct Notification_Console_MemberJoinedParty;
struct Notification_Console_MemberLeftParty;
struct Notification_Console_Game_Started;
struct Notification_Console_Game_Ending;
struct Notification_Console_Game_Ended;
struct Notification_Console_Got_Room_Invite;
struct Notification_Console_Accepted_Room_Invite;

// --------------------------------------------- Callback interface for all messages, notifies the user --------------------------------------------

/// Every Lobby2Message processed with Lobby2Client::SendMsg() while connected will call the callback registered with Lobby2Client::SetCallbackInterface().
/// \ingroup LOBBY_2_GROUP
struct Lobby2Callbacks
{
	Lobby2Callbacks() {callbackId=nextCallbackId++;}
	virtual ~Lobby2Callbacks() {}
	virtual void MessageResult(Platform_Startup *message);
	virtual void MessageResult(Platform_Shutdown *message);
	virtual void MessageResult(System_CreateDatabase *message);
	virtual void MessageResult(System_DestroyDatabase *message);
	virtual void MessageResult(System_CreateTitle *message);
	virtual void MessageResult(System_DestroyTitle *message);
	virtual void MessageResult(System_GetTitleRequiredAge *message);
	virtual void MessageResult(System_GetTitleBinaryData *message);
	virtual void MessageResult(System_RegisterProfanity *message);
	virtual void MessageResult(System_BanUser *message);
	virtual void MessageResult(System_UnbanUser *message);
	virtual void MessageResult(CDKey_Add *message);
	virtual void MessageResult(CDKey_GetStatus *message);
	virtual void MessageResult(CDKey_Use *message);
	virtual void MessageResult(CDKey_FlagStolen *message);
	virtual void MessageResult(Client_Login *message);
	virtual void MessageResult(Client_Logoff *message);
	virtual void MessageResult(Client_RegisterAccount *message);
	virtual void MessageResult(System_SetEmailAddressValidated *message);
	virtual void MessageResult(Client_ValidateHandle *message);
	virtual void MessageResult(System_DeleteAccount *message);
	virtual void MessageResult(System_PruneAccounts *message);
	virtual void MessageResult(Client_GetEmailAddress *message);
	virtual void MessageResult(Client_GetPasswordRecoveryQuestionByHandle *message);
	virtual void MessageResult(Client_GetPasswordByPasswordRecoveryAnswer *message);
	virtual void MessageResult(Client_ChangeHandle *message);
	virtual void MessageResult(Client_UpdateAccount *message);
	virtual void MessageResult(Client_GetAccountDetails *message);
	virtual void MessageResult(Client_StartIgnore *message);
	virtual void MessageResult(Client_StopIgnore *message);
	virtual void MessageResult(Client_GetIgnoreList *message);
	virtual void MessageResult(Client_PerTitleIntegerStorage *message);
	virtual void MessageResult(Client_PerTitleBinaryStorage *message);
	virtual void MessageResult(Client_SetPresence *message);
	virtual void MessageResult(Client_GetPresence *message);
	virtual void MessageResult(Friends_SendInvite *message);
	virtual void MessageResult(Friends_AcceptInvite *message);
	virtual void MessageResult(Friends_RejectInvite *message);
	virtual void MessageResult(Friends_GetInvites *message);
	virtual void MessageResult(Friends_GetFriends *message);
	virtual void MessageResult(Friends_Remove *message);
	virtual void MessageResult(BookmarkedUsers_Add *message);
	virtual void MessageResult(BookmarkedUsers_Remove *message);
	virtual void MessageResult(BookmarkedUsers_Get *message);
	virtual void MessageResult(Emails_Send *message);
	virtual void MessageResult(Emails_Get *message);
	virtual void MessageResult(Emails_Delete *message);
	virtual void MessageResult(Emails_SetStatus *message);
	virtual void MessageResult(Ranking_SubmitMatch *message);
	virtual void MessageResult(Ranking_GetMatches *message);
	virtual void MessageResult(Ranking_GetMatchBinaryData *message);
	virtual void MessageResult(Ranking_GetTotalScore *message);
	virtual void MessageResult(Ranking_WipeScoresForPlayer *message);
	virtual void MessageResult(Ranking_WipeMatches *message);
	virtual void MessageResult(Ranking_PruneMatches *message);
	virtual void MessageResult(Ranking_UpdateRating *message);
	virtual void MessageResult(Ranking_WipeRatings *message);
	virtual void MessageResult(Ranking_GetRating *message);
	virtual void MessageResult(Clans_Create *message);
	virtual void MessageResult(Clans_SetProperties *message);
	virtual void MessageResult(Clans_GetProperties *message);
	virtual void MessageResult(Clans_SetMyMemberProperties *message);
	virtual void MessageResult(Clans_GrantLeader *message);
	virtual void MessageResult(Clans_SetSubleaderStatus *message);
	virtual void MessageResult(Clans_SetMemberRank *message);
	virtual void MessageResult(Clans_GetMemberProperties *message);
	virtual void MessageResult(Clans_ChangeHandle *message);
	virtual void MessageResult(Clans_Leave *message);
	virtual void MessageResult(Clans_Get *message);
	virtual void MessageResult(Clans_SendJoinInvitation *message);
	virtual void MessageResult(Clans_WithdrawJoinInvitation *message);
	virtual void MessageResult(Clans_AcceptJoinInvitation *message);
	virtual void MessageResult(Clans_RejectJoinInvitation *message);
	virtual void MessageResult(Clans_DownloadInvitationList *message);
	virtual void MessageResult(Clans_SendJoinRequest *message);
	virtual void MessageResult(Clans_WithdrawJoinRequest *message);
	virtual void MessageResult(Clans_AcceptJoinRequest *message);
	virtual void MessageResult(Clans_RejectJoinRequest *message);
	virtual void MessageResult(Clans_DownloadRequestList *message);
	virtual void MessageResult(Clans_KickAndBlacklistUser *message);
	virtual void MessageResult(Clans_UnblacklistUser *message);
	virtual void MessageResult(Clans_GetBlacklist *message);
	virtual void MessageResult(Clans_GetMembers *message);
	virtual void MessageResult(Clans_GetList *message);
	virtual void MessageResult(Clans_CreateBoard *message);
	virtual void MessageResult(Clans_DestroyBoard *message);
	virtual void MessageResult(Clans_CreateNewTopic *message);
	virtual void MessageResult(Clans_ReplyToTopic *message);
	virtual void MessageResult(Clans_RemovePost *message);
	virtual void MessageResult(Clans_GetBoards *message);
	virtual void MessageResult(Clans_GetTopics *message);
	virtual void MessageResult(Clans_GetPosts *message);
	virtual void MessageResult(Console_GameBootCheck *message);
	virtual void MessageResult(Console_GetGameBootInviteDetails *message);
	virtual void MessageResult(Console_GetServerStatus *message);
	virtual void MessageResult(Console_GetWorldListFromServer *message);
	virtual void MessageResult(Console_GetLobbyListFromWorld *message);
	virtual void MessageResult(Console_JoinLobby *message);
	virtual void MessageResult(Console_LeaveLobby *message);
	virtual void MessageResult(Console_SendLobbyChatMessage *message);
	virtual void MessageResult(Console_SearchRooms *message);
	virtual void MessageResult(Console_GetRoomDetails *message);
	virtual void MessageResult(Console_GetLobbyMemberData *message);
	virtual void MessageResult(Console_CreateRoom *message);
	virtual void MessageResult(Console_SignIntoRoom *message);
	virtual void MessageResult(Console_SetRoomSearchProperties *message);
	virtual void MessageResult(Console_UpdateRoomParameters *message);
	virtual void MessageResult(Console_JoinRoom *message);
	virtual void MessageResult(Console_LeaveRoom *message);
	virtual void MessageResult(Console_SendLobbyInvitationToRoom *message);
	virtual void MessageResult(Console_SendGUIInvitationToRoom *message);
	virtual void MessageResult(Console_SendDataMessageToUser *message);
	virtual void MessageResult(Console_SendRoomChatMessage *message);
	virtual void MessageResult(Console_ShowFriendsUI *message);
	virtual void MessageResult(Console_EndGame *message);
	virtual void MessageResult(Console_StartGame *message);
	virtual void MessageResult(Console_ShowPartyUI *message);
	virtual void MessageResult(Console_ShowMessagesUI *message);
	virtual void MessageResult(Console_ShowGUIInvitationsToRooms *message);
	virtual void MessageResult(Console_EnableDisableRoomVoiceChat *message);
	virtual void MessageResult(Notification_Client_RemoteLogin *message);
	virtual void MessageResult(Notification_Client_IgnoreStatus *message);
	virtual void MessageResult(Notification_Friends_StatusChange *message);
	virtual void MessageResult(Notification_Friends_PresenceUpdate *message);
	virtual void MessageResult(Notification_User_ChangedHandle *message);
	virtual void MessageResult(Notification_Friends_CreatedClan *message);
	virtual void MessageResult(Notification_Emails_Received *message);
	virtual void MessageResult(Notification_Clans_GrantLeader *message);
	virtual void MessageResult(Notification_Clans_SetSubleaderStatus *message);
	virtual void MessageResult(Notification_Clans_SetMemberRank *message);
	virtual void MessageResult(Notification_Clans_ChangeHandle *message);
	virtual void MessageResult(Notification_Clans_Leave *message);
	virtual void MessageResult(Notification_Clans_PendingJoinStatus *message);
	virtual void MessageResult(Notification_Clans_NewClanMember *message);
	virtual void MessageResult(Notification_Clans_KickAndBlacklistUser *message);
	virtual void MessageResult(Notification_Clans_UnblacklistUser *message);
	virtual void MessageResult(Notification_Clans_Destroyed *message);
	virtual void MessageResult(Notification_Console_CableDisconnected *message);
	virtual void MessageResult(Notification_Console_ContextError *message);
	virtual void MessageResult(Notification_Console_MemberJoinedLobby *message);
	virtual void MessageResult(Notification_Console_MemberLeftLobby *message);
	virtual void MessageResult(Notification_Console_LobbyDestroyed *message);
	virtual void MessageResult(Notification_Console_LobbyMemberDataUpdated *message);
	virtual void MessageResult(Notification_Console_LobbyGotChatMessage *message);
	virtual void MessageResult(Notification_Console_LobbyGotRoomInvitation *message);
	virtual void MessageResult(Notification_Console_MemberJoinedRoom *message);
	virtual void MessageResult(Notification_Console_MemberLeftRoom *message);
	virtual void MessageResult(Notification_Console_KickedOutOfRoom *message);
	virtual void MessageResult(Notification_Console_RoomWasDestroyed *message);
	virtual void MessageResult(Notification_Console_UpdateRoomParameters *message);
	virtual void MessageResult(Notification_Console_RoomOwnerChanged *message);
	virtual void MessageResult(Notification_Console_RoomChatMessage *message);
	virtual void MessageResult(Notification_Console_RoomMessage *message);
//	virtual void MessageResult(Notification_Console_RoomMemberConnectivityUpdate *message);
	virtual void MessageResult(Notification_Console_ChatEvent *message);
	virtual void MessageResult(Notification_Console_MuteListChanged *message);
	virtual void MessageResult(Notification_Console_Local_Users_Changed *message);
	virtual void MessageResult(Notification_ReceivedDataMessageFromUser *message);
	virtual void MessageResult(Notification_Console_MemberJoinedParty *message);
	virtual void MessageResult(Notification_Console_MemberLeftParty *message);
	virtual void MessageResult(Notification_Console_Game_Started *message);
	virtual void MessageResult(Notification_Console_Game_Ending *message);
	virtual void MessageResult(Notification_Console_Game_Ended *message);
	virtual void MessageResult(Notification_Console_Got_Room_Invite *message);
	virtual void MessageResult(Notification_Console_Accepted_Room_Invite *message);

	virtual void ExecuteDefaultResult(Lobby2Message *message) { (void)message; }

	uint32_t callbackId;
	static uint32_t nextCallbackId;
};

/// Just print out the name of the message by default. This class is used in the sample.
/// \ingroup LOBBY_2_GROUP
struct Lobby2Printf : public Lobby2Callbacks
{
	virtual void ExecuteDefaultResult(Lobby2Message *message) {message->DebugPrintf();}
};

// --------------------------------------------- Types --------------------------------------------

struct BinaryDataBlock
{
	char *binaryData;
	unsigned int binaryDataLength;
	BinaryDataBlock() {binaryData=0; binaryDataLength=0;}
	~BinaryDataBlock() {
		if (binaryData)
			rakFree_Ex(binaryData, _FILE_AND_LINE_ );
	}
	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Used to unify different platforms for room search and search properties. Only applies if specifically used
struct IndexedIntegerValue
{
	IndexedIntegerValue() {index=0; value=0; searchOperator=-1;}
	IndexedIntegerValue(unsigned int idx, unsigned int val) : index(idx), value(val) {searchOperator=-1;}
	/// Index of the value to set, required.
	unsigned int index;
	/// Value to set
	unsigned int value;
	/// Used for room searches only, -1 means use default equals
	int searchOperator;
};
/// Used to unify different platforms for room search and search properties. Only applies if specifically used
struct IndexedBinaryValue
{
	IndexedBinaryValue() {index=0; value=0; valueByteLength=0; searchOperator=-1; type=(unsigned char)-1;}
	IndexedBinaryValue(unsigned int idx, char* val, unsigned short valLength) : index(idx), value(val), valueByteLength(valLength) {searchOperator=-1; type=(unsigned char)-1;}
	~IndexedBinaryValue() {if (value) rakFree_Ex(value,_FILE_AND_LINE_);};

	/// Index of the value to set, required.
	unsigned int index;
	/// Value is deallocated in the destructor, use rakMalloc_Ex to allocate!
	char *value;
	/// Length of value
	unsigned short valueByteLength;
	/// Used for room searches only, -1 means use default equals
	int searchOperator;
	/// Used on 360 only. -1 means use default BINARY
	unsigned char type;
};
struct CreateAccountParameters
{
	CreateAccountParameters() {ageInDays=0; binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~CreateAccountParameters() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}
	/// [in] Self-apparent
	RakNet::RakString firstName;
	/// [in] Self-apparent
	RakNet::RakString middleName;
	/// [in] Self-apparent
	RakNet::RakString lastName;
	/// [in] Self-apparent
	RakNet::RakString race;
	/// [in] Self-apparent
	bool sex_male;
	/// [in] Self-apparent
	RakNet::RakString homeAddress1;
	/// [in] Self-apparent
	RakNet::RakString homeAddress2;
	/// [in] Self-apparent
	RakNet::RakString homeCity;
	/// [in] Self-apparent
	RakNet::RakString homeState;
	/// [in] Self-apparent
	RakNet::RakString homeCountry;
	/// [in] Self-apparent
	RakNet::RakString homeZipCode;
	/// [in] Self-apparent
	RakNet::RakString billingAddress1;
	/// [in] Self-apparent
	RakNet::RakString billingAddress2;
	/// [in] Self-apparent
	RakNet::RakString billingCity;
	/// [in] Self-apparent
	RakNet::RakString billingState;
	/// [in] Self-apparent
	RakNet::RakString billingCountry;
	/// [in] Self-apparent
	RakNet::RakString billingZipCode;
	/// [in] Self-apparent
	RakNet::RakString emailAddress;
	/// [in] Self-apparent
	RakNet::RakString password;
	/// [in] If the user needs to retrieve their password; you could ask them this question.
	RakNet::RakString passwordRecoveryQuestion;
	/// [in] If the user needs to retrieve their password; you could use this for the answer.
	RakNet::RakString passwordRecoveryAnswer;
	/// [in] Lobbies often allow users to set a text description of their user in some fashion.
	RakNet::RakString caption1;
	/// [in] Lobbies often allow users to set a text description of their user in some fashion.
	RakNet::RakString caption2;
	/// [in] Self-apparent
	unsigned int ageInDays;
	/// [in] binary data
	RakNetSmartPtr<BinaryDataBlock> binaryData;
	

	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};
struct PendingInvite
{
	PendingInvite() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~PendingInvite() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}
	RakNet::RakString sender;
	RakNet::RakString subject;
	RakNet::RakString body;
	RakNetSmartPtr<BinaryDataBlock> binaryData;

	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};
struct UsernameAndOnlineStatus
{
	UsernameAndOnlineStatus();
	UsernameAndOnlineStatus(const UsernameAndOnlineStatus& input);
	~UsernameAndOnlineStatus() {}
	UsernameAndOnlineStatus& operator = ( const UsernameAndOnlineStatus& input );

	RakNet::RakString handle;
	bool isOnline;
	uint64_t uid; // For XBOX
	RakNet::Lobby2Presence presence;

	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};
struct FriendInfo
{
	FriendInfo();
	FriendInfo(const FriendInfo& input);
	FriendInfo& operator = ( const FriendInfo& input );

	UsernameAndOnlineStatus usernameAndStatus;
	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream) {usernameAndStatus.Serialize(writeToBitstream,bitStream);}
};
struct EmailResult
{
	EmailResult() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~EmailResult() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}
	RakNet::RakString sender;
	RakNet::RakString recipient;
	RakNet::RakString subject;
	RakNet::RakString body;
	unsigned int status;
	bool wasSendByMe;
	bool wasReadByMe;
	unsigned int emailID; // Unique ID for this email, used in Emails_Delete, etc.
	RakNetSmartPtr<BinaryDataBlock> binaryData;
	RakNet::RakString creationDate;

	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};
struct MatchParticipant
{
	MatchParticipant() {}
	MatchParticipant(RakNet::RakString _handle, float _score) : handle(_handle), score(_score) {}
	RakNet::RakString handle;
	float score;

	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};
struct SubmittedMatch
{
	SubmittedMatch() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~SubmittedMatch() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}
	DataStructures::List<MatchParticipant> matchParticipants;
	RakNet::RakString matchNote;
	RakNetSmartPtr<BinaryDataBlock> binaryData;
	// Use EpochTimeToString to convert to a date
	double whenSubmittedDate;
	unsigned int matchID; // Unique key, Output parameter to Ranking_GetMatches

	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};
struct ClanInfo
{
	ClanInfo() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~ClanInfo() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}
	RakNet::RakString clanName;
	RakNet::RakString description;
	RakNet::RakString clanLeader;
	RakNetSmartPtr<BinaryDataBlock> binaryData;
	DataStructures::List<RakNet::RakString> clanMembersOtherThanLeader;

	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};
struct OpenInvite
{
	RakNet::RakString clanHandle;
	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};
struct ClanJoinRequest
{
	RakNet::RakString targetClan;
	RakNet::RakString dateSent;
	RakNet::RakString joinRequestSender;
	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};
struct ClanJoinInvite
{
	RakNet::RakString sourceClan;
	RakNet::RakString dateSent;
	RakNet::RakString joinRequestTarget;
	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};
struct BookmarkedUser
{
	RakNet::RakString targetHandle;
	int type;
	RakNet::RakString description;
	RakNet::RakString dateWhenAdded;
	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};

// --------------------------------------------- Actual body of all messages, including DB specific implementation --------------------------------------------

#define __L2_MSG_BASE_IMPL(__NAME__) \
	virtual void CallCallback(Lobby2Callbacks *cb) {cb->MessageResult(this);}; \
	virtual Lobby2MessageID GetID(void) const {return (Lobby2MessageID) L2MID_##__NAME__;} \
	virtual const char* GetName(void) const {return #__NAME__;} \
	virtual void DebugMsg(RakNet::RakString &out) const {out.Set(#__NAME__ " result=%s\n", Lobby2ResultCodeDescription::ToEnglish(resultCode));};

/// \brief Platform specific startup. Unused on the PC
/// \ingroup LOBBY_2_COMMANDS
struct Platform_Startup : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Platform_Startup)
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool PrevalidateInput(void) {return true;}
	virtual bool ServerPreDBMemoryImpl( Lobby2Server *server, RakString userHandle ) { (void)server; (void)userHandle; return true; }
};

/// \brief Platform specific startup. Unused on the PC
/// \ingroup LOBBY_2_COMMANDS
struct Platform_Shutdown : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Platform_Shutdown)
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool PrevalidateInput(void) {return true;}
	virtual bool ServerPreDBMemoryImpl( Lobby2Server *server, RakString userHandle ) { (void)server; (void)userHandle; return true; }
};

/// \brief Create all tables and stored procedures on a system that does not already have them
/// \ingroup LOBBY_2_COMMANDS
struct System_CreateDatabase : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(System_CreateDatabase)
	virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool PrevalidateInput(void) {return true;}
};
/// \brief Destroy all tables and stored procedures created with System_CreateDatabase
/// \ingroup LOBBY_2_COMMANDS
struct System_DestroyDatabase : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(System_DestroyDatabase)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool PrevalidateInput(void) {return true;}
};
/// \brief Each title essentially corresponds to a game. For example, the same lobby system may be used for both asteroids and Pac-man. When logging in, and for some functions, it is necessary to specify which title you are logging in under. This way users playing asteroids do not interact with users playing pac-man, where such interations are game specific (such as ranking).
/// \ingroup LOBBY_2_COMMANDS
struct System_CreateTitle : public Lobby2Message
{
	System_CreateTitle() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~System_CreateTitle() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}
	__L2_MSG_BASE_IMPL(System_CreateTitle)
	virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString titleName;
	RakNet::RakString titleSecretKey;
	int requiredAge;
	RakNetSmartPtr<BinaryDataBlock> binaryData;
};
/// \brief Destroy a previously added title
/// \ingroup LOBBY_2_COMMANDS
struct System_DestroyTitle : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(System_DestroyTitle)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	virtual bool PrevalidateInput(void) {return true;}

	// Input parameters
	RakNet::RakString titleName;

};
/// \brief Get the required age set with System_CreateTitle
/// \ingroup LOBBY_2_COMMANDS
struct System_GetTitleRequiredAge : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(System_GetTitleRequiredAge)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	virtual bool PrevalidateInput(void) {return true;}

	// Input parameters
	RakNet::RakString titleName;

	// Output parameters
	int requiredAge;
};
/// \brief Get the binary data set with System_CreateTitle
/// \ingroup LOBBY_2_COMMANDS
struct System_GetTitleBinaryData : public Lobby2Message
{
	System_GetTitleBinaryData() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~System_GetTitleBinaryData() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}
	__L2_MSG_BASE_IMPL(System_GetTitleBinaryData)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	virtual bool PrevalidateInput(void) {return true;}

	// Input parameters
	RakNet::RakString titleName;

	// Output parameters
	RakNetSmartPtr<BinaryDataBlock> binaryData;
};
/// \brief Adds the input strings to a table of profanity. non-unique or empty strings can be ignored. This table will be used internally to ensure that handles and clan names do not contain profanity. Multiple calls add to the table. This table will be used for functions that take a user-defined string that is highly visible, such as clan and user names. It does not need to be checked for emails or message boards.
/// \ingroup LOBBY_2_COMMANDS
struct System_RegisterProfanity : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(System_RegisterProfanity)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	virtual bool PrevalidateInput(void)
	{
		for (unsigned int i=0; i < profanityWords.Size(); i++)
		{
			if (profanityWords[i].IsEmpty())
			{
				resultCode=L2RC_System_RegisterProfanity_CANNOT_REGISTER_EMPTY_STRINGS;
				return false;
			}
		}
		return true;
	}

	// Input parameters
	DataStructures::List<RakNet::RakString> profanityWords;

	// Output parameters
};
/// \brief Bans a specific user (will be most likely called by a moderator). Adds the user's primary key to a ban table, along with the name of the moderator, the reason for the ban. Banning is used to prevent the banned user from logging on for some specified duration. A date column should be present and automatically filled in. When bans are expired, the ban can be deleted from the database. However, a separate table should log bans, so that even expired bans can be looked up in case.
/// \ingroup LOBBY_2_COMMANDS
struct System_BanUser : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(System_BanUser)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString banReason;
	unsigned int durationHours;
	RakNet::RakString userName;

	// Output parameters


};
/// \brief Unban a user banned with System_BanUser
/// \ingroup LOBBY_2_COMMANDS
struct System_UnbanUser : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(System_UnbanUser)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString reason;
	RakNet::RakString userName;
};
/// \brief Adds CDKeys to the database. Duplicate CDKeys for a particular title are ignored. CDKeys can be identical for different titles.
/// \ingroup LOBBY_2_COMMANDS
struct CDKey_Add : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(CDKey_Add)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	DataStructures::List<RakNet::RakString> cdKeys;
	RakNet::RakString titleName;

	// Output parameters
};
/// \brief Returns if a CD key was previously added with AddCDKey.
/// \ingroup LOBBY_2_COMMANDS
struct CDKey_GetStatus : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(CDKey_GetStatus)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString cdKey;
	RakNet::RakString titleName;

	// Output parameters
	bool usable;
	RakNet::RakString usedBy;
	RakNet::RakString activationDate;
	bool wasStolen;
};
/// \brief Associates a cd key with a user, such that the cd key cannot be used again. If Client_Login() is called with check cd key as true, then this table will be checked to make sure UserCDKey() was previously called with this user and a valid key. If this user is already associated with a CD Key, add the new key, and use the most recent key. All CD Key usage should be logged in a separate table, including the date used and result.
/// \ingroup LOBBY_2_COMMANDS
struct CDKey_Use : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(CDKey_Use)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString cdKey;
	RakNet::RakString titleName;
	RakNet::RakString userName;

	// Output parameters
};

/// \brief Flags one or more CD keys as stolen. Stolen CD keys will prevent Client_Login() if check cd key is true. They will also prevent these cd keys from being used with CDKey_Use. If this key is already in use by certain users for this particular title, then log this similarly to how CDKey_Use does so.
/// \ingroup LOBBY_2_COMMANDS
struct CDKey_FlagStolen : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(CDKey_FlagStolen)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString cdKey;
	RakNet::RakString titleName;
	bool wasStolen;

	// Output parameters
	RakNet::RakString userUsingThisKey;
};
/// \brief Logon with a previously registered account
/// \details Once a client creates an account with Client_RegisterAccount, the client is able to logon. The login process will check
/// <OL>
/// <LI>The CDKey associated with this user (See CDKey_Use) if checkCDKey is true
/// <LI>The userPassword passed to this function
/// <LI>The titleName and titleSecretKey, to ensure this title was previously created with System_CreateTitle
/// <LI>If allowLoginWithoutEmailAddressValidation==false for this user (See Client_RegisterAccount) and System_SetEmailAddressValidated was not called for that email address, fail.
/// <LI>If this user was banned with a ban still in effect via System_BanUser
/// </OL>
/// If all checks pass, store in a logging table that the user has logged in at this time. No status flag needs be set, this will be done in C++.
/// \ingroup LOBBY_2_COMMANDS
struct Client_Login : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Client_Login)
	Client_Login() {allowMultipleLogins=false;}
	virtual ~Client_Login() {}
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString userPassword;
	bool allowMultipleLogins; // PC only, allow login with the same username from multiple computers at once
	// Used if check 
	RakNet::RakString titleName;
	RakNet::RakString titleSecretKey;
	RakNet::RakString userName;

	// Output parameters
	RakNet::RakString bannedReason;
	RakNet::RakString whenBanned;
	RakNet::RakString bannedExpiration;
};
/// \brief Logoff, after logging in
/// \ingroup LOBBY_2_COMMANDS
struct Client_Logoff : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Client_Logoff)
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
};
/// \brief This creates a new account in the database, which can be used in a subsequent call to Login. Most parameters are optional. handle is not optional, and must be unique using case-insensitive compare. emailAddress should be validated to have a sensible format, including an @ sign and a period with a 3 letter extension. allowLoginWithoutEmailAddressValidation is used in Client_Login to potentially disallow logon attempts with unverified email addresses.
/// \ingroup LOBBY_2_COMMANDS
struct Client_RegisterAccount : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Client_RegisterAccount)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	CreateAccountParameters createAccountParameters;
	// Only used if registration requires a CD key
	RakNet::RakString cdKey;
	RakNet::RakString titleName;
	RakNet::RakString userName;

	// Output parameters
};
/// \brief For the client with the given handle, mark a column emailAddressValidated as true or false as appropriate. This is potentially used in Client_Login
/// \ingroup LOBBY_2_COMMANDS
struct System_SetEmailAddressValidated : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(System_SetEmailAddressValidated)
	virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	virtual bool PrevalidateInput(void);

	// Input parameters
	bool validated;
	RakNet::RakString userName;

	// Output parameters
};
/// \brief Looks up in the database if this handle is already in use, subject to the usual constraints of handles. This will be used by the user to quickly check for available handles.
/// \ingroup LOBBY_2_COMMANDS
struct Client_ValidateHandle : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Client_ValidateHandle)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	virtual bool PrevalidateInput(void);

	RakNet::RakString userName;
};
/// \brief Flags as deleted an account registered with RegisterAccount. Accounts are not actually deleted, only tagged as deleted.
/// \ingroup LOBBY_2_COMMANDS
struct System_DeleteAccount : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(System_DeleteAccount)
	virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString userName;
	RakNet::RakString password;
};
/// \brief Unused accounts are deleted. This is cascading, such that emails and other tables that reference this key are also deleted. unused accounts are defined as:
/// \details
/// <OL>
/// <LI>Deleted accounts over deletedPruneTime seconds old
/// <LI>Accounts which have not been logged into for over loggedInPruneTime seconds
/// <\OL>
/// \ingroup LOBBY_2_COMMANDS
struct System_PruneAccounts : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(System_PruneAccounts)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	virtual bool PrevalidateInput(void);

	// Input parameters
	unsigned int deleteAccountsNotLoggedInDays;

	// Output parameters
};
/// \brief Returns the email address associated with a specific handle, invalid handle. This is used for password recovery.
/// \ingroup LOBBY_2_COMMANDS
struct Client_GetEmailAddress : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Client_GetEmailAddress)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString userName;

	// Output parameters
	RakNet::RakString emailAddress;
	bool emailAddressValidated;

};
/// \brief Returns the passwordRecoveryQuestion associated with handle, invalid handle
/// \ingroup LOBBY_2_COMMANDS
struct Client_GetPasswordRecoveryQuestionByHandle : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Client_GetPasswordRecoveryQuestionByHandle)
		virtual bool RequiresAdmin(void) const {return false;}virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	virtual bool PrevalidateInput(void);

	// Input/output parameters
	RakNet::RakString userName;

	// Output parameters
	RakNet::RakString emailAddress;
	RakNet::RakString passwordRecoveryQuestion;
};
/// \brief Returns the password associated with a handle, if the passwordRecoveryAnswer is correct
/// \ingroup LOBBY_2_COMMANDS
struct Client_GetPasswordByPasswordRecoveryAnswer : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Client_GetPasswordByPasswordRecoveryAnswer)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString userName;
	RakNet::RakString passwordRecoveryAnswer;

	// Output parameters
	RakNet::RakString password;
};
/// \brief Changes the handle for a user.
/// \ingroup LOBBY_2_COMMANDS
struct Client_ChangeHandle : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Client_ChangeHandle)
	Client_ChangeHandle() {requiresPasswordToChangeHandle=false;}
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString userName;
	bool requiresPasswordToChangeHandle;
	RakNet::RakString password;
	RakNet::RakString newHandle;

	// Output parameters
};

/// \brief Will update any or all of the inputs that were previously passed to Client_RegisterAccount, except handle.
/// \details For input parameters, see Client_RegisterAccount() createAccountParameters
/// \ingroup LOBBY_2_COMMANDS
struct Client_UpdateAccount : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Client_UpdateAccount)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	CreateAccountParameters createAccountParameters;

	// Output parameters
};

/// \brief Get the parameters set with Client_RegisterAccount
/// \ingroup LOBBY_2_COMMANDS
struct Client_GetAccountDetails : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Client_GetAccountDetails)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	// Input parameters

	// Output parameters
	CreateAccountParameters createAccountParameters;
};

/// \brief Adds the specified user to an ignore list for my user. Recommended to store the primary key of the remote user, both for speed and so if the other use changes their handle it still works. The ignore list is checked for friend invites, emails, and elsewhere where indicated. Ignoring is uni-directional, so if A ignores B, A will block messages from B where appropriate, but B will not immediately block messages from A.
/// \ingroup LOBBY_2_COMMANDS
struct Client_StartIgnore : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Client_StartIgnore)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	virtual bool ClientImpl( RakNet::Lobby2Plugin *client);

	// Input parameters
	RakNet::RakString targetHandle;

	// Output parameters
};

/// \brief Removes an entry in the database such that myHandle will no longer ignore theirHandle.
/// \ingroup LOBBY_2_COMMANDS
struct Client_StopIgnore : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Client_StopIgnore)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	virtual bool ClientImpl( RakNet::Lobby2Plugin *client);

	// Input parameters
	RakNet::RakString targetHandle;

	// Output parameters

};
/// \brief Returns all users I have ignored
/// \ingroup LOBBY_2_COMMANDS
struct Client_GetIgnoreList : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Client_GetIgnoreList)
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool ClientImpl( RakNet::Lobby2Plugin *client);

	// Input parameters

	// Output parameters
	DataStructures::List<RakNet::RakString> ignoredHandles;
};
struct Client_PerTitleIntegerStorage : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Client_PerTitleIntegerStorage)
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	virtual bool PrevalidateInput(void);

	/// [in] Name of a created title
	RakNet::RakString titleName;
	/// [in] Slot index can be any value, and just lets you store more than one 64 bit integer
	unsigned int slotIndex;
	/// [in] Compared against the current value
	double conditionValue;
	/// [in] How conditionValue is compared against the conditional value
	/// Used for PTISO_ADD only
	enum PTIS_Condition
	{
		PTISC_EQUAL,
		PTISC_NOT_EQUAL,
		PTISC_GREATER_THAN,
		PTISC_GREATER_OR_EQUAL,
		PTISC_LESS_THAN,
		PTISC_LESS_OR_EQUAL,
	} addConditionForOperation;
	/// [in] What value is written (used for PTISO_WRITE and PTISO_ADD only)
	double inputValue;
	/// [in] What to do. Write will overwrite the existing value with inputValue
	/// Read will return the existing value in outputValue
	/// Delete will delete the entry, if it exists
	/// Add will add inputValue to the current value.
	enum PTIS_Operation
	{
		PTISO_WRITE,
		PTISO_READ,
		PTISO_DELETE,
		PTISO_ADD,
	} operationToPerform;
	/// [out] On return, new value is returned in outputValue
	/// For write, it will be the same as inputValue
	/// For read, it will be the current value (or 0, if the row does not exist)
	/// For delete, it is undefined
	/// For add, it is inputValue plus the existing value. If no existing value, 0 is used as the existing value.
	double outputValue;
};

/// \brief For each combination of user and title, structures can be stored
/// \ingroup LOBBY_2_COMMANDS
struct Client_PerTitleBinaryStorage : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Client_PerTitleBinaryStorage)
		Client_PerTitleBinaryStorage() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Client_PerTitleBinaryStorage() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	virtual bool PrevalidateInput(void);

	/// [in] Name of a created title
	RakNet::RakString titleName;
	/// [in] Slot index can be any value, and just lets you store more than one 64 bit integer
	unsigned int slotIndex;
	/// [in/out] Binary data. On Write, will be written to the row. On Read, will be filled in with the value of the row. Unused for delete
	/// Max length of binaryData is 256K
	RakNetSmartPtr<BinaryDataBlock> binaryData;
	/// [in] What to do. Write will overwrite the existing value with binaryData
	/// Read will return the existing value in binaryData
	enum PTIS_Operation
	{
		PTISO_WRITE,
		PTISO_READ,
		PTISO_DELETE,
	} operationToPerform;
};

/// \brief Sets in-memory information about your login state, such as which game you are playing, or if you are playing a game
/// Online friends will be notified when you presence changes
/// For the XBOX, just use XUserSetProperty and XUserSetContext directly, as there is no analogue to this function
struct Client_SetPresence : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Client_SetPresence)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
//	virtual bool ServerPreDBMemoryImpl( Lobby2Server *server, RakString userHandle );

	/// \param[in] Presence info to set.
	RakNet::Lobby2Presence presence;
};

/// \brief Gets in-memory information about a user's login state, such as which game they are playing, or if they are playing a game
/// This can also be a quick way to query if a user is logged in or not. If they are not logged in, \a presence will be set to Lobby2Presence::NOT_ONLINE
struct Client_GetPresence : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Client_GetPresence)
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
//	virtual bool ServerPreDBMemoryImpl( Lobby2Server *server, RakString userHandle );

	/// \param[in] Which user we are looking up ( can be ourselves )
	RakNet::RakString userHandle;

	/// \param[out] Presence info to set.
	RakNet::Lobby2Presence presence;
};

/// \brief Stores in the database an add friend invite from my handle to their handle. The combination of my handle and their handle must be unique, so you cannot send more than one add friend invite to a single user. Sends an email to their handle the subject, body, and binary data. Note: if myHandle is ignored by theirHandle, then the function fails. See Client_StartIgnore.
/// \ingroup LOBBY_2_COMMANDS
struct Friends_SendInvite : public Lobby2Message
{
	Friends_SendInvite() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Friends_SendInvite() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}
	__L2_MSG_BASE_IMPL(Friends_SendInvite)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString targetHandle;
	RakNet::RakString subject;
	RakNet::RakString body;
	int emailStatus;
	RakNetSmartPtr<BinaryDataBlock> binaryData;

	// Output parameters
};

/// \brief Stores in the database that this user is now my friend. This is bidirectional, which is to say if someone accepts an add friend invite, they are my friend, and I am their friend. Recommended to store by primary key for speed and in case the friend's handle changes. Store in the emails table from my handle to their handle the subject, body, and binary data. Note: if myHandle is ignored by theirHandle, then the function fails. See AddToIgnoreList.
/// \ingroup LOBBY_2_COMMANDS
struct Friends_AcceptInvite : public Lobby2Message
{

	Friends_AcceptInvite() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Friends_AcceptInvite() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}
	__L2_MSG_BASE_IMPL(Friends_AcceptInvite)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString targetHandle;
	RakNet::RakString subject;
	RakNet::RakString body;
	int emailStatus;
	RakNetSmartPtr<BinaryDataBlock> binaryData;

	// Output parameters
	// Your new buddy's presence status
	RakNet::Lobby2Presence presence;
};
/// \brief Removes from the database the pending add friend invite. Operation completes even if ignored. Unless ignored, store in the emails table from my handle to their handle the subject, body,  binary data, and procedure type flag.
/// \ingroup LOBBY_2_COMMANDS
struct Friends_RejectInvite : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Friends_RejectInvite)

	Friends_RejectInvite() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Friends_RejectInvite() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString targetHandle;
	RakNet::RakString subject;
	RakNet::RakString body;
	int emailStatus;
	RakNetSmartPtr<BinaryDataBlock> binaryData;

	// Output parameters
};
/// \brief Returns all invites for this user
/// \ingroup LOBBY_2_COMMANDS
struct Friends_GetInvites : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Friends_GetInvites)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	// Input parameters

	// Output parameters
	/// Up to caller to deallocate binaryData if needed
	DataStructures::List<FriendInfo> invitesSent;
	DataStructures::List<FriendInfo> invitesReceived;
};
/// \brief Gets all friends to this user
/// \ingroup LOBBY_2_COMMANDS
struct Friends_GetFriends : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Friends_GetFriends)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	// Input parameters

	// Output parameters
	DataStructures::List<FriendInfo> myFriends;
};
/// \brief Ends a friendship between two users. Remove from the database the friend entry between my handle and their handle. As with accept add friend invite, this is bidirectional. Either user can terminate the friendship. Store in the emails table from my handle to their handle the subject, body, and binary data, and procedure type flag.
/// \ingroup LOBBY_2_COMMANDS
struct Friends_Remove : public Lobby2Message
{
	Friends_Remove() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Friends_Remove() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Friends_Remove)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString targetHandle;
	RakNet::RakString subject;
	RakNet::RakString body;
	int emailStatus;
	RakNetSmartPtr<BinaryDataBlock> binaryData;

	// Output parameters
};
/// \brief Remembers a user, with a type integer and description for you to use, if desired.
/// \details Can be used for recent users or other types of lists
/// The combination of targetHandle and type uniquely identifies a bookmarked user.
/// If you want more than one list of bookmarked usrs, use a different value for type
/// \ingroup LOBBY_2_COMMANDS
struct BookmarkedUsers_Add : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(BookmarkedUsers_Add)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString targetHandle;
	int type;
	RakNet::RakString description;

};
/// \brief Remove a user added with BookmarkedUsers_Add
/// \ingroup LOBBY_2_COMMANDS
struct BookmarkedUsers_Remove : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(BookmarkedUsers_Remove)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString targetHandle;
	int type;
};
/// \brief Returns all users added to BookmarkedUsers_Add
/// \ingroup LOBBY_2_COMMANDS
struct BookmarkedUsers_Get : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(BookmarkedUsers_Get)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	// Input parameters

	// Output parameters
	/// \param[out] recentlyMetUsers Handles of recently met users, by BookmarkedUsers_Add, subject to expirationTimeSeconds
	DataStructures::List<BookmarkedUser> bookmarkedUsers;
};
/// \brief Adds to an emails table from myHandle (store primary key) to recipient handles (store primary key) the specified subject, body, and binary data. Emails are persistent, therefore emails should be stored in a separate table and referenced by the user. Deleting the user does not delete previously send email. Emails should have an automatic timestamp to store when they were created. Email should be flagged as sent=true (boolean), markedRead=true (boolean), deletedBySender=false (boolean), deletedByReciever=false (boolean).
/// \ingroup LOBBY_2_COMMANDS
struct Emails_Send : public Lobby2Message
{

	Emails_Send() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	virtual ~Emails_Send() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Emails_Send)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	DataStructures::List<RakNet::RakString> recipients;
	RakNet::RakString subject;
	RakNet::RakString body;
	int status;
	RakNetSmartPtr<BinaryDataBlock> binaryData;

	// Output parameters
};
/// \brief Returns emails as noted. Emails which were marked deleted are not returned.
/// \ingroup LOBBY_2_COMMANDS
struct Emails_Get : public Lobby2Message
{
	Emails_Get() {unreadEmailsOnly=false; emailIdsOnly=false;}
	virtual ~Emails_Get() {}

	__L2_MSG_BASE_IMPL(Emails_Get)
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	// Input parameters
	bool unreadEmailsOnly;  //If this is true then it will only return emails that have not been read by the user.
	bool emailIdsOnly; //When this is set only the emailIds are filled out and no data is returned.
	DataStructures::List<unsigned int> emailsToRetrieve; //If this has entries then it will only get the emails in the list, otherwise it will get all emails.

	// Output parameters
	/// \param[out] emailResults Up to caller to deallocate binary data
	DataStructures::List<EmailResult> emailResults;
};
/// \brief Deletes an email with a specified ID. This ID is returned in GetEmail and should uniquely identify an email (it's fine to use the primary key). Note: Emails are not actually deleted from the database in this function. This just sets the deletedBySender or deletedByReciever flags. Emails are actually stored in a log recording past emails and sender and receiver primary key. They are not truly destroyed until done so with System_PruneAccounts.
/// \ingroup LOBBY_2_COMMANDS
struct Emails_Delete : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Emails_Delete)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	// Input parameters
	unsigned int emailId;

	// Output parameters
};
/// \brief Sets the status flag for an email. This is a property defined by and used by the user
/// \ingroup LOBBY_2_COMMANDS
struct Emails_SetStatus : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Emails_SetStatus)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	virtual bool PrevalidateInput(void);

	// Input parameters
	unsigned int emailId;
	bool updateStatusFlag;
	bool updateMarkedRead;
	int newStatusFlag;
	bool isNowMarkedRead;

	// Output parameters
};
/// \brief Will record in the database the results of a match. This will store in the database the the match which is defined by the the match notes, match id, winner and loser participant primary keys, winner and loser participant scores, and binary data.
/// \ingroup LOBBY_2_COMMANDS
struct Ranking_SubmitMatch : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Ranking_SubmitMatch)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return true;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString titleName;
	RakNet::RakString gameType;
	SubmittedMatch submittedMatch;

	// Output parameters
};
/// \brief Gets matches recorded with Ranking_SubmitMatch ordered from most recent to least recent. Each returned match has all columns submitted to Ranking_SubmitMatch, except binary data, which can be retrieved with Ranking_GetMatchBinaryData. Additionally, each returned match returns the primary key of each match, to be passed to Ranking_GetMatchBinaryData
/// \ingroup LOBBY_2_COMMANDS
struct Ranking_GetMatches : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Ranking_GetMatches)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString titleName;
	RakNet::RakString gameType;

	// Output parameters
	/// \param[out] submittedMatches (excluding binary data, up to caller to deallocate)
	DataStructures::List<SubmittedMatch> submittedMatches;
};
/// \brief Because of the large amount of binary data potentially returned, this function is used to retrieve binary data for a particular match. 
/// \ingroup LOBBY_2_COMMANDS
struct Ranking_GetMatchBinaryData : public Lobby2Message
{

	Ranking_GetMatchBinaryData() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Ranking_GetMatchBinaryData() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Ranking_GetMatchBinaryData)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	// Input parameters
	unsigned int matchID;

	// Output parameters
	RakNetSmartPtr<BinaryDataBlock> binaryData;
};
/// \brief When a match is submitted with Ranking_SubmitMatch, the total running score and number of matches played for each player for each game title and game mode combination should be recorded. Because matches can be pruned wth PruneMatches(), the total score sum and number of scores submitted should be stored, rather than summed up from prior submitted matches.
/// \ingroup LOBBY_2_COMMANDS
struct Ranking_GetTotalScore : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Ranking_GetTotalScore)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString titleName;
	RakNet::RakString gameType;
	RakNet::RakString targetHandle;

	// Output parameters
	float scoreSum;
	unsigned int numScoresSubmitted;
};

/// \brief Resets the sum of all submitted scores to 0, the number of scores submitted to 0
/// \ingroup LOBBY_2_COMMANDS
struct Ranking_WipeScoresForPlayer : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Ranking_WipeScoresForPlayer)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return true;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString titleName;
	RakNet::RakString gameType;
	RakNet::RakString targetHandle;

	// Output parameters
};
/// \brief Deletes all matches submitted with submit match. Also deletes all scores for all players associated with this titleName and gameType (e.g. same thing that WipeScoresForPlayer does, but for all players).
/// \ingroup LOBBY_2_COMMANDS
struct Ranking_WipeMatches : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Ranking_WipeMatches)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return true;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString titleName;
	RakNet::RakString gameType;

	// Output parameters
};
/// \brief Will delete all matches submitted with SubmitMatch over PruneTime days old. Will also prune matches if the total storage space of all matches exceeds PruneSizeMB megabytes in the database.
/// \ingroup LOBBY_2_COMMANDS
struct Ranking_PruneMatches : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Ranking_PruneMatches)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return true;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	// Input parameters
	unsigned int pruneTimeDays;

	// Output parameters
};
/// \brief Add or update a rating for a user, in a particular game and game mode
/// \ingroup LOBBY_2_COMMANDS
struct Ranking_UpdateRating : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Ranking_UpdateRating)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return true;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString titleName;
	RakNet::RakString gameType;
	RakNet::RakString targetHandle;
	float targetRating;

	// Output parameters
};
/// \brief Deletes all ratings for all players for this combination of titleName and gameType.
/// \ingroup LOBBY_2_COMMANDS
struct Ranking_WipeRatings : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Ranking_WipeRatings)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return true;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString titleName;
	RakNet::RakString gameType;

	// Output parameters
};
/// \brief Get rating for a player
/// \ingroup LOBBY_2_COMMANDS
struct Ranking_GetRating : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Ranking_GetRating)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString titleName;
	RakNet::RakString gameType;
	RakNet::RakString targetHandle;

	// Output parameters
	/// \param[out] currentRating Defaults to 100 if no matches submitted yet
	float currentRating;
};

/// \brief userHandle updates the clanDescription and clanBinaryData of a clan with the specified clanHandle. userHandle must be the clan leader.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_Create : public Lobby2Message
{

	Clans_Create() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Clans_Create() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_Create)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	bool failIfAlreadyInClan;
	bool requiresInvitationsToJoin;
	RakNet::RakString description;
	int emailStatus;
	RakNetSmartPtr<BinaryDataBlock> binaryData;

	// Output parameters
};

/// \brief userHandle updates the clanDescription and clanBinaryData of a clan with the specified clanHandle. userHandle must be the clan leader.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_SetProperties : public Lobby2Message
{
	Clans_SetProperties() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Clans_SetProperties() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_SetProperties)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString description;
	RakNetSmartPtr<BinaryDataBlock> binaryData;
};
/// \brief Returns clanDescription and clanBinaryData for the given clan.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_GetProperties : public Lobby2Message
{
	Clans_GetProperties() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Clans_GetProperties() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_GetProperties)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;

	// Output parameters
	RakNet::RakString description;
	RakNetSmartPtr<BinaryDataBlock> binaryData;
};

/// \brief Each member of each clan has the the properties clanMemberDescription and clanMemberBinaryData which default to empty. These properties can be set here, and retrieved via GetClanMemberProperties
/// \ingroup LOBBY_2_COMMANDS
struct Clans_SetMyMemberProperties : public Lobby2Message
{

	Clans_SetMyMemberProperties() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Clans_SetMyMemberProperties() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_SetMyMemberProperties)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString description;
	RakNetSmartPtr<BinaryDataBlock> binaryData;

	// Output parameters
};
/// \brief myPrimaryKey becomes a subleader. newLeaderHandle becomes the leader. An email is sent with Emails_Send() to all members with the specified subject and body
/// \ingroup LOBBY_2_COMMANDS
struct Clans_GrantLeader : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Clans_GrantLeader)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString targetHandle;
};
/// \brief Promotes a clan member to a subleader, or demotes a subleader to a regular member. On promotion, email is sent to all members from myPrimary key with the specified subject and body. On demotion, email is sent to all leaders from myPrimary key with the specified subject and body.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_SetSubleaderStatus : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Clans_SetSubleaderStatus)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString targetHandle;
	bool setToSubleader;
};

/// \brief Lets the clan leader set the rank property for a clan member
/// \ingroup LOBBY_2_COMMANDS
struct Clans_SetMemberRank : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Clans_SetMemberRank)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString targetHandle;
	unsigned int newRank;
};
/// \brief Returns properties for a clan member of a given clan
/// \ingroup LOBBY_2_COMMANDS
struct Clans_GetMemberProperties : public Lobby2Message
{
	Clans_GetMemberProperties() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Clans_GetMemberProperties() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_GetMemberProperties)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString targetHandle;

	// Output parameters
	RakNet::RakString description;
	RakNetSmartPtr<BinaryDataBlock> binaryData;
	unsigned int rank;
	bool isSubleader;
	ClanMemberState clanMemberState;
	RakNet::RakString banReason;
};
/// \brief Renames the clan. Note that this may be called asynchronously, in which case the stored procedure should account for this occuring at the same time as another function that uses the old clan handle.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_ChangeHandle : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Clans_ChangeHandle)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString oldClanHandle;
	RakNet::RakString newClanHandle;

	// Output parameters
};
/// \brief Remove user identified by myPrimaryKey from clan identified by clanHandle.
/// \details If this user is the leader of the clan, and dissolveIfClanLeader is true, then also destroy the clan and remove all members from the clan, as well as all data associated with the clan (clan boards, join requests, etc). If the clan is automatically destroyed in this way,  use Emails_Send() to each clan member with clanDissolvedSubject and clanDissolvedBody. The sender of the email should be the clan leader. If the clan is not destroyed, then leadership passes to the oldest subleader. If no subleaders exist, leadership passes to the oldest member. If no other members exist, the clan is destroyed.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_Leave : public Lobby2Message
{
	Clans_Leave() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Clans_Leave() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_Leave)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	bool dissolveIfClanLeader;
	RakNet::RakString subject;
	RakNet::RakString body;
	int emailStatus;
	RakNetSmartPtr<BinaryDataBlock> binaryData;

	// Output parameters
	bool wasDissolved;
	RakNet::RakString newClanLeader; // If not dissolved	
};
/// \brief Returns all clans that userHandle is a member of. Clans and clan members should be sorted by name, using ascending or descending sort as specified.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_Get : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Clans_Get)
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );


	// Input parameters

	// Output parameters
	DataStructures::List<ClanInfo> clans;
};
/// \brief if myPrimaryKey is a leader or subleader of clanHandle, and invitedUserHandle is a valid user not already invited to this clan, add this user to the invite table. The invite table contains the clan, who send the invite, and who the invite was sent to, and when it was sent. Invites expire after expiration time in seconds. Also, use Emails_Send() to send an email from myPrimaryKey to invitedUserHandle with the specified subject and body.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_SendJoinInvitation : public Lobby2Message
{
	
	Clans_SendJoinInvitation() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
		~Clans_SendJoinInvitation() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_SendJoinInvitation)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString targetHandle;
	RakNet::RakString subject;
	RakNet::RakString body;
	int emailStatus;
	RakNetSmartPtr<BinaryDataBlock> binaryData;

	// Output parameters
};
/// \brief if myPrimaryKey is a leader or subleader of clanHandle, and invitedUserHandle is a valid user with an invite to this clan, remove this invite.  Also, use Emails_Send() to send an email from myPrimaryKey to invitedUserHandle with the specified subject and body.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_WithdrawJoinInvitation : public Lobby2Message
{
	Clans_WithdrawJoinInvitation() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Clans_WithdrawJoinInvitation() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_WithdrawJoinInvitation)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString targetHandle;
	RakNet::RakString subject;
	RakNet::RakString body;
	int emailStatus;
	RakNetSmartPtr<BinaryDataBlock> binaryData;
};
/// \brief If myPrimaryKey has an invitation to the specified clan, add him to the clan. Fail on specified output parameters. Use Emails_Send() to send an email from myPrimaryKey to all clan members with the specified subject and body.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_AcceptJoinInvitation : public Lobby2Message
{
	Clans_AcceptJoinInvitation() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Clans_AcceptJoinInvitation() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_AcceptJoinInvitation)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString subject;
	RakNet::RakString body;
	int emailStatus;
	RakNetSmartPtr<BinaryDataBlock> binaryData;
	bool failIfAlreadyInClan;

	// Output parameters
};
/// \brief If we have an open clan invitation, reject it (just delete it from the database).
/// \ingroup LOBBY_2_COMMANDS
struct Clans_RejectJoinInvitation : public Lobby2Message
{
	Clans_RejectJoinInvitation() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Clans_RejectJoinInvitation() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_RejectJoinInvitation)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString subject;
	RakNet::RakString body;
	int emailStatus;
	RakNetSmartPtr<BinaryDataBlock> binaryData;

	// Output parameters
};
/// \brief Returns all invites sent by Clans_SendJoinInvitation that were not yet acted upon (withdrawn, accepted, rejected).
/// \ingroup LOBBY_2_COMMANDS
struct Clans_DownloadInvitationList : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Clans_DownloadInvitationList)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	// Output parameters
	DataStructures::List<OpenInvite> invitationsSentToMe;

	DataStructures::List<ClanJoinInvite> usersThatHaveAnInvitationFromClansThatIAmAMemberOf;
};
/// \brief Function has two forms:
/// \details If requiresInvitationsToJoin==true when CreateClan() was called, will join the specified clan immediately. Sends subject and body to all other members in the clan.
/// If requiresInvitationsToJoin==false when CreateClan() was called, send a join request to the specified clan, if we don't have one already. Join request expires after expiration time in seconds. Also, use Emails_Send() to send an email from myPrimaryKey to the clan leader and all subleaders with the specified subject and body.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_SendJoinRequest : public Lobby2Message
{
	Clans_SendJoinRequest() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Clans_SendJoinRequest() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_SendJoinRequest)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString subject;
	RakNet::RakString body;
	int emailStatus;
	RakNetSmartPtr<BinaryDataBlock> binaryData;

	// Output parameters
	bool clanJoined;
};
/// \brief Withdraws a previously sent clan join request via SendClanJoinRequest.  Use Emails_Send() to send an email from myPrimaryKey to the clan leader and all subleaders with the specified subject and body.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_WithdrawJoinRequest : public Lobby2Message
{
	Clans_WithdrawJoinRequest() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Clans_WithdrawJoinRequest() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_WithdrawJoinRequest)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString subject;
	RakNet::RakString body;
	int emailStatus;
	RakNetSmartPtr<BinaryDataBlock> binaryData;

	// Output parameters

};
/// \brief A clan leader or subleader accepts a join request from requestingUserHandle to this clan. requestingUserHandle joins the clan as a regular member.  Use Emails_Send() to send an email from requestingUserHandle to all clan members with the specified subject and body.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_AcceptJoinRequest : public Lobby2Message
{
	Clans_AcceptJoinRequest() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Clans_AcceptJoinRequest() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_AcceptJoinRequest)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString subject;
	RakNet::RakString body;
	int emailStatus;
	RakNetSmartPtr<BinaryDataBlock> binaryData;
	RakNet::RakString requestingUserHandle;
	bool failIfAlreadyInClan;	

	// Output parameters
};
/// \brief Rejects a clan join request from requestingUserHandle. Send an email from myPrimaryKey to requestingUserHandle with the specified subject and body.
/// \details 
struct Clans_RejectJoinRequest : public Lobby2Message
{
	Clans_RejectJoinRequest() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Clans_RejectJoinRequest() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_RejectJoinRequest)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString subject;
	RakNet::RakString body;
	int emailStatus;
	RakNetSmartPtr<BinaryDataBlock> binaryData;
	RakNet::RakString requestingUserHandle;

	// Output parameters
};
/// \brief Returns all open requests this user has sent to clans, that have not yet acted upon (withdrawn, accepted, rejected, expired).
/// \ingroup LOBBY_2_COMMANDS
struct Clans_DownloadRequestList : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Clans_DownloadRequestList)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );


	// Input parameters

	// Output parameters
	// joinRequestsToMyClan is only filled out for clans you are a leader or subleader in
	DataStructures::List<ClanJoinRequest> joinRequestsToMyClan, joinRequestsFromMe;
};
/// \brief Kicks a user from the clan and/or blacklists a user so they cannot join. Only a clan leader or subleader can perform this operation. The operation can only be performed on members of lower status (leader can perform on subleader or regular member or nonmember, subleader on regular members or nonmember). If a member is banned, they are added to the banned table which contains the member's primary key, which user banned them, and the reason. Email is sent from myPrimaryKey to all leaders if a clan member is banned. Emails is furthermore sent to all clan members if successfully kicked. 
/// \ingroup LOBBY_2_COMMANDS
struct Clans_KickAndBlacklistUser : public Lobby2Message
{
	Clans_KickAndBlacklistUser() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Clans_KickAndBlacklistUser() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_KickAndBlacklistUser)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString subject;
	RakNet::RakString body;
	int emailStatus;
	RakNetSmartPtr<BinaryDataBlock> binaryData;
	RakNet::RakString targetHandle;
	bool kick;
	bool blacklist;
	RakNet::RakString reason;
};
/// \brief Removes a user from the blacklist for this clan.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_UnblacklistUser : public Lobby2Message
{
	Clans_UnblacklistUser() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Clans_UnblacklistUser() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_UnblacklistUser)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString targetHandle;
	RakNet::RakString subject;
	RakNet::RakString body;
	int emailStatus;
	RakNetSmartPtr<BinaryDataBlock> binaryData;
};
/// \brief Returns a list of all members  blacklisted from this clan. Each element in the list contains the handle of the user that did the ban, who was banned, when the user was banned, and the reason passed to ClanKickAndBlacklistUser
/// \ingroup LOBBY_2_COMMANDS
struct Clans_GetBlacklist : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Clans_GetBlacklist)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;

	// Output parameters
	DataStructures::List<RakNet::RakString> blacklistedUsers;
};
/// \brief Returns all clan members for this clan. Each entry returned contains handle, description, binary data, status (leader, regular member, subleader).
/// \ingroup LOBBY_2_COMMANDS
struct Clans_GetMembers : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Clans_GetMembers)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;

	// Output parameters
	RakNet::RakString clanLeader;
	DataStructures::List<RakNet::RakString> clanMembersOtherThanLeader;
};
/// \brief Returns all clans names
/// \ingroup LOBBY_2_COMMANDS
struct Clans_GetList : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Clans_GetList)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	// Input parameters

	// Output parameters
	DataStructures::List<RakNet::RakString> clanNames;
};
/// \brief Creates a new clan board for clan members to post in using AddPostToClanBoard. Clan boards are unique, and are destroyed when the clan is destroyed, or if DestroyClanBoard is called.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_CreateBoard : public Lobby2Message
{
	Clans_CreateBoard() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Clans_CreateBoard() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_CreateBoard)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString clanBoardName;
	bool allowPublicReads;
	bool allowPublicWrites;
	RakNet::RakString description;
	RakNetSmartPtr<BinaryDataBlock> binaryData;

	// Output parameters
};
/// \brief Success, invalid parameter syntax, myPrimaryKey is not the leader or subleader of clanHandle, unknown myPrimaryKey, unknown clanHandle, unknown clanBoardName
/// \ingroup LOBBY_2_COMMANDS
struct Clans_DestroyBoard : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Clans_DestroyBoard)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString clanBoardName;

	// Output parameters
};
/// \brief Each clan has a clan board that only clan members can post to. This adds a topic to the clan board. Posts should reference the primary key of the poster, so that even if the poster chagnes his or her handle, the post author is updated properly. Each post automatically stores the timestamp when it was created. Banned users may not add new posts to the clan board.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_CreateNewTopic : public Lobby2Message
{
	Clans_CreateNewTopic() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Clans_CreateNewTopic() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_CreateNewTopic)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString clanBoardName;
	RakNet::RakString body;
	RakNet::RakString subject;
	RakNetSmartPtr<BinaryDataBlock> binaryData;

	// Output parameters
	unsigned int postId; // (unique for clanHandle)
};
/// \brief Replies to a topic created with Clans_CreateTopic(). If postId references a post within a topic, just add the reply to the last post.  Banned users may not add new posts to the clan board.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_ReplyToTopic : public Lobby2Message
{
	Clans_ReplyToTopic() {binaryData=RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);}
	~Clans_ReplyToTopic() {/*RakNet::OP_DELETE(binaryData,_FILE_AND_LINE_);*/}

	__L2_MSG_BASE_IMPL(Clans_ReplyToTopic)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	unsigned int postId; // returned from Clans_CreateTopic()
	RakNet::RakString subject;
	RakNet::RakString body;
	RakNetSmartPtr<BinaryDataBlock> binaryData;
};
/// \brief The clan leader or subleaders may remove posts or topics from a clan board.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_RemovePost : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Clans_RemovePost)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	// Input parameters
	unsigned int postId; // returned from Clans_CreateTopic()
	bool removeEntireTopic;


};

/// \brief Gets clan boards created for clanHandle. Boards are returned if we are a clan member, or if allowPublicReads in Clans_CreateBoard() was passed as false. However, if we are banned from this clan, no boards are returned.
/// \ingroup LOBBY_2_COMMANDS
struct Clans_GetBoards : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Clans_GetBoards)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;

	// Output parameters
	DataStructures::List<RakNet::RakString> clanBoardsNames;


};
/// \brief Gets topics (posts that are not replies to other posts, created with Clans_CreateTopic()) for the specified clanHandle and clanBoardName. If we are not a clan member and the clan was created with allowPublicReads==false, then the user is not allowed to read topics
/// \ingroup LOBBY_2_COMMANDS
struct Clans_GetTopics : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Clans_GetTopics)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	RakNet::RakString clanHandle;
	RakNet::RakString clanBoardName;
};
/// \brief Gets all posts for a particular topic. If postId is not a topic but is instead a post in a topic, treat it as if the topic postId was passed. If we are not a clan member and the clan was created with allowPublicReads==false, then the user is not allowed to read topics
/// \ingroup LOBBY_2_COMMANDS
struct Clans_GetPosts : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Clans_GetPosts)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	virtual bool PrevalidateInput(void);

	// Input parameters
	unsigned int postId;
};

struct Console_GameBootCheck : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_GameBootCheck)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};

struct Console_GetGameBootInviteDetails : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_GetGameBootInviteDetails)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};

// \brief Call the function to get the list of servers available.
// \note Does nothing on the PC.
struct Console_GetServerStatus : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_GetServerStatus)
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};

// \brief Given a server, get the list of worlds
// \note Does nothing on the PC.
struct Console_GetWorldListFromServer : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_GetWorldListFromServer)
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};

// Given a world, get the list of lobbies
// Does nothing on the PC.
struct Console_GetLobbyListFromWorld : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_GetLobbyListFromWorld)
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};

// Given a lobby, join that lobby
// Does nothing on the PC.
struct Console_JoinLobby : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_JoinLobby)
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};

// Given a lobby, leave that lobby.
// Does nothing on the PC.
struct Console_LeaveLobby : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_LeaveLobby)
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};

// Send a message to a lobby that you have joined
// Does nothing on the PC.
struct Console_SendLobbyChatMessage : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_SendLobbyChatMessage)
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
	
	// Input
	RakNet::RakString message;
};

// Search rooms in the lobby
// Does nothing on the PC.
struct Console_SearchRooms : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_SearchRooms)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};


// Get information about a room
// Does nothing on the PC.
struct Console_GetRoomDetails : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_GetRoomDetails)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};


// Send a chat message to the lobby
// Does nothing on the PC.
struct Console_GetLobbyMemberData : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_GetLobbyMemberData)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};

struct Console_CreateRoom : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_CreateRoom)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}

	int publicSlots;
	int privateSlots;
};

// XBOX only - needed after creating or joining a room
struct Console_SignIntoRoom : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_SignIntoRoom)
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};

struct Console_SetRoomSearchProperties : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_SetRoomSearchProperties)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};
struct Console_UpdateRoomParameters : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_UpdateRoomParameters)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};
struct Console_JoinRoom : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_JoinRoom)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};

struct Console_LeaveRoom : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_LeaveRoom)
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};

struct Console_SendLobbyInvitationToRoom : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_SendLobbyInvitationToRoom)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};

struct Console_SendGUIInvitationToRoom : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_SendGUIInvitationToRoom)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};

struct Console_SendDataMessageToUser : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_SendDataMessageToUser)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};

struct Console_SendRoomChatMessage : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_SendRoomChatMessage)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}

	RakNet::RakString message;
};
struct Console_ShowFriendsUI : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_ShowFriendsUI)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};
struct Console_EndGame : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_EndGame)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};
struct Console_StartGame : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_StartGame)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};
struct Console_ShowPartyUI : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_ShowPartyUI)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};
struct Console_ShowMessagesUI : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_ShowMessagesUI)
		virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};
struct Console_ShowGUIInvitationsToRooms : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_ShowGUIInvitationsToRooms)
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};
struct Console_EnableDisableRoomVoiceChat : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Console_EnableDisableRoomVoiceChat)
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Client_RemoteLogin : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Client_RemoteLogin)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	RakNet::RakString handle;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Client_IgnoreStatus : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Client_IgnoreStatus)
	virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	bool nowIgnored;
	RakNet::RakString otherHandle;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Friends_StatusChange : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Friends_StatusChange)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	enum Status
	{
		FRIEND_LOGGED_IN,
		FRIEND_LOGGED_IN_DIFFERENT_CONTEXT, // PS3 only
		FRIEND_LOGGED_OFF,
		FRIEND_AFK, // Vita only
		FRIEND_ACCOUNT_WAS_DELETED,
		YOU_WERE_REMOVED_AS_A_FRIEND,
		GOT_INVITATION_TO_BE_FRIENDS,
		THEY_ACCEPTED_OUR_INVITATION_TO_BE_FRIENDS,
		THEY_REJECTED_OUR_INVITATION_TO_BE_FRIENDS,
	} op;
	const char *OpToString(void) const {
		switch (op)
		{
		case FRIEND_LOGGED_IN:
			return "FRIEND_LOGGED_IN";
		case FRIEND_LOGGED_IN_DIFFERENT_CONTEXT:
			return "FRIEND_LOGGED_IN_DIFFERENT_CONTEXT";
		case FRIEND_LOGGED_OFF:
			return "FRIEND_LOGGED_OFF";
		case FRIEND_ACCOUNT_WAS_DELETED:
			return "FRIEND_ACCOUNT_WAS_DELETED";
		case YOU_WERE_REMOVED_AS_A_FRIEND:
			return "YOU_WERE_REMOVED_AS_A_FRIEND";
		case GOT_INVITATION_TO_BE_FRIENDS:
			return "GOT_INVITATION_TO_BE_FRIENDS";
		case THEY_ACCEPTED_OUR_INVITATION_TO_BE_FRIENDS:
			return "THEY_ACCEPTED_OUR_INVITATION_TO_BE_FRIENDS";
		case THEY_REJECTED_OUR_INVITATION_TO_BE_FRIENDS:
			return "THEY_REJECTED_OUR_INVITATION_TO_BE_FRIENDS";
		}
		return "Error in OpToString::Notification_Friends_StatusChange";
	}
	RakNet::RakString otherHandle;
	RakNet::Lobby2Presence presence;
	// If \a op was generated due to YOU_WERE_REMOVED_AS_A_FRIEND,GOT_INVITATION_TO_BE_FRIENDS,THEY_ACCEPTED_OUR_INVITATION_TO_BE_FRIENDS, or THEY_REJECTED_OUR_INVITATION_TO_BE_FRIENDS
	// Then a copy of the subject and body of the corresponding email is here for convenience
	RakNet::RakString subject;
	RakNet::RakString body;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Friends_PresenceUpdate : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Friends_PresenceUpdate)
	virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	
	Lobby2Presence newPresence; 
	RakNet::RakString otherHandle;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_User_ChangedHandle : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_User_ChangedHandle)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	RakNet::RakString oldHandle, newHandle;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Friends_CreatedClan : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Friends_CreatedClan)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	RakNet::RakString otherHandle;
	RakNet::RakString clanName;	
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Emails_Received : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Emails_Received)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	RakNet::RakString sender;
	RakNet::RakString subject;
	unsigned int emailId;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Clans_GrantLeader : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Clans_GrantLeader)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	RakNet::RakString clanHandle;
	RakNet::RakString newLeader;
	RakNet::RakString oldLeader;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Clans_SetSubleaderStatus : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Clans_SetSubleaderStatus)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	RakNet::RakString clanHandle;
	RakNet::RakString targetHandle;
	RakNet::RakString leaderHandle;
	bool setToSubleader;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Clans_SetMemberRank : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Clans_SetMemberRank)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	RakNet::RakString clanHandle;
	RakNet::RakString targetHandle;
	RakNet::RakString leaderHandle;
	unsigned int newRank;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Clans_ChangeHandle : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Clans_ChangeHandle)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	RakNet::RakString oldClanHandle;
	RakNet::RakString newClanHandle;
	RakNet::RakString leaderHandle;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Clans_Leave : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Clans_Leave)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	RakNet::RakString clanHandle;
	RakNet::RakString targetHandle;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Clans_PendingJoinStatus : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Clans_PendingJoinStatus)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	RakNet::RakString clanHandle;
	RakNet::RakString sourceHandle;
	RakNet::RakString targetHandle;
	// clanMemberHandle is is the clan member that is withdrawing an invitation, or rejecting a request
	RakNet::RakString clanMemberHandle;

	// The combination of major and minor op describe this notification
	// For example, JOIN_CLAN_INVITATION + JOIN_WITHDRAWN means that an invitation to join the clan was withdrawn by targetHandle
	// JOIN_CLAN_REQUEST + JOIN_REJECTED means that our request to join this clan was rejected by targetHandle
	enum MajorOp
	{
		JOIN_CLAN_INVITATION,
		JOIN_CLAN_REQUEST,
	} majorOp;
	enum MinorOp
	{
		JOIN_SENT,
		JOIN_WITHDRAWN,
		JOIN_REJECTED,
	} minorOp;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Clans_NewClanMember : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Clans_NewClanMember)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	RakNet::RakString clanHandle;
	RakNet::RakString targetHandle;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Clans_KickAndBlacklistUser : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Clans_KickAndBlacklistUser)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	RakNet::RakString clanHandle;
	RakNet::RakString targetHandle;
	RakNet::RakString blacklistingUserHandle;
	// If true, they were both kicked and blacklisted. If false, they were only blacklisted (not currently in the clan)
	bool targetHandleWasKicked;
	RakNet::RakString reason;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Clans_UnblacklistUser : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Clans_UnblacklistUser)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	RakNet::RakString clanHandle;
	RakNet::RakString targetHandle;
	// Currently this is always the clan leader, since subleaders cannot unblacklist
	RakNet::RakString unblacklistingUserHandle;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Clans_Destroyed : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Clans_Destroyed)
	virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );

	RakNet::RakString clanHandle;
	RakNet::RakString oldClanLeader;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_CableDisconnected : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_CableDisconnected)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_ContextError : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_ContextError)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_MemberJoinedLobby : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_MemberJoinedLobby)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}

	RakNet::RakString targetHandle;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_MemberLeftLobby : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_MemberLeftLobby)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}

	RakNet::RakString targetHandle;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_LobbyDestroyed : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_LobbyDestroyed)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_LobbyMemberDataUpdated : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_LobbyMemberDataUpdated)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_LobbyGotChatMessage : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_LobbyGotChatMessage)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}

	RakNet::RakString sender;
	RakNet::RakString message;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_LobbyGotRoomInvitation : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_LobbyGotRoomInvitation)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}

	RakNet::RakString sender;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_MemberJoinedRoom : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_MemberJoinedRoom)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}

	RakNet::RakString memberName;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_MemberLeftRoom : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_MemberLeftRoom)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}

	RakNet::RakString memberName;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_KickedOutOfRoom : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_KickedOutOfRoom)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_RoomWasDestroyed : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_RoomWasDestroyed)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_UpdateRoomParameters : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_UpdateRoomParameters)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_RoomOwnerChanged : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_RoomOwnerChanged)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_RoomChatMessage : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_RoomChatMessage)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}

	RakNet::RakString sender;
	RakNet::RakString message;
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_RoomMessage : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_RoomMessage)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}


	RakNet::RakString sender;
	RakNet::RakString message;
};
// Now merged into Notification_Console_MemberJoinedRoom
/*
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_RoomMemberConnectivityUpdate : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_RoomMemberConnectivityUpdate)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}

	// Out
	SystemAddress systemAddress;
};
*/
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_ChatEvent : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_ChatEvent)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_MuteListChanged : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_MuteListChanged)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_Local_Users_Changed : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_Local_Users_Changed)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_ReceivedDataMessageFromUser : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_ReceivedDataMessageFromUser)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_MemberJoinedParty : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_MemberJoinedParty)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_MemberLeftParty : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_MemberLeftParty)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_Game_Started : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_Game_Started)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_Game_Ending : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_Game_Ending)
	virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_Game_Ended : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_Game_Ended)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_Got_Room_Invite : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_Got_Room_Invite)
	virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
/// \ingroup LOBBY_2_NOTIFICATIONS
struct Notification_Console_Accepted_Room_Invite : public Lobby2Message
{
	__L2_MSG_BASE_IMPL(Notification_Console_Accepted_Room_Invite)
		virtual bool RequiresAdmin(void) const {return true;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return false;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual bool IsNotification(void) const {return true;}
};
// --------------------------------------------- Base interface of factory class for all messages --------------------------------------------
#define __L2_ALLOCATE_AND_DEFINE(FACTORY, __TYPE__,VAR_NAME) RakNet::__TYPE__ *VAR_NAME = (RakNet::__TYPE__ *) FACTORY->Alloc(L2MID_##__TYPE__); RakAssert(VAR_NAME);
#define __L2_MSG_FACTORY_BASE(__NAME__) {case L2MID_##__NAME__ : Lobby2Message *m = RakNet::OP_NEW< __NAME__ >( _FILE_AND_LINE_ ) ; RakAssert(m->GetID()==L2MID_##__NAME__ ); m->requestId=nextRequestId++; return m;}
/// \ingroup LOBBY_2_GROUP
struct Lobby2MessageFactory
{
	Lobby2MessageFactory() {nextRequestId=0;}
	virtual ~Lobby2MessageFactory() {}
	virtual Lobby2Message *Alloc(Lobby2MessageID id)
	{
		switch (id)
		{
			__L2_MSG_FACTORY_BASE(Platform_Startup);
			__L2_MSG_FACTORY_BASE(Platform_Shutdown);
			__L2_MSG_FACTORY_BASE(System_CreateDatabase);
			__L2_MSG_FACTORY_BASE(System_DestroyDatabase);
			__L2_MSG_FACTORY_BASE(System_CreateTitle);
			__L2_MSG_FACTORY_BASE(System_DestroyTitle);
			__L2_MSG_FACTORY_BASE(System_GetTitleRequiredAge);
			__L2_MSG_FACTORY_BASE(System_GetTitleBinaryData);
			__L2_MSG_FACTORY_BASE(System_RegisterProfanity);
			__L2_MSG_FACTORY_BASE(System_BanUser);
			__L2_MSG_FACTORY_BASE(System_UnbanUser);
			__L2_MSG_FACTORY_BASE(CDKey_Add);
			__L2_MSG_FACTORY_BASE(CDKey_GetStatus);
			__L2_MSG_FACTORY_BASE(CDKey_Use);
			__L2_MSG_FACTORY_BASE(CDKey_FlagStolen);
			__L2_MSG_FACTORY_BASE(Client_Login);
			__L2_MSG_FACTORY_BASE(Client_Logoff);
			__L2_MSG_FACTORY_BASE(Client_RegisterAccount);
			__L2_MSG_FACTORY_BASE(System_SetEmailAddressValidated);
			__L2_MSG_FACTORY_BASE(Client_ValidateHandle);
			__L2_MSG_FACTORY_BASE(System_DeleteAccount);
			__L2_MSG_FACTORY_BASE(System_PruneAccounts);
			__L2_MSG_FACTORY_BASE(Client_GetEmailAddress);
			__L2_MSG_FACTORY_BASE(Client_GetPasswordRecoveryQuestionByHandle);
			__L2_MSG_FACTORY_BASE(Client_GetPasswordByPasswordRecoveryAnswer);
			__L2_MSG_FACTORY_BASE(Client_ChangeHandle);
			__L2_MSG_FACTORY_BASE(Client_UpdateAccount);
			__L2_MSG_FACTORY_BASE(Client_GetAccountDetails);
			__L2_MSG_FACTORY_BASE(Client_StartIgnore);
			__L2_MSG_FACTORY_BASE(Client_StopIgnore);
			__L2_MSG_FACTORY_BASE(Client_GetIgnoreList);
			__L2_MSG_FACTORY_BASE(Client_PerTitleIntegerStorage);
			__L2_MSG_FACTORY_BASE(Client_PerTitleBinaryStorage);
			__L2_MSG_FACTORY_BASE(Client_SetPresence);
			__L2_MSG_FACTORY_BASE(Client_GetPresence);
			__L2_MSG_FACTORY_BASE(Friends_SendInvite);
			__L2_MSG_FACTORY_BASE(Friends_AcceptInvite);
			__L2_MSG_FACTORY_BASE(Friends_RejectInvite);
			__L2_MSG_FACTORY_BASE(Friends_GetInvites);
			__L2_MSG_FACTORY_BASE(Friends_GetFriends);
			__L2_MSG_FACTORY_BASE(Friends_Remove);
			__L2_MSG_FACTORY_BASE(BookmarkedUsers_Add);
			__L2_MSG_FACTORY_BASE(BookmarkedUsers_Remove);
			__L2_MSG_FACTORY_BASE(BookmarkedUsers_Get);
			__L2_MSG_FACTORY_BASE(Emails_Send);
			__L2_MSG_FACTORY_BASE(Emails_Get);
			__L2_MSG_FACTORY_BASE(Emails_Delete);
			__L2_MSG_FACTORY_BASE(Emails_SetStatus);
			__L2_MSG_FACTORY_BASE(Ranking_SubmitMatch);
			__L2_MSG_FACTORY_BASE(Ranking_GetMatches);
			__L2_MSG_FACTORY_BASE(Ranking_GetMatchBinaryData);
			__L2_MSG_FACTORY_BASE(Ranking_GetTotalScore);
			__L2_MSG_FACTORY_BASE(Ranking_WipeScoresForPlayer);
			__L2_MSG_FACTORY_BASE(Ranking_WipeMatches);
			__L2_MSG_FACTORY_BASE(Ranking_PruneMatches);
			__L2_MSG_FACTORY_BASE(Ranking_UpdateRating);
			__L2_MSG_FACTORY_BASE(Ranking_WipeRatings);
			__L2_MSG_FACTORY_BASE(Ranking_GetRating);
			__L2_MSG_FACTORY_BASE(Clans_Create);
			__L2_MSG_FACTORY_BASE(Clans_SetProperties);
			__L2_MSG_FACTORY_BASE(Clans_GetProperties);
			__L2_MSG_FACTORY_BASE(Clans_SetMyMemberProperties);
			__L2_MSG_FACTORY_BASE(Clans_GrantLeader);
			__L2_MSG_FACTORY_BASE(Clans_SetSubleaderStatus);
			__L2_MSG_FACTORY_BASE(Clans_SetMemberRank);
			__L2_MSG_FACTORY_BASE(Clans_GetMemberProperties);
			__L2_MSG_FACTORY_BASE(Clans_ChangeHandle);
			__L2_MSG_FACTORY_BASE(Clans_Leave);
			__L2_MSG_FACTORY_BASE(Clans_Get);
			__L2_MSG_FACTORY_BASE(Clans_SendJoinInvitation);
			__L2_MSG_FACTORY_BASE(Clans_WithdrawJoinInvitation);
			__L2_MSG_FACTORY_BASE(Clans_AcceptJoinInvitation);
			__L2_MSG_FACTORY_BASE(Clans_RejectJoinInvitation);
			__L2_MSG_FACTORY_BASE(Clans_DownloadInvitationList);
			__L2_MSG_FACTORY_BASE(Clans_SendJoinRequest);
			__L2_MSG_FACTORY_BASE(Clans_WithdrawJoinRequest);
			__L2_MSG_FACTORY_BASE(Clans_AcceptJoinRequest);
			__L2_MSG_FACTORY_BASE(Clans_RejectJoinRequest);
			__L2_MSG_FACTORY_BASE(Clans_DownloadRequestList);
			__L2_MSG_FACTORY_BASE(Clans_KickAndBlacklistUser);
			__L2_MSG_FACTORY_BASE(Clans_UnblacklistUser);
			__L2_MSG_FACTORY_BASE(Clans_GetBlacklist);
			__L2_MSG_FACTORY_BASE(Clans_GetMembers);
			__L2_MSG_FACTORY_BASE(Clans_GetList);
			__L2_MSG_FACTORY_BASE(Clans_CreateBoard);
			__L2_MSG_FACTORY_BASE(Clans_DestroyBoard);
			__L2_MSG_FACTORY_BASE(Clans_CreateNewTopic);
			__L2_MSG_FACTORY_BASE(Clans_ReplyToTopic);
			__L2_MSG_FACTORY_BASE(Clans_RemovePost);
			__L2_MSG_FACTORY_BASE(Clans_GetBoards);
			__L2_MSG_FACTORY_BASE(Clans_GetTopics);
			__L2_MSG_FACTORY_BASE(Clans_GetPosts);
			__L2_MSG_FACTORY_BASE(Console_GameBootCheck);
			__L2_MSG_FACTORY_BASE(Console_GetGameBootInviteDetails);
			__L2_MSG_FACTORY_BASE(Console_GetServerStatus);
			__L2_MSG_FACTORY_BASE(Console_GetWorldListFromServer);
			__L2_MSG_FACTORY_BASE(Console_GetLobbyListFromWorld);
			__L2_MSG_FACTORY_BASE(Console_JoinLobby);
			__L2_MSG_FACTORY_BASE(Console_LeaveLobby);
			__L2_MSG_FACTORY_BASE(Console_SendLobbyChatMessage);
			__L2_MSG_FACTORY_BASE(Console_SearchRooms);
			__L2_MSG_FACTORY_BASE(Console_GetRoomDetails);
			__L2_MSG_FACTORY_BASE(Console_GetLobbyMemberData);
			__L2_MSG_FACTORY_BASE(Console_CreateRoom);
			__L2_MSG_FACTORY_BASE(Console_SignIntoRoom);
			__L2_MSG_FACTORY_BASE(Console_SetRoomSearchProperties);
			__L2_MSG_FACTORY_BASE(Console_UpdateRoomParameters);
			__L2_MSG_FACTORY_BASE(Console_JoinRoom);
			__L2_MSG_FACTORY_BASE(Console_LeaveRoom);
			__L2_MSG_FACTORY_BASE(Console_SendLobbyInvitationToRoom);
			__L2_MSG_FACTORY_BASE(Console_SendGUIInvitationToRoom);
			__L2_MSG_FACTORY_BASE(Console_SendDataMessageToUser);
			__L2_MSG_FACTORY_BASE(Console_SendRoomChatMessage);
			__L2_MSG_FACTORY_BASE(Console_ShowFriendsUI);
			__L2_MSG_FACTORY_BASE(Console_EndGame); // Currently xbox only
			__L2_MSG_FACTORY_BASE(Console_StartGame); // Currently xbox only
			__L2_MSG_FACTORY_BASE(Console_ShowPartyUI);
			__L2_MSG_FACTORY_BASE(Console_ShowMessagesUI);
			__L2_MSG_FACTORY_BASE(Console_ShowGUIInvitationsToRooms);
			__L2_MSG_FACTORY_BASE(Console_EnableDisableRoomVoiceChat);
			__L2_MSG_FACTORY_BASE(Notification_Client_RemoteLogin);
			__L2_MSG_FACTORY_BASE(Notification_Client_IgnoreStatus);
			__L2_MSG_FACTORY_BASE(Notification_Friends_StatusChange);
			__L2_MSG_FACTORY_BASE(Notification_Friends_PresenceUpdate);
			__L2_MSG_FACTORY_BASE(Notification_User_ChangedHandle);
			__L2_MSG_FACTORY_BASE(Notification_Friends_CreatedClan);
			__L2_MSG_FACTORY_BASE(Notification_Emails_Received);
			__L2_MSG_FACTORY_BASE(Notification_Clans_GrantLeader);
			__L2_MSG_FACTORY_BASE(Notification_Clans_SetSubleaderStatus);
			__L2_MSG_FACTORY_BASE(Notification_Clans_SetMemberRank);
			__L2_MSG_FACTORY_BASE(Notification_Clans_ChangeHandle);
			__L2_MSG_FACTORY_BASE(Notification_Clans_Leave);
			__L2_MSG_FACTORY_BASE(Notification_Clans_PendingJoinStatus);
			__L2_MSG_FACTORY_BASE(Notification_Clans_NewClanMember);
			__L2_MSG_FACTORY_BASE(Notification_Clans_KickAndBlacklistUser);
			__L2_MSG_FACTORY_BASE(Notification_Clans_UnblacklistUser);
			__L2_MSG_FACTORY_BASE(Notification_Clans_Destroyed);
			__L2_MSG_FACTORY_BASE(Notification_Console_MemberJoinedLobby);
			__L2_MSG_FACTORY_BASE(Notification_Console_MemberLeftLobby);
			__L2_MSG_FACTORY_BASE(Notification_Console_LobbyDestroyed);
			__L2_MSG_FACTORY_BASE(Notification_Console_LobbyMemberDataUpdated);
			__L2_MSG_FACTORY_BASE(Notification_Console_LobbyGotChatMessage);
			__L2_MSG_FACTORY_BASE(Notification_Console_LobbyGotRoomInvitation);
			__L2_MSG_FACTORY_BASE(Notification_Console_MemberJoinedRoom);
			__L2_MSG_FACTORY_BASE(Notification_Console_MemberLeftRoom);
			__L2_MSG_FACTORY_BASE(Notification_Console_KickedOutOfRoom);
			__L2_MSG_FACTORY_BASE(Notification_Console_RoomWasDestroyed);
			__L2_MSG_FACTORY_BASE(Notification_Console_UpdateRoomParameters);
			__L2_MSG_FACTORY_BASE(Notification_Console_RoomOwnerChanged);
			__L2_MSG_FACTORY_BASE(Notification_Console_RoomChatMessage);
			__L2_MSG_FACTORY_BASE(Notification_Console_RoomMessage);
//			__L2_MSG_FACTORY_BASE(Notification_Console_RoomMemberConnectivityUpdate);
			__L2_MSG_FACTORY_BASE(Notification_Console_ChatEvent);
			__L2_MSG_FACTORY_BASE(Notification_Console_MuteListChanged);
			__L2_MSG_FACTORY_BASE(Notification_Console_Local_Users_Changed);
			__L2_MSG_FACTORY_BASE(Notification_ReceivedDataMessageFromUser);
			__L2_MSG_FACTORY_BASE(Notification_Console_MemberJoinedParty);
			__L2_MSG_FACTORY_BASE(Notification_Console_MemberLeftParty);
			__L2_MSG_FACTORY_BASE(Notification_Console_Game_Started); // Currently XBOX only
			__L2_MSG_FACTORY_BASE(Notification_Console_Game_Ending); // Currently XBOX only
			__L2_MSG_FACTORY_BASE(Notification_Console_Game_Ended); // Currently XBOX only
			__L2_MSG_FACTORY_BASE(Notification_Console_Got_Room_Invite);
			__L2_MSG_FACTORY_BASE(Notification_Console_Accepted_Room_Invite);

		default:
			return 0;
		};
	}
	void Dealloc(Lobby2Message *msg) {
		msg->Deref();
		if (msg->GetRefCount()<=0)
		{
			// Only delete one message at a time or else GetRefCount may be called on the same message in two threads at the same time and not be accurate
			deallocateLockMutex.Lock();
			if (msg->GetRefCount()<=0)
				RakNet::OP_DELETE<Lobby2Message>(msg, _FILE_AND_LINE_ );
			deallocateLockMutex.Unlock();
		}
	}

	unsigned int nextRequestId;
	SimpleMutex deallocateLockMutex;
};

} // namespace RakNet

#endif

