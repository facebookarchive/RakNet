/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "Lobby2Message.h"
#include "Lobby2Client.h"

using namespace RakNet;

uint32_t Lobby2Callbacks::nextCallbackId=0;

void Lobby2Callbacks::MessageResult(Platform_Startup *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Platform_Shutdown *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(System_CreateDatabase *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(System_DestroyDatabase *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(System_CreateTitle *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(System_DestroyTitle *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(System_GetTitleRequiredAge *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(System_GetTitleBinaryData *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(System_RegisterProfanity *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(System_BanUser *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(System_UnbanUser *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(CDKey_Add *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(CDKey_GetStatus *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(CDKey_Use *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(CDKey_FlagStolen *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Client_Login *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Client_Logoff *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Client_RegisterAccount *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(System_SetEmailAddressValidated *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Client_ValidateHandle *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(System_DeleteAccount *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(System_PruneAccounts *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Client_GetEmailAddress *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Client_GetPasswordRecoveryQuestionByHandle *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Client_GetPasswordByPasswordRecoveryAnswer *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Client_ChangeHandle *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Client_UpdateAccount *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Client_GetAccountDetails *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Client_StartIgnore *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Client_StopIgnore *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Client_GetIgnoreList *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Client_PerTitleIntegerStorage *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Client_PerTitleBinaryStorage *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Client_SetPresence *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Client_GetPresence *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Friends_SendInvite *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Friends_AcceptInvite *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Friends_RejectInvite *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Friends_GetInvites *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Friends_GetFriends *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Friends_Remove *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(BookmarkedUsers_Add *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(BookmarkedUsers_Remove *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(BookmarkedUsers_Get *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Emails_Send *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Emails_Get *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Emails_Delete *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Emails_SetStatus *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Ranking_SubmitMatch *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Ranking_GetMatches *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Ranking_GetMatchBinaryData *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Ranking_GetTotalScore *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Ranking_WipeScoresForPlayer *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Ranking_WipeMatches *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Ranking_PruneMatches *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Ranking_UpdateRating *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Ranking_WipeRatings *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Ranking_GetRating *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_Create *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_SetProperties *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_GetProperties *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_SetMyMemberProperties *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_GrantLeader *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_SetSubleaderStatus *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_SetMemberRank *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_GetMemberProperties *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_ChangeHandle *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_Leave *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_Get *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_SendJoinInvitation *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_WithdrawJoinInvitation *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_AcceptJoinInvitation *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_RejectJoinInvitation *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_DownloadInvitationList *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_SendJoinRequest *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_WithdrawJoinRequest *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_AcceptJoinRequest *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_RejectJoinRequest *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_DownloadRequestList *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_KickAndBlacklistUser *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_UnblacklistUser *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_GetBlacklist *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_GetMembers *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_GetList *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_CreateBoard *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_DestroyBoard *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_CreateNewTopic *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_ReplyToTopic *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_RemovePost *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_GetBoards *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_GetTopics *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Clans_GetPosts *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_GameBootCheck *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_GetGameBootInviteDetails *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_GetServerStatus *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_GetWorldListFromServer *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_GetLobbyListFromWorld *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_JoinLobby *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_LeaveLobby *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_SendLobbyChatMessage *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_SearchRooms *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_GetRoomDetails *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_GetLobbyMemberData *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_CreateRoom *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_SignIntoRoom *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_SetRoomSearchProperties *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_UpdateRoomParameters *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_JoinRoom *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_LeaveRoom *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_SendLobbyInvitationToRoom *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_SendGUIInvitationToRoom *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_SendDataMessageToUser *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_SendRoomChatMessage *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_ShowFriendsUI *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_EndGame *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_StartGame *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_ShowPartyUI *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_ShowMessagesUI *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_ShowGUIInvitationsToRooms *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Console_EnableDisableRoomVoiceChat *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Client_RemoteLogin *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Client_IgnoreStatus *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Friends_StatusChange *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Friends_PresenceUpdate *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_User_ChangedHandle *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Friends_CreatedClan *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Emails_Received *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Clans_GrantLeader *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Clans_SetSubleaderStatus *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Clans_SetMemberRank *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Clans_ChangeHandle *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Clans_Leave *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Clans_PendingJoinStatus *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Clans_NewClanMember *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Clans_KickAndBlacklistUser *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Clans_UnblacklistUser *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Clans_Destroyed *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_CableDisconnected *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_ContextError *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_MemberJoinedLobby *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_MemberLeftLobby *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_LobbyDestroyed *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_LobbyMemberDataUpdated *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_LobbyGotChatMessage *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_LobbyGotRoomInvitation *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_MemberJoinedRoom *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_MemberLeftRoom *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_KickedOutOfRoom *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_RoomWasDestroyed *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_UpdateRoomParameters *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_RoomOwnerChanged *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_RoomChatMessage *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_RoomMessage *message) {ExecuteDefaultResult(message);}
//void Lobby2Callbacks::MessageResult(Notification_Console_RoomMemberConnectivityUpdate *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_ChatEvent *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_MuteListChanged *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_Local_Users_Changed *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_ReceivedDataMessageFromUser *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_MemberJoinedParty *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_MemberLeftParty *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_Game_Started *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_Game_Ending *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_Game_Ended *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_Got_Room_Invite *message) {ExecuteDefaultResult(message);}
void Lobby2Callbacks::MessageResult(Notification_Console_Accepted_Room_Invite *message) {ExecuteDefaultResult(message);}

Lobby2Message::Lobby2Message() {refCount=1; requestId=(unsigned int)-1; callbackId=(uint32_t)-1;



}
void Lobby2Message::SerializeBase(bool writeToBitstream, bool serializeOutput, BitStream *bitStream)
{
	bitStream->Serialize(writeToBitstream, requestId);
	bitStream->Serialize(writeToBitstream, callbackId);
	if (serializeOutput)
		bitStream->Serialize(writeToBitstream, resultCode);
}
void Lobby2Message::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream ) { SerializeBase(writeToBitstream, serializeOutput, bitStream); }
bool Lobby2Message::ValidateBinary( RakNetSmartPtr<BinaryDataBlock>binaryDataBlock)
{
	if (binaryDataBlock->binaryDataLength>L2_MAX_BINARY_DATA_LENGTH)
	{
		resultCode=L2RC_BINARY_DATA_LENGTH_EXCEEDED;
		return false;
	}

	if (binaryDataBlock->binaryDataLength>0 && binaryDataBlock->binaryData==0)
	{
		resultCode=L2RC_BINARY_DATA_NULL_POINTER;
		return false;
	}

	return true;
}
bool Lobby2Message::ValidateHandle( RakString *handle )
{
	if (handle->IsEmpty())
	{
		resultCode=L2RC_HANDLE_IS_EMPTY;
		return false;
	}
	if (handle->C_String()[0]==' ')
	{
		resultCode=L2RC_HANDLE_STARTS_WITH_SPACES;
		return false;
	}
	size_t len = handle->GetLength();
	if (handle->C_String()[len-1]==' ')
	{
		resultCode=L2RC_HANDLE_ENDS_WITH_SPACES;
		return false;
	}
	if (len>50)
	{
		resultCode=L2RC_HANDLE_IS_TOO_LONG;
		return false;
	}
	if (len<=2)
	{
		resultCode=L2RC_HANDLE_IS_TOO_SHORT;
		return false;
	}
	if (handle->ContainsNonprintableExceptSpaces())
	{
		resultCode=L2RC_HANDLE_CONTAINS_NON_PRINTABLE;
		return false;
	}
	if (strstr(handle->C_String(), "  ")!=0)
	{
		resultCode=L2RC_HANDLE_HAS_CONSECUTIVE_SPACES;
		return false;
	}
	return true;
}
bool Lobby2Message::ValidateRequiredText( RakString *text )
{
	if (text->IsEmpty())
	{
		resultCode=L2RC_REQUIRED_TEXT_IS_EMPTY;
		return false;
	}
	return true;
}
bool Lobby2Message::ValidatePassword( RakString *text )
{
	if (text->IsEmpty())
	{
		resultCode=L2RC_PASSWORD_IS_EMPTY;
		return false;
	}

	size_t len = text->GetLength();
	if (len>50)
	{
		resultCode=L2RC_PASSWORD_IS_TOO_LONG;
		return false;
	}
	if (len<4)
	{
		resultCode=L2RC_PASSWORD_IS_TOO_SHORT;
		return false;
	}

	return true;
}
bool Lobby2Message::ValidateEmailAddress( RakString *text )
{
	if (text->IsEmpty())
	{
		resultCode=L2RC_EMAIL_ADDRESS_IS_EMPTY;
		return false;
	}
	if (text->IsEmailAddress()==false)
	{
		resultCode=L2RC_EMAIL_ADDRESS_IS_INVALID;
		return false;
	}
	return true;
}
bool Lobby2Message::PrevalidateInput(void) {return true;}
bool Lobby2Message::ClientImpl( RakNet::Lobby2Plugin *client) { (void)client; return true; }
bool Lobby2Message::ServerPreDBMemoryImpl( Lobby2Server *server, RakString userHandle ) { (void)server; (void)userHandle; return false; }
void Lobby2Message::ServerPostDBMemoryImpl( Lobby2Server *server, RakString userHandle ) { (void)server; (void)userHandle; }
bool Lobby2Message::ServerDBImpl( Lobby2ServerCommand *command, void *databaseInterface ) { (void)command; (void)databaseInterface; resultCode=L2RC_COUNT; return true; }

void CreateAccountParameters::Serialize(bool writeToBitstream, BitStream *bitStream)
{
	bitStream->Serialize(writeToBitstream, firstName);
	bitStream->Serialize(writeToBitstream, middleName);
	bitStream->Serialize(writeToBitstream, lastName);
	bitStream->Serialize(writeToBitstream, race);
	bitStream->Serialize(writeToBitstream, sex_male);
	bitStream->Serialize(writeToBitstream, homeAddress1);
	bitStream->Serialize(writeToBitstream, homeAddress2);
	bitStream->Serialize(writeToBitstream, homeCity);
	bitStream->Serialize(writeToBitstream, homeState);
	bitStream->Serialize(writeToBitstream, homeCountry);
	bitStream->Serialize(writeToBitstream, homeZipCode);
	bitStream->Serialize(writeToBitstream, billingAddress1);
	bitStream->Serialize(writeToBitstream, billingAddress2);
	bitStream->Serialize(writeToBitstream, billingCity);
	bitStream->Serialize(writeToBitstream, billingState);
	bitStream->Serialize(writeToBitstream, billingCountry);
	bitStream->Serialize(writeToBitstream, billingZipCode);
	bitStream->Serialize(writeToBitstream, emailAddress);
	bitStream->Serialize(writeToBitstream, password);
	bitStream->Serialize(writeToBitstream, passwordRecoveryQuestion);
	bitStream->Serialize(writeToBitstream, passwordRecoveryAnswer);
	bitStream->Serialize(writeToBitstream, caption1);
	bitStream->Serialize(writeToBitstream, caption2);
	bitStream->Serialize(writeToBitstream, ageInDays);
	binaryData->Serialize(writeToBitstream,bitStream);
}
void BinaryDataBlock::Serialize(bool writeToBitstream, BitStream *bitStream)
{
	bool hasData=binaryData && binaryDataLength>0;
	bitStream->Serialize(writeToBitstream, hasData);
	if (hasData==false)
		return;
	bitStream->Serialize(writeToBitstream, binaryDataLength);
	if (writeToBitstream==false)
	{
		if (binaryData)
			rakFree_Ex(binaryData, _FILE_AND_LINE_ );

		if (binaryDataLength<=L2_MAX_BINARY_DATA_LENGTH)
			binaryData = (char*) rakMalloc_Ex(binaryDataLength, _FILE_AND_LINE_);
		else
			binaryData=0;
	}
	if (binaryData)
		bitStream->Serialize(writeToBitstream, binaryData, binaryDataLength);
	else if (writeToBitstream==false)
		bitStream->IgnoreBytes(binaryDataLength);
}
void PendingInvite::Serialize(bool writeToBitstream, BitStream *bitStream)
{
	bitStream->Serialize(writeToBitstream, sender);
	bitStream->Serialize(writeToBitstream, subject);
	bitStream->Serialize(writeToBitstream, body);
	binaryData->Serialize(writeToBitstream, bitStream);		
}

FriendInfo::FriendInfo() {}
FriendInfo::FriendInfo(const FriendInfo& input) {usernameAndStatus = input.usernameAndStatus;}
FriendInfo& FriendInfo::operator = ( const FriendInfo& input )
{
	usernameAndStatus = input.usernameAndStatus;
	return *this;
}

UsernameAndOnlineStatus::UsernameAndOnlineStatus() {isOnline=false; uid=0;}
UsernameAndOnlineStatus::UsernameAndOnlineStatus(const UsernameAndOnlineStatus& input) {
	handle=input.handle;
	isOnline=input.isOnline;
	uid=input.uid;
	presence=input.presence;
}
UsernameAndOnlineStatus& UsernameAndOnlineStatus::operator = ( const UsernameAndOnlineStatus& input )
{
	handle=input.handle;
	isOnline=input.isOnline;
	uid=input.uid;
	presence=input.presence;
	return *this;
}
void UsernameAndOnlineStatus::Serialize(bool writeToBitstream, BitStream *bitStream)
{
	bitStream->Serialize(writeToBitstream, handle);
	bitStream->Serialize(writeToBitstream, isOnline);
	bitStream->Serialize(writeToBitstream, uid);
	presence.Serialize(bitStream,writeToBitstream);
}
void EmailResult::Serialize(bool writeToBitstream, BitStream *bitStream)
{
	bitStream->Serialize(writeToBitstream, sender);
	bitStream->Serialize(writeToBitstream, recipient);
	bitStream->Serialize(writeToBitstream, subject);
	bitStream->Serialize(writeToBitstream, body);
	bitStream->Serialize(writeToBitstream, status);
	bitStream->Serialize(writeToBitstream, wasSendByMe);
	bitStream->Serialize(writeToBitstream, wasReadByMe);
	bitStream->Serialize(writeToBitstream, subject);
	bitStream->Serialize(writeToBitstream, emailID);
	binaryData->Serialize(writeToBitstream, bitStream);
	bitStream->Serialize(writeToBitstream, creationDate);
}
void MatchParticipant::Serialize(bool writeToBitstream, BitStream *bitStream)
{
	bitStream->Serialize(writeToBitstream, handle);
	bitStream->Serialize(writeToBitstream, score);
}
void SubmittedMatch::Serialize(bool writeToBitstream, BitStream *bitStream)
{
	bitStream->Serialize(writeToBitstream, matchNote);
	bitStream->Serialize(writeToBitstream, whenSubmittedDate);
	bitStream->Serialize(writeToBitstream, matchID);
	binaryData->Serialize(writeToBitstream, bitStream);
	unsigned short listSize = (unsigned short) matchParticipants.Size();
	bitStream->SerializeCompressed(writeToBitstream, listSize);
	for (unsigned int i=0; i < listSize; i++)
	{
		MatchParticipant obj;
		if (writeToBitstream)
		{
			matchParticipants[i].Serialize(writeToBitstream, bitStream);
		}
		else
		{
			obj.Serialize(writeToBitstream, bitStream);
			matchParticipants.Insert(obj, _FILE_AND_LINE_ );
		}
	}
}
void ClanInfo::Serialize(bool writeToBitstream, BitStream *bitStream)
{
	bitStream->Serialize(writeToBitstream, clanName);
	bitStream->Serialize(writeToBitstream, description);
	bitStream->Serialize(writeToBitstream, clanLeader);
	binaryData->Serialize(writeToBitstream, bitStream);
	unsigned short listSize = (unsigned short) clanMembersOtherThanLeader.Size();
	bitStream->SerializeCompressed(writeToBitstream, listSize);
	for (unsigned int i=0; i < listSize; i++)
	{
		RakString obj;
		if (writeToBitstream)
		{
			bitStream->Serialize(writeToBitstream, clanMembersOtherThanLeader[i]);
		}
		else
		{
			bitStream->Serialize(writeToBitstream, obj);
			clanMembersOtherThanLeader.Insert(obj, _FILE_AND_LINE_ );
		}
	}
}
void OpenInvite::Serialize(bool writeToBitstream, BitStream *bitStream)
{
	bitStream->Serialize(writeToBitstream, clanHandle);
}
void ClanJoinRequest::Serialize(bool writeToBitstream, BitStream *bitStream)
{
	bitStream->Serialize(writeToBitstream, targetClan);
	bitStream->Serialize(writeToBitstream, dateSent);
	bitStream->Serialize(writeToBitstream, joinRequestSender);
}
void ClanJoinInvite::Serialize(bool writeToBitstream, BitStream *bitStream)
{
	bitStream->Serialize(writeToBitstream, sourceClan);
	bitStream->Serialize(writeToBitstream, dateSent);
	bitStream->Serialize(writeToBitstream, joinRequestTarget);
}
void BookmarkedUser::Serialize(bool writeToBitstream, BitStream *bitStream)
{
	bitStream->Serialize(writeToBitstream, targetHandle);
	bitStream->Serialize(writeToBitstream, type);
	bitStream->Serialize(writeToBitstream, description);
	bitStream->Serialize(writeToBitstream, dateWhenAdded);
}
void System_CreateTitle::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, titleName);
	bitStream->Serialize(writeToBitstream, titleSecretKey);
	bitStream->Serialize(writeToBitstream, requiredAge);
	binaryData->Serialize(writeToBitstream, bitStream);
}
bool System_CreateTitle::PrevalidateInput(void)
{
	//
	if (!ValidateRequiredText(&titleName)) return false;
	if (!ValidatePassword(&titleSecretKey)) return false;
	if (!ValidateBinary(binaryData)) return false;
	return true;
}

void System_DestroyTitle::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, titleName);
}

void System_GetTitleRequiredAge::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, titleName);
	if (serializeOutput)
		bitStream->Serialize(writeToBitstream, requiredAge);
}

void System_GetTitleBinaryData::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, titleName);
	if (serializeOutput)
		binaryData->Serialize(writeToBitstream,bitStream);
}

void System_RegisterProfanity::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	unsigned short listSize = (unsigned short) profanityWords.Size();
	bitStream->SerializeCompressed(writeToBitstream, listSize);
	for (unsigned int i=0; i < listSize; i++)
	{
		RakString obj;
		if (writeToBitstream)
		{
			bitStream->Serialize(writeToBitstream, profanityWords[i]);
		}
		else
		{
			bitStream->Serialize(writeToBitstream, obj);
			profanityWords.Insert(obj, _FILE_AND_LINE_);
		}
	}
}

void System_BanUser::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,banReason);
	bitStream->Serialize(writeToBitstream,durationHours);
	bitStream->Serialize(writeToBitstream,userName);
}

bool System_BanUser::PrevalidateInput( void )
{
	if (!ValidateHandle(&userName)) return false;
	if (!ValidateRequiredText(&banReason)) return false;
	if (durationHours<=0)
	{
		resultCode=L2RC_System_BanUser_INVALID_DURATION;
		return false;
	}
	return true;
}

void System_UnbanUser::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,reason);
	bitStream->Serialize(writeToBitstream,userName);
}

bool System_UnbanUser::PrevalidateInput( void )
{
	if (!ValidateHandle(&userName)) return false;	
	if (!ValidateRequiredText(&reason)) return false;
	return true;
}

void CDKey_Add::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,titleName);
	unsigned short listSize = (unsigned short) cdKeys.Size();
	bitStream->SerializeCompressed(writeToBitstream, listSize);
	for (unsigned int i=0; i < listSize; i++)
	{
		RakString obj;
		if (writeToBitstream)
		{
			bitStream->Serialize(writeToBitstream, cdKeys[i]);
		}
		else
		{
			bitStream->Serialize(writeToBitstream, obj);
			cdKeys.Insert(obj, _FILE_AND_LINE_);
		}
	}
}

bool CDKey_Add::PrevalidateInput( void )
{
	for (unsigned int i=0; i < cdKeys.Size(); i++)
		if (!ValidateRequiredText(&cdKeys[i])) return false;
	if (!ValidateRequiredText(&titleName)) return false;
	return true;
}

void CDKey_GetStatus::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,cdKey);
	bitStream->Serialize(writeToBitstream,titleName);
	if (serializeOutput)
	{
		bitStream->Serialize(writeToBitstream,usable);
		bitStream->Serialize(writeToBitstream,usedBy);
		bitStream->Serialize(writeToBitstream,activationDate);
	}
}

bool CDKey_GetStatus::PrevalidateInput( void )
{
	if (!ValidateRequiredText(&cdKey)) return false;
	if (!ValidateRequiredText(&titleName)) return false;
	return true;
}

void CDKey_Use::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,cdKey);
	bitStream->Serialize(writeToBitstream,titleName);
	bitStream->Serialize(writeToBitstream,userName);
}

