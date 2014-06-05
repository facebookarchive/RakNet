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
#include "RakPeerInterface.h"

#include "MessageIdentifiers.h"
#include "Lobby2Client.h"
#include "Kbhit.h"
#include "RakSleep.h"
#include "RoomsErrorCodes.h"
#include "DS_Queue.h"
#include <ctype.h>
#include <stdlib.h>
#include "LinuxStrings.h"
#include "Gets.h"

static const int NUM_CONNECTIONS=2;
RakNet::Lobby2Client lobby2Client[NUM_CONNECTIONS];
RakNet::Lobby2MessageFactory messageFactory;
RakNet::RakString testUserName[NUM_CONNECTIONS];
RakNet::RakPeerInterface *rakPeer[NUM_CONNECTIONS];
struct AutoExecutionPlanNode
{
	AutoExecutionPlanNode() {}
	AutoExecutionPlanNode(int i, RakNet::Lobby2MessageID o) {instanceNumber=i; operation=o;}
	int instanceNumber;
	RakNet::Lobby2MessageID operation;
};
DataStructures::Queue<AutoExecutionPlanNode> executionPlan;

void PrintCommands(RakNet::Lobby2MessageFactory *messageFactory)
{
	unsigned int i;
	for (i=0; i < RakNet::L2MID_COUNT; i++)
	{
		RakNet::Lobby2Message *m = messageFactory->Alloc((RakNet::Lobby2MessageID)i);
		if (m)
		{
			printf("%i. %s", i+1, m->GetName());
			if (m->RequiresAdmin())
				printf(" (Admin command)");
			if (m->RequiresRankingPermission())
				printf(" (Ranking server command)");
			printf("\n");
			messageFactory->Dealloc(m);
		}		
		
	}
}

void ExecuteCommand(RakNet::Lobby2MessageID command, RakNet::RakString userName, int instanceNumber);
struct Lobby2ClientSampleCB : public RakNet::Lobby2Printf
{
	virtual void ExecuteDefaultResult(RakNet::Lobby2Message *message) {
		message->DebugPrintf();
		if (message->resultCode==RakNet::REC_SUCCESS && executionPlan.Size())
		{
			AutoExecutionPlanNode aepn = executionPlan.Pop();
			ExecuteCommand(aepn.operation, RakNet::RakString("user%i", aepn.instanceNumber), aepn.instanceNumber);
		}
	}
} callback[NUM_CONNECTIONS];

int main()
{
	printf("This sample creates two Lobby2Clients.\n");
	printf("They both connect to the server and performs queued operations on startup.");
	printf("(RANKING AND CLANS NOT YET DONE).\n");
	printf("Difficulty: Advanced\n\n");

	RakNet::Lobby2ResultCodeDescription::Validate();

	/// Do all these operations in this order once we are logged in.
	/// This is for easier testing.
	/// This plan will create the database, register two users, and log them both in
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_System_CreateDatabase), _FILE_AND_LINE_ );
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_System_CreateTitle), _FILE_AND_LINE_ );

	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_CDKey_Add), _FILE_AND_LINE_ );
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_System_RegisterProfanity), _FILE_AND_LINE_ );
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Client_RegisterAccount), _FILE_AND_LINE_ );
	executionPlan.Push(AutoExecutionPlanNode(1, RakNet::L2MID_Client_RegisterAccount), _FILE_AND_LINE_ );
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_System_SetEmailAddressValidated), _FILE_AND_LINE_ );
	executionPlan.Push(AutoExecutionPlanNode(1, RakNet::L2MID_System_SetEmailAddressValidated), _FILE_AND_LINE_ );
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Client_Login), _FILE_AND_LINE_ );
	executionPlan.Push(AutoExecutionPlanNode(1, RakNet::L2MID_Client_Login), _FILE_AND_LINE_ );
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Emails_Send), _FILE_AND_LINE_ );
	executionPlan.Push(AutoExecutionPlanNode(1, RakNet::L2MID_Emails_Get), _FILE_AND_LINE_ );
// 	/// Create 2 clans
// 	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_Create), _FILE_AND_LINE_ );
// 	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_Create), _FILE_AND_LINE_ );
// 	// Invite to both
// 	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_SendJoinInvitation), _FILE_AND_LINE_ );
// 	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_SendJoinInvitation), _FILE_AND_LINE_ );
// 	executionPlan.Push(AutoExecutionPlanNode(1, RakNet::L2MID_Clans_RejectJoinInvitation), _FILE_AND_LINE_ );
// 	// Download invitations this clan has sent
// 	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_DownloadInvitationList), _FILE_AND_LINE_ );

	/*

	executionPlan.Push(AutoExecutionPlanNode(1, RakNet::L2MID_Client_SetPresence), _FILE_AND_LINE_ );
	executionPlan.Push(AutoExecutionPlanNode(1, RakNet::L2MID_Client_GetAccountDetails), _FILE_AND_LINE_ );
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Client_PerTitleIntegerStorage), _FILE_AND_LINE_ );
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Client_PerTitleIntegerStorage), _FILE_AND_LINE_ );

	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Client_StartIgnore), _FILE_AND_LINE_ );
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Client_GetIgnoreList), _FILE_AND_LINE_ );

	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Friends_SendInvite), _FILE_AND_LINE_);
	executionPlan.Push(AutoExecutionPlanNode(1, RakNet::L2MID_Friends_AcceptInvite), _FILE_AND_LINE_);

	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Ranking_SubmitMatch));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Ranking_SubmitMatch));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Ranking_UpdateRating));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Ranking_GetRating));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Ranking_WipeRatings));
	*/
