/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

// Common includes
#include <stdio.h>
#include <stdlib.h>

#include "GetTime.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "Lobby2ClientGFx3Impl.h"
#include "Lobby2Client.h"

using namespace RakNet;

Lobby2ClientGFx3Impl::Lobby2ClientGFx3Impl()
{
}
Lobby2ClientGFx3Impl::~Lobby2ClientGFx3Impl()
{
	Shutdown();
}
void Lobby2ClientGFx3Impl::Init(RakNet::Lobby2Client *_lobby2Client, RakNet::Lobby2MessageFactory *_messageFactory, RakPeerInterface *_rakPeer, GPtr<FxDelegate> pDelegate, GPtr<GFxMovieView> pMovie)
{
	lobby2Client=_lobby2Client;
	messageFactory=_messageFactory;
	pDelegate->RegisterHandler(this);
	delegate=pDelegate;
	movie=pMovie;
	rakPeer=_rakPeer;

}
void Lobby2ClientGFx3Impl::Update(void)
{
	
}
void Lobby2ClientGFx3Impl::Shutdown(void)
{
	if (delegate.GetPtr()!=0)
	{
		delegate->UnregisterHandler(this);
		delegate.Clear();
	}
	movie.Clear();
	
}

ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_ResetDatabase)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, System_CreateDatabase, m1);
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Connect)
{
	rakPeer->Connect(pparams[0].GetString(), atoi(pparams[1].GetString()), 0,0);
	lobby2Client->SetServerAddress(
		SystemAddress(pparams[0].GetString(), atoi(pparams[1].GetString()))
		);

	FxResponseArgs<0> rargs;
	FxDelegate::Invoke2(movie, "c2f_NotifyConnectingToServer", rargs);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_RecoverPasswordByUsername)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Client_GetPasswordRecoveryQuestionByHandle, m1);
	m1->userName=pparams[0].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_GetPasswordByPasswordRecoveryAnswer)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Client_GetPasswordByPasswordRecoveryAnswer, m1);
	m1->userName=pparams[0].GetString();
	m1->passwordRecoveryAnswer=pparams[1].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_RegisterAccountStateQuery)
{
	bool needsCdKeyToLogin=true;
	if (needsCdKeyToLogin)
	{
		FxResponseArgs<0> rargs;
		FxDelegate::Invoke2(movie, "c2f_SetStateEnterCDKey", rargs);
	}
	else
	{
		FxResponseArgs<0> rargs;
		FxDelegate::Invoke2(movie, "c2f_SetStateRegisterAccount", rargs);
	}
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_DeleteAccount)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, System_DeleteAccount, m1);
	m1->userName=pparams[0].GetString();
	m1->password=pparams[1].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_GetAccountDetails)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Client_GetAccountDetails, m1);
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_LoginToAccount)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Client_Login, m1);
	m1->titleName="Test Title Name";
	m1->titleSecretKey="Test secret key";
	m1->userName=pparams[0].GetString();
	m1->userPassword=pparams[1].GetString();
	bool savePassword=pparams[2].GetBool();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_DisconnectFromServer)
{
	rakPeer->CloseConnection(lobby2Client->GetServerAddress(), true);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_CheckCDKey)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, CDKey_GetStatus, m1);
	m1->titleName="Test Title Name";
	m1->cdKey=pparams[0].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