bool CDKey_Use::PrevalidateInput( void )
{
	if (!ValidateHandle(&userName)) return false;
	if (!ValidateRequiredText(&cdKey)) return false;
	if (!ValidateRequiredText(&titleName)) return false;
	return true;
}

void CDKey_FlagStolen::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,cdKey);
	bitStream->Serialize(writeToBitstream,titleName);
	bitStream->Serialize(writeToBitstream,userUsingThisKey);
	bitStream->Serialize(writeToBitstream,wasStolen);
}

bool CDKey_FlagStolen::PrevalidateInput( void )
{
//	
	if (!ValidateRequiredText(&cdKey)) return false;
	if (!ValidateRequiredText(&titleName)) return false;
	return true;
}

void Client_Login::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,userPassword);
	bitStream->Serialize(writeToBitstream,allowMultipleLogins);
	bitStream->Serialize(writeToBitstream,titleName);
	bitStream->Serialize(writeToBitstream,titleSecretKey);
	bitStream->Serialize(writeToBitstream,userName);

	if (serializeOutput)
	{
		bitStream->Serialize(writeToBitstream,bannedReason);
		bitStream->Serialize(writeToBitstream,whenBanned);
		bitStream->Serialize(writeToBitstream,bannedExpiration);
	}
}

bool Client_Login::PrevalidateInput( void )
{
	if (!ValidateHandle(&userName)) return false;
	if (!ValidateHandle(&titleName)) return false;
	if (!ValidateRequiredText(&userPassword)) return false;
	if (!ValidateRequiredText(&titleSecretKey)) return false;
	return true;
}
void Client_RegisterAccount::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	createAccountParameters.Serialize(writeToBitstream, bitStream);
	bitStream->Serialize(writeToBitstream,cdKey);
	bitStream->Serialize(writeToBitstream,titleName);
	bitStream->Serialize(writeToBitstream,userName);
}

