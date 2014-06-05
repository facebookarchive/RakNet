/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __LOBBY_2_CLIENT_GFX3_IMPL_H
#define __LOBBY_2_CLIENT_GFX3_IMPL_H

#include "WindowsIncludes.h"
#include "RakNetTypes.h"
#include "FxGameDelegate.h"
#include "Lobby2Message.h"
#include "PluginInterface2.h"

#define ACTIONSCRIPT_CALLABLE_HEADER(functionName) virtual void functionName(const FxDelegateArgs& pparams);
#define ACTIONSCRIPT_CALLABLE_FUNCTION(className, functionName) \
void functionName(const FxDelegateArgs& pparams) \
{ \
	((className*)pparams.GetHandler())->functionName(pparams);  \
} \
void className::functionName(const FxDelegateArgs& pparams)

namespace RakNet {

// GFxPlayerTinyD3D9.cpp has an instance of this class, and callls the corresponding 3 function
// This keeps the patching code out of the GFx sample as much as possible
class Lobby2ClientGFx3Impl : public FxDelegateHandler, public RakNet::Lobby2Callbacks, public PluginInterface2
{
public:
	Lobby2ClientGFx3Impl();
	~Lobby2ClientGFx3Impl();
	void Init(RakNet::Lobby2Client *_lobby2Client, RakNet::Lobby2MessageFactory *_messageFactory, RakPeerInterface *_rakPeer, GPtr<FxDelegate> pDelegate, GPtr<GFxMovieView> pMovie);
	void Update(void);
	void Shutdown(void);

	// Update all callbacks from flash
	void                Accept(CallbackProcessor* cbreg);

