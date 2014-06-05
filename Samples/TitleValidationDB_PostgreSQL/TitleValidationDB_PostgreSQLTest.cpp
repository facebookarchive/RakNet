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
#include "TitleValidationDB_PostgreSQL.h"
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
class DBResultHandler : public TitleValidationDBCBInterface
{
	virtual void AddTitle_CB(AddTitle_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("AddTitle_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("AddTitle_CB call DB failure:\n");
			printf("%s", callResult->titleValidationServer->GetLastError());
		}
		else
		{
			printf("AddTitle_CB success:\n");
		}

		printf("\n");
	}
	virtual void GetTitles_CB(GetTitles_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("GetTitles_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("GetTitles_CB call DB failure:\n");
			printf("%s", callResult->titleValidationServer->GetLastError());
		}
		else
		{
			printf("GetTitles_CB success:\n");

			unsigned i;
			for (i=0; i < callResult->titles.Size(); i++)
			{
				printf("%i. titleId=%i\n", i+1, callResult->titles[i]->titleId);
				printf("%i. titleName=%s\n", i+1, callResult->titles[i]->titleName.C_String());
				printf("%i. titlePasswordLength=%i\n", i+1, callResult->titles[i]->titlePasswordLength);
				printf("%i. allowClientAccountCreation=%i\n", i+1, callResult->titles[i]->allowClientAccountCreation);
				printf("%i. lobbyIsGameIndependent=%i\n", i+1, callResult->titles[i]->lobbyIsGameIndependent);
				printf("%i. requireUserKeyToLogon=%i\n", i+1, callResult->titles[i]->requireUserKeyToLogon);
				printf("%i. defaultAllowUpdateHandle=%i\n", i+1, callResult->titles[i]->defaultAllowUpdateHandle);
				printf("%i. defaultAllowUpdateCCInfo=%i\n", i+1, callResult->titles[i]->defaultAllowUpdateCCInfo);
				printf("%i. defaultAllowUpdateAccountNumber=%i\n", i+1, callResult->titles[i]->defaultAllowUpdateAccountNumber);
				printf("%i. defaultAllowUpdateAdminLevel=%i\n", i+1, callResult->titles[i]->defaultAllowUpdateAdminLevel);
				printf("%i. defaultAllowUpdateAccountBalance=%i\n", i+1, callResult->titles[i]->defaultAllowUpdateAccountBalance);
				printf("%i. defaultAllowClientsUploadActionHistory=%i\n", i+1, callResult->titles[i]->defaultAllowClientsUploadActionHistory);
				printf("%i. defaultPermissionsStr=%s\n", i+1, callResult->titles[i]->defaultPermissionsStr.C_String());
				printf("%i. creationDate=%s\n", i+1, EpochTimeToString(callResult->titles[i]->creationDate));
			}
		}

		printf("\n");
	}
	virtual void UpdateUserKey_CB(UpdateUserKey_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("UpdateUserKey_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("UpdateUserKey_CB call DB failure:\n");
			printf("%s", callResult->titleValidationServer->GetLastError());
		}
		else
		{
			printf("UpdateUserKey_CB success:\n");
			printf("[out] userKeyId=%i\n", callResult->userKeyId);
		}

		printf("\n");
	}
	virtual void GetUserKeys_CB(GetUserKeys_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("GetUserKeys_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("GetUserKeys_CB call DB failure:\n");
			printf("%s", callResult->titleValidationServer->GetLastError());
		}
		else
		{
			printf("GetUserKeys_CB success:\n");

			unsigned i;
			for (i=0; i < callResult->keys.Size(); i++)
			{
				printf("%i. userKeyId=%i\n", i+1, callResult->keys[i]->userKeyId);
				printf("%i. titleId=%i\n", i+1, callResult->keys[i]->titleId);
				printf("%i. keyPasswordLength=%i\n", i+1, callResult->keys[i]->keyPasswordLength);
				printf("%i. userId=%i\n", i+1, callResult->keys[i]->userId);
				printf("%i. userIP=%s\n", i+1, callResult->keys[i]->userIP.C_String());
				printf("%i. numRegistrations=%i\n", i+1, callResult->keys[i]->numRegistrations);
				printf("%i. invalidKey=%i\n", i+1, callResult->keys[i]->invalidKey);
				printf("%i. invalidKeyReason=%s\n", i+1, callResult->keys[i]->invalidKeyReason.C_String());
				printf("%i. binaryDataLength=%i\n", i+1, callResult->keys[i]->binaryDataLength);
				printf("%i. creationDate=%s\n", i+1, EpochTimeToString(callResult->keys[i]->creationDate));
			}
		}

		printf("\n");
	}
	virtual void ValidateUserKey_CB(ValidateUserKey_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("ValidateUserKey_CB call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("ValidateUserKey_CB call DB failure:\n");
			printf("%s", callResult->titleValidationServer->GetLastError());
		}
		else if (callResult->successful==0)
		{
			printf("ValidateUserKey_CB success\n");
		}
		else
			printf("ValidateUserKey_CB failure. Reason=%s\n", callResult->invalidKeyReason.C_String());
	}
};