bool Client_RegisterAccount::PrevalidateInput( void )
{
	if (!ValidateHandle(&userName)) return false;
	// If either first name or last name is set, then both must be set correctly.
	if (createAccountParameters.firstName.IsEmpty()==false ||
		createAccountParameters.lastName.IsEmpty()==false)
	{
		if (!ValidateHandle(&createAccountParameters.firstName)) return false;
		if (!ValidateHandle(&createAccountParameters.lastName)) return false;
	}
	if (!ValidatePassword(&createAccountParameters.password)) return false;
	// Don't require password recovery
// 	if (!ValidateRequiredText(&createAccountParameters.passwordRecoveryQuestion)) return false;
// 	if (!ValidateRequiredText(&createAccountParameters.passwordRecoveryAnswer)) return false;
	if (createAccountParameters.emailAddress.IsEmpty()==false &&
		!ValidateEmailAddress(&createAccountParameters.emailAddress)) return false;
	return true;
}

void System_SetEmailAddressValidated::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,validated);
	bitStream->Serialize(writeToBitstream,userName);
}
bool System_SetEmailAddressValidated::PrevalidateInput( void )
{
	if (!ValidateHandle(&userName)) return false;
	return true;
}
void Client_ValidateHandle::Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,userName);
}
bool Client_ValidateHandle::PrevalidateInput(void)
{
	if (!ValidateHandle(&userName)) return false;
	return true;
}