	// Calls from Flash
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_ResetDatabase);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Connect);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_RecoverPasswordByUsername);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_GetPasswordByPasswordRecoveryAnswer);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_RegisterAccountStateQuery);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_DeleteAccount);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_GetAccountDetails);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_LoginToAccount);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_DisconnectFromServer);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_CheckCDKey);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_RegisterAccount);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_ValidateHandle);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_ChangeHandle);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Logoff);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_UpdateAccount);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_StartIgnore);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_StopIgnore);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_GetIgnoreList);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_SendInvite);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_AcceptInvite);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_RejectInvite);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_RemoveFriend);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_GetFriendInvites);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_GetFriends);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_SendEmail);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_DeleteEmail);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_UpdateEmail);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_GetEmails);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_Create);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_SetProperties);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_GetProperties);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_SetMyMemberProperties);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_GrantLeader);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_SetSubleaderStatus);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_SetMemberRank);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_GetMemberProperties);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_ChangeHandle);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_Leave);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_Get);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_SendJoinInvitation);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_WithdrawJoinInvitation);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_AcceptJoinInvitation);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_RejectJoinInvitation);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_DownloadInvitationList);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_SendJoinRequest);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_WithdrawJoinRequest);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_AcceptJoinRequest);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_RejectJoinRequest);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_DownloadRequestList);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_KickAndBlacklistUser);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_UnblacklistUser);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_GetBlacklist);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_GetMembers);
	ACTIONSCRIPT_CALLABLE_HEADER(f2c_Clans_GetList);
	// Callbacks from lobby
	virtual void MessageResult(RakNet::CDKey_GetStatus *message);
	virtual void MessageResult(RakNet::System_DeleteAccount *message);
	virtual void MessageResult(RakNet::Client_Login *message);
	virtual void MessageResult(RakNet::System_CreateDatabase *message);
	virtual void MessageResult(RakNet::System_CreateTitle *message);
	virtual void MessageResult(RakNet::Client_ValidateHandle *message);
	virtual void MessageResult(RakNet::Client_RegisterAccount *message);
	virtual void MessageResult(RakNet::Client_UpdateAccount *message);
	virtual void MessageResult(RakNet::Client_ChangeHandle *message);
	virtual void MessageResult(RakNet::Client_GetAccountDetails *message);
	virtual void MessageResult(RakNet::Client_StartIgnore *message);
	virtual void MessageResult(RakNet::Client_StopIgnore *message);
	virtual void MessageResult(RakNet::Client_GetIgnoreList *message);
	virtual void MessageResult(RakNet::Client_GetPasswordRecoveryQuestionByHandle *message);
	virtual void MessageResult(RakNet::Client_GetPasswordByPasswordRecoveryAnswer *message);
	virtual void MessageResult(RakNet::Friends_SendInvite *message);
	virtual void MessageResult(RakNet::Friends_AcceptInvite *message);
	virtual void MessageResult(RakNet::Friends_RejectInvite *message);
	virtual void MessageResult(RakNet::Friends_Remove *message);
	virtual void MessageResult(RakNet::Friends_GetInvites *message);
	virtual void MessageResult(RakNet::Friends_GetFriends *message);
	virtual void MessageResult(RakNet::Emails_Send *message);
	virtual void MessageResult(RakNet::Emails_Delete *message);
	virtual void MessageResult(RakNet::Emails_SetStatus *message);
	virtual void MessageResult(RakNet::Emails_Get *message);
	virtual void MessageResult(RakNet::Clans_Create *message);
	virtual void MessageResult(RakNet::Clans_SetProperties *message);
	virtual void MessageResult(RakNet::Clans_GetProperties *message);
	virtual void MessageResult(RakNet::Clans_SetMyMemberProperties *message);
	virtual void MessageResult(RakNet::Clans_GrantLeader *message);
	virtual void MessageResult(RakNet::Clans_SetSubleaderStatus *message);
	virtual void MessageResult(RakNet::Clans_SetMemberRank *message);
	virtual void MessageResult(RakNet::Clans_GetMemberProperties *message);
	virtual void MessageResult(RakNet::Clans_ChangeHandle *message);
	virtual void MessageResult(RakNet::Clans_Leave *message);
	virtual void MessageResult(RakNet::Clans_Get *message);
	virtual void MessageResult(RakNet::Clans_SendJoinInvitation *message);
	virtual void MessageResult(RakNet::Clans_WithdrawJoinInvitation *message);
	virtual void MessageResult(RakNet::Clans_AcceptJoinInvitation *message);
	virtual void MessageResult(RakNet::Clans_RejectJoinInvitation *message);
	virtual void MessageResult(RakNet::Clans_DownloadInvitationList *message);
	virtual void MessageResult(RakNet::Clans_SendJoinRequest *message);
	virtual void MessageResult(RakNet::Clans_WithdrawJoinRequest *message);
	virtual void MessageResult(RakNet::Clans_AcceptJoinRequest *message);
	virtual void MessageResult(RakNet::Clans_RejectJoinRequest *message);
	virtual void MessageResult(RakNet::Clans_DownloadRequestList *message);
	virtual void MessageResult(RakNet::Clans_KickAndBlacklistUser *message);
	virtual void MessageResult(RakNet::Clans_UnblacklistUser *message);
	virtual void MessageResult(RakNet::Clans_GetBlacklist *message);
	virtual void MessageResult(RakNet::Clans_GetMembers *message);
	virtual void MessageResult(RakNet::Clans_GetList *message);

	virtual void MessageResult(RakNet::Notification_Client_RemoteLogin *message);
	virtual void MessageResult(RakNet::Notification_Client_IgnoreStatus *message);
	virtual void MessageResult(RakNet::Notification_Friends_StatusChange *message);
	virtual void MessageResult(RakNet::Notification_User_ChangedHandle *message);
	virtual void MessageResult(RakNet::Notification_Friends_CreatedClan *message);
	virtual void MessageResult(RakNet::Notification_Emails_Received *message);
	virtual void MessageResult(RakNet::Notification_Clans_GrantLeader *message);
	virtual void MessageResult(RakNet::Notification_Clans_SetSubleaderStatus *message);
	virtual void MessageResult(RakNet::Notification_Clans_SetMemberRank *message);
	virtual void MessageResult(RakNet::Notification_Clans_ChangeHandle *message);
	virtual void MessageResult(RakNet::Notification_Clans_Leave *message);
	virtual void MessageResult(RakNet::Notification_Clans_PendingJoinStatus *message);
	virtual void MessageResult(RakNet::Notification_Clans_NewClanMember *message);
	virtual void MessageResult(RakNet::Notification_Clans_KickAndBlacklistUser *message);
	virtual void MessageResult(RakNet::Notification_Clans_UnblacklistUser *message);
	virtual void MessageResult(RakNet::Notification_Clans_Destroyed *message);


	static void	OpenSite(const FxDelegateArgs& pparams);

	// PluginInterface2
	virtual void OnClosedConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );
	virtual void OnNewConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, bool isIncoming);
	virtual void OnFailedConnectionAttempt(Packet *packet, PI2_FailedConnectionAttemptReason failedConnectionAttemptReason);


	GPtr<FxDelegate> delegate;
	GPtr<GFxMovieView>      movie;

	RakNet::Lobby2Client *lobby2Client;
	RakNet::Lobby2MessageFactory *messageFactory;
	RakNet::RakPeerInterface *rakPeer;
};

} // namespace RakNet

#endif // __LOBBY_2_CLIENT_GFX3_IMPL_H