void main(int argc, char **argv)
{
	printf("A sample on using the provided implementation\nof the Title Validation specification based on PostgreSQL\n");
	printf("Unlike the other samples, this is a server-only\nprocess so does not involve networking with RakNet.\n");
	printf("The goal of this class is to allow you to lookup\ngame titles based on a password, and verify CD keys.\n");
	printf("Difficulty: Intermediate\n\n");

	// A function thread is class that spawns a thread that operates on functors.
	// A functor is an instance of a class that has two pre-defined functions: One to perform processing, another to get the result.
	// One per application is enough to not block
	RakNet::FunctionThread ft;

	// Start one thread. Starting more than one may be advantageous for multi-core processors. However, in this scenario we are
	// blocking on database calls rather than processing.
	ft.StartThreads(1);

	// The real functionality of the TitleValidationDB class is contained in the functors defined in TitleValidationDB_PostgreSQL.h/.cpp.
	// However, this class contains some utility functions and members, such as the functionThread and a pointer to
	// the postgreSQL interface.
	TitleValidation_PostgreSQL titleValidationDB;

	// The class internally uses a functionThread so that database queries can happen asynchronously,
	// as opposed to blocking and slowing down the game.
	// If you don't assign one it will create one automatically and start it with one thread.
	titleValidationDB.AssignFunctionThread(&ft);

	// The default implementation of the functors pass Functor::HandleResult through to a callback, instantiated here.
	// Alternatively, you could derive from the *_PostgreSQLImpl functors found in TitleValidationDB_PostgreSQL.h/.cpp
	// and override the behavior of Functor::HandleResult
	DBResultHandler resultHandler;
	titleValidationDB.AssignCallback(&resultHandler);

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
	if (titleValidationDB.Connect(connectionString)==false)
	{
		printf("Database connection failed.\n");
		return;
	}
	printf("Database connection succeeded.\n");
	printf("(A) Drop tables\n"
		"(B) Create tables\n"
		"(C) Add title.\n"
		"(D) Get titles.\n"
		"(E) Update user key.\n"
		"(F) Get user keys.\n"
		"(F) Validate user key.\n"
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
				if (titleValidationDB.DestroyTitleValidationTables()==false)
					printf("%s", titleValidationDB.GetLastError());
				else
					printf("Tables dropped.\n");
			}
			else if (ch=='b')
			{
				if (titleValidationDB.CreateTitleValidationTables()==false)
					printf("%s", titleValidationDB.GetLastError());
				else
					printf("Tables created.\n");
			}
			else if (ch=='c')
			{
				// We could do new and delete, but this ensures the class is allocated and deallocated in the same DLL, should we use one.
				// By default deallocation takes place in HandleResult()
				AddTitle_PostgreSQLImpl *functor = AddTitle_PostgreSQLImpl::Alloc();
				printf("Adds a title to the database\n");

				printf("Enter title name: ");
				gets(inputStr);
				functor->titleName = inputStr;
				printf("Enter title password (binary): ");
				gets(inputStr);
				functor->titlePassword = AddTitle_PostgreSQLImpl::AllocBytes((int) strlen(inputStr)+1);
				functor->titlePasswordLength = (int) strlen(inputStr)+1;
				memcpy(functor->titlePassword, inputStr, functor->titlePasswordLength);
				functor->allowClientAccountCreation=true;
				functor->lobbyIsGameIndependent=true;
				functor->requireUserKeyToLogon=false;
				functor->allowClientAccountCreation=true;
				functor->lobbyIsGameIndependent=true;
				functor->defaultAllowUpdateHandle=true;
				functor->defaultAllowUpdateCCInfo=true;
				functor->defaultAllowUpdateAccountNumber=true;
				functor->defaultAllowUpdateAdminLevel=true;
				functor->defaultAllowUpdateAccountBalance=true;
				functor->defaultAllowClientsUploadActionHistory=true;

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				titleValidationDB.PushFunctor(functor);
			}
			else if (ch=='d')
			{
				GetTitles_PostgreSQLImpl *functor = GetTitles_PostgreSQLImpl::Alloc();
				printf("Getting previously added titles\n");
				titleValidationDB.PushFunctor(functor);
			}
			else if (ch=='e')
			{
				UpdateUserKey_PostgreSQLImpl *functor = UpdateUserKey_PostgreSQLImpl::Alloc();
				printf("Update/Inserting a user key\n");

				printf("Insert a new key? (y/n): ");
				gets(inputStr);
				functor->userKeyIsSet = (inputStr[0]=='n');
				if (functor->userKeyIsSet)
				{
					printf("Enter existing key (integer): ");
					gets(inputStr);
					functor->userKeyId=atoi(inputStr);
				}

				printf("Enter title Id (integer, required): ");
				gets(inputStr);
				functor->titleId=atoi(inputStr);

				printf("Enter key password (binary): ");
				gets(inputStr);
				functor->keyPassword = UpdateUserKey_PostgreSQLImpl::AllocBytes((int) strlen(inputStr)+1);
				functor->keyPasswordLength = (int) strlen(inputStr)+1;
				memcpy(functor->keyPassword, inputStr, functor->keyPasswordLength);


				printf("Enter userId (integer, optional): ");
				gets(inputStr);
				functor->userId=atoi(inputStr);

				functor->userIP="127.0.0.1";

				functor->numRegistrations=0;
				functor->invalidKey=false;

				titleValidationDB.PushFunctor(functor);				
			}
			else if (ch=='f')
			{
				GetUserKeys_PostgreSQLImpl *functor = GetUserKeys_PostgreSQLImpl::Alloc();
				printf("Getting previously added user keys\n");
				titleValidationDB.PushFunctor(functor);
			}
			else if (ch=='g')
			{
				ValidateUserKey_PostgreSQLImpl *functor = ValidateUserKey_PostgreSQLImpl::Alloc();
				printf("Validating key\n");
				printf("Enter key password (binary): ");
				gets(inputStr);
				functor->keyPassword = UpdateUserKey_PostgreSQLImpl::AllocBytes((int) strlen(inputStr)+1);
				functor->keyPasswordLength = (int) strlen(inputStr)+1;
				titleValidationDB.PushFunctor(functor);
			}
			
		}

		// Causes Functor::HandleResult calls. If this is forgotten you won't get processing result calls.
		ft.CallResultHandlers();

		Sleep(30);
	}
}