void System_DeleteAccount::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,password);
	bitStream->Serialize(writeToBitstream,userName);
}

bool System_DeleteAccount::PrevalidateInput( void )
{
	if (!ValidateHandle(&userName)) return false;
	if (!ValidatePassword(&password)) return false;
	return true;
}
 bool System_PruneAccounts::PrevalidateInput(void)
{
	if (deleteAccountsNotLoggedInDays==0) return false;
	return true;
}
void System_PruneAccounts::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,deleteAccountsNotLoggedInDays);
}
bool Client_GetEmailAddress::PrevalidateInput(void)
{
	if (!ValidateHandle(&userName)) return false;
	return true;
}
void Client_GetEmailAddress::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,userName);
	if (serializeOutput)
	{
		bitStream->Serialize(writeToBitstream,emailAddress);
		bitStream->Serialize(writeToBitstream,emailAddressValidated);
	}	
}
bool Client_GetPasswordRecoveryQuestionByHandle::PrevalidateInput(void)
{
	if (!ValidateHandle(&userName)) return false;
	return true;
}
void Client_GetPasswordRecoveryQuestionByHandle::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,userName);
	if (serializeOutput)
	{
		bitStream->Serialize(writeToBitstream,passwordRecoveryQuestion);
	}
}
bool Client_GetPasswordByPasswordRecoveryAnswer::PrevalidateInput(void)
{
	if (!ValidateHandle(&userName)) return false;
	if (!ValidateRequiredText( &passwordRecoveryAnswer )) return false;
	return true;
}
void Client_GetPasswordByPasswordRecoveryAnswer::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,userName);
	bitStream->Serialize(writeToBitstream,passwordRecoveryAnswer);
	if (serializeOutput)
	{
		bitStream->Serialize(writeToBitstream,password);
	}
}

void Client_ChangeHandle::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,userName);
	bitStream->Serialize(writeToBitstream,newHandle);
}
bool Client_ChangeHandle::PrevalidateInput(void)
{
	if (!ValidateHandle(&userName)) return false;
	if (!ValidateHandle(&newHandle)) return false;
	if (userName==newHandle)
	{
		resultCode=L2RC_Client_ChangeHandle_HANDLE_NOT_CHANGED;
		return false;
	}
	return true;
}
void Client_UpdateAccount::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	createAccountParameters.Serialize(writeToBitstream, bitStream);
}

bool Client_UpdateAccount::PrevalidateInput( void )
{
	if (!ValidateHandle(&createAccountParameters.firstName)) return false;
	if (!ValidateHandle(&createAccountParameters.lastName)) return false;
	if (!ValidatePassword(&createAccountParameters.password)) return false;
	if (!ValidateRequiredText(&createAccountParameters.passwordRecoveryQuestion)) return false;
	if (!ValidateRequiredText(&createAccountParameters.passwordRecoveryAnswer)) return false;
	return true;
}

void Client_GetAccountDetails::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	if (serializeOutput)
	{
		createAccountParameters.Serialize(writeToBitstream, bitStream);
	}
}

void Client_StartIgnore::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,targetHandle);
}

bool Client_StartIgnore::PrevalidateInput( void )
{
	if (!ValidateHandle(&targetHandle)) return false;
	return true;
}

void Client_StopIgnore::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,targetHandle);
}

bool Client_StopIgnore::PrevalidateInput( void )
{
	if (!ValidateHandle(&targetHandle)) return false;
	return true;
}

void Client_GetIgnoreList::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	if (serializeOutput)
	{
		unsigned short listSize = (unsigned short) ignoredHandles.Size();
		bitStream->SerializeCompressed(writeToBitstream, listSize);
		for (unsigned int i=0; i < listSize; i++)
		{
			RakString obj;
			if (writeToBitstream)
			{
				bitStream->Serialize(writeToBitstream, ignoredHandles[i]);
			}
			else
			{
				bitStream->Serialize(writeToBitstream, obj);
				ignoredHandles.Insert(obj, _FILE_AND_LINE_);
			}
		}
	}
}
bool Client_PerTitleIntegerStorage::PrevalidateInput(void)
{
	if (!ValidateRequiredText(&titleName)) return false;
	return true;
}
void Client_PerTitleIntegerStorage::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,titleName);
	bitStream->Serialize(writeToBitstream,slotIndex);
	unsigned char c;
	c = (unsigned char)addConditionForOperation;
	bitStream->Serialize(writeToBitstream,c);
	addConditionForOperation = (PTIS_Condition) c;
	c = (unsigned char)operationToPerform;
	bitStream->Serialize(writeToBitstream,c);
	operationToPerform = (PTIS_Operation) c;
	bitStream->Serialize(writeToBitstream,conditionValue);
	if (operationToPerform!=PTISO_DELETE)
	{
		if (operationToPerform==PTISO_ADD || operationToPerform==PTISO_WRITE)
			bitStream->Serialize(writeToBitstream,inputValue);
		if (serializeOutput)
		{
			if (operationToPerform==PTISO_WRITE)
				outputValue=inputValue;
			else if (operationToPerform==PTISO_READ ||
				operationToPerform==PTISO_ADD)
				bitStream->Serialize(writeToBitstream,outputValue);
		}
	}
}
bool Client_PerTitleBinaryStorage::PrevalidateInput(void)
{
	if (!ValidateRequiredText(&titleName)) return false;
	if (!ValidateBinary(binaryData)) return false;
	return true;
}
void Client_PerTitleBinaryStorage::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,titleName);
	bitStream->Serialize(writeToBitstream,slotIndex);
	if (operationToPerform!=PTISO_DELETE)
		binaryData->Serialize(writeToBitstream, bitStream);
}
void Client_SetPresence::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	presence.Serialize(bitStream,writeToBitstream);
}
// bool Client_SetPresence::ServerPreDBMemoryImpl( Lobby2Server *server, RakString userHandle )
// {
// 	// This has to go in Lobby2Message_PGSQL.h because the server and client both share this file, and the client doesn't know about the server
// 	server->SetPresence( presence, systemAddress );
// 	return true;
// }
void Client_GetPresence::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,userHandle);
	if (serializeOutput)
	{
		presence.Serialize(bitStream,writeToBitstream);
	}
}
// bool Client_GetPresence::ServerPreDBMemoryImpl( Lobby2Server *server, RakString userHandle )
// {
// 	// This has to go in Lobby2Message_PGSQL.h because the server and client both share this file, and the client doesn't know about the server
// 	server->GetPresence( presence, userHandle );
// 	return true;
// }
void Friends_SendInvite::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,targetHandle);
	bitStream->Serialize(writeToBitstream,subject);
	bitStream->Serialize(writeToBitstream,body);
	bitStream->Serialize(writeToBitstream,emailStatus);
	binaryData->Serialize(writeToBitstream, bitStream);
}

bool Friends_SendInvite::PrevalidateInput( void )
{
	if (!ValidateHandle(&targetHandle)) return false;
	// if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body)) return false;
	if (!ValidateBinary(binaryData)) return false;
	return true;
}

void Friends_AcceptInvite::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,targetHandle);
	bitStream->Serialize(writeToBitstream,subject);
	bitStream->Serialize(writeToBitstream,body);
	bitStream->Serialize(writeToBitstream,emailStatus);
	binaryData->Serialize(writeToBitstream, bitStream);
	if (serializeOutput)
		presence.Serialize(bitStream,writeToBitstream);
}

bool Friends_AcceptInvite::PrevalidateInput( void )
{
	if (!ValidateHandle(&targetHandle)) return false;
	// if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body)) return false;
	if (!ValidateBinary(binaryData)) return false;
	return true;
}