// 	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_Create), _FILE_AND_LINE_ );
// 	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_Get), _FILE_AND_LINE_ );
	/*
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_SetProperties));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_SetMyMemberProperties));
	*/
	/*
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_SendJoinInvitation));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_WithdrawJoinInvitation));
	executionPlan.Push(AutoExecutionPlanNode(1, RakNet::L2MID_Clans_DownloadInvitationList));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_SendJoinInvitation));
	executionPlan.Push(AutoExecutionPlanNode(1, RakNet::L2MID_Clans_RejectJoinInvitation));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_SendJoinInvitation));
	executionPlan.Push(AutoExecutionPlanNode(1, RakNet::L2MID_Clans_AcceptJoinInvitation));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_SetSubleaderStatus));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_SetMemberRank));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_GrantLeader));
	*/

	/*
	executionPlan.Push(AutoExecutionPlanNode(1, RakNet::L2MID_Clans_SendJoinRequest));
	executionPlan.Push(AutoExecutionPlanNode(1, RakNet::L2MID_Clans_WithdrawJoinRequest));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_AcceptJoinRequest));
	*/

//	executionPlan.Push(AutoExecutionPlanNode(1, RakNet::L2MID_Clans_SendJoinRequest));
//	executionPlan.Push(AutoExecutionPlanNode(1, RakNet::L2MID_Clans_DownloadRequestList));
	// TODO - test from here
	/*
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_RejectJoinRequest));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_AcceptJoinRequest));
	executionPlan.Push(AutoExecutionPlanNode(1, RakNet::L2MID_Clans_SendJoinRequest));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_AcceptJoinRequest));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_KickAndBlacklistUser));
	executionPlan.Push(AutoExecutionPlanNode(1, RakNet::L2MID_Clans_SendJoinRequest));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_GetBlacklist));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_UnblacklistUser));
	executionPlan.Push(AutoExecutionPlanNode(1, RakNet::L2MID_Clans_SendJoinRequest));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_AcceptJoinRequest));
	executionPlan.Push(AutoExecutionPlanNode(0, RakNet::L2MID_Clans_GetMembers));
	*/

	/*
	// TODO
	L2MID_Clans_CreateBoard,
		L2MID_Clans_DestroyBoard,
		L2MID_Clans_CreateNewTopic,
		L2MID_Clans_ReplyToTopic,
		L2MID_Clans_RemovePost,
		L2MID_Clans_GetBoards,
		L2MID_Clans_GetTopics,
		L2MID_Clans_GetPosts,
		*/
	

	char ip[64], serverPort[30], clientPort[30];
	int i;
	for (i=0; i < NUM_CONNECTIONS; i++)
		rakPeer[i]=RakNet::RakPeerInterface::GetInstance();
	puts("Enter the rakPeer1 port to listen on");
	clientPort[0]=0;
	RakNet::SocketDescriptor socketDescriptor(atoi(clientPort),0);
	Gets(clientPort,sizeof(clientPort));
	if (clientPort[0]==0)
		strcpy(clientPort, "0");

	puts("Enter IP to connect to");;
	ip[0]=0;
	Gets(ip,sizeof(ip));
	if (ip[0]==0)
		strcpy(ip, "127.0.0.1");

	puts("Enter the port to connect to");
	serverPort[0]=0;
	Gets(serverPort,sizeof(serverPort));
	if (serverPort[0]==0)
		strcpy(serverPort, "61111");

	for (i=0; i < NUM_CONNECTIONS; i++)
	{
		rakPeer[i]->Startup(1,&socketDescriptor, 1);
		rakPeer[i]->Connect(ip, atoi(serverPort), 0,0);

		rakPeer[i]->AttachPlugin(&lobby2Client[i]);
		lobby2Client[i].SetMessageFactory(&messageFactory);
		lobby2Client[i].SetCallbackInterface(&callback[i]);
		testUserName[i]=RakNet::RakString("user%i", i);
	}

	RakNet::Packet *packet;
	// Loop for input
	while (1)
	{
		for (i=0; i < NUM_CONNECTIONS; i++)
		{
			RakNet::RakPeerInterface *peer = rakPeer[i];
			for (packet=peer->Receive(); packet; peer->DeallocatePacket(packet), packet=peer->Receive())
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
					// This tells the rakPeer1 they have connected
					printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
					int j;
					for (j=0; j < NUM_CONNECTIONS; j++)
						lobby2Client[j].SetServerAddress(packet->systemAddress);
					if (i==NUM_CONNECTIONS-1)
					{
						PrintCommands(&messageFactory);
						printf("Enter instance number 1 to %i followed by command number.\n", NUM_CONNECTIONS);

						if (executionPlan.Size())
						{
							/// Execute the first command now that both clients have connected.
							AutoExecutionPlanNode aepn = executionPlan.Pop();
							ExecuteCommand(aepn.operation, RakNet::RakString("user%i", aepn.instanceNumber), aepn.instanceNumber);
						}
					}
					break;
				case ID_LOBBY2_SERVER_ERROR:
					{
						RakNet::BitStream bs(packet->data,packet->length,false);
						bs.IgnoreBytes(2); // ID_LOBBY2_SERVER_ERROR and error code
						printf("ID_LOBBY2_SERVER_ERROR: ");
						if (packet->data[1]==RakNet::L2SE_UNKNOWN_MESSAGE_ID)
						{
							unsigned int messageId;
							bs.Read(messageId);
							printf("L2SE_UNKNOWN_MESSAGE_ID %i", messageId);
						}
						else
							printf("Unknown");
						printf("\n");				
					}

					break;
				}
			}
		}
		
		
		// This sleep keeps RakNet responsive
		RakSleep(30);

		if (kbhit())
		{
			char ch = getch();
			if (ch <= '0' || ch > '9')
			{
				printf("Bad instance number\n");
				continue;
			}
			int instanceNumber = ch - 1 - '0';
			if (instanceNumber >= NUM_CONNECTIONS)
			{
				printf("Enter between 1 and %i to pick the instance of RakPeer to run\n", 1+NUM_CONNECTIONS);
				continue;
			}

			printf("Enter message number or 'quit' to quit.\n");
			char str[128];
			Gets(str, sizeof(str));
			if (_stricmp(str, "quit")==0)
			{
				printf("Quitting.\n");
				break;
			}
			else
			{
				int command = atoi(str);
				if (command <=0 || command > RakNet::L2MID_COUNT)
				{
					printf("Invalid message index %i. Commands:\n", command);
					PrintCommands(&messageFactory);
				}
				else
				{
					ExecuteCommand((RakNet::Lobby2MessageID)(command-1), RakNet::RakString("user%i", instanceNumber), instanceNumber);
				}
			}
		}
	}

	for (i=0; i < NUM_CONNECTIONS; i++)
		RakNet::RakPeerInterface::DestroyInstance(rakPeer[i]);
	return 0;
}
/// In a real application these parameters would be filled out from application data
/// Here I've just hardcoded everything for fast testing
void ExecuteCommand(RakNet::Lobby2MessageID command, RakNet::RakString userName, int instanceNumber)
{
	RakNet::Lobby2Message *m = messageFactory.Alloc(command);
	RakAssert(m);
	printf("Executing %s (message %i)\n", m->GetName(), command+1);
	// If additional requires are needed to test the command, stick them here
	switch (m->GetID())
	{
	case RakNet::L2MID_System_CreateTitle:
		{
			RakNet::System_CreateTitle *arg = (RakNet::System_CreateTitle *) m;
			arg->requiredAge=22;
			arg->titleName="Test Title Name";
			arg->titleSecretKey="Test secret key";
		}
		break;
	case RakNet::L2MID_System_DestroyTitle:
		{
			RakNet::System_DestroyTitle *arg = (RakNet::System_DestroyTitle *) m;
			arg->titleName="Test Title Name";
		}
		break;
	case RakNet::L2MID_System_GetTitleRequiredAge:
		{
			RakNet::System_GetTitleRequiredAge *arg = (RakNet::System_GetTitleRequiredAge *) m;
			arg->titleName="Test Title Name";
		}
		break;
	case RakNet::L2MID_System_GetTitleBinaryData:
		{
			RakNet::System_GetTitleBinaryData *arg = (RakNet::System_GetTitleBinaryData *) m;
			arg->titleName="Test Title Name";
		}
		break;
	case RakNet::L2MID_System_RegisterProfanity:
		{
			RakNet::System_RegisterProfanity *arg = (RakNet::System_RegisterProfanity *) m;
			arg->profanityWords.Insert("Bodily Functions", _FILE_AND_LINE_ );
			arg->profanityWords.Insert("Racial Epithet", _FILE_AND_LINE_ );
			arg->profanityWords.Insert("Euphemism treadmill", _FILE_AND_LINE_ );
		}
		break;
	case RakNet::L2MID_System_BanUser:
		{
			RakNet::System_BanUser *arg = (RakNet::System_BanUser *) m;
			arg->durationHours=12;
			arg->banReason="Ban Reason";
			arg->userName=userName;
		}
		break;
	case RakNet::L2MID_System_UnbanUser:
		{
			RakNet::System_UnbanUser *arg = (RakNet::System_UnbanUser *) m;
			arg->reason="Unban Reason";
			arg->userName=userName;
		}
		break;
	case RakNet::L2MID_CDKey_Add:
		{
			RakNet::CDKey_Add *arg = (RakNet::CDKey_Add *) m;
			arg->cdKeys.Insert("Test CD Key", _FILE_AND_LINE_ );
			arg->cdKeys.Insert("Test CD Key 2", _FILE_AND_LINE_ );
			arg->titleName="Test Title Name";
		}
		break;
	case RakNet::L2MID_CDKey_GetStatus:
		{
			RakNet::CDKey_GetStatus *arg = (RakNet::CDKey_GetStatus *) m;
			arg->cdKey="Test CD Key";
			arg->titleName="Test Title Name";
		}
		break;
	case RakNet::L2MID_CDKey_Use:
		{
			RakNet::CDKey_Use *arg = (RakNet::CDKey_Use *) m;
			arg->cdKey="Test CD Key";
			arg->titleName="Test Title Name";
			arg->userName=userName;
		}
		break;
	case RakNet::L2MID_CDKey_FlagStolen:
		{
			RakNet::CDKey_FlagStolen *arg = (RakNet::CDKey_FlagStolen *) m;
			arg->cdKey="Test CD Key";
			arg->titleName="Test Title Name";
			arg->wasStolen=true;
		}
		break;
	case RakNet::L2MID_Client_Login:
		{
			RakNet::Client_Login *arg = (RakNet::Client_Login *) m;
			arg->titleName="Test Title Name";
			arg->titleSecretKey="Test secret key";
			arg->userPassword="asdf";
			arg->userName=userName;
		}
		break;
	case RakNet::L2MID_Client_SetPresence:
		{
			RakNet::Client_SetPresence *arg = (RakNet::Client_SetPresence *) m;
			arg->presence.isVisible=true;
			arg->presence.status=RakNet::Lobby2Presence::IN_LOBBY;
//			arg->presence.titleName="Test Title Name";
		}
		break;
	case RakNet::L2MID_Client_RegisterAccount:
		{
			RakNet::Client_RegisterAccount *arg = (RakNet::Client_RegisterAccount *) m;
			arg->createAccountParameters.ageInDays=9999;
			arg->createAccountParameters.firstName="Firstname";
			arg->createAccountParameters.lastName="Lastname";
			arg->createAccountParameters.password="asdf";
			arg->createAccountParameters.passwordRecoveryQuestion="1+2=?";
			arg->createAccountParameters.passwordRecoveryAnswer="3";
			arg->createAccountParameters.emailAddress="username@provider.com";
			arg->createAccountParameters.homeCountry="United States";
			arg->createAccountParameters.homeState="california";
			arg->createAccountParameters.sex_male=true;
			arg->userName=userName;
			arg->cdKey="Test CD Key";
			arg->titleName="Test Title Name";
		}
		break;
	case RakNet::L2MID_System_SetEmailAddressValidated:
		{
			RakNet::System_SetEmailAddressValidated *arg = (RakNet::System_SetEmailAddressValidated *) m;
			arg->validated=true;
			arg->userName=userName;
		}
		break;
	case RakNet::L2MID_Client_ValidateHandle:
		{
			RakNet::Client_ValidateHandle *arg = (RakNet::Client_ValidateHandle *) m;
			arg->userName=userName;
		}
		break;

	case RakNet::L2MID_System_DeleteAccount:
		{
			RakNet::System_DeleteAccount *arg = (RakNet::System_DeleteAccount *) m;
			arg->userName=userName;
			arg->password="asdf";
		}
		break;

	case RakNet::L2MID_System_PruneAccounts:
		{
			RakNet::System_PruneAccounts *arg = (RakNet::System_PruneAccounts *) m;
			arg->deleteAccountsNotLoggedInDays=1;
		}
		break;

	case RakNet::L2MID_Client_GetEmailAddress:
		{
			RakNet::Client_GetEmailAddress *arg = (RakNet::Client_GetEmailAddress *) m;
			arg->userName=userName;
		}
		break;

	case RakNet::L2MID_Client_GetPasswordRecoveryQuestionByHandle:
		{
			RakNet::Client_GetPasswordRecoveryQuestionByHandle *arg = (RakNet::Client_GetPasswordRecoveryQuestionByHandle *) m;
			arg->userName=userName;
		}
		break;

	case RakNet::L2MID_Client_GetPasswordByPasswordRecoveryAnswer:
		{
			RakNet::Client_GetPasswordByPasswordRecoveryAnswer *arg = (RakNet::Client_GetPasswordByPasswordRecoveryAnswer *) m;
			arg->userName=userName;
			arg->passwordRecoveryAnswer="3";
		}
		break;

	case RakNet::L2MID_Client_ChangeHandle:
		{
			RakNet::Client_ChangeHandle *arg = (RakNet::Client_ChangeHandle *) m;
			arg->userName=userName;
			arg->newHandle="New user handle";
		}
		break;

	case RakNet::L2MID_Client_UpdateAccount:
		{
			RakNet::Client_UpdateAccount *arg = (RakNet::Client_UpdateAccount *) m;
		}
		break;

	case RakNet::L2MID_Client_GetAccountDetails:
		{
			RakNet::Client_GetAccountDetails *arg = (RakNet::Client_GetAccountDetails *) m;
		}
		break;

	case RakNet::L2MID_Client_StartIgnore:
		{
			RakNet::Client_StartIgnore *arg = (RakNet::Client_StartIgnore *) m;
			arg->targetHandle=RakNet::RakString("user%i", instanceNumber+1);
		}
		break;

	case RakNet::L2MID_Client_StopIgnore:
		{
			RakNet::Client_StopIgnore *arg = (RakNet::Client_StopIgnore *) m;
			arg->targetHandle=RakNet::RakString("user%i", instanceNumber+1);
		}
		break;

	case RakNet::L2MID_Client_GetIgnoreList:
		{
			RakNet::Client_GetIgnoreList *arg = (RakNet::Client_GetIgnoreList *) m;
		}
		break;

	case RakNet::L2MID_Client_PerTitleIntegerStorage:
		{
			RakNet::Client_PerTitleIntegerStorage *arg = (RakNet::Client_PerTitleIntegerStorage *) m;
			arg->titleName="Test Title Name";
			arg->slotIndex=0;
			arg->conditionValue=1.0;
			arg->addConditionForOperation=RakNet::Client_PerTitleIntegerStorage::PTISC_GREATER_THAN;
			arg->inputValue=0.0;
			static int runCount=0;
			if (runCount++%2==0)
				arg->operationToPerform=RakNet::Client_PerTitleIntegerStorage::PTISO_WRITE;
			else
				arg->operationToPerform=RakNet::Client_PerTitleIntegerStorage::PTISO_READ;
		}
		break;

	case RakNet::L2MID_Friends_SendInvite:
		{
			RakNet::Friends_SendInvite *arg = (RakNet::Friends_SendInvite *) m;
			arg->targetHandle=RakNet::RakString("user%i", instanceNumber+1);
			arg->subject="Friends_SendInvite subject";
			arg->body="Friends_SendInvite body";
		}
		break;

	case RakNet::L2MID_Friends_AcceptInvite:
		{
			RakNet::Friends_AcceptInvite *arg = (RakNet::Friends_AcceptInvite *) m;
			arg->targetHandle=RakNet::RakString("user%i", 0);
			arg->subject="Friends_AcceptInvite subject";
			arg->body="Friends_AcceptInvite body";
			arg->emailStatus=0;
		}
		break;

	case RakNet::L2MID_Friends_RejectInvite:
		{
			RakNet::Friends_RejectInvite *arg = (RakNet::Friends_RejectInvite *) m;
			arg->targetHandle=RakNet::RakString("user%i", 0);
			arg->subject="L2MID_Friends_RejectInvite subject";
			arg->body="L2MID_Friends_RejectInvite body";
			arg->emailStatus=0;
		}
		break;

	case RakNet::L2MID_Friends_GetInvites:
		{
			RakNet::Friends_GetInvites *arg = (RakNet::Friends_GetInvites *) m;
		}
		break;

	case RakNet::L2MID_Friends_GetFriends:
		{
			RakNet::Friends_GetFriends *arg = (RakNet::Friends_GetFriends *) m;
		}
		break;

	case RakNet::L2MID_Friends_Remove:
		{
			RakNet::Friends_Remove *arg = (RakNet::Friends_Remove *) m;
			arg->targetHandle=RakNet::RakString("user%i", 0);
			arg->subject="L2MID_Friends_Remove subject";
			arg->body="L2MID_Friends_Remove body";
			arg->emailStatus=0;
		}
		break;

	case RakNet::L2MID_BookmarkedUsers_Add:
		{
			RakNet::BookmarkedUsers_Add *arg = (RakNet::BookmarkedUsers_Add *) m;
			arg->targetHandle=RakNet::RakString("user%i", instanceNumber+1);
			arg->type=0;
			arg->description="L2MID_BookmarkedUsers_Add description";
		}
		break;
	case RakNet::L2MID_BookmarkedUsers_Remove:
		{
			RakNet::BookmarkedUsers_Remove *arg = (RakNet::BookmarkedUsers_Remove *) m;
			arg->targetHandle=RakNet::RakString("user%i", instanceNumber+1);
			arg->type=0;
		}
		break;
	case RakNet::L2MID_BookmarkedUsers_Get:
		{
			RakNet::BookmarkedUsers_Get *arg = (RakNet::BookmarkedUsers_Get *) m;
		}
		break;

	case RakNet::L2MID_Emails_Send:
		{
			RakNet::Emails_Send *arg = (RakNet::Emails_Send *) m;
			arg->recipients.Insert(RakNet::RakString("user%i", instanceNumber+1), _FILE_AND_LINE_ );
			arg->recipients.Insert(RakNet::RakString("user%i", instanceNumber+2), _FILE_AND_LINE_ );
			arg->subject="L2MID_Emails_Send subject";
			arg->body="L2MID_Emails_Send body";
			arg->status=0;
		}
		break;

	case RakNet::L2MID_Emails_Get:
		{
			RakNet::Emails_Get *arg = (RakNet::Emails_Get *) m;
			arg->unreadEmailsOnly=true;
			arg->emailIdsOnly=true;
		}
		break;

	case RakNet::L2MID_Emails_Delete:
		{
			RakNet::Emails_Delete *arg = (RakNet::Emails_Delete *) m;
			arg->emailId=1;
		}
		break;

	case RakNet::L2MID_Emails_SetStatus:
		{
			RakNet::Emails_SetStatus *arg = (RakNet::Emails_SetStatus *) m;
			arg->emailId=2;
			arg->updateStatusFlag=true;
			arg->updateMarkedRead=true;
			arg->newStatusFlag=1234;
			arg->isNowMarkedRead=true;
		}
		break;

	case RakNet::L2MID_Ranking_SubmitMatch:
		{
			RakNet::Ranking_SubmitMatch *arg = (RakNet::Ranking_SubmitMatch *) m;
			arg->gameType="Match game type";
			arg->titleName="Test Title Name";
			arg->submittedMatch.matchNote="Ranking match note";
			arg->submittedMatch.matchParticipants.Insert(RakNet::MatchParticipant("user0", 5.0f), _FILE_AND_LINE_ );
			arg->submittedMatch.matchParticipants.Insert(RakNet::MatchParticipant("user1", 10.0f), _FILE_AND_LINE_ );
		}
		break;

	case RakNet::L2MID_Ranking_GetMatches:
		{
			RakNet::Ranking_GetMatches *arg = (RakNet::Ranking_GetMatches *) m;
			arg->gameType="Match game type";
			arg->titleName="Test Title Name";
		}
		break;

	case RakNet::L2MID_Ranking_GetMatchBinaryData:
		{
			RakNet::Ranking_GetMatchBinaryData *arg = (RakNet::Ranking_GetMatchBinaryData *) m;
			arg->matchID=1;
		}
		break;

	case RakNet::L2MID_Ranking_GetTotalScore:
		{
			RakNet::Ranking_GetTotalScore *arg = (RakNet::Ranking_GetTotalScore *) m;
			arg->targetHandle=RakNet::RakString("user%i", instanceNumber);
			arg->gameType="Match game type";
			arg->titleName="Test Title Name";
		}
		break;

	case RakNet::L2MID_Ranking_WipeScoresForPlayer:
		{
			RakNet::Ranking_WipeScoresForPlayer *arg = (RakNet::Ranking_WipeScoresForPlayer *) m;
			arg->targetHandle=RakNet::RakString("user%i", instanceNumber);
			arg->gameType="Match game type";
			arg->titleName="Test Title Name";
		}
		break;

	case RakNet::L2MID_Ranking_WipeMatches:
		{
			RakNet::Ranking_WipeMatches *arg = (RakNet::Ranking_WipeMatches *) m;
			arg->gameType="Match game type";
			arg->titleName="Test Title Name";
		}
		break;

	case RakNet::L2MID_Ranking_PruneMatches:
		{
			RakNet::Ranking_PruneMatches *arg = (RakNet::Ranking_PruneMatches *) m;
			arg->pruneTimeDays=1;
		}
		break;

	case RakNet::L2MID_Ranking_UpdateRating:
		{
			RakNet::Ranking_UpdateRating *arg = (RakNet::Ranking_UpdateRating *) m;
			arg->targetHandle=RakNet::RakString("user%i", instanceNumber);
			arg->gameType="Match game type";
			arg->titleName="Test Title Name";
			arg->targetRating=1234.0f;
		}
		break;

	case RakNet::L2MID_Ranking_WipeRatings:
		{
			RakNet::Ranking_WipeRatings *arg = (RakNet::Ranking_WipeRatings *) m;
			arg->gameType="Match game type";
			arg->titleName="Test Title Name";
		}
		break;

	case RakNet::L2MID_Ranking_GetRating:
		{
			RakNet::Ranking_GetRating *arg = (RakNet::Ranking_GetRating *) m;
			arg->targetHandle=RakNet::RakString("user%i", instanceNumber);
			arg->gameType="Match game type";
			arg->titleName="Test Title Name";
			arg->targetHandle=RakNet::RakString("user%i", instanceNumber);
		}
		break;

	case RakNet::L2MID_Clans_Create:
		{
			RakNet::Clans_Create *arg = (RakNet::Clans_Create *) m;
			static int idx=0;
			arg->clanHandle=RakNet::RakString("Clan handle %i", idx++);
			arg->failIfAlreadyInClan=false;
			arg->requiresInvitationsToJoin=true;
			arg->description="Clan Description";
			arg->binaryData->binaryData=new char[10];
			strcpy(arg->binaryData->binaryData,"Hello");
			arg->binaryData->binaryDataLength=10;
		}
		break;

	case RakNet::L2MID_Clans_SetProperties:
		{
			RakNet::Clans_SetProperties *arg = (RakNet::Clans_SetProperties *) m;
			arg->clanHandle="Clan handle";
			arg->description="Updated description";
		}
		break;

	case RakNet::L2MID_Clans_GetProperties:
		{
			RakNet::Clans_GetProperties *arg = (RakNet::Clans_GetProperties *) m;
			arg->clanHandle="Clan handle";
		}
		break;

	case RakNet::L2MID_Clans_SetMyMemberProperties:
		{
			RakNet::Clans_SetMyMemberProperties *arg = (RakNet::Clans_SetMyMemberProperties *) m;
			arg->clanHandle="Clan handle";
			arg->description="Updated description";
		}
		break;

	case RakNet::L2MID_Clans_GrantLeader:
		{
			RakNet::Clans_GrantLeader *arg = (RakNet::Clans_GrantLeader *) m;
			arg->clanHandle="Clan handle";
			arg->targetHandle=RakNet::RakString("user%i", instanceNumber+1);
		}
		break;

	case RakNet::L2MID_Clans_SetSubleaderStatus:
		{
			RakNet::Clans_SetSubleaderStatus *arg = (RakNet::Clans_SetSubleaderStatus *) m;
			arg->clanHandle="Clan handle";
			arg->targetHandle=RakNet::RakString("user%i", instanceNumber+1);
			arg->setToSubleader=true;
		}
		break;

	case RakNet::L2MID_Clans_SetMemberRank:
		{
			RakNet::Clans_SetMemberRank *arg = (RakNet::Clans_SetMemberRank *) m;
			arg->clanHandle="Clan handle";
			arg->targetHandle=RakNet::RakString("user%i", instanceNumber+1);
			arg->newRank=666;
		}
		break;

	case RakNet::L2MID_Clans_GetMemberProperties:
		{
			RakNet::Clans_GetMemberProperties *arg = (RakNet::Clans_GetMemberProperties *) m;
			arg->clanHandle="Clan handle";
			arg->targetHandle=RakNet::RakString("user%i", instanceNumber);
		}
		break;

	case RakNet::L2MID_Clans_ChangeHandle:
		{
			RakNet::Clans_ChangeHandle *arg = (RakNet::Clans_ChangeHandle *) m;
			arg->oldClanHandle="Clan handle";
			arg->newClanHandle="New Clan handle";
		}
		break;

	case RakNet::L2MID_Clans_Leave:
		{
			RakNet::Clans_Leave *arg = (RakNet::Clans_Leave *) m;
			arg->clanHandle="Clan handle";
			arg->dissolveIfClanLeader=false;
			arg->subject="L2MID_Clans_Leave";
			arg->emailStatus=0;
		}
		break;

	case RakNet::L2MID_Clans_Get:
		{
			RakNet::Clans_Get *arg = (RakNet::Clans_Get *) m;
		}
		break;

	case RakNet::L2MID_Clans_SendJoinInvitation:
		{
			RakNet::Clans_SendJoinInvitation *arg = (RakNet::Clans_SendJoinInvitation *) m;
			static int idx=0;
			arg->clanHandle=RakNet::RakString("Clan handle %i", idx++);
			arg->targetHandle=RakNet::RakString("user%i", instanceNumber+1);
			arg->subject="L2MID_Clans_SendJoinInvitation";
		}
		break;

	case RakNet::L2MID_Clans_WithdrawJoinInvitation:
		{
			RakNet::Clans_WithdrawJoinInvitation *arg = (RakNet::Clans_WithdrawJoinInvitation *) m;
			arg->clanHandle="Clan handle";
			arg->targetHandle=RakNet::RakString("user%i", instanceNumber+1);
			arg->subject="L2MID_Clans_WithdrawJoinInvitation";
		}
		break;

	case RakNet::L2MID_Clans_AcceptJoinInvitation:
		{
			RakNet::Clans_AcceptJoinInvitation *arg = (RakNet::Clans_AcceptJoinInvitation *) m;
			static int idx=0;
			arg->clanHandle=RakNet::RakString("Clan handle %i", idx++);
			arg->subject="L2MID_Clans_AcceptJoinInvitation";
			arg->failIfAlreadyInClan=false;
		}
		break;

	case RakNet::L2MID_Clans_RejectJoinInvitation:
		{
			RakNet::Clans_RejectJoinInvitation *arg = (RakNet::Clans_RejectJoinInvitation *) m;
			static int idx=0;
			arg->clanHandle=RakNet::RakString("Clan handle %i", idx++);
			arg->subject="L2MID_Clans_WithdrawJoinInvitation";
		}
		break;

	case RakNet::L2MID_Clans_DownloadInvitationList:
		{
			RakNet::Clans_DownloadInvitationList *arg = (RakNet::Clans_DownloadInvitationList *) m;
		}
		break;

	case RakNet::L2MID_Clans_SendJoinRequest:
		{
			RakNet::Clans_SendJoinRequest *arg = (RakNet::Clans_SendJoinRequest *) m;
			arg->clanHandle="Clan handle";
			arg->subject="L2MID_Clans_SendJoinRequest";
		}
		break;

	case RakNet::L2MID_Clans_WithdrawJoinRequest:
		{
			RakNet::Clans_WithdrawJoinRequest *arg = (RakNet::Clans_WithdrawJoinRequest *) m;
			arg->clanHandle="Clan handle";
			arg->subject="L2MID_Clans_WithdrawJoinRequest";
		}
		break;

	case RakNet::L2MID_Clans_AcceptJoinRequest:
		{
			RakNet::Clans_AcceptJoinRequest *arg = (RakNet::Clans_AcceptJoinRequest *) m;
			arg->clanHandle="Clan handle";
			arg->requestingUserHandle=RakNet::RakString("user%i", instanceNumber+1);
			arg->subject="L2MID_Clans_AcceptJoinRequest";
		}
		break;

	case RakNet::L2MID_Clans_RejectJoinRequest:
		{
			RakNet::Clans_RejectJoinRequest *arg = (RakNet::Clans_RejectJoinRequest *) m;
			arg->clanHandle="Clan handle";
			arg->requestingUserHandle=RakNet::RakString("user%i", instanceNumber+1);
		}
		break;

	case RakNet::L2MID_Clans_DownloadRequestList:
		{
			RakNet::Clans_DownloadRequestList *arg = (RakNet::Clans_DownloadRequestList *) m;
		}
		break;

	case RakNet::L2MID_Clans_KickAndBlacklistUser:
		{
			RakNet::Clans_KickAndBlacklistUser *arg = (RakNet::Clans_KickAndBlacklistUser *) m;
			arg->clanHandle="Clan handle";
			arg->targetHandle=RakNet::RakString("user%i", instanceNumber+1);
			arg->kick=true;
			arg->blacklist=true;
		}
		break;

	case RakNet::L2MID_Clans_UnblacklistUser:
		{
			RakNet::Clans_UnblacklistUser *arg = (RakNet::Clans_UnblacklistUser *) m;
			arg->clanHandle="Clan handle";
			arg->targetHandle=RakNet::RakString("user%i", instanceNumber+1);
		}
		break;

	case RakNet::L2MID_Clans_GetBlacklist:
		{
			RakNet::Clans_GetBlacklist *arg = (RakNet::Clans_GetBlacklist *) m;
			arg->clanHandle="Clan handle";
		}
		break;

	case RakNet::L2MID_Clans_GetMembers:
		{
			RakNet::Clans_GetMembers *arg = (RakNet::Clans_GetMembers *) m;
			arg->clanHandle="Clan handle";
		}
		break;

	case RakNet::L2MID_Clans_CreateBoard:
		{
			RakNet::Clans_CreateBoard *arg = (RakNet::Clans_CreateBoard *) m;
		}
		break;

	case RakNet::L2MID_Clans_DestroyBoard:
		{
			RakNet::Clans_DestroyBoard *arg = (RakNet::Clans_DestroyBoard *) m;
		}
		break;

	case RakNet::L2MID_Clans_CreateNewTopic:
		{
			RakNet::Clans_CreateNewTopic *arg = (RakNet::Clans_CreateNewTopic *) m;
		}
		break;

	case RakNet::L2MID_Clans_ReplyToTopic:
		{
			RakNet::Clans_ReplyToTopic *arg = (RakNet::Clans_ReplyToTopic *) m;
		}
		break;

	case RakNet::L2MID_Clans_RemovePost:
		{
			RakNet::Clans_RemovePost *arg = (RakNet::Clans_RemovePost *) m;
		}
		break;

	case RakNet::L2MID_Clans_GetBoards:
		{
			RakNet::Clans_GetBoards *arg = (RakNet::Clans_GetBoards *) m;
		}
		break;

	case RakNet::L2MID_Clans_GetTopics:
		{
			RakNet::Clans_GetTopics *arg = (RakNet::Clans_GetTopics *) m;
		}
		break;

	case RakNet::L2MID_Clans_GetPosts:
		{
			RakNet::Clans_GetPosts *arg = (RakNet::Clans_GetPosts *) m;
		}
		break;
	}
	lobby2Client[instanceNumber].SendMsg(m);
	messageFactory.Dealloc(m);
}