void ReadAccountBinaryData(FxResponseArgsList &rargs,
						   RakNet::BitStream *serializedBinaryData)
{

	RakNet::RakString aboutMe;
	RakNet::RakString activities;
	RakNet::RakString interests;
	RakNet::RakString favoriteGames;
	RakNet::RakString favoriteMovies;
	RakNet::RakString favoriteBooks;
	RakNet::RakString favoriteQuotations;
	serializedBinaryData->Read(aboutMe);
	serializedBinaryData->Read(activities);
	serializedBinaryData->Read(interests);
	serializedBinaryData->Read(favoriteGames);
	serializedBinaryData->Read(favoriteMovies);
	serializedBinaryData->Read(favoriteBooks);
	serializedBinaryData->Read(favoriteQuotations);
	rargs.Add(aboutMe.C_String());
	rargs.Add(activities.C_String());
	rargs.Add(interests.C_String());
	rargs.Add(favoriteGames.C_String());
	rargs.Add(favoriteMovies.C_String());
	rargs.Add(favoriteBooks.C_String());
	rargs.Add(favoriteQuotations.C_String());
}
void WriteAccountBinaryData(RakNet::BitStream *serializedBinaryData, const FxDelegateArgs& pparams, int &index)
{
	RakNet::RakString aboutMe = pparams[index++].GetString();
	RakNet::RakString activities = pparams[index++].GetString();
	RakNet::RakString interests = pparams[index++].GetString();
	RakNet::RakString favoriteGames = pparams[index++].GetString();
	RakNet::RakString favoriteMovies = pparams[index++].GetString();
	RakNet::RakString favoriteBooks = pparams[index++].GetString();
	RakNet::RakString favoriteQuotations = pparams[index++].GetString();
	serializedBinaryData->Write(aboutMe);
	serializedBinaryData->Write(activities);
	serializedBinaryData->Write(interests);
	serializedBinaryData->Write(favoriteGames);
	serializedBinaryData->Write(favoriteMovies);
	serializedBinaryData->Write(favoriteBooks);
	serializedBinaryData->Write(favoriteQuotations);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_RegisterAccount)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Client_RegisterAccount, m1);
	int index=0;
	m1->createAccountParameters.firstName=pparams[index++].GetString();
	m1->createAccountParameters.middleName=pparams[index++].GetString();
	m1->createAccountParameters.lastName=pparams[index++].GetString();
	m1->createAccountParameters.race=pparams[index++].GetString();
	m1->createAccountParameters.sex_male=pparams[index++].GetBool();
	m1->createAccountParameters.homeAddress1=pparams[index++].GetString();
	m1->createAccountParameters.homeAddress2=pparams[index++].GetString();
	m1->createAccountParameters.homeCity=pparams[index++].GetString();
	m1->createAccountParameters.homeState=pparams[index++].GetString();
	m1->createAccountParameters.homeCountry=pparams[index++].GetString();
	m1->createAccountParameters.homeZipCode=pparams[index++].GetString();
	m1->createAccountParameters.billingAddress1=pparams[index++].GetString();
	m1->createAccountParameters.billingAddress2=pparams[index++].GetString();
	m1->createAccountParameters.billingCity=pparams[index++].GetString();
	m1->createAccountParameters.billingState=pparams[index++].GetString();
	m1->createAccountParameters.billingCountry=pparams[index++].GetString();
	m1->createAccountParameters.billingZipCode=pparams[index++].GetString();
	m1->createAccountParameters.emailAddress=pparams[index++].GetString();
	m1->createAccountParameters.password=pparams[index++].GetString();
	m1->createAccountParameters.passwordRecoveryQuestion=pparams[index++].GetString();
	m1->createAccountParameters.passwordRecoveryAnswer=pparams[index++].GetString();
	m1->createAccountParameters.caption1=pparams[index++].GetString();
	m1->createAccountParameters.caption2=pparams[index++].GetString();
	m1->createAccountParameters.ageInDays=atoi(pparams[index++].GetString());
	m1->titleName="Test Title Name";
	m1->cdKey="Test CD Key";
	m1->userName=pparams[index++].GetString();

	RakNet::BitStream serializedBinaryData;
	WriteAccountBinaryData(&serializedBinaryData, pparams, index);
	m1->createAccountParameters.binaryData = RakNet::OP_NEW<BinaryDataBlock>(_FILE_AND_LINE_);
	m1->createAccountParameters.binaryData->binaryData=(char*) serializedBinaryData.GetData();
	m1->createAccountParameters.binaryData->binaryDataLength=serializedBinaryData.GetNumberOfBytesUsed();
	lobby2Client->SendMsg(m1);
	m1->createAccountParameters.binaryData->binaryData=0; // So it doesn't get deallocated
	messageFactory->Dealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_ValidateHandle)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Client_ValidateHandle, m1);
	m1->userName=pparams[0].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_ChangeHandle)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Client_ChangeHandle, m1);
	m1->userName=pparams[0].GetString();
	m1->password=pparams[1].GetString();
	m1->newHandle=pparams[2].GetString();
	m1->requiresPasswordToChangeHandle=true;
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Logoff)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Client_Logoff, m1);
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_UpdateAccount)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Client_UpdateAccount, m1);
	int index=0;
	m1->createAccountParameters.firstName=pparams[index++].GetString();
	m1->createAccountParameters.middleName=pparams[index++].GetString();
	m1->createAccountParameters.lastName=pparams[index++].GetString();
	m1->createAccountParameters.race=pparams[index++].GetString();
	m1->createAccountParameters.sex_male=pparams[index++].GetBool();
	m1->createAccountParameters.homeAddress1=pparams[index++].GetString();
	m1->createAccountParameters.homeAddress2=pparams[index++].GetString();
	m1->createAccountParameters.homeCity=pparams[index++].GetString();
	m1->createAccountParameters.homeState=pparams[index++].GetString();
	m1->createAccountParameters.homeCountry=pparams[index++].GetString();
	m1->createAccountParameters.homeZipCode=pparams[index++].GetString();
	m1->createAccountParameters.billingAddress1=pparams[index++].GetString();
	m1->createAccountParameters.billingAddress2=pparams[index++].GetString();
	m1->createAccountParameters.billingCity=pparams[index++].GetString();
	m1->createAccountParameters.billingState=pparams[index++].GetString();
	m1->createAccountParameters.billingCountry=pparams[index++].GetString();
	m1->createAccountParameters.billingZipCode=pparams[index++].GetString();
	m1->createAccountParameters.emailAddress=pparams[index++].GetString();
	m1->createAccountParameters.password=pparams[index++].GetString();
	m1->createAccountParameters.passwordRecoveryQuestion=pparams[index++].GetString();
	m1->createAccountParameters.passwordRecoveryAnswer=pparams[index++].GetString();
	m1->createAccountParameters.caption1=pparams[index++].GetString();
	m1->createAccountParameters.caption2=pparams[index++].GetString();
	m1->createAccountParameters.ageInDays=atoi(pparams[index++].GetString());

	RakNet::BitStream serializedBinaryData;
	WriteAccountBinaryData(&serializedBinaryData, pparams,index);
	m1->createAccountParameters.binaryData->binaryData=(char*) serializedBinaryData.GetData();
	m1->createAccountParameters.binaryData->binaryDataLength=serializedBinaryData.GetNumberOfBytesUsed();
	lobby2Client->SendMsg(m1);
	m1->createAccountParameters.binaryData->binaryData=0;
	messageFactory->Dealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_StartIgnore)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Client_StartIgnore, m1);
	m1->targetHandle=pparams[0].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_StopIgnore)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Client_StopIgnore, m1);
	m1->targetHandle=pparams[0].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_GetIgnoreList)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Client_GetIgnoreList, m1);
	lobby2Client->SendMsgAndDealloc(m1);
}

ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_SendInvite)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Friends_SendInvite, m1);
	m1->targetHandle=pparams[0].GetString();
	m1->subject=pparams[1].GetString();
	m1->body=pparams[2].GetString();
	m1->emailStatus=atoi(pparams[3].GetString());
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_AcceptInvite)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Friends_AcceptInvite, m1);
	m1->targetHandle=pparams[0].GetString();
	m1->subject=pparams[1].GetString();
	m1->body=pparams[2].GetString();
	m1->emailStatus=atoi(pparams[3].GetString());
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_RejectInvite)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Friends_RejectInvite, m1);
	m1->targetHandle=pparams[0].GetString();
	m1->subject=pparams[1].GetString();
	m1->body=pparams[2].GetString();
	m1->emailStatus=atoi(pparams[3].GetString());
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_RemoveFriend)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Friends_Remove, m1);
	m1->targetHandle=pparams[0].GetString();
	m1->subject=pparams[1].GetString();
	m1->body=pparams[2].GetString();
	m1->emailStatus=atoi(pparams[3].GetString());
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_GetFriendInvites)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Friends_GetInvites, m1);
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_GetFriends)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Friends_GetFriends, m1);
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_SendEmail)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Emails_Send, m1);
	unsigned int index;
	for (index=0; index < 8; index++)
	{
		if (pparams[index].GetString() && pparams[index].GetString()[0])
			m1->recipients.Push(RakNet::RakString(pparams[index].GetString()), _FILE_AND_LINE_);
	}
	m1->subject=pparams[index++].GetString();
	m1->body=pparams[index++].GetString();
	m1->status=atoi(pparams[index++].GetString());
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_DeleteEmail)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Emails_Delete, m1);
	m1->emailId=atoi(pparams[0].GetString());
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_UpdateEmail)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Emails_SetStatus, m1);
	m1->emailId=atoi(pparams[0].GetString());
	m1->updateStatusFlag=pparams[1].GetBool();
	m1->updateMarkedRead=pparams[2].GetBool();
	m1->newStatusFlag=atoi(pparams[3].GetString());
	m1->isNowMarkedRead=pparams[4].GetBool();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_GetEmails)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Emails_Get, m1);
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_Create)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_Create, m1);
	m1->clanHandle=pparams[0].GetString();
	m1->failIfAlreadyInClan=pparams[1].GetBool();
	m1->requiresInvitationsToJoin=pparams[2].GetBool();
	m1->description=pparams[3].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_SetProperties)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_SetProperties, m1);
	m1->clanHandle=pparams[0].GetString();
	m1->description=pparams[1].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_GetProperties)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_GetProperties, m1);
	m1->clanHandle=pparams[0].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_SetMyMemberProperties)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_SetMyMemberProperties, m1);
	m1->clanHandle=pparams[0].GetString();
	m1->description=pparams[1].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_GrantLeader)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_GrantLeader, m1);
	m1->clanHandle=pparams[0].GetString();
	m1->targetHandle=pparams[1].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_SetSubleaderStatus)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_SetSubleaderStatus, m1);
	m1->clanHandle=pparams[0].GetString();
	m1->targetHandle=pparams[1].GetString();
	m1->setToSubleader=pparams[2].GetBool();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_SetMemberRank)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_SetMemberRank, m1);
	m1->clanHandle=pparams[0].GetString();
	m1->targetHandle=pparams[1].GetString();
	m1->newRank=atoi(pparams[2].GetString());
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_GetMemberProperties)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_GetMemberProperties, m1);
	m1->clanHandle=pparams[0].GetString();
	m1->targetHandle=pparams[1].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_ChangeHandle)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_ChangeHandle, m1);
	m1->oldClanHandle=pparams[0].GetString();
	m1->newClanHandle=pparams[1].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_Leave)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_Leave, m1);
	m1->clanHandle=pparams[0].GetString();
	m1->subject=pparams[1].GetString();
	m1->body=pparams[2].GetString();
	m1->emailStatus=atoi(pparams[3].GetString());
	m1->dissolveIfClanLeader=pparams[4].GetBool();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_Get)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_Get, m1);
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_SendJoinInvitation)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_SendJoinInvitation, m1);
	m1->clanHandle=pparams[0].GetString();
	m1->subject=pparams[1].GetString();
	m1->body=pparams[2].GetString();
	m1->emailStatus=atoi(pparams[3].GetString());
	m1->targetHandle=pparams[4].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_WithdrawJoinInvitation)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_WithdrawJoinInvitation, m1);
	m1->clanHandle=pparams[0].GetString();
	m1->subject=pparams[1].GetString();
	m1->body=pparams[2].GetString();
	m1->emailStatus=atoi(pparams[3].GetString());
	m1->targetHandle=pparams[4].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_AcceptJoinInvitation)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_AcceptJoinInvitation, m1);
	m1->clanHandle=pparams[0].GetString();
	m1->subject=pparams[1].GetString();
	m1->body=pparams[2].GetString();
	m1->emailStatus=atoi(pparams[3].GetString());
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_RejectJoinInvitation)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_RejectJoinInvitation, m1);
	m1->clanHandle=pparams[0].GetString();
	m1->subject=pparams[1].GetString();
	m1->body=pparams[2].GetString();
	m1->emailStatus=atoi(pparams[3].GetString());
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_DownloadInvitationList)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_DownloadInvitationList, m1);
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_SendJoinRequest)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_SendJoinRequest, m1);
	m1->clanHandle=pparams[0].GetString();
	m1->subject=pparams[1].GetString();
	m1->body=pparams[2].GetString();
	m1->emailStatus=atoi(pparams[3].GetString());
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_WithdrawJoinRequest)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_WithdrawJoinRequest, m1);
	m1->clanHandle=pparams[0].GetString();
	m1->subject=pparams[1].GetString();
	m1->body=pparams[2].GetString();
	m1->emailStatus=atoi(pparams[3].GetString());
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_AcceptJoinRequest)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_AcceptJoinRequest, m1);
	m1->clanHandle=pparams[0].GetString();
	m1->subject=pparams[1].GetString();
	m1->body=pparams[2].GetString();
	m1->emailStatus=atoi(pparams[3].GetString());
	m1->requestingUserHandle=pparams[4].GetString();
	m1->failIfAlreadyInClan=pparams[5].GetBool();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_RejectJoinRequest)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_RejectJoinRequest, m1);
	m1->clanHandle=pparams[0].GetString();
	m1->subject=pparams[1].GetString();
	m1->body=pparams[2].GetString();
	m1->emailStatus=atoi(pparams[3].GetString());
	m1->requestingUserHandle=pparams[4].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_DownloadRequestList)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_DownloadRequestList, m1);
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_KickAndBlacklistUser)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_KickAndBlacklistUser, m1);
	m1->clanHandle=pparams[0].GetString();
	m1->subject=pparams[1].GetString();
	m1->body=pparams[2].GetString();
	m1->emailStatus=atoi(pparams[3].GetString());
	m1->targetHandle=pparams[4].GetString();
	m1->kick=pparams[5].GetBool();
	m1->blacklist=pparams[6].GetBool();
	m1->reason=pparams[7].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_UnblacklistUser)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_UnblacklistUser, m1);
	m1->clanHandle=pparams[0].GetString();
	m1->subject=pparams[1].GetString();
	m1->body=pparams[2].GetString();
	m1->emailStatus=atoi(pparams[3].GetString());
	m1->targetHandle=pparams[4].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_GetBlacklist)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_GetBlacklist, m1);
	m1->clanHandle=pparams[0].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_GetMembers)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_GetMembers, m1);
	m1->clanHandle=pparams[0].GetString();
	lobby2Client->SendMsgAndDealloc(m1);
}
ACTIONSCRIPT_CALLABLE_FUNCTION(Lobby2ClientGFx3Impl, f2c_Clans_GetList)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, Clans_GetList, m1);
	lobby2Client->SendMsgAndDealloc(m1);
}
void Lobby2ClientGFx3Impl::MessageResult(Client_ValidateHandle *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_ValidateHandleResult", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Client_RegisterAccount *message)
{
	FxResponseArgs<3> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	rargs.Add(message->userName.C_String());
	rargs.Add(message->createAccountParameters.password.C_String());
	FxDelegate::Invoke2(movie, "c2f_RegisterAccountResult", rargs);

	// TODO - once the GUI is implemented, comment this out to make sure the implemtor handled not having a valid email address
	__L2_ALLOCATE_AND_DEFINE(messageFactory, System_SetEmailAddressValidated, m1);
	m1->validated=true;
	m1->userName=message->userName;
	lobby2Client->SendMsgAndDealloc(m1);
}
void Lobby2ClientGFx3Impl::MessageResult(Client_UpdateAccount *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_UpdateAccountResult", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(CDKey_GetStatus *message)
{
	FxResponseArgs<5> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	rargs.Add(message->usedBy.C_String());
	rargs.Add(message->activationDate.C_String());
	rargs.Add(message->wasStolen);
	rargs.Add(message->usable);
	FxDelegate::Invoke2(movie, "c2f_CheckCDKeyResult", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(System_DeleteAccount *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_DeleteAccountResult", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Client_Login *message)
{
	if (message->resultCode==L2RC_SUCCESS)
	{
		FxResponseArgs<0> rargs;
		FxDelegate::Invoke2(movie, "c2f_NotifyLoginResultSuccess", rargs);
	}
	else
	{
		FxResponseArgs<4> rargs;
		switch(message->resultCode)
		{
		case L2RC_Client_Login_BANNED:
			rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
			rargs.Add(message->bannedReason.C_String());
			rargs.Add(message->whenBanned.C_String());
			rargs.Add(message->bannedExpiration.C_String());
			break;
		default:
			rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
			rargs.Add(""); rargs.Add(""); rargs.Add("");
			break;
		}

		FxDelegate::Invoke2(movie, "c2f_NotifyLoginResultFailure", rargs);
	}

	
}
void Lobby2ClientGFx3Impl::MessageResult(System_CreateDatabase *message)
{
	// After the database is created (immediately above), create the first title and cd key
	__L2_ALLOCATE_AND_DEFINE(messageFactory, System_CreateTitle, m2);
	m2->requiredAge=0;
	m2->titleName="Test Title Name";
	m2->titleSecretKey="Test secret key";
	lobby2Client->SendMsgAndDealloc(m2);
}
void Lobby2ClientGFx3Impl::MessageResult(System_CreateTitle *message)
{
	__L2_ALLOCATE_AND_DEFINE(messageFactory, CDKey_Add, m3);
	m3->cdKeys.Insert("Test CD Key", _FILE_AND_LINE_);
	m3->titleName="Test Title Name";
	lobby2Client->SendMsgAndDealloc(m3);
}
void Lobby2ClientGFx3Impl::MessageResult(Client_ChangeHandle *message)
{
	FxResponseArgs<3> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	rargs.Add(message->userName.C_String());
	rargs.Add(message->newHandle.C_String());
	FxDelegate::Invoke2(movie, "c2f_ChangeHandleResult", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Client_GetAccountDetails *message)
{
	FxResponseArgsList rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	rargs.Add(message->createAccountParameters.firstName.C_String());
	rargs.Add(message->createAccountParameters.middleName.C_String());
	rargs.Add(message->createAccountParameters.lastName.C_String());
	rargs.Add(message->createAccountParameters.race.C_String());
	rargs.Add(message->createAccountParameters.sex_male);
	rargs.Add(message->createAccountParameters.homeAddress1.C_String());
	rargs.Add(message->createAccountParameters.homeAddress2.C_String());
	rargs.Add(message->createAccountParameters.homeCity.C_String());
	rargs.Add(message->createAccountParameters.homeState.C_String());
	rargs.Add(message->createAccountParameters.homeCountry.C_String());
	rargs.Add(message->createAccountParameters.homeZipCode.C_String());
	rargs.Add(message->createAccountParameters.billingAddress1.C_String());
	rargs.Add(message->createAccountParameters.billingAddress2.C_String());
	rargs.Add(message->createAccountParameters.billingCity.C_String());
	rargs.Add(message->createAccountParameters.billingState.C_String());
	rargs.Add(message->createAccountParameters.billingCountry.C_String());
	rargs.Add(message->createAccountParameters.billingZipCode.C_String());
	rargs.Add(message->createAccountParameters.emailAddress.C_String());
	rargs.Add(message->createAccountParameters.password.C_String());
	rargs.Add(message->createAccountParameters.passwordRecoveryQuestion.C_String());
	rargs.Add(message->createAccountParameters.passwordRecoveryAnswer.C_String());
	rargs.Add(message->createAccountParameters.caption1.C_String());
	rargs.Add(message->createAccountParameters.caption2.C_String());
	rargs.Add((Double)message->createAccountParameters.ageInDays);

	RakNet::BitStream serializedBinaryData((unsigned char*) message->createAccountParameters.binaryData->binaryData, message->createAccountParameters.binaryData->binaryDataLength,false);
	ReadAccountBinaryData(rargs,&serializedBinaryData);
	FxDelegate::Invoke2(movie, "c2f_GetAccountDetailsResult", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(RakNet::Client_StartIgnore *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_StartIgnore", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(RakNet::Client_StopIgnore *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_StopIgnore", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(RakNet::Client_GetIgnoreList *message)
{
	FxResponseArgsList rargs;
	for (unsigned int i=0; i < message->ignoredHandles.Size(); i++)
		rargs.Add(message->ignoredHandles[i].C_String());
	FxDelegate::Invoke2(movie, "c2f_GetIgnoreListResult", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(RakNet::Client_GetPasswordRecoveryQuestionByHandle *message)
{
	// TODO - email them
	FxResponseArgs<4> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	rargs.Add(message->userName.C_String());
	rargs.Add(message->emailAddress.C_String());
	rargs.Add(message->passwordRecoveryQuestion.C_String());
	FxDelegate::Invoke2(movie, "c2f_RecoverPasswordByUsername", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(RakNet::Client_GetPasswordByPasswordRecoveryAnswer *message)
{
	FxResponseArgs<4> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	rargs.Add(message->userName.C_String());
	rargs.Add(message->passwordRecoveryAnswer.C_String());
	rargs.Add(message->password.C_String());
	FxDelegate::Invoke2(movie, "c2f_GetPasswordByPasswordRecoveryAnswer", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Friends_SendInvite *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_SendInviteResult", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Friends_AcceptInvite *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_AcceptInviteResult", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Friends_RejectInvite *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_RejectInviteResult", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Friends_Remove *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_RemoveFriendResult", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Friends_GetInvites *message)
{
	FxResponseArgsList rargs;
	rargs.Add((Double)message->invitesSent.Size());
	rargs.Add((Double)message->invitesReceived.Size());
	for (unsigned int i=0; i < message->invitesSent.Size(); i++)
		rargs.Add(message->invitesSent[i].usernameAndStatus.handle.C_String());
	for (unsigned int i=0; i < message->invitesReceived.Size(); i++)
		rargs.Add(message->invitesReceived[i].usernameAndStatus.handle.C_String());
	FxDelegate::Invoke2(movie, "c2f_GetFriendInvites", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Friends_GetFriends *message)
{
	FxResponseArgsList rargs;
	for (unsigned int i=0; i < message->myFriends.Size(); i++)
		rargs.Add(message->myFriends[i].usernameAndStatus.handle.C_String());
	FxDelegate::Invoke2(movie, "c2f_GetFriends", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Emails_Send *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_SendEmail", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Emails_Delete *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_DeleteEmail", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Emails_SetStatus *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_UpdateEmail", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Emails_Get *message)
{
	FxResponseArgsList rargs;
	for (unsigned int i=0; i < message->emailResults.Size(); i++)
	{
		rargs.Add(message->emailResults[i].sender.C_String());
		rargs.Add(message->emailResults[i].recipient.C_String());
		rargs.Add(message->emailResults[i].subject.C_String());
		rargs.Add(message->emailResults[i].body.C_String());
		rargs.Add((Double) message->emailResults[i].status);
		rargs.Add(message->emailResults[i].wasSendByMe);
		rargs.Add(message->emailResults[i].wasReadByMe);
		rargs.Add((Double) message->emailResults[i].emailID);
		rargs.Add(message->emailResults[i].creationDate.C_String());
	}

	FxDelegate::Invoke2(movie, "c2f_GetEmails", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Clans_Create *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Clans_Create", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_SetProperties *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Clans_SetProperties", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_GetProperties *message)
{
	FxResponseArgs<2> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	rargs.Add(message->description.C_String());
	FxDelegate::Invoke2(movie, "c2f_Clans_GetProperties", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_SetMyMemberProperties *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Clans_SetMyMemberProperties", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_GrantLeader *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Clans_GrantLeader", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_SetSubleaderStatus *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Clans_SetSubleaderStatus", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_SetMemberRank *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Clans_SetMemberRank", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_GetMemberProperties *message)
{
	FxResponseArgs<6> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	rargs.Add(message->description.C_String());
	rargs.Add((Double)message->rank);
	rargs.Add(message->isSubleader);
	switch (message->clanMemberState)
	{
	case CMD_UNDEFINED:
		rargs.Add("CMD_UNDEFINED");
		break;
	case CMD_ACTIVE:
		rargs.Add("CMD_ACTIVE");
		break;
	case CMD_BANNED:
		rargs.Add("CMD_BANNED");
		break;
	case CMD_JOIN_INVITED:
		rargs.Add("CMD_JOIN_INVITED");
		break;
	case CMD_JOIN_REQUESTED:
		rargs.Add("CMD_JOIN_REQUESTED");
		break;
	}
	rargs.Add(message->banReason.C_String());

	FxDelegate::Invoke2(movie, "c2f_Clans_GetMemberProperties", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_ChangeHandle *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Clans_ChangeHandle", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_Leave *message)
{
	FxResponseArgs<3> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	rargs.Add(message->wasDissolved);
	rargs.Add(message->newClanLeader.C_String());
	FxDelegate::Invoke2(movie, "c2f_Clans_Leave", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_Get *message)
{
	FxResponseArgsList rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	rargs.Add((Double)message->clans.Size());
	for (unsigned int i=0; i < message->clans.Size(); i++)
	{
		rargs.Add(message->clans[i].clanName.C_String());
		rargs.Add(message->clans[i].description.C_String());
		rargs.Add(message->clans[i].clanLeader.C_String());
		rargs.Add((Double)message->clans[i].clanMembersOtherThanLeader.Size());
		for (unsigned int j=0; j < message->clans[i].clanMembersOtherThanLeader.Size(); j++)
			rargs.Add(message->clans[i].clanMembersOtherThanLeader[j].C_String());
	}
	FxDelegate::Invoke2(movie, "c2f_Clans_Get", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_SendJoinInvitation *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Clans_SendJoinInvitation", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_WithdrawJoinInvitation *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Clans_WithdrawJoinInvitation", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_AcceptJoinInvitation *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Clans_AcceptJoinInvitation", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_RejectJoinInvitation *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Clans_RejectJoinInvitation", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_DownloadInvitationList *message)
{
	FxResponseArgsList rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	rargs.Add((Double) message->invitationsSentToMe.Size());
	for (unsigned int i=0; i < message->invitationsSentToMe.Size(); i++)
	{
		rargs.Add(message->invitationsSentToMe[i].clanHandle.C_String());
	}
	
	rargs.Add((Double) message->usersThatHaveAnInvitationFromClansThatIAmAMemberOf.Size());
	for (unsigned int i=0; i < message->usersThatHaveAnInvitationFromClansThatIAmAMemberOf.Size(); i++)
	{
		rargs.Add(message->usersThatHaveAnInvitationFromClansThatIAmAMemberOf[i].sourceClan.C_String());
		rargs.Add(message->usersThatHaveAnInvitationFromClansThatIAmAMemberOf[i].dateSent.C_String());
		rargs.Add(message->usersThatHaveAnInvitationFromClansThatIAmAMemberOf[i].joinRequestTarget.C_String());
	}

	FxDelegate::Invoke2(movie, "c2f_Clans_DownloadInvitationList", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_SendJoinRequest *message)
{
	FxResponseArgs<2> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	rargs.Add(message->clanJoined);
	FxDelegate::Invoke2(movie, "c2f_Clans_SendJoinRequest", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_WithdrawJoinRequest *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Clans_WithdrawJoinRequest", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_AcceptJoinRequest *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Clans_AcceptJoinRequest", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_RejectJoinRequest *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Clans_RejectJoinRequest", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_DownloadRequestList *message)
{
	FxResponseArgsList rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	rargs.Add((Double)message->joinRequestsToMyClan.Size());
	rargs.Add((Double)message->joinRequestsFromMe.Size());
	for (unsigned int i=0; i < message->joinRequestsToMyClan.Size(); i++)
	{
		rargs.Add(message->joinRequestsToMyClan[i].targetClan.C_String());
		rargs.Add(message->joinRequestsToMyClan[i].dateSent.C_String());
		rargs.Add(message->joinRequestsToMyClan[i].joinRequestSender.C_String());
	}
	for (unsigned int i=0; i < message->joinRequestsFromMe.Size(); i++)
	{
		rargs.Add(message->joinRequestsFromMe[i].targetClan.C_String());
		rargs.Add(message->joinRequestsFromMe[i].dateSent.C_String());
		rargs.Add(message->joinRequestsFromMe[i].joinRequestSender.C_String());
	}

	FxDelegate::Invoke2(movie, "c2f_Clans_DownloadRequestList", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_KickAndBlacklistUser *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Clans_KickAndBlacklistUser", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_UnblacklistUser *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	FxDelegate::Invoke2(movie, "c2f_Clans_UnblacklistUser", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_GetBlacklist *message)
{
	FxResponseArgsList rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	for (unsigned int i=0; i < message->blacklistedUsers.Size(); i++)
		rargs.Add(message->blacklistedUsers[i].C_String());
	FxDelegate::Invoke2(movie, "c2f_Clans_GetBlacklist", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_GetMembers *message)
{
	FxResponseArgsList rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	rargs.Add(message->clanLeader.C_String());
	for (unsigned int i=0; i < message->clanMembersOtherThanLeader.Size(); i++)
		rargs.Add(message->clanMembersOtherThanLeader[i].C_String());
	FxDelegate::Invoke2(movie, "c2f_Clans_GetMembers", rargs);
}

void Lobby2ClientGFx3Impl::MessageResult(Clans_GetList *message)
{
	FxResponseArgsList rargs;
	rargs.Add(Lobby2ResultCodeDescription::ToEnglish(message->resultCode));
	for (unsigned int i=0; i < message->clanNames.Size(); i++)
		rargs.Add(message->clanNames[i].C_String());
	FxDelegate::Invoke2(movie, "c2f_Clans_GetList", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Notification_Client_RemoteLogin *message)
{
	FxResponseArgs<1> rargs;
	rargs.Add(message->handle.C_String());
	FxDelegate::Invoke2(movie, "c2f_Notification_Client_RemoteLogin", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Notification_Client_IgnoreStatus *message)
{
	FxResponseArgs<2> rargs;
	rargs.Add(message->nowIgnored);
	rargs.Add(message->otherHandle.C_String());
	FxDelegate::Invoke2(movie, "c2f_Notification_Client_IgnoreStatus", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Notification_Friends_StatusChange *message)
{
	FxResponseArgs<2> rargs;
	switch (message->op)
	{
	case Notification_Friends_StatusChange::FRIEND_LOGGED_IN:
		rargs.Add("FRIEND_LOGGED_IN");
		break;
	case Notification_Friends_StatusChange::FRIEND_LOGGED_IN_DIFFERENT_CONTEXT:
		rargs.Add("FRIEND_LOGGED_IN_DIFFERENT_CONTEXT");
		break;
	case Notification_Friends_StatusChange::FRIEND_LOGGED_OFF:
		rargs.Add("FRIEND_LOGGED_OFF");
		break;
	case Notification_Friends_StatusChange::FRIEND_ACCOUNT_WAS_DELETED:
		rargs.Add("FRIEND_ACCOUNT_WAS_DELETED");
		break;
	case Notification_Friends_StatusChange::YOU_WERE_REMOVED_AS_A_FRIEND:
		rargs.Add("YOU_WERE_REMOVED_AS_A_FRIEND");
		break;
	case Notification_Friends_StatusChange::GOT_INVITATION_TO_BE_FRIENDS:
		rargs.Add("GOT_INVITATION_TO_BE_FRIENDS");
		break;
	case Notification_Friends_StatusChange::THEY_ACCEPTED_OUR_INVITATION_TO_BE_FRIENDS:
		rargs.Add("THEY_ACCEPTED_OUR_INVITATION_TO_BE_FRIENDS");
		break;
	case Notification_Friends_StatusChange::THEY_REJECTED_OUR_INVITATION_TO_BE_FRIENDS:
		rargs.Add("THEY_REJECTED_OUR_INVITATION_TO_BE_FRIENDS");
		break;
	}
	rargs.Add(message->otherHandle.C_String());
	FxDelegate::Invoke2(movie, "c2f_Notification_Friends_StatusChange", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Notification_User_ChangedHandle *message)
{
	FxResponseArgs<2> rargs;
	rargs.Add(message->oldHandle.C_String());
	rargs.Add(message->newHandle.C_String());
	FxDelegate::Invoke2(movie, "c2f_Notification_User_ChangedHandle", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Notification_Friends_CreatedClan *message)
{
	FxResponseArgs<2> rargs;
	rargs.Add(message->otherHandle.C_String());
	rargs.Add(message->clanName.C_String());
	FxDelegate::Invoke2(movie, "c2f_Notification_Friends_CreatedClan", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Notification_Emails_Received *message)
{
	FxResponseArgs<3> rargs;
	rargs.Add(message->sender.C_String());
	rargs.Add(message->subject.C_String());
	rargs.Add((Double)message->emailId);
	FxDelegate::Invoke2(movie, "c2f_Notification_Emails_Received", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Notification_Clans_GrantLeader *message)
{
	FxResponseArgs<3> rargs;
	rargs.Add(message->clanHandle.C_String());
	rargs.Add(message->newLeader.C_String());
	rargs.Add(message->oldLeader.C_String());
	FxDelegate::Invoke2(movie, "c2f_Notification_Clans_GrantLeader", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Notification_Clans_SetSubleaderStatus *message)
{
	FxResponseArgs<4> rargs;
	rargs.Add(message->clanHandle.C_String());
	rargs.Add(message->targetHandle.C_String());
	rargs.Add(message->leaderHandle.C_String());
	rargs.Add(message->setToSubleader);
	FxDelegate::Invoke2(movie, "c2f_Notification_Clans_SetSubleaderStatus", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Notification_Clans_SetMemberRank *message)
{
	FxResponseArgs<4> rargs;
	rargs.Add(message->clanHandle.C_String());
	rargs.Add(message->targetHandle.C_String());
	rargs.Add(message->leaderHandle.C_String());
	rargs.Add((Double)message->newRank);
	FxDelegate::Invoke2(movie, "c2f_Notification_Clans_SetMemberRank", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Notification_Clans_ChangeHandle *message)
{
	FxResponseArgs<3> rargs;
	rargs.Add(message->oldClanHandle.C_String());
	rargs.Add(message->newClanHandle.C_String());
	rargs.Add(message->leaderHandle.C_String());
	FxDelegate::Invoke2(movie, "c2f_Notification_Clans_ChangeHandle", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Notification_Clans_Leave *message)
{
	FxResponseArgs<2> rargs;
	rargs.Add(message->clanHandle.C_String());
	rargs.Add(message->targetHandle.C_String());
	FxDelegate::Invoke2(movie, "c2f_Notification_Clans_Leave", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Notification_Clans_PendingJoinStatus *message)
{
	FxResponseArgs<6> rargs;
	rargs.Add(message->clanHandle.C_String());
	rargs.Add(message->sourceHandle.C_String());
	rargs.Add(message->targetHandle.C_String());
	rargs.Add(message->clanMemberHandle.C_String());

	switch (message->majorOp)
	{
	case Notification_Clans_PendingJoinStatus::JOIN_CLAN_INVITATION:
		rargs.Add("JOIN_CLAN_INVITATION");
		break;
	case Notification_Clans_PendingJoinStatus::JOIN_CLAN_REQUEST:
		rargs.Add("JOIN_CLAN_REQUEST");
		break;
	}
	switch (message->minorOp)
	{
	case Notification_Clans_PendingJoinStatus::JOIN_SENT:
		rargs.Add("JOIN_SENT");
		break;
	case Notification_Clans_PendingJoinStatus::JOIN_WITHDRAWN:
		rargs.Add("JOIN_WITHDRAWN");
		break;
	case Notification_Clans_PendingJoinStatus::JOIN_REJECTED:
		rargs.Add("JOIN_REJECTED");
		break;
	}

	FxDelegate::Invoke2(movie, "c2f_Notification_Clans_PendingJoinStatus", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Notification_Clans_NewClanMember *message)
{
	FxResponseArgs<2> rargs;
	rargs.Add(message->clanHandle.C_String());
	rargs.Add(message->targetHandle.C_String());
	FxDelegate::Invoke2(movie, "c2f_Notification_Clans_NewClanMember", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Notification_Clans_KickAndBlacklistUser *message)
{
	FxResponseArgs<5> rargs;
	rargs.Add(message->clanHandle.C_String());
	rargs.Add(message->targetHandle.C_String());
	rargs.Add(message->blacklistingUserHandle.C_String());
	rargs.Add(message->targetHandleWasKicked);
	rargs.Add(message->reason.C_String());
	FxDelegate::Invoke2(movie, "c2f_Notification_Clans_KickAndBlacklistUser", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Notification_Clans_UnblacklistUser *message)
{
	FxResponseArgs<3> rargs;
	rargs.Add(message->clanHandle.C_String());
	rargs.Add(message->targetHandle.C_String());
	rargs.Add(message->unblacklistingUserHandle.C_String());
	FxDelegate::Invoke2(movie, "c2f_Notification_Clans_UnblacklistUser", rargs);
}
void Lobby2ClientGFx3Impl::MessageResult(Notification_Clans_Destroyed *message)
{
	FxResponseArgs<2> rargs;
	rargs.Add(message->clanHandle.C_String());
	rargs.Add(message->oldClanLeader.C_String());
	FxDelegate::Invoke2(movie, "c2f_Notification_Clans_Destroyed", rargs);
}
void Lobby2ClientGFx3Impl::Accept(CallbackProcessor* cbreg)
{
	cbreg->Process( "f2c_ResetDatabase", &::f2c_ResetDatabase );
	cbreg->Process( "f2c_Connect", &::f2c_Connect );
	cbreg->Process( "f2c_RecoverPasswordByUsername", &::f2c_RecoverPasswordByUsername );
	cbreg->Process( "f2c_GetPasswordByPasswordRecoveryAnswer", &::f2c_GetPasswordByPasswordRecoveryAnswer );
	cbreg->Process( "f2c_RegisterAccountStateQuery", &::f2c_RegisterAccountStateQuery );
	cbreg->Process( "f2c_DeleteAccount", &::f2c_DeleteAccount );
	cbreg->Process( "f2c_LoginToAccount", &::f2c_LoginToAccount );
	cbreg->Process( "f2c_GetAccountDetails", &::f2c_GetAccountDetails );
	cbreg->Process( "f2c_DisconnectFromServer", &::f2c_DisconnectFromServer );
	cbreg->Process( "f2c_CheckCDKey", &::f2c_CheckCDKey );
	cbreg->Process( "f2c_RegisterAccount", &::f2c_RegisterAccount );
	cbreg->Process( "f2c_ValidateHandle", &::f2c_ValidateHandle );
	cbreg->Process( "f2c_ChangeHandle", &::f2c_ChangeHandle );
	cbreg->Process( "f2c_Logoff", &::f2c_Logoff );
	cbreg->Process( "f2c_UpdateAccount", &::f2c_UpdateAccount );
	cbreg->Process( "f2c_StartIgnore", &::f2c_StartIgnore );
	cbreg->Process( "f2c_StopIgnore", &::f2c_StopIgnore );
	cbreg->Process( "f2c_GetIgnoreList", &::f2c_GetIgnoreList );
	cbreg->Process( "f2c_SendInvite", &::f2c_SendInvite );
	cbreg->Process( "f2c_AcceptInvite", &::f2c_AcceptInvite );
	cbreg->Process( "f2c_RejectInvite", &::f2c_RejectInvite );
	cbreg->Process( "f2c_RemoveFriend", &::f2c_RemoveFriend );
	cbreg->Process( "f2c_GetFriendInvites", &::f2c_GetFriendInvites );
	cbreg->Process( "f2c_GetFriends", &::f2c_GetFriends );
	cbreg->Process( "f2c_SendEmail", &::f2c_SendEmail );
	cbreg->Process( "f2c_DeleteEmail", &::f2c_DeleteEmail );
	cbreg->Process( "f2c_UpdateEmail", &::f2c_UpdateEmail );
	cbreg->Process( "f2c_GetEmails", &::f2c_GetEmails );
	cbreg->Process( "f2c_Clans_Create", &::f2c_Clans_Create );
	cbreg->Process( "f2c_Clans_SetProperties", &::f2c_Clans_SetProperties );
	cbreg->Process( "f2c_Clans_GetProperties", &::f2c_Clans_GetProperties );
	cbreg->Process( "f2c_Clans_SetMyMemberProperties", &::f2c_Clans_SetMyMemberProperties );
	cbreg->Process( "f2c_Clans_GrantLeader", &::f2c_Clans_GrantLeader );
	cbreg->Process( "f2c_Clans_SetSubleaderStatus", &::f2c_Clans_SetSubleaderStatus );
	cbreg->Process( "f2c_Clans_SetMemberRank", &::f2c_Clans_SetMemberRank );
	cbreg->Process( "f2c_Clans_GetMemberProperties", &::f2c_Clans_GetMemberProperties );
	cbreg->Process( "f2c_Clans_ChangeHandle", &::f2c_Clans_ChangeHandle );
	cbreg->Process( "f2c_Clans_Leave", &::f2c_Clans_Leave );
	cbreg->Process( "f2c_Clans_Get", &::f2c_Clans_Get );
	cbreg->Process( "f2c_Clans_SendJoinInvitation", &::f2c_Clans_SendJoinInvitation );
	cbreg->Process( "f2c_Clans_WithdrawJoinInvitation", &::f2c_Clans_WithdrawJoinInvitation );
	cbreg->Process( "f2c_Clans_AcceptJoinInvitation", &::f2c_Clans_AcceptJoinInvitation );
	cbreg->Process( "f2c_Clans_RejectJoinInvitation", &::f2c_Clans_RejectJoinInvitation );
	cbreg->Process( "f2c_Clans_DownloadInvitationList", &::f2c_Clans_DownloadInvitationList );
	cbreg->Process( "f2c_Clans_SendJoinRequest", &::f2c_Clans_SendJoinRequest );
	cbreg->Process( "f2c_Clans_WithdrawJoinRequest", &::f2c_Clans_WithdrawJoinRequest );
	cbreg->Process( "f2c_Clans_AcceptJoinRequest", &::f2c_Clans_AcceptJoinRequest );
	cbreg->Process( "f2c_Clans_RejectJoinRequest", &::f2c_Clans_RejectJoinRequest );
	cbreg->Process( "f2c_Clans_DownloadRequestList", &::f2c_Clans_DownloadRequestList );
	cbreg->Process( "f2c_Clans_KickAndBlacklistUser", &::f2c_Clans_KickAndBlacklistUser );
	cbreg->Process( "f2c_Clans_UnblacklistUser", &::f2c_Clans_UnblacklistUser );
	cbreg->Process( "f2c_Clans_GetBlacklist", &::f2c_Clans_GetBlacklist );
	cbreg->Process( "f2c_Clans_GetMembers", &::f2c_Clans_GetMembers );
	cbreg->Process( "f2c_Clans_GetList", &::f2c_Clans_GetList );

	cbreg->Process( "openSite", &Lobby2ClientGFx3Impl::OpenSite );


}
void Lobby2ClientGFx3Impl::OnClosedConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
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
		FxDelegate::Invoke2(movie, "c2f_NotifyConnectionLost", rargs);
	}
}
void Lobby2ClientGFx3Impl::OnNewConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, bool isIncoming)
{
	if (systemAddress==lobby2Client->GetServerAddress())
	{
		// is connected
		FxResponseArgs<0> rargs;
		FxDelegate::Invoke2(movie, "c2f_NotifyConnectionResultSuccess", rargs);
	}
}
void Lobby2ClientGFx3Impl::OnFailedConnectionAttempt(Packet *packet, PI2_FailedConnectionAttemptReason failedConnectionAttemptReason)
{
	if (packet->systemAddress==lobby2Client->GetServerAddress())
	{
		// isn't connected
		FxResponseArgs<1> rargs;
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
		FxDelegate::Invoke2(movie, "c2f_NotifyConnectionResultFailure", rargs);
	}
}

void Lobby2ClientGFx3Impl::OpenSite(const FxDelegateArgs& pparams)
{
	Lobby2ClientGFx3Impl* prt = (Lobby2ClientGFx3Impl*)pparams.GetHandler();
	const char *siteType = pparams[0].GetString();
	ShellExecute(NULL, "open", siteType,NULL, NULL, SW_SHOWNORMAL);
}