void Friends_RejectInvite::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,targetHandle);
	bitStream->Serialize(writeToBitstream,subject);
	bitStream->Serialize(writeToBitstream,body);
	bitStream->Serialize(writeToBitstream,emailStatus);
	binaryData->Serialize(writeToBitstream, bitStream);
}

bool Friends_RejectInvite::PrevalidateInput( void )
{
	if (!ValidateHandle(&targetHandle)) return false;
	// if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body)) return false;
	if (!ValidateBinary(binaryData)) return false;
	return true;
}

void Friends_GetInvites::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	if (serializeOutput)
	{
		unsigned short listSize = (unsigned short) invitesSent.Size();
		bitStream->SerializeCompressed(writeToBitstream, listSize);
		for (unsigned int i=0; i < listSize; i++)
		{
			FriendInfo obj;
			if (writeToBitstream)
			{
				invitesSent[i].Serialize(writeToBitstream, bitStream);
			}
			else
			{
				obj.Serialize(writeToBitstream, bitStream);
				invitesSent.Insert(obj, _FILE_AND_LINE_);
			}
		}
		listSize = (unsigned short) invitesReceived.Size();
		bitStream->SerializeCompressed(writeToBitstream, listSize);
		for (unsigned int i=0; i < listSize; i++)
		{
			FriendInfo obj;
			if (writeToBitstream)
			{
				invitesReceived[i].Serialize(writeToBitstream, bitStream);
			}
			else
			{
				obj.Serialize(writeToBitstream, bitStream);
				invitesReceived.Insert(obj, _FILE_AND_LINE_);
			}
		}
	}
}

void Friends_GetFriends::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	if (serializeOutput)
	{
		unsigned short listSize = (unsigned short) myFriends.Size();
		bitStream->SerializeCompressed(writeToBitstream, listSize);
		for (unsigned int i=0; i < listSize; i++)
		{
			FriendInfo obj;
			if (writeToBitstream)
			{
				myFriends[i].Serialize(writeToBitstream, bitStream);
			}
			else
			{
				obj.Serialize(writeToBitstream, bitStream);
				myFriends.Insert(obj, _FILE_AND_LINE_);
			}
		}
	}
}

void Friends_Remove::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,targetHandle);
	bitStream->Serialize(writeToBitstream,subject);
	bitStream->Serialize(writeToBitstream,body);
	bitStream->Serialize(writeToBitstream,emailStatus);
	binaryData->Serialize(writeToBitstream, bitStream);
}

bool Friends_Remove::PrevalidateInput( void )
{
	if (!ValidateHandle(&targetHandle)) return false;
	// if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body)) return false;
	if (!ValidateBinary(binaryData)) return false;
	return true;
}

void BookmarkedUsers_Add::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,targetHandle);
	bitStream->Serialize(writeToBitstream,type);
	bitStream->Serialize(writeToBitstream,description);
}

bool BookmarkedUsers_Add::PrevalidateInput( void )
{
	if (!ValidateHandle(&targetHandle)) return false;
	return true;
}
void BookmarkedUsers_Remove::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,targetHandle);
	bitStream->Serialize(writeToBitstream,type);
}

bool BookmarkedUsers_Remove::PrevalidateInput( void )
{
	if (!ValidateHandle(&targetHandle)) return false;
	return true;
}
void BookmarkedUsers_Get::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	if (serializeOutput)
	{
		unsigned short listSize = (unsigned short) bookmarkedUsers.Size();
		bitStream->SerializeCompressed(writeToBitstream, listSize);
		for (unsigned int i=0; i < listSize; i++)
		{
			BookmarkedUser obj;
			if (writeToBitstream)
			{
				bookmarkedUsers[i].Serialize(writeToBitstream, bitStream);
			}
			else
			{
				obj.Serialize(writeToBitstream, bitStream);
				bookmarkedUsers.Insert(obj, _FILE_AND_LINE_);
			}
		}
	}
}

void Emails_Send::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,subject);
	bitStream->Serialize(writeToBitstream,body);
	bitStream->Serialize(writeToBitstream,status);
	binaryData->Serialize(writeToBitstream, bitStream);
	unsigned short listSize = (unsigned short) recipients.Size();
	bitStream->SerializeCompressed(writeToBitstream, listSize);
	for (unsigned int i=0; i < listSize; i++)
	{
		RakString obj;
		if (writeToBitstream)
		{
			bitStream->Serialize(writeToBitstream, recipients[i]);
		}
		else
		{
			bitStream->Serialize(writeToBitstream, obj);
			recipients.Insert(obj, _FILE_AND_LINE_);
		}
	}
}

bool Emails_Send::PrevalidateInput( void )
{
	for (unsigned int i=0; i < recipients.Size(); i++)
		if (!ValidateHandle(&recipients[i])) return false;
	if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body)) return false;
	if (!ValidateBinary(binaryData)) return false;
	if (recipients.Size()==0)
	{
		resultCode=L2RC_Emails_Send_NO_RECIPIENTS;
		return false;
	}
	return true;
}

void Emails_Get::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);

	unsigned short listSize;

	bitStream->Serialize(writeToBitstream,unreadEmailsOnly);
	bitStream->Serialize(writeToBitstream,emailIdsOnly);

	listSize = (unsigned short) emailsToRetrieve.Size();
	bitStream->SerializeCompressed(writeToBitstream, listSize);
	for (unsigned int i=0; i < listSize; i++)
	{
		unsigned int id;
		if (writeToBitstream)
		{
			bitStream->Write(emailsToRetrieve[i]);
		}
		else
		{
			bitStream->Read(id);
			emailsToRetrieve.Push(id,__FILE__, __LINE__);
		}
	}

	if (serializeOutput)
	{
		listSize = (unsigned short) emailResults.Size();
		bitStream->SerializeCompressed(writeToBitstream, listSize);
		for (unsigned int i=0; i < listSize; i++)
		{
			EmailResult obj;
			if (writeToBitstream)
			{
				emailResults[i].Serialize( writeToBitstream, bitStream );
			}
			else
			{
				obj.Serialize( writeToBitstream, bitStream );
				emailResults.Insert(obj, _FILE_AND_LINE_);
			}
		}
	}
}

void Emails_Delete::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,emailId);
}

void Emails_SetStatus::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,emailId);
	bitStream->Serialize(writeToBitstream,updateStatusFlag);
	bitStream->Serialize(writeToBitstream,updateMarkedRead);
	bitStream->Serialize(writeToBitstream,newStatusFlag);
	bitStream->Serialize(writeToBitstream,isNowMarkedRead);
}
bool Emails_SetStatus::PrevalidateInput(void)
{
	if (updateStatusFlag==false && updateMarkedRead==false)
	{
		resultCode=L2RC_Emails_SetStatus_NOTHING_TO_DO;
		return false;
	}
	return true;
}
void Ranking_SubmitMatch::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,titleName);
	bitStream->Serialize(writeToBitstream,gameType);
	submittedMatch.Serialize(writeToBitstream, bitStream);
}

bool Ranking_SubmitMatch::PrevalidateInput( void )
{
	if (!ValidateRequiredText(&titleName)) return false;
	if (!ValidateRequiredText(&gameType)) return false;
	if (!ValidateBinary(submittedMatch.binaryData)) return false;
	if (submittedMatch.matchParticipants.Size()==0)
	{
		resultCode=L2RC_Ranking_SubmitMatch_NO_PARTICIPANTS;
		return false;
	}
	for (unsigned int i=0; i < submittedMatch.matchParticipants.Size(); i++)
		if (!ValidateHandle(&submittedMatch.matchParticipants[i].handle)) return false;
	return true;
}

