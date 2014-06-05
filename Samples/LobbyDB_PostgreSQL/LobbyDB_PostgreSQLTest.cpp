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
#include "Kbhit.h"
#include "GetTime.h"
#include "FunctionThread.h"
#include "LobbyDB_PostgreSQL.h"
#include "EpochTimeToString.h"

#ifdef _WIN32
#include <windows.h> // Sleep
#else
#include <unistd.h> // usleep
#endif

// localtime
#include <stdio.h>
#include <string.h>
#include <time.h>

// Database queries are asynchronous so the results are read in Functor::HandleResult. The default implementation passes it to a callback
class DBResultHandler : public LobbyDBCBInterface
{
	virtual void CreateUser_CB(CreateUser_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("CreateUser call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("CreateUser call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("CreateUser result:\n");
			RakNet::RakString queryResultString;

			printf("[out] queryOK=%i\n", callResult->queryOK);
			printf("[out] queryResultString=%s\n", callResult->queryResultString.C_String());
		}

		printf("\n");
	}
	virtual void GetUser_CB(GetUser_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("GetUser call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("GetUser call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else if (callResult->userFound==false)
		{
			printf("GetUser call did not find specified user.\n");
		}
		else
		{
			printf("GetUser result:\n");

			printf("[out] isBanned=%i\n", callResult->isBanned);
			printf("[out] isSuspended=%i\n", callResult->isSuspended);
			printf("[out] suspensionExpiration=%s\n", EpochTimeToString(callResult->suspensionExpiration));
			printf("[out] banOrSuspensionReason=%s\n", callResult->banOrSuspensionReason.C_String());
			printf("[out] creationTime=%s\n", EpochTimeToString(callResult->creationTime));
			printf("[out] id.userId=%i\n", callResult->id.databaseRowId);
			printf("[out] handle=%s\n", callResult->output.handle.C_String());
			printf("[out] firstName=%s\n", callResult->output.firstName.C_String());
			printf("[out] middleName=%s\n", callResult->output.middleName.C_String());
			printf("[out] lastName=%s\n", callResult->output.lastName.C_String());
			printf("[out] race=%s\n", callResult->output.race.C_String());
			printf("[out] sex=%s\n", callResult->output.sex.C_String());
			printf("[out] homeAddress1=%s\n", callResult->output.homeAddress1.C_String());
			printf("[out] homeAddress2=%s\n", callResult->output.homeAddress2.C_String());
			printf("[out] homeCity=%s\n", callResult->output.homeCity.C_String());
			printf("[out] homeState=%s\n", callResult->output.homeState.C_String());
			printf("[out] homeProvince=%s\n", callResult->output.homeProvince.C_String());
			printf("[out] homeCountry=%s\n", callResult->output.homeCountry.C_String());
			printf("[out] homeZipCode=%s\n", callResult->output.homeZipCode.C_String());
			printf("[out] billingAddress1=%s\n", callResult->output.billingAddress1.C_String());
			printf("[out] billingAddress2=%s\n", callResult->output.billingAddress2.C_String());
			printf("[out] billingCity=%s\n", callResult->output.billingCity.C_String());
			printf("[out] billingState=%s\n", callResult->output.billingState.C_String());
			printf("[out] billingProvince=%s\n", callResult->output.billingProvince.C_String());
			printf("[out] billingCountry=%s\n", callResult->output.billingCountry.C_String());
			printf("[out] billingZipCode=%s\n", callResult->output.billingZipCode.C_String());
			printf("[out] emailAddress=%s\n", callResult->output.emailAddress.C_String());
			printf("[out] password=%s\n", callResult->output.password.C_String());
			printf("[out] passwordRecoveryQuestion=%s\n", callResult->output.passwordRecoveryQuestion.C_String());
			printf("[out] passwordRecoveryAnswer=%s\n", callResult->output.passwordRecoveryAnswer.C_String());
			printf("[out] caption1=%s\n", callResult->output.caption1.C_String());
			printf("[out] caption2=%s\n", callResult->output.caption2.C_String());
			printf("[out] caption3=%s\n", callResult->output.caption3.C_String());
			printf("[out] dateOfBirth=%s\n", callResult->output.dateOfBirth.C_String());
			printf("[out] accountNumber=%i\n", callResult->output.accountNumber);
			printf("[out] creditCardNumber=%s\n", callResult->output.creditCardNumber.C_String());
			printf("[out] creditCardExpiration=%s\n", callResult->output.creditCardExpiration.C_String());
			printf("[out] creditCardCVV=%s\n", callResult->output.creditCardCVV.C_String());
			printf("[out] adminLevel=%s\n", callResult->output.adminLevel.C_String());
			printf("[out] permissions=%s\n", callResult->output.permissions.C_String());
			printf("[out] accountBalance=%f\n", callResult->output.accountBalance);
			printf("[out] sourceIPAddress=%s\n", callResult->output.sourceIPAddress.C_String());
			printf("[out] binaryDataLength=%i\n", callResult->output.binaryDataLength);
		}

		printf("\n");
	}
	virtual void UpdateUser_CB(UpdateUser_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("UpdateUser call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("UpdateUser call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("UpdateUser Done.");
		}
		printf("\n");
	}
	virtual void DeleteUser_CB(DeleteUser_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("DeleteUser call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("DeleteUser call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("DeleteUser Done.");
		}
		printf("\n");
	}
	virtual void ChangeUserHandle_CB(ChangeUserHandle_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("ChangeUserHandle call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("ChangeUserHandle call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else if (callResult->success==false)
		{
			printf("ChangeUserHandle failed: %s.", callResult->queryResult.C_String());
		}
		else
		{
			printf("ChangeUserHandle Done.");
		}
		printf("\n");
	}
	virtual void AddAccountNote_CB(AddAccountNote_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("AddAccountNote call canceled:\n");
		else if (callResult->userFound==false)
		{
			printf("AddAccountNote call user not found:\n");
		}
		else if (callResult->dbQuerySuccess==false)	
		{
			printf("AddAccountNote call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("AddAccountNote Done.");
		}
		printf("\n");
	}
	virtual void GetAccountNotes_CB(GetAccountNotes_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("GetAccountNotes call canceled:\n");
		else if (callResult->dbQuerySuccess==false)	
		{
			printf("GetAccountNotes call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			if (callResult->accountNotes.Size()>0)
			{
				printf("GetAccountNotes Result:\n");
				unsigned i;
				for (i=0; i < callResult->accountNotes.Size(); i++)
				{
					printf("%i. moderatorId=%i\n", i+1, callResult->accountNotes[i]->moderatorId);
					printf("%i. moderatorUsername=%s\n", i+1, callResult->accountNotes[i]->moderatorUsername.C_String());
					printf("%i. type=%s\n", i+1, callResult->accountNotes[i]->type.C_String());
					printf("%i. subject=%s\n", i+1, callResult->accountNotes[i]->subject.C_String());
					printf("%i. body=%s\n", i+1, callResult->accountNotes[i]->body.C_String());
					printf("%i. creationTime=%s\n", i+1, EpochTimeToString(callResult->accountNotes[i]->time));
				}
			}
			else
			{
				printf("GetAccountNotes: No records found");
			}
		}
		printf("\n");
	}
	virtual void AddFriend_CB(AddFriend_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("AddFriend call canceled:\n");
		else if (callResult->success==false)
		{
			printf("AddFriend failed: %s.\n", callResult->queryResult.C_String());
		}
		else if (callResult->dbQuerySuccess==false)
		{
			printf("AddFriend call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("AddFriend success.\n");
		}

		printf("\n");
	}
	virtual void RemoveFriend_CB(RemoveFriend_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("RemoveFriend call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("RemoveFriend call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("RemoveFriend success.\n");
		}

		printf("\n");
	}
	virtual void GetFriends_CB(GetFriends_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("GetFriends call canceled:\n");
		else if (callResult->dbQuerySuccess==false)	
		{
			printf("GetFriends call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			if (callResult->friends.Size()>0)
			{
				printf("GetFriends Result:\n");
				unsigned i;
				for (i=0; i < callResult->friends.Size(); i++)
				{
					printf("%i. yourId=%i\n", i+1, callResult->friends[i]->id.databaseRowId);
					printf("%i. yourUsername=%s\n", i+1, callResult->friends[i]->id.handle.C_String());
					printf("%i. friendId=%i\n", i+1, callResult->friends[i]->friendId.databaseRowId);
					printf("%i. friendUsername=%s\n", i+1, callResult->friends[i]->friendId.handle.C_String());
					printf("%i. creationTime=%s\n", i+1, EpochTimeToString(callResult->friends[i]->creationTime));
				}
			}
			else
			{
				printf("GetFriends: No records found");
			}
		}
		printf("\n");
	}
	virtual void AddToIgnoreList_CB(AddToIgnoreList_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
		{
			printf("AddToIgnoreList call canceled.");
		}
		else if (callResult->success==false)
		{
			printf("AddToIgnoreList failed: %s.\n", callResult->queryResult.C_String());
		}
		else if (callResult->dbQuerySuccess==false)	
		{
			printf("AddToIgnoreList call DB failure.\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("AddToIgnoreList success.");
		}

		printf("\n");
	}
	virtual void RemoveFromIgnoreList_CB(RemoveFromIgnoreList_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("RemoveFromIgnoreList call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("RemoveFromIgnoreList call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("RemoveFromIgnoreList success.\n");
		}

		printf("\n");
	}
	virtual void GetIgnoreList_CB(GetIgnoreList_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("GetIgnoreList call canceled:\n");
		else if (callResult->dbQuerySuccess==false)	
		{
			printf("GetIgnoreList call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			if (callResult->ignoredUsers.Size()>0)
			{
				printf("GetIgnoreList Result:\n");
				unsigned i;
				for (i=0; i < callResult->ignoredUsers.Size(); i++)
				{
					printf("%i. yourId=%i\n", i+1, callResult->ignoredUsers[i]->id.databaseRowId);
					printf("%i. yourUsername=%s\n", i+1, callResult->ignoredUsers[i]->id.handle.C_String());
					printf("%i. ignoredUser.id=%i\n", i+1, callResult->ignoredUsers[i]->ignoredUser.databaseRowId);
					printf("%i. ignoredUser.userHandle=%s\n", i+1, callResult->ignoredUsers[i]->ignoredUser.handle.C_String());
					printf("%i. creationTime=%s\n", i+1, EpochTimeToString(callResult->ignoredUsers[i]->creationTime));
				}
			}
			else
			{
				printf("GetIgnoreList: No records found");
			}
		}
		printf("\n");
	}
	virtual void SendEmail_CB(SendEmail_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("SendEmail call canceled:\n");
		else if (callResult->validParameters==false)
		{
			printf("SendEmail invalid parameters:\n");
			printf("%s", callResult->failureMessage.C_String());
		}
		else if (callResult->dbQuerySuccess==false)	
		{
			printf("SendEmail call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("SendEmail success");
		}
		printf("\n");
	}
	virtual void GetEmails_CB(GetEmails_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("GetEmails_CB call canceled:\n");
		else if (callResult->validParameters==false)
		{
			printf("GetEmails invalid parameters:\n");
			printf("%s", callResult->failureMessage.C_String());
		}
		else if (callResult->dbQuerySuccess==false)	
		{
			printf("GetEmails_CB call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("GetEmails success\n");
			printf("inbox=%i\n", callResult->inbox);

			unsigned i;
			for (i=0; i < callResult->emails.Size(); i++)
			{
				printf("%i. id.userId=%i\n", i+1, callResult->emails[i]->id.databaseRowId);
				printf("%i. id.userHandle=%s\n", i+1, callResult->emails[i]->id.handle.C_String());
				printf("%i. emailMessageID=%i\n", i+1, callResult->emails[i]->emailMessageID);
				printf("%i. subject=%s\n", i+1, callResult->emails[i]->subject.C_String());
				printf("%i. body=%s\n", i+1, callResult->emails[i]->body.C_String());
				printf("%i. creationTime=%s\n", i+1, EpochTimeToString(callResult->emails[i]->creationTime));
				printf("%i. status=%i\n", i+1, callResult->emails[i]->status);
				printf("%i. attachmentLength=%i\n", i+1, callResult->emails[i]->attachmentLength);
			}
		}
		printf("\n");
	}
	virtual void DeleteEmail_CB(DeleteEmail_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("DeleteEmail_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)	
		{
			printf("DeleteEmail_CB call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("DeleteEmail success\n");
		}
		printf("\n");
	}
	virtual void UpdateEmailStatus_CB(UpdateEmailStatus_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("UpdateEmailStatus_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)	
		{
			printf("UpdateEmailStatus_CB call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("UpdateEmailStatus success\n");
		}
		printf("\n");
	}
	virtual void GetHandleFromUserId_CB(GetHandleFromUserId_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("GetHandleFromUserId_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)	
		{
			printf("GetHandleFromUserId_CB call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else if (callResult->success==false)
		{
			printf("GetHandleFromUserId_CB lookup failure:\n");
		}
		else
		{
			printf("GetHandleFromUserId_CB success\n");
			printf("[in] userId=%i\n", callResult->userId);
			printf("[out] handle=%s", callResult->handle.C_String());
		}
		printf("\n");
	}
	virtual void GetUserIdFromHandle_CB(GetUserIdFromHandle_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("GetUserIdFromHandle_CB call canceled.");
		else if (callResult->dbQuerySuccess==false)	
		{
			printf("GetUserIdFromHandle_CB call DB failure.");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else if (callResult->success==false)
		{
			printf("GetUserIdFromHandle_CB lookup failure.");
		}
		else
		{
			printf("GetUserIdFromHandle_CB success\n");
			printf("[in] handle=%s\n", callResult->handle.C_String());
			printf("[out] userId=%i", callResult->userId);
		}
		printf("\n");
	}
	virtual void AddDisallowedHandle_CB(AddDisallowedHandle_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("AddDisallowedHandle_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("AddDisallowedHandle_CB call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("AddDisallowedHandle_CB Done.");
		}
		printf("\n");
	}
	virtual void RemoveDisallowedHandle_CB(RemoveDisallowedHandle_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("RemoveDisallowedHandle_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("RemoveDisallowedHandle_CB call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("RemoveDisallowedHandle_CB Done.");
		}
		printf("\n");
	}
	virtual void IsDisallowedHandle_CB(IsDisallowedHandle_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("IsDisallowedHandle_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("IsDisallowedHandle_CB call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("IsDisallowedHandle_CB Done.\n");
			printf("exists=%i", callResult->exists);
		}
		printf("\n");
	}
	virtual void AddToActionHistory_CB(AddToActionHistory_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("AddToActionHistory_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("AddToActionHistory_CB call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("AddToActionHistory_CB Done.\n");
		}
		printf("\n");
	}
	virtual void GetActionHistory_CB(GetActionHistory_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("GetActionHistory_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("GetActionHistory_CB call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("GetActionHistory_CB Done.\n");
		}
		printf("\n");

		unsigned i;
		for (i=0; i < callResult->history.Size(); i++)
		{
			printf("%i. id.userId=%i\n", i+1, callResult->history[i]->id.databaseRowId);
			printf("%i. id.userHandle=%s\n", i+1, callResult->history[i]->id.handle.C_String());
			printf("%i. actionTime=%s\n", i+1, callResult->history[i]->actionTime);
			printf("%i. actionTaken=%s\n", i+1, callResult->history[i]->actionTaken.C_String());
			printf("%i. description=%s\n", i+1, callResult->history[i]->description.C_String());
			printf("%i. sourceIpAddress=%s\n", i+1, callResult->history[i]->sourceIpAddress.C_String());
			printf("%i. creationTime=%s\n", i+1, EpochTimeToString(callResult->history[i]->creationTime));
		}
	}

	// CLANS

	virtual void UpdateClanMember_CB(UpdateClanMember_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("UpdateClanMember_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("UpdateClanMember_CB call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else if (callResult->failureMessage.IsEmpty()==false)
		{
			printf("UpdateClanMember_CB general failure:\n");
			printf("%s", callResult->failureMessage.C_String());
		}
		else
		{
			printf("UpdateClanMember_CB Done.\n");
		}
		printf("\n");
	}
	virtual void UpdateClan_CB(UpdateClan_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("UpdateClan_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("UpdateClan_CB call DB failure:\n");
			printf("%s", callResult->lobbyServer->GetLastError());
		}
		else if (callResult->failureMessage.IsEmpty()==false)
		{
			printf("UpdateClan_CB general failure:\n");
			printf("%s", callResult->failureMessage.C_String());
		}
		else
		{
			printf("UpdateClan_CB Done.\n");
		}
		printf("\n");
	}
	virtual void CreateClan_CB(CreateClan_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("CreateClan_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{	
			printf("CreateClan_CB call DB failure:\n");
			printf("%s\n", callResult->lobbyServer->GetLastError());
			printf("%s\n", callResult->initialClanData.failureMessage.C_String());
			printf("%s", callResult->leaderData.failureMessage.C_String());
		}	
		else if (callResult->initialClanData.failureMessage.IsEmpty()==false || callResult->leaderData.failureMessage.IsEmpty()==false	)
		{
			printf("CreateClan_CB general failure:\n");
			printf("%s\n", callResult->initialClanData.failureMessage.C_String());
			printf("%s", callResult->leaderData.failureMessage.C_String());
		}
		else
		{
			printf("CreateClan_CB Done. New clan ID = %i\n", callResult->initialClanData.clanId.databaseRowId);
		}
		printf("\n");
	}
	virtual void ChangeClanHandle_CB(ChangeClanHandle_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("ChangeClanHandle_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{	
			printf("ChangeClanHandle_CB call DB failure:\n");
			printf("%s\n", callResult->lobbyServer->GetLastError());
		}	
		else if (callResult->failureMessage.IsEmpty()==false)
		{
			printf("ChangeClanHandle_CB general failure:\n");
			printf("%s\n", callResult->failureMessage.C_String());
		}
		else
		{
			printf("ChangeClanHandle_CB Done.");
		}
		printf("\n");
	}
	virtual void DeleteClan_CB(DeleteClan_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("DeleteClan_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{	
			printf("DeleteClan_CB call DB failure:\n");
			printf("%s\n", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("DeleteClan_CB Done.");
		}
		printf("\n");
	}
	virtual void GetClans_CB(GetClans_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("GetClans_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{	
			printf("GetClans_CB call DB failure:\n");
			printf("%s\n", callResult->lobbyServer->GetLastError());
		}
		else
		{
			unsigned clanIndex, memberIndex;
			LobbyDBSpec::UpdateClanMember_Data *member;
			LobbyDBSpec::UpdateClan_Data *clan;

			if (callResult->clans.Size()>0)
			{
				printf("%i CLANS FOUND for user %i\n", callResult->clans.Size(), callResult->userId.databaseRowId);
				for (clanIndex=0; clanIndex < callResult->clans.Size(); clanIndex++)
				{
					clan = callResult->clans[clanIndex];
					printf("(%i.) CLAN %s (ID=%i):\n", clanIndex+1, clan->handle.C_String(), clan->clanId.databaseRowId);
					printf("%i %i %i %i:\n", clan->integers[0], clan->integers[1],clan->integers[2],clan->integers[3]);
					printf("%.1f %.1f %.1f %.1f:\n", clan->floats[0], clan->floats[1],clan->floats[2],clan->floats[3]);
					printf("DESC1: %s\n", clan->descriptions[0].C_String());
					printf("DESC2: %s\n", clan->descriptions[1].C_String());
					printf("Requires invitations to join = ");
					if (clan->requiresInvitationsToJoin)
						printf("true\n");
					else
						printf("false\n");

					printf("Member list for clan %s:\n", clan->handle.C_String());
					for (memberIndex=0; memberIndex < callResult->clans[clanIndex]->members.Size(); memberIndex++)
					{
						member = clan->members[memberIndex];
						printf("(%i.) ", memberIndex+1);
						if (member->mEStatus1==LobbyDBSpec::CLAN_MEMBER_STATUS_LEADER)
							printf("<Leader>");
						else if (member->mEStatus1==LobbyDBSpec::CLAN_MEMBER_STATUS_SUBLEADER)
							printf("<Subleader>");
						else if (member->mEStatus1==LobbyDBSpec::CLAN_MEMBER_STATUS_MEMBER)
							printf("<Member>");
						else if (member->mEStatus1==LobbyDBSpec::CLAN_MEMBER_STATUS_REQUESTED_TO_JOIN)
							printf("<Requested>");
						else if (member->mEStatus1==LobbyDBSpec::CLAN_MEMBER_STATUS_INVITED_TO_JOIN)
							printf("<Invited>");
						else if (member->mEStatus1==LobbyDBSpec::CLAN_MEMBER_STATUS_BLACKLISTED)
							printf("<Blacklisted>");
						else
							printf("<Unknown>");
						printf(" %s (ID=%i)\n", member->userId.handle.C_String(), member->userId.databaseRowId);

						printf("%i %i %i %i:\n", member->integers[0], member->integers[1],member->integers[2],member->integers[3]);
						printf("%.1f %.1f %.1f %.1f:\n", member->floats[0], member->floats[1],member->floats[2],member->floats[3]);
						printf("DESC1: %s\n", member->descriptions[0].C_String());
						printf("DESC2: %s\n", member->descriptions[1].C_String());
					}
				}
				
			}
			else
				printf("No clans found");

		}
		printf("\n");
	}
	virtual void RemoveFromClan_CB(RemoveFromClan_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("RemoveFromClan_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{	
			printf("RemoveFromClan_CB call DB failure:\n");
			printf("%s\n", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("RemoveFromClan_CB Done.");
		}
		printf("\n");
	}
	virtual void AddToClanBoard_CB(AddToClanBoard_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("AddToClanBoard_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{	
			printf("AddToClanBoard_CB call DB failure:\n");
			printf("%s\n", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("AddToClanBoard_CB Done.");
		}
		printf("\n");
	}
	virtual void RemoveFromClanBoard_CB(RemoveFromClanBoard_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("RemoveFromClanBoard_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{	
			printf("RemoveFromClanBoard_CB call DB failure:\n");
			printf("%s\n", callResult->lobbyServer->GetLastError());
		}
		else
		{
			printf("RemoveFromClanBoard_CB Done.");
		}
		printf("\n");
	}
	virtual void GetClanBoard_CB(GetClanBoard_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("GetClanBoard_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{	
			printf("GetClanBoard_CB call DB failure:\n");
			printf("%s\n", callResult->lobbyServer->GetLastError());
		}
		else
		{
			unsigned index;
			LobbyDBSpec::AddToClanBoard_Data *post;

			if (callResult->board.Size()>0)
			{
				printf("%i POSTS FOUND\n", callResult->board.Size());
				for (index=0; index < callResult->board.Size(); index++)
				{
					post = callResult->board[index];
					printf("(%i.) Author=%s (ID=%i)\n", index+1, post->userId.handle.C_String(), post->userId.databaseRowId);
					printf("%i %i %i %i:\n", post->integers[0], post->integers[1],post->integers[2],post->integers[3]);
					printf("%.1f %.1f %.1f %.1f:\n", post->floats[0], post->floats[1],post->floats[2],post->floats[3]);
					printf("Subject: %s\n", post->subject.C_String());
					printf("Body: %s\n", post->body.C_String());
				}
			}
			else
			{
				printf("No posts found.");
			}
		}
		printf("\n");
	}
};

void main(int argc, char **argv)
{
	printf("A sample on using the provided implementation\nof the Lobby Server specification based on PostgreSQL\n");
	printf("Unlike the other samples, this is a server-only\nprocess so does not involve networking with RakNet.\n");
	printf("The goal of this class is to allow you to save\nand retrieve users, emails, and transactions for a lobby service\n");
	printf("Difficulty: Intermediate\n\n");

	// A function thread is class that spawns a thread that operates on functors.
	// A functor is an instance of a class that has two pre-defined functions: One to perform processing, another to get the result.
	// One per application is enough to not block
	RakNet::FunctionThread ft;

	// Start one thread. Starting more than one may be advantageous for multi-core processors. However, in this scenario we are
	// blocking on database calls rather than processing.
	ft.StartThreads(1);

	// The real functionality of the LobbyServer is contained in the functors defined in LobbyDB_PostgreSQL.h/.cpp.
	// However, this class contains some utility functions and members, such as the functionThread and a pointer to
	// the postgreSQL interface.
	LobbyDB_PostgreSQL lobbyServer;

	// LobbyDB_PostgreSQL internally uses a functionThread so that database queries can happen asynchronously,
	// as opposed to blocking and slowing down the game.
	// If you don't assign one it will create one automatically and start it with one thread.
	lobbyServer.AssignFunctionThread(&ft);

	// The default implementation of the functors pass Functor::HandleResult through to a callback, instantiated here.
	// Alternatively, you could derive from the *_PostgreSQLImpl functors found in LobbyDB_PostgreSQL.h/.cpp
	// and override the behavior of Functor::HandleResult
	DBResultHandler resultHandler;
	lobbyServer.AssignCallback(&resultHandler);

	printf("Enter database password:\n");
	char connectionString[256],password[128];
	char username[256];
	strcpy(username, "postgres");
	gets(password);
	strcpy(connectionString, "user=");
	strcat(connectionString, username);
	strcat(connectionString, " password=");
	strcat(connectionString, password);
	// database=blah
	if (lobbyServer.Connect(connectionString)==false)
	{
		printf("Database connection failed.\n");
		return;
	}
	printf("Database connection succeeded.\n");
	printf("(A) Drop tables\n"
		"(B) Create tables.\n"
		"(C) Create user.\n"
		"(D) Get user.\n"
		"(E) Update user.\n"
		"(F) Add account note for a user.\n"
		"(G) Get account notes for a user.\n"
		"(H) Add friend.\n"
		"(I) Remove friend.\n"
		"(J) Get friends.\n"
		"(K) Add to ignore list.\n"
		"(L) Remove from ignore list.\n"
		"(M) Get ignore list.\n"
		"(O) Send email to recipient(s).\n"
		"(P) Get email(s).\n"
		"(Q) Delete email.\n"
		"(R) Update email status flags.\n"
		"(S) Get handle from user Id.\n"
		"(T) Get user Id from handle.\n"
		"(U) Delete user.\n"
		"(V) Change user handle.\n"
		"(W) Add Disallowed Handle.\n"
		"(X) Remove Disallowed Handle.\n"
		"(Y) Is Disallowed Handle.\n"
		"(Z) Add to user action history.\n"
		"(0) Get action history.\n"
		"(1) Add or update clan member.\n"
		"(2) Update clan.\n"
		"(3) Create clan.\n"
		"(4) Change clan handle.\n"
		"(5) Delete clan.\n"
		"(6) Get all clans.\n"
		"(7) Remove member from clan.\n"
		"(8) Add to clan bulletin board.\n"
		"(9) Remove from clan bulletin board.\n"
		"(!) Get clan bulletin board.\n"
		"(~) Quit\n");


	char inputStr[512];
	char ch;
	while (1)
	{
		if (kbhit())
		{
			ch=getch();
			if (ch=='~')
				break;
			else if (ch=='a')
			{
				if (lobbyServer.DestroyLobbyServerTables()==false)
					printf("%s", lobbyServer.GetLastError());
				else
					printf("Tables dropped.\n");
			}
			else if (ch=='b')
			{
				if (lobbyServer.CreateLobbyServerTables()==false)
					printf("%s", lobbyServer.GetLastError());
				else
					printf("Tables created.\n");
			}
			else if (ch=='c')
			{
				// We could do new and delete, but this ensures the class is allocated and deallocated in the same DLL, should we use one.
				// By default deallocation takes place in HandleResult()
				CreateUser_PostgreSQLImpl *functor = CreateUser_PostgreSQLImpl::Alloc();

				printf("Create a user and add him to the database\n");
				printf("Enter user handle: ");
				gets(inputStr);
				functor->handle = inputStr; // Test possibly blank handle

				// Put in test data for everything else, too many to ask the user
				functor->firstName = "Kevin";;
				functor->middleName = "M";
				functor->lastName = "Jenkins";
				functor->race = "Martian";
				functor->sex = "Lots";
				functor->homeAddress1 = "1234 Martian Lane.";
				functor->homeAddress2; // Test blank
				functor->homeCity = "Martian colony 5582";
				functor->homeState = "Galaxon State";
				functor->homeProvince;
				functor->homeCountry = "Galaxon Country";
				functor->homeZipCode = "123456";
				functor->billingAddress1;
				functor->billingAddress2;
				functor->billingCity;
				functor->billingState;
				functor->billingProvince;
				functor->billingCountry;
				functor->billingZipCode;
				functor->emailAddress = "rakkar@jenkinssoftware.com";
				functor->password = "123456";
				functor->passwordRecoveryQuestion = "What is your mom's occupation?"; // Test ' in a string
				functor->passwordRecoveryAnswer = "Unemployed";
				functor->caption1 = "caption1";
				functor->caption2;
				functor->caption3 = "%s"; // Test formatting chars
				functor->dateOfBirth = "01/01/0001";
				functor->accountNumber;
				functor->creditCardNumber;
				functor->creditCardExpiration;
				functor->creditCardCVV;
				functor->adminLevel = "God";
				functor->permissions = "Omniscient | MORE";
				functor->accountBalance;
				functor->sourceIPAddress = "127.0.0.1";
				functor->binaryData = CreateUser_PostgreSQLImpl::AllocBytes(3);
				functor->binaryData[0]='B';
				functor->binaryData[1]='D';
				functor->binaryData[2]=0;
				functor->binaryDataLength = 3;

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='d')
			{
				GetUser_PostgreSQLImpl *functor = GetUser_PostgreSQLImpl::Alloc();
				printf("Gets info on a user by ID (faster) or handle (slower).\n");

				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=true;
					functor->id.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=false;
					functor->id.handle=inputStr;
				}

				functor->getCCInfo=true;
				functor->getBinaryData=true;
				functor->getPersonalInfo=true;
				functor->getEmailAddr=true;
				functor->getPassword=true;

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='e')
			{
				UpdateUser_PostgreSQLImpl *functor = UpdateUser_PostgreSQLImpl::Alloc();
				printf("Updates database info for part of all of a user.\nSpecify user by ID (faster) or handle (slower).\n");

				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=true;
					functor->id.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=false;
					functor->id.handle=inputStr;
				}

				functor->updateCCInfo=true;
				functor->updateBinaryData=true;
				functor->updatePersonalInfo=true;
				functor->updateEmailAddr=true;
				functor->updatePassword=true;
				functor->updateCaptions=true;
				functor->updateOtherInfo=true;

				// Put in test data for everything else, too many to ask the user
				functor->input.firstName = "Kevin (updated)";;
				functor->input.middleName = "M (updated)";
				functor->input.lastName = "Jenkins (updated)";
				functor->input.race = "Martian (updated)";
				functor->input.sex = "Lots (updated)";
				functor->input.homeAddress1 = "1234 Martian Lane. (updated)";
				functor->input.homeCity = "Martian colony 5582 (updated)";
				functor->input.homeState = "Galaxon State (updated)";
				functor->input.homeCountry = "Galaxon Country (updated)";
				functor->input.homeZipCode = "123456 (updated)";
				functor->input.emailAddress = "rakkar@jenkinssoftware.com (updated)";
				functor->input.password = "123456 (updated)";
				functor->input.passwordRecoveryQuestion = "What is your mom's occupation? (updated)"; // Test ' in a string
				functor->input.passwordRecoveryAnswer = "Unemployed (updated)";
				functor->input.caption1 = "caption1 (updated)";
				functor->input.caption3 = "%s (updated)"; // Test formatting chars
				functor->input.dateOfBirth = "01/01/0001 (updated)";
				functor->input.adminLevel = "God (updated)";
				functor->input.permissions = "Omniscient | MORE (updated)";
				functor->input.sourceIPAddress = "127.0.0.1 (updated)";
				functor->input.binaryData = UpdateUser_PostgreSQLImpl::AllocBytes(3);
				functor->input.binaryData[0]='U';
				functor->input.binaryData[1]='P';
				functor->input.binaryData[2]=0;
				functor->input.binaryDataLength = 3;

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='f')
			{
				AddAccountNote_PostgreSQLImpl *functor = AddAccountNote_PostgreSQLImpl::Alloc();
				printf("Adds a note to a user's account.\nSpecify user by ID (faster) or handle (slower).\n");

				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=true;
					functor->id.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=false;
					functor->id.handle=inputStr;
				}

				functor->writeModeratorId=true;
				functor->moderatorId=1234;
				functor->moderatorUsername="moderator username";
				functor->type="account note type";
				functor->subject="account subject";
				functor->body="account body";

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='g')
			{
				GetAccountNotes_PostgreSQLImpl *functor = GetAccountNotes_PostgreSQLImpl::Alloc();
				printf("Gets all notes for a user.\nSpecify user by ID (faster) or handle (slower).\n");

				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=true;
					functor->id.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=false;
					functor->id.handle=inputStr;
				}
				printf("Ascending sort? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
					functor->ascendingSort=true;
				else
					functor->ascendingSort=false;

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='h')
			{
				AddFriend_PostgreSQLImpl *functor = AddFriend_PostgreSQLImpl::Alloc();
				printf("Adds a friend for a user.\nSpecify user by ID (faster) or handle (slower).\n");

				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=true;
					functor->id.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=false;
					functor->id.handle=inputStr;
				}
				

				printf("Enter friend ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter friend ID (integer): ");
					gets(inputStr);
					functor->friendId.hasDatabaseRowId=true;
					functor->friendId.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter friend handle: ");
					gets(inputStr);
					functor->friendId.hasDatabaseRowId=false;
					functor->friendId.handle=inputStr;
				}

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='i')
			{
				RemoveFriend_PostgreSQLImpl *functor = RemoveFriend_PostgreSQLImpl::Alloc();
				printf("Removes a friend for a user.\nSpecify user by ID (faster) or handle (slower).\n");

				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->removeFriendInput.id.hasDatabaseRowId=true;
					functor->removeFriendInput.id.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->removeFriendInput.id.hasDatabaseRowId=false;
					functor->removeFriendInput.id.handle=inputStr;
				}


				printf("Enter friend ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter friend ID (integer): ");
					gets(inputStr);
					functor->removeFriendInput.friendId.hasDatabaseRowId=true;
					functor->removeFriendInput.friendId.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter friend handle: ");
					gets(inputStr);
					functor->removeFriendInput.friendId.hasDatabaseRowId=false;
					functor->removeFriendInput.friendId.handle=inputStr;
				}

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='j')
			{
				GetFriends_PostgreSQLImpl *functor = GetFriends_PostgreSQLImpl::Alloc();
				printf("Gets the list of friends for a user.\nSpecify user by ID (faster) or handle (slower).\n");

				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=true;
					functor->id.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=false;
					functor->id.handle=inputStr;
				}
				printf("Ascending sort? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
					functor->ascendingSort=true;
				else
					functor->ascendingSort=false;

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='k')
			{
				AddToIgnoreList_PostgreSQLImpl *functor = AddToIgnoreList_PostgreSQLImpl::Alloc();
				printf("Adds a user to the ignore list of another user.\nSpecify user by ID (faster) or handle (slower).\n");

				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=true;
					functor->id.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=false;
					functor->id.handle=inputStr;
				}

				printf("Enter ignored user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter ignored user ID (integer): ");
					gets(inputStr);
					functor->ignoredUser.hasDatabaseRowId=true;
					functor->ignoredUser.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter ignored user handle: ");
					gets(inputStr);
					functor->ignoredUser.hasDatabaseRowId=false;
					functor->ignoredUser.handle=inputStr;
				}

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='l')
			{
				RemoveFromIgnoreList_PostgreSQLImpl *functor = RemoveFromIgnoreList_PostgreSQLImpl::Alloc();
				printf("Stops ignoring a user.\nSpecify user by ID (faster) or handle (slower).\n");

				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=true;
					functor->id.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=false;
					functor->id.handle=inputStr;
				}


				printf("Enter friend ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter ignored user ID (integer): ");
					gets(inputStr);
					functor->ignoredUser.hasDatabaseRowId=true;
					functor->ignoredUser.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter ignored user handle: ");
					gets(inputStr);
					functor->ignoredUser.hasDatabaseRowId=false;
					functor->ignoredUser.handle=inputStr;
				}

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='m')
			{
				GetIgnoreList_PostgreSQLImpl *functor = GetIgnoreList_PostgreSQLImpl::Alloc();
				printf("Gets the list of ignored users for a user.\nSpecify user by ID (faster) or handle (slower).\n");

				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=true;
					functor->id.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=false;
					functor->id.handle=inputStr;
				}
				printf("Ascending sort? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
					functor->ascendingSort=true;
				else
					functor->ascendingSort=false;

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='o')
			{
				SendEmail_PostgreSQLImpl *functor = SendEmail_PostgreSQLImpl::Alloc();
				printf("Send email to recipient(s).\n");

				printf("Enter FROM: user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter FROM: user ID (integer): ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=true;
					functor->id.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter FROM: user handle: ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=false;
					functor->id.handle=inputStr;
				}

				while (1)
				{
					LobbyDBSpec::RowIdOrHandle uioh;

					printf("Enter TO: recipient ID? (Enter to stop) (y/n): ");
					gets(inputStr);
					if (inputStr[0]=='y')
					{
						printf("Enter TO: user ID (integer): ");
						gets(inputStr);
						uioh.hasDatabaseRowId=true;
						uioh.databaseRowId=atoi(inputStr);
					}
					else if (inputStr[0]==0)
						break;
					else
					{
						printf("Enter TO: user handle: ");
						gets(inputStr);
						uioh.hasDatabaseRowId=false;
						uioh.handle=inputStr;
					}
					functor->to.Insert(uioh);
				}

				printf("Enter subject: ");
				gets(inputStr);
				functor->subject=inputStr;

				printf("Enter body: ");
				gets(inputStr);
				functor->body=inputStr;

				// Test attachments
				functor->attachment=SendEmail_PostgreSQLImpl::AllocBytes(2);
				functor->attachmentLength=2;
				functor->attachment[0]='Z';
				functor->attachment[1]=0;

				// Emails can be tagged with a status number. Can use for read, priority, etc.
				functor->initialSenderStatus=63;
				functor->initialRecipientStatus=64;

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='p')
			{
				GetEmails_PostgreSQLImpl *functor = GetEmails_PostgreSQLImpl::Alloc();
				printf("Get emails for a user.\n");

				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=true;
					functor->id.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=false;
					functor->id.handle=inputStr;
				}

				printf("Check inbox? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					functor->inbox=true;
				}
				else
				{
					// Check sent items
					functor->inbox=false;

				}

				printf("Ascending sort? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
					functor->ascendingSort=true;
				else
					functor->ascendingSort=false;

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='q')
			{
				DeleteEmail_PostgreSQLImpl *functor = DeleteEmail_PostgreSQLImpl::Alloc();
				printf("Delete email.\n");

				printf("Enter email message ID (integer)\n");
				printf("This is found by calling GetEmails and checking emails.emailMessageID\n");
				gets(inputStr);
				functor->emailMessageID=atoi(inputStr);

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);

			}
			else if (ch=='r')
			{
				UpdateEmailStatus_PostgreSQLImpl *functor = UpdateEmailStatus_PostgreSQLImpl::Alloc();
				printf("Update user-defined status flags for an email.\n");

				printf("Enter email message ID (integer)\n");
				printf("This is found by calling GetEmails and checking emails.emailMessageID\n");
				gets(inputStr);
				functor->emailMessageID=atoi(inputStr);

				printf("Enter new status: ");
				gets(inputStr);
				functor->status=atoi(inputStr);

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='s')
			{
				GetHandleFromUserId_PostgreSQLImpl *functor = GetHandleFromUserId_PostgreSQLImpl::Alloc();
				printf("Gets the handle given a user Id.\n");
				printf("Enter the user Id (integer): ");
				gets(inputStr);
				functor->userId=atoi(inputStr);
				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='t')
			{
				GetUserIdFromHandle_PostgreSQLImpl *functor = GetUserIdFromHandle_PostgreSQLImpl::Alloc();
				printf("Gets the user Id given a handle.\n");
				printf("Enter the handle (string): ");
				gets(inputStr);
				functor->handle=inputStr;
				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='u')
			{
				DeleteUser_PostgreSQLImpl *functor = DeleteUser_PostgreSQLImpl::Alloc();
				printf("Deletes a user.\n");
				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=true;
					functor->id.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=false;
					functor->id.handle=inputStr;
				}

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='v')
			{
				ChangeUserHandle_PostgreSQLImpl *functor = ChangeUserHandle_PostgreSQLImpl::Alloc();
				printf("Changes a user's handle.\n");
				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=true;
					functor->id.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=false;
					functor->id.handle=inputStr;
				}

				printf("Enter new handle: ");
				gets(inputStr);
				functor->newHandle=inputStr;

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='w')
			{
				AddDisallowedHandle_PostgreSQLImpl *functor = AddDisallowedHandle_PostgreSQLImpl::Alloc();
				printf("Add a disallowed handle.\n");
				printf("Enter handle: ");
				gets(inputStr);
				functor->handle=inputStr;
				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='x')
			{
				RemoveDisallowedHandle_PostgreSQLImpl *functor = RemoveDisallowedHandle_PostgreSQLImpl::Alloc();
				printf("Removes a disallowed handle.\n");
				printf("Enter handle: ");
				gets(inputStr);
				functor->handle=inputStr;
				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='y')
			{
				IsDisallowedHandle_PostgreSQLImpl *functor = IsDisallowedHandle_PostgreSQLImpl::Alloc();
				printf("Checks if a handle is disallowed.\n");
				printf("Enter handle: ");
				gets(inputStr);
				functor->handle=inputStr;
				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='z')
			{
				AddToActionHistory_PostgreSQLImpl *functor = AddToActionHistory_PostgreSQLImpl::Alloc();
				printf("Records an action this user has taken.\n");

				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=true;
					functor->id.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=false;
					functor->id.handle=inputStr;
				}

				printf("Enter actionTaken: ");
				gets(inputStr);
				functor->actionTaken=inputStr;

				printf("Enter description: ");
				gets(inputStr);
				functor->description=inputStr;

				functor->actionTime; // Leave blank
				functor->sourceIpAddress="127.0.0.1";

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='0')
			{
				GetActionHistory_PostgreSQLImpl *functor = GetActionHistory_PostgreSQLImpl::Alloc();
				printf("Gets the actions this user has taken.\n");

				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=true;
					functor->id.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->id.hasDatabaseRowId=false;
					functor->id.handle=inputStr;
				}

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='1')
			{
				UpdateClanMember_PostgreSQLImpl *functor = UpdateClanMember_PostgreSQLImpl::Alloc();
				printf("Update or add a clan member.\n");

				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->userId.hasDatabaseRowId=true;
					functor->userId.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->userId.hasDatabaseRowId=false;
					functor->userId.handle=inputStr;
				}

				printf("Enter clan ID (0 to add new): ");
				gets(inputStr);
				functor->clanId.databaseRowId=atoi(inputStr);
				functor->clanId.hasDatabaseRowId=true;

				printf("Enter operation:\n(1) Member\n(2) Join request\n(3) Join invite\n(4) Blacklist\n");
				gets(inputStr);
				if (inputStr[0]=='1')
					functor->mEStatus1=LobbyDBSpec::CLAN_MEMBER_STATUS_MEMBER;
				else if (inputStr[0]=='2')
					functor->mEStatus1=LobbyDBSpec::CLAN_MEMBER_STATUS_REQUESTED_TO_JOIN;
				else if (inputStr[0]=='3')
					functor->mEStatus1=LobbyDBSpec::CLAN_MEMBER_STATUS_INVITED_TO_JOIN;
				else
					functor->mEStatus1=LobbyDBSpec::CLAN_MEMBER_STATUS_BLACKLISTED;

				functor->integers[0]=1;
				functor->integers[1]=2;
				functor->integers[2]=3;
				functor->integers[3]=4;

				functor->floats[0]=1;
				functor->floats[1]=2;	
				functor->floats[2]=3;
				functor->floats[3]=4;

				printf("Enter member description 1: ");
				gets(inputStr);
				functor->descriptions[0]=inputStr;

				printf("Enter member description 2: ");
				gets(inputStr);
				functor->descriptions[1]=inputStr;

				functor->binaryData=0;
				
				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='2')
			{
				UpdateClan_PostgreSQLImpl *functor = UpdateClan_PostgreSQLImpl::Alloc();
				printf("Update an existing clan, any values but the name.\n");

				printf("Enter clan ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter clan ID (integer): ");
					gets(inputStr);
					functor->clanId.hasDatabaseRowId=true;
					functor->clanId.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter clan handle: ");
					gets(inputStr);
					functor->clanId.hasDatabaseRowId=false;
					functor->clanId.handle=inputStr;
				}

				functor->SetUpdateInts(true);
				functor->integers[0]=111;
				functor->integers[1]=112;
				functor->integers[2]=113;
				functor->integers[3]=114;
				functor->SetUpdateFloats(false);
				functor->updateDescriptions[0]=true;
				functor->descriptions[0]="Updated clan description 1";
				functor->updateDescriptions[1]=false;
				functor->updateBinaryData=true;
				functor->binaryData=0;
				functor->updateRequiresInvitationsToJoin=false;

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='3')
			{
				CreateClan_PostgreSQLImpl *functor = CreateClan_PostgreSQLImpl::Alloc();
				printf("Create a new clan.\n");

				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->leaderData.userId.hasDatabaseRowId=true;
					functor->leaderData.userId.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->leaderData.userId.hasDatabaseRowId=false;
					functor->leaderData.userId.handle=inputStr;
				}

				printf("Enter clan name: ");
				gets(inputStr);
				functor->initialClanData.handle = inputStr;

				functor->initialClanData.integers[0] = 1;
				functor->initialClanData.integers[1] = 2;
				functor->initialClanData.integers[2] = 3;
				functor->initialClanData.integers[3] = 4;

				functor->initialClanData.floats[0] = 11;
				functor->initialClanData.floats[1] = 12;
				functor->initialClanData.floats[2] = 13;
				functor->initialClanData.floats[3] = 14;

				functor->initialClanData.descriptions[0] = "CreateClan_PostgreSQLImpl Description 1";
				functor->initialClanData.descriptions[1] = "CreateClan_PostgreSQLImpl Description 2";

				functor->initialClanData.binaryData=0;

				functor->initialClanData.requiresInvitationsToJoin=true;

				functor->leaderData.integers[0] = 21;
				functor->leaderData.integers[1] = 22;
				functor->leaderData.integers[2] = 23;
				functor->leaderData.integers[3] = 24;

				functor->leaderData.floats[0] = 31;
				functor->leaderData.floats[1] = 32;
				functor->leaderData.floats[2] = 33;
				functor->leaderData.floats[3] = 34;

				functor->leaderData.descriptions[0]="Big clan boss";
				functor->leaderData.binaryData=0;
				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='4')
			{
				ChangeClanHandle_PostgreSQLImpl *functor = ChangeClanHandle_PostgreSQLImpl::Alloc();
				printf("Change your clan's handle.\n");

				printf("Enter clan row ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter clan ID (integer): ");
					gets(inputStr);
					functor->clanId.hasDatabaseRowId=true;
					functor->clanId.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter clan handle: ");
					gets(inputStr);
					functor->clanId.hasDatabaseRowId=false;
					functor->clanId.handle=inputStr;
				}

				printf("Enter new handle: ");
				gets(inputStr);
				functor->newHandle=inputStr;

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='5')
			{
				DeleteClan_PostgreSQLImpl *functor = DeleteClan_PostgreSQLImpl::Alloc();
				printf("Delete a clan.\n");

				printf("Enter clan row ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter clan ID (integer): ");
					gets(inputStr);
					functor->clanId.hasDatabaseRowId=true;
					functor->clanId.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter clan handle: ");
					gets(inputStr);
					functor->clanId.hasDatabaseRowId=false;
					functor->clanId.handle=inputStr;
				}
				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='6')
			{
				GetClans_PostgreSQLImpl *functor = GetClans_PostgreSQLImpl::Alloc();
				printf("Get all clans and clan members for a user.\n");

				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->userId.hasDatabaseRowId=true;
					functor->userId.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->userId.hasDatabaseRowId=false;
					functor->userId.handle=inputStr;
				}

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='7')
			{
				RemoveFromClan_PostgreSQLImpl *functor = RemoveFromClan_PostgreSQLImpl::Alloc();
				printf("Remove member from clan.\n");

				printf("Enter user ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->userId.hasDatabaseRowId=true;
					functor->userId.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->userId.hasDatabaseRowId=false;
					functor->userId.handle=inputStr;
				}

				printf("Enter clan row ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter clan ID (integer): ");
					gets(inputStr);
					functor->clanId.hasDatabaseRowId=true;
					functor->clanId.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter clan handle: ");
					gets(inputStr);
					functor->clanId.hasDatabaseRowId=false;
					functor->clanId.handle=inputStr;
				}

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
			else if (ch=='8')
			{
				AddToClanBoard_PostgreSQLImpl *functor = AddToClanBoard_PostgreSQLImpl::Alloc();
				printf("Write to clan bulletin board.\n");

				printf("Enter user row ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter user ID (integer): ");
					gets(inputStr);
					functor->userId.hasDatabaseRowId=true;
					functor->userId.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter user handle: ");
					gets(inputStr);
					functor->userId.hasDatabaseRowId=false;
					functor->userId.handle=inputStr;
				}

				printf("Enter clan row ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter clan ID (integer): ");
					gets(inputStr);
					functor->clanId.hasDatabaseRowId=true;
					functor->clanId.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter clan handle: ");
					gets(inputStr);
					functor->clanId.hasDatabaseRowId=false;
					functor->clanId.handle=inputStr;
				}

				printf("Enter subject: ");
				gets(inputStr);
				functor->subject=inputStr;

				printf("Enter body: ");
				gets(inputStr);
				functor->body=inputStr;

				functor->integers[0]=0;
				functor->integers[1]=0;
				functor->integers[2]=0;
				functor->integers[3]=0;
				functor->floats[0]=0;
				functor->floats[1]=0;
				functor->floats[2]=0;
				functor->floats[3]=0;

				functor->binaryData=0;
				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);

			}
			else if (ch=='9')
			{
				RemoveFromClanBoard_PostgreSQLImpl *functor = RemoveFromClanBoard_PostgreSQLImpl::Alloc();
				printf("Remove from clan bulletin board.\n");

				printf("Enter post ID: ");
				gets(inputStr);
				functor->postId=atoi(inputStr);

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);

			}
			else if (ch=='!')
			{
				GetClanBoard_PostgreSQLImpl *functor = GetClanBoard_PostgreSQLImpl::Alloc();
				printf("Get clan bulletin board.\n");

				printf("Enter clan row ID? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
				{
					printf("Enter clan ID (integer): ");
					gets(inputStr);
					functor->clanId.hasDatabaseRowId=true;
					functor->clanId.databaseRowId=atoi(inputStr);
				}
				else
				{
					printf("Enter clan handle: ");
					gets(inputStr);
					functor->clanId.hasDatabaseRowId=false;
					functor->clanId.handle=inputStr;
				}

				printf("Ascending sort? (y/n): ");
				gets(inputStr);
				if (inputStr[0]=='y')
					functor->ascendingSort=true;
				else
					functor->ascendingSort=false;

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				lobbyServer.PushFunctor(functor,0);
			}
		}

		// Causes Functor::HandleResult calls. If this is forgotten you won't get processing result calls.
		ft.CallResultHandlers();

		Sleep(30);
	}
}