void Ranking_GetMatches::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,titleName);
	bitStream->Serialize(writeToBitstream,gameType);
	if (serializeOutput)
	{
		unsigned short listSize = (unsigned short) submittedMatches.Size();
		bitStream->SerializeCompressed(writeToBitstream, listSize);
		for (unsigned int i=0; i < listSize; i++)
		{
			SubmittedMatch obj;
			if (writeToBitstream)
			{
				bitStream->Serialize(writeToBitstream, submittedMatches[i]);
			}
			else
			{
				bitStream->Serialize(writeToBitstream, obj);
				submittedMatches.Insert(obj, _FILE_AND_LINE_);
			}
		}
	}
}

bool Ranking_GetMatches::PrevalidateInput( void )
{
	if (!ValidateRequiredText(&titleName)) return false;
	if (!ValidateRequiredText(&gameType)) return false;
	return true;
}

void Ranking_GetMatchBinaryData::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,matchID);
	if (serializeOutput)
	{
		binaryData->Serialize(writeToBitstream,bitStream);
	}
}

void Ranking_GetTotalScore::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,titleName);
	bitStream->Serialize(writeToBitstream,gameType);
	bitStream->Serialize(writeToBitstream,targetHandle);
	if (serializeOutput)
	{
		bitStream->Serialize(writeToBitstream,scoreSum);
		bitStream->Serialize(writeToBitstream,numScoresSubmitted);
	}
}

bool Ranking_GetTotalScore::PrevalidateInput( void )
{
	if (!ValidateHandle(&targetHandle)) return false;
	if (!ValidateRequiredText(&titleName)) return false;
	if (!ValidateRequiredText(&gameType)) return false;
	return true;
}

void Ranking_WipeScoresForPlayer::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,titleName);
	bitStream->Serialize(writeToBitstream,gameType);
	bitStream->Serialize(writeToBitstream,targetHandle);
}

bool Ranking_WipeScoresForPlayer::PrevalidateInput( void )
{
	if (!ValidateHandle(&targetHandle)) return false;
	if (!ValidateRequiredText(&titleName)) return false;
	if (!ValidateRequiredText(&gameType)) return false;
	return true;
}

void Ranking_WipeMatches::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,titleName);
	bitStream->Serialize(writeToBitstream,gameType);
}

bool Ranking_WipeMatches::PrevalidateInput( void )
{
	if (!ValidateRequiredText(&titleName)) return false;
	if (!ValidateRequiredText(&gameType)) return false;
	return true;
}

void Ranking_PruneMatches::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,pruneTimeDays);
}

void Ranking_UpdateRating::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,titleName);
	bitStream->Serialize(writeToBitstream,gameType);
	bitStream->Serialize(writeToBitstream,targetHandle);
	bitStream->Serialize(writeToBitstream,targetRating);
}

bool Ranking_UpdateRating::PrevalidateInput( void )
{	
	if (!ValidateRequiredText(&titleName)) return false;
	if (!ValidateRequiredText(&gameType)) return false;
	if (!ValidateHandle(&targetHandle)) return false;

	return true;
}

void Ranking_WipeRatings::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,titleName);
	bitStream->Serialize(writeToBitstream,gameType);
}

bool Ranking_WipeRatings::PrevalidateInput( void )
{
	
	if (!ValidateRequiredText(&titleName)) return false;
	if (!ValidateRequiredText(&gameType)) return false;
	return true;
}

void Ranking_GetRating::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,titleName);
	bitStream->Serialize(writeToBitstream,gameType);
	bitStream->Serialize(writeToBitstream,targetHandle);
	if (serializeOutput)
		bitStream->Serialize(writeToBitstream,currentRating);
}

bool Ranking_GetRating::PrevalidateInput( void )
{
	
	if (!ValidateRequiredText(&titleName)) return false;
	if (!ValidateRequiredText(&gameType)) return false;
	if (!ValidateHandle(&targetHandle)) return false;
	return true;
}

void Clans_Create::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,failIfAlreadyInClan);
	bitStream->Serialize(writeToBitstream,requiresInvitationsToJoin);
	bitStream->Serialize(writeToBitstream,description);
	binaryData->Serialize(writeToBitstream,bitStream);
}

bool Clans_Create::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	return true;
}

void Clans_SetProperties::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,description);
	binaryData->Serialize(writeToBitstream, bitStream);
}

bool Clans_SetProperties::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateBinary(binaryData)) return false;
	return true;
}

void Clans_GetProperties::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	if (serializeOutput)
	{
		bitStream->Serialize(writeToBitstream,description);
		binaryData->Serialize(writeToBitstream, bitStream);
	}
}

bool Clans_GetProperties::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	return true;
}

void Clans_SetMyMemberProperties::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,description);
	binaryData->Serialize(writeToBitstream, bitStream);
}

bool Clans_SetMyMemberProperties::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateBinary(binaryData)) return false;
	return true;
}

void Clans_GrantLeader::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,targetHandle);
}

bool Clans_GrantLeader::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateHandle(&targetHandle)) return false;
	return true;
}

void Clans_SetSubleaderStatus::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,targetHandle);
	bitStream->Serialize(writeToBitstream,setToSubleader);
}

bool Clans_SetSubleaderStatus::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateHandle(&targetHandle)) return false;
	return true;
}

void Clans_SetMemberRank::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,targetHandle);
	bitStream->Serialize(writeToBitstream,newRank);
}

bool Clans_SetMemberRank::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateHandle(&targetHandle)) return false;
	return true;
}

void Clans_GetMemberProperties::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,targetHandle);
	if (serializeOutput)
	{
		bitStream->Serialize(writeToBitstream,description);
		bitStream->Serialize(writeToBitstream,rank);
		binaryData->Serialize(writeToBitstream,bitStream);
		bitStream->Serialize(writeToBitstream,isSubleader);
		bitStream->Serialize(writeToBitstream,clanMemberState);
		bitStream->Serialize(writeToBitstream,banReason);
	}
}

bool Clans_GetMemberProperties::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	return true;
}

void Clans_ChangeHandle::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,oldClanHandle);
	bitStream->Serialize(writeToBitstream,newClanHandle);
}

bool Clans_ChangeHandle::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&oldClanHandle)) return false;
	if (!ValidateHandle(&newClanHandle)) return false;
	if (oldClanHandle.StrICmp(newClanHandle)==0)
	{
		resultCode=L2RC_Clans_ChangeHandle_HANDLE_NOT_CHANGED;
		return false;
	}
	return true;
}

void Clans_Leave::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,dissolveIfClanLeader);
	bitStream->Serialize(writeToBitstream,subject);
	bitStream->Serialize(writeToBitstream,body);
	binaryData->Serialize(writeToBitstream, bitStream);
	if (serializeOutput)
	{
		bitStream->Serialize(writeToBitstream,wasDissolved);
		bitStream->Serialize(writeToBitstream,newClanLeader);
	}
}

bool Clans_Leave::PrevalidateInput( void )
{
	if (!ValidateHandle(&clanHandle)) return false;
	return true;
}

void Clans_Get::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	if (serializeOutput)
	{
		unsigned short listSize = (unsigned short) clans.Size();
		bitStream->SerializeCompressed(writeToBitstream, listSize);
		for (unsigned int i=0; i < listSize; i++)
		{
			ClanInfo obj;
			if (writeToBitstream)
			{
				clans[i].Serialize(writeToBitstream, bitStream);
			}
			else
			{
				obj.Serialize(writeToBitstream, bitStream);
				clans.Insert(obj, _FILE_AND_LINE_);
			}
		}
	}
}

void Clans_SendJoinInvitation::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,targetHandle);
	bitStream->Serialize(writeToBitstream,subject);
	bitStream->Serialize(writeToBitstream,body);
	bitStream->Serialize(writeToBitstream,emailStatus);
	binaryData->Serialize(writeToBitstream, bitStream);
}

bool Clans_SendJoinInvitation::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateHandle(&targetHandle)) return false;
	if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body)) return false;
	return true;
}

void Clans_WithdrawJoinInvitation::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,targetHandle);
	bitStream->Serialize(writeToBitstream,subject);
	bitStream->Serialize(writeToBitstream,body);
	bitStream->Serialize(writeToBitstream,emailStatus);
	binaryData->Serialize(writeToBitstream, bitStream);
}

bool Clans_WithdrawJoinInvitation::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateHandle(&targetHandle)) return false;
	if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body)) return false;
	return true;
}

void Clans_AcceptJoinInvitation::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,subject);
	bitStream->Serialize(writeToBitstream,body);
	bitStream->Serialize(writeToBitstream,emailStatus);
	binaryData->Serialize(writeToBitstream, bitStream);
	bitStream->Serialize(writeToBitstream,failIfAlreadyInClan);
	binaryData->Serialize(writeToBitstream, bitStream);
}

bool Clans_AcceptJoinInvitation::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body)) return false;
	return true;
}

void Clans_RejectJoinInvitation::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,subject);
	bitStream->Serialize(writeToBitstream,body);
	bitStream->Serialize(writeToBitstream,emailStatus);
	binaryData->Serialize(writeToBitstream, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
}

bool Clans_RejectJoinInvitation::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body)) return false;
	return true;
}

void Clans_DownloadInvitationList::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	if (serializeOutput)
	{
		unsigned short listSize = (unsigned short) invitationsSentToMe.Size();
		bitStream->SerializeCompressed(writeToBitstream, listSize);
		for (unsigned int i=0; i < listSize; i++)
		{
			OpenInvite obj;
			if (writeToBitstream)
			{
				invitationsSentToMe[i].Serialize(writeToBitstream, bitStream);
			}
			else
			{
				obj.Serialize(writeToBitstream, bitStream);
				invitationsSentToMe.Insert(obj, _FILE_AND_LINE_);
			}
		}

		listSize = (unsigned short) usersThatHaveAnInvitationFromClansThatIAmAMemberOf.Size();
		bitStream->SerializeCompressed(writeToBitstream, listSize);
		for (unsigned int i=0; i < listSize; i++)
		{
			if (writeToBitstream)
			{
				usersThatHaveAnInvitationFromClansThatIAmAMemberOf[i].Serialize(writeToBitstream, bitStream);
			}
			else
			{
				ClanJoinInvite obj;
				obj.Serialize(writeToBitstream, bitStream);
				usersThatHaveAnInvitationFromClansThatIAmAMemberOf.Insert(obj, _FILE_AND_LINE_);
			}
		}
	}
}

void Clans_SendJoinRequest::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,subject);
	bitStream->Serialize(writeToBitstream,body);
	bitStream->Serialize(writeToBitstream,emailStatus);
	binaryData->Serialize(writeToBitstream, bitStream);
	if (serializeOutput)
	{
		bitStream->Serialize(writeToBitstream,clanJoined);
	}
}

bool Clans_SendJoinRequest::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body)) return false;
	return true;
}

void Clans_WithdrawJoinRequest::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,subject);
	bitStream->Serialize(writeToBitstream,body);
	bitStream->Serialize(writeToBitstream,emailStatus);
	binaryData->Serialize(writeToBitstream, bitStream);
}

bool Clans_WithdrawJoinRequest::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body)) return false;
	return true;
}

void Clans_AcceptJoinRequest::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,subject);
	bitStream->Serialize(writeToBitstream,body);
	bitStream->Serialize(writeToBitstream,emailStatus);
	binaryData->Serialize(writeToBitstream, bitStream);
	bitStream->Serialize(writeToBitstream,requestingUserHandle);
	bitStream->Serialize(writeToBitstream,failIfAlreadyInClan);
}

bool Clans_AcceptJoinRequest::PrevalidateInput( void )
{
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateHandle(&requestingUserHandle)) return false;
	if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body)) return false;
	return true;
}

void Clans_RejectJoinRequest::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,subject);
	bitStream->Serialize(writeToBitstream,body);
	bitStream->Serialize(writeToBitstream,emailStatus);
	binaryData->Serialize(writeToBitstream, bitStream);
	bitStream->Serialize(writeToBitstream,requestingUserHandle);
}

bool Clans_RejectJoinRequest::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateHandle(&requestingUserHandle)) return false;
	if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body)) return false;
	return true;
}

void Clans_DownloadRequestList::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	if (serializeOutput)
	{
		unsigned short listSize = (unsigned short) joinRequestsToMyClan.Size();
		bitStream->SerializeCompressed(writeToBitstream, listSize);
		for (unsigned int i=0; i < listSize; i++)
		{
			ClanJoinRequest obj;
			if (writeToBitstream)
			{
				joinRequestsToMyClan[i].Serialize(writeToBitstream, bitStream);
			}
			else
			{
				obj.Serialize(writeToBitstream, bitStream);
				joinRequestsToMyClan.Insert(obj, _FILE_AND_LINE_);
			}
		}

		listSize = (unsigned short) joinRequestsFromMe.Size();
		bitStream->SerializeCompressed(writeToBitstream, listSize);
		for (unsigned int i=0; i < listSize; i++)
		{
			ClanJoinRequest obj;
			if (writeToBitstream)
			{
				joinRequestsFromMe[i].Serialize(writeToBitstream, bitStream);
			}
			else
			{
				obj.Serialize(writeToBitstream, bitStream);
				joinRequestsFromMe.Insert(obj, _FILE_AND_LINE_);
			}
		}
	}
}

void Clans_KickAndBlacklistUser::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,subject);
	bitStream->Serialize(writeToBitstream,body);
	bitStream->Serialize(writeToBitstream,targetHandle);
	bitStream->Serialize(writeToBitstream,kick);
	bitStream->Serialize(writeToBitstream,blacklist);
	bitStream->Serialize(writeToBitstream,reason);
	bitStream->Serialize(writeToBitstream,emailStatus);
	binaryData->Serialize(writeToBitstream, bitStream);
}

bool Clans_KickAndBlacklistUser::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateHandle(&targetHandle)) return false;
	if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body)) return false;
	if (!ValidateRequiredText(&reason)) return false;
	return true;
}

void Clans_UnblacklistUser::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,subject);
	bitStream->Serialize(writeToBitstream,body);
	bitStream->Serialize(writeToBitstream,targetHandle);
	bitStream->Serialize(writeToBitstream,emailStatus);
	binaryData->Serialize(writeToBitstream, bitStream);
}

bool Clans_UnblacklistUser::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateHandle(&targetHandle)) return false;
	if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body)) return false;
	return true;
}

void Clans_GetBlacklist::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	if (serializeOutput)
	{
		unsigned short listSize = (unsigned short) blacklistedUsers.Size();
		bitStream->SerializeCompressed(writeToBitstream, listSize);
		for (unsigned int i=0; i < listSize; i++)
		{
			RakString obj;
			if (writeToBitstream)
			{
				bitStream->Serialize(writeToBitstream, blacklistedUsers[i]);
			}
			else
			{
				bitStream->Serialize(writeToBitstream, obj);
				blacklistedUsers.Insert(obj, _FILE_AND_LINE_);
			}
		}
	}
}

bool Clans_GetBlacklist::PrevalidateInput( void )
{
	
	for (unsigned int i=0; i < blacklistedUsers.Size(); i++)
		if (!ValidateHandle(&blacklistedUsers[i])) return false;
	return true;
}

void Clans_GetMembers::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	if (serializeOutput)
	{
		bitStream->Serialize(writeToBitstream,clanLeader);

		unsigned short listSize = (unsigned short) clanMembersOtherThanLeader.Size();
		bitStream->SerializeCompressed(writeToBitstream, listSize);
		for (unsigned int i=0; i < listSize; i++)
		{
			RakString obj;
			if (writeToBitstream)
			{
				bitStream->Serialize(writeToBitstream, clanMembersOtherThanLeader[i]);
			}
			else
			{
				bitStream->Serialize(writeToBitstream, obj);
				clanMembersOtherThanLeader.Insert(obj, _FILE_AND_LINE_);
			}
		}
	}
}

bool Clans_GetMembers::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	return true;
}

void Clans_GetList::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	if (serializeOutput)
	{
		unsigned short listSize = (unsigned short) clanNames.Size();
		bitStream->SerializeCompressed(writeToBitstream, listSize);
		for (unsigned int i=0; i < listSize; i++)
		{
			RakString obj;
			if (writeToBitstream)
			{
				bitStream->Serialize(writeToBitstream, clanNames[i]);
			}
			else
			{
				bitStream->Serialize(writeToBitstream, obj);
				clanNames.Insert(obj, _FILE_AND_LINE_);
			}
		}
	}
}

void Clans_CreateBoard::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,clanBoardName);
	bitStream->Serialize(writeToBitstream,allowPublicReads);
	bitStream->Serialize(writeToBitstream,allowPublicWrites);
	bitStream->Serialize(writeToBitstream,description);
	binaryData->Serialize(writeToBitstream, bitStream);
}

bool Clans_CreateBoard::PrevalidateInput( void )
{
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateHandle(&clanBoardName)) return false;
	return true;
}

void Clans_DestroyBoard::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,clanBoardName);
}

bool Clans_DestroyBoard::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateHandle(&clanBoardName)) return false;
	return true;
}

void Clans_CreateNewTopic::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,clanBoardName);
	bitStream->Serialize(writeToBitstream,body);
	bitStream->Serialize(writeToBitstream,subject);
	binaryData->Serialize(writeToBitstream, bitStream);

	if (serializeOutput)
	{
		bitStream->Serialize(writeToBitstream,postId);
	}
}

bool Clans_CreateNewTopic::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateHandle(&clanBoardName)) return false;
	if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body)) return false;
	if (!ValidateBinary(binaryData)) return false;
	return true;
}

void Clans_ReplyToTopic::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,postId);
	bitStream->Serialize(writeToBitstream,body);
	bitStream->Serialize(writeToBitstream,subject);
	binaryData->Serialize(writeToBitstream, bitStream);
}

bool Clans_ReplyToTopic::PrevalidateInput( void )
{
	
	if (!ValidateRequiredText(&subject) && !ValidateRequiredText(&body)) return false;
	if (!ValidateBinary(binaryData)) return false;
	return true;
}

void Clans_RemovePost::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,postId);
	bitStream->Serialize(writeToBitstream,removeEntireTopic);
}

void Clans_GetBoards::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	if (serializeOutput)
	{
		unsigned short listSize = (unsigned short) clanBoardsNames.Size();
		bitStream->SerializeCompressed(writeToBitstream, listSize);
		for (unsigned int i=0; i < listSize; i++)
		{
			RakString obj;
			if (writeToBitstream)
			{
				bitStream->Serialize(writeToBitstream, clanBoardsNames[i]);
			}
			else
			{
				bitStream->Serialize(writeToBitstream, obj);
				clanBoardsNames.Insert(obj, _FILE_AND_LINE_);
			}
		}
	}
}

bool Clans_GetBoards::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	return true;
}

void Clans_GetTopics::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,clanHandle);
	bitStream->Serialize(writeToBitstream,clanBoardName);
}

bool Clans_GetTopics::PrevalidateInput( void )
{
	
	if (!ValidateHandle(&clanHandle)) return false;
	if (!ValidateHandle(&clanBoardName)) return false;
	return true;
}

void Clans_GetPosts::Serialize( bool writeToBitstream, bool serializeOutput, BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream,postId);
}

bool Clans_GetPosts::PrevalidateInput( void )
{
	
	return true;
}
void RakNet::Notification_Client_RemoteLogin::Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, handle);
}
void RakNet::Notification_Client_IgnoreStatus::Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, nowIgnored);
	bitStream->Serialize(writeToBitstream, otherHandle);
}

void RakNet::Notification_Friends_StatusChange::Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, op);
	bitStream->Serialize(writeToBitstream, otherHandle);
	bitStream->Serialize(writeToBitstream, subject);
	bitStream->Serialize(writeToBitstream, body);
	presence.Serialize(bitStream,writeToBitstream);
}

void RakNet::Notification_Friends_PresenceUpdate::Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	newPresence.Serialize(bitStream,writeToBitstream);
	bitStream->Serialize(writeToBitstream, otherHandle);
}
void RakNet::Notification_User_ChangedHandle::Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, oldHandle);
	bitStream->Serialize(writeToBitstream, newHandle);
}

void RakNet::Notification_Friends_CreatedClan::Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, otherHandle);
	bitStream->Serialize(writeToBitstream, clanName);
}

void RakNet::Notification_Emails_Received::Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, sender);
	bitStream->Serialize(writeToBitstream, subject);
	bitStream->Serialize(writeToBitstream, emailId);
}

void RakNet::Notification_Clans_GrantLeader::Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, clanHandle);
	bitStream->Serialize(writeToBitstream, newLeader);
	bitStream->Serialize(writeToBitstream, oldLeader);
}

void RakNet::Notification_Clans_SetSubleaderStatus::Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, clanHandle);
	bitStream->Serialize(writeToBitstream, targetHandle);
	bitStream->Serialize(writeToBitstream, leaderHandle);
	bitStream->Serialize(writeToBitstream, setToSubleader);
}

void RakNet::Notification_Clans_SetMemberRank::Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, clanHandle);
	bitStream->Serialize(writeToBitstream, targetHandle);
	bitStream->Serialize(writeToBitstream, leaderHandle);
	bitStream->Serialize(writeToBitstream, newRank);
}

void RakNet::Notification_Clans_ChangeHandle::Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, oldClanHandle);
	bitStream->Serialize(writeToBitstream, newClanHandle);
	bitStream->Serialize(writeToBitstream, leaderHandle);
}

void RakNet::Notification_Clans_Leave::Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, clanHandle);
	bitStream->Serialize(writeToBitstream, targetHandle);
}

void RakNet::Notification_Clans_PendingJoinStatus::Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, clanHandle);
	bitStream->Serialize(writeToBitstream, sourceHandle);
	bitStream->Serialize(writeToBitstream, targetHandle);
	bitStream->Serialize(writeToBitstream, clanMemberHandle);	
	unsigned char c1 = (unsigned char) majorOp;
	unsigned char c2 = (unsigned char) minorOp;
	bitStream->Serialize(writeToBitstream, c1);
	bitStream->Serialize(writeToBitstream, c2);
	majorOp=(Notification_Clans_PendingJoinStatus::MajorOp) c1;
	minorOp=(Notification_Clans_PendingJoinStatus::MinorOp) c2;
}

void RakNet::Notification_Clans_NewClanMember::Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, clanHandle);
	bitStream->Serialize(writeToBitstream, targetHandle);
}

void RakNet::Notification_Clans_KickAndBlacklistUser::Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, clanHandle);
	bitStream->Serialize(writeToBitstream, targetHandle);
	bitStream->Serialize(writeToBitstream, blacklistingUserHandle);
	bitStream->Serialize(writeToBitstream, targetHandleWasKicked);
	bitStream->Serialize(writeToBitstream, reason);
}

void RakNet::Notification_Clans_UnblacklistUser::Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, clanHandle);
	bitStream->Serialize(writeToBitstream, targetHandle);
	bitStream->Serialize(writeToBitstream, unblacklistingUserHandle);
}

void RakNet::Notification_Clans_Destroyed::Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream )
{
	SerializeBase(writeToBitstream, serializeOutput, bitStream);
	bitStream->Serialize(writeToBitstream, oldClanLeader);
	bitStream->Serialize(writeToBitstream, clanHandle);
}


bool RakNet::Client_StartIgnore::ClientImpl( RakNet::Lobby2Plugin *client )
{
	(void)client;
//	if (resultCode==L2RC_SUCCESS)
//		client->AddToIgnoreList(targetHandle);
	return true;
}

bool RakNet::Client_StopIgnore::ClientImpl( RakNet::Lobby2Plugin *client )
{
	(void)client;
//	if (resultCode==L2RC_SUCCESS)
//		client->RemoveFromIgnoreList(targetHandle);
	return true;
}

bool RakNet::Client_GetIgnoreList::ClientImpl( RakNet::Lobby2Plugin *client )
{
	(void)client;
//	if (resultCode==L2RC_SUCCESS)
//		client->SetIgnoreList(ignoredHandles);
	return true;
}