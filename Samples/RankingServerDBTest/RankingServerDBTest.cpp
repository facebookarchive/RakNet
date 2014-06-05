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
#include "RankingServer_PostgreSQL.h"

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
class DBResultHandler : public RankingServerCBInterface
{
	virtual void SubmitMatch_CB(SubmitMatch_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("SubmitMatch call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("SubmitMatch call DB failure:\n");
			printf("%s", callResult->rankingServer->GetLastError());
		}
		else
		{
				printf("SubmitMatch Completed (No data is returned)");
		}

		printf("\n");
	}
	virtual void GetRatingForParticipant_CB(GetRatingForParticipant_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("GetRatingForParticipant call cancelled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("GetRatingForParticipant call DB failure:\n");
			printf("%s", callResult->rankingServer->GetLastError());
		}
		else
		{
			printf("GetRatingForParticipant result:\n");
			printf("[in] participantDbId.primaryKey=%i\n", callResult->participantDbId.primaryKey);
			printf("[in] participantDbId.secondaryKey=%i\n", callResult->participantDbId.secondaryKey);
			printf("[in] gameDbId.primaryKey=%i\n", callResult->gameDbId.primaryKey);
			printf("[in] gameDbId.secondaryKey=%i\n", callResult->gameDbId.secondaryKey);
			printf("[out] participantFound=%i\n", callResult->participantFound); // False means no such participant
			if (callResult->participantFound)
				printf("[out] rating=%f\n", callResult->rating);
		}

		printf("\n");
	}
	virtual void GetRatingForParticipants_CB(GetRatingForParticipants_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("GetRatingForParticipants call canceled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("GetRatingForParticipants call DB failure:\n");
			printf("%s", callResult->rankingServer->GetLastError());
		}
		else
		{
			printf("GetRatingForParticipants result:\n");
			printf("[in] gameDbId.primaryKey=%i\n", callResult->gameDbId.primaryKey);
			printf("[in] gameDbId.secondaryKey=%i\n", callResult->gameDbId.secondaryKey);
			printf("[out] %i participants found.\n", callResult->outputList.Size());
			unsigned i;
			for (i=0; i < callResult->outputList.Size(); i++)
			{
				printf("[out] %i. participantDbId.primaryKey=%i\n", i+1, callResult->outputList[i].participantDbId.primaryKey);
				printf("[out] %i. participantDbId.secondaryKey=%i\n", i+1, callResult->outputList[i].participantDbId.secondaryKey);
				printf("[out] %i. newRating (most recent rating)=%f\n", i+1, callResult->outputList[i].newRating);
			}
		}

		printf("\n");
	}
	virtual void GetHistoryForParticipant_CB(GetHistoryForParticipant_PostgreSQLImpl *callResult, bool wasCancelled, void *context)
	{
		if (wasCancelled)
			printf("GetHistoryForParticipant call cancelled:\n");
		else if (callResult->dbQuerySuccess==false)
		{
			printf("GetHistoryForParticipant call DB failure:\n");
			printf("%s", callResult->rankingServer->GetLastError());
		}
		else
		{
			printf("GetHistoryForParticipant result:\n");
			printf("[in] participantDbId.primaryKey=%i\n", callResult->participantDbId.primaryKey);
			printf("[in] participantDbId.secondaryKey=%i\n", callResult->participantDbId.secondaryKey);
			printf("[in] gameDbId.primaryKey=%i\n", callResult->gameDbId.primaryKey);
			printf("[in] gameDbId.secondaryKey=%i\n", callResult->gameDbId.secondaryKey);
			printf("[out] %i matches found for this participant.\n", callResult->matchHistoryList.Size());
			unsigned i;
			for (i=0; i < callResult->matchHistoryList.Size(); i++)
			{
				printf("[out] %i. participantBDbId.primaryKey (opponent) =%i\n", i+1, callResult->matchHistoryList[i]->participantBDbId.primaryKey);
				printf("[out] %i. participantBDbId.secondaryKey (opponent) =%i\n", i+1, callResult->matchHistoryList[i]->participantBDbId.secondaryKey);
				printf("[out] %i. participantAScore (us) =%f\n", i+1, callResult->matchHistoryList[i]->participantAScore);
				printf("[out] %i. participantBScore (opponent) =%f\n", i+1, callResult->matchHistoryList[i]->participantBScore);
				printf("[out] %i. participantAOldRating (us) =%f\n", i+1, callResult->matchHistoryList[i]->participantAOldRating);
				printf("[out] %i. participantANewRating (us) =%f\n", i+1, callResult->matchHistoryList[i]->participantANewRating);
				printf("[out] %i. participantBOldRating (opponent) =%f\n", i+1, callResult->matchHistoryList[i]->participantBOldRating);
				printf("[out] %i. participantBNewRating (opponent) =%f\n", i+1, callResult->matchHistoryList[i]->participantBNewRating);
				printf("[out] %i. Match Notes=%s\n", i+1, callResult->matchHistoryList[i]->matchNotes.C_String());
				printf("[out] %i. matchBinaryDataLength=%i\n", i+1, callResult->matchHistoryList[i]->matchBinaryDataLength);

				// Copied from the docs
				struct tm *newtime;
				newtime = _localtime64(& callResult->matchHistoryList[i]->matchTime);
				char buff[30];
				asctime_s( buff, sizeof(buff), newtime );
				printf("[out] %i. matchTime (converted) =\n%s\n", i+1, buff );
			}
		}

		printf("\n");
	}
};

void main(int argc, char **argv)
{
	printf("A sample on using the provided implementation\nof the Ranking Server specification based on PostgreSQL\n");
	printf("Unlike the other samples, this is a server-only\nprocess so does not involve networking with RakNet.\n");
	printf("The goal of this class is to allow you to save\nand retrieve a history of matches for a given participant and game ID\n");
	printf("Difficulty: Intermediate\n\n");

	// A function thread is class that spawns a thread that operates on functors.
	// A functor is an instance of a class that has two pre-defined functions: One to perform processing, another to get the result.
	// One per application is enough to not block
	RakNet::FunctionThread ft;

	// Start one thread. Starting more than one may be advantageous for multi-core processors. However, in this scenario we are
	// blocking on database calls rather than processing.
	ft.StartThreads(1);

	// The real functionality of the RankingServer is contained in the functors defined in RankingServer_PostgreSQL.h/.cpp.
	// However, this class contains some utility functions and members, such as the functionThread and a pointer to
	// the postgreSQL interface.
	RankingServer_PostgreSQL rankingServer;

	// RankingServer_PostgreSQL internally uses a functionThread so that database queries can happen asynchronously,
	// as opposed to blocking and slowing down the game.
	// If you don't assign one it will create one automatically and start it with one thread.
	rankingServer.AssignFunctionThread(&ft);

	// The default implementation of the functors pass Functor::HandleResult through to a callback, instantiated here.
	// Alternatively, you could derive from the *_PostgreSQLImpl functors found in RankingServer_PostgreSQL.h/.cpp
	// and override the behavior of Functor::HandleResult
	DBResultHandler resultHandler;
	rankingServer.AssignCallback(&resultHandler);

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
	if (rankingServer.Connect(connectionString)==false)
	{
		printf("Database connection failed.\n");
		return;
	}
	printf("Database connection suceeded.\n");
	printf("(D)rop tables\n"
		"(C)reate tables.\n"
		"(S)ubmit a match\n"
		"Get (R)ating for one participant.\n"
		"Get ratings for (A)ll participants\n"
		"Get match (H)istory for one participant\n"
		"(Q)uit\n");

	char inputStr[512];
	char ch;
	while (1)
	{
		if (kbhit())
		{
			ch=getch();
			if (ch=='q')
				break;
			else if (ch=='c')
			{
				if (rankingServer.CreateRankingServerTables()==false)
					printf("%s", rankingServer.GetLastError());
				else
					printf("Tables created.\n");
			}
			else if (ch=='d')
			{
                if (rankingServer.DestroyRankingServerTables()==false)
					printf("%s", rankingServer.GetLastError());
				else
					printf("Tables dropped.\n");
			}
			else if (ch=='s')
			{
				// We could do new and delete, but this ensures the class is allocated and deallocated in the same DLL, should we use one.
				// By default deallocation takes place in HandleResult()
				SubmitMatch_PostgreSQLImpl *functor = SubmitMatch_PostgreSQLImpl::Alloc();

				printf("Submit a match\n");

				/// Every participant is represented by a pair of numbers.
				/// The primary ID can be used to reference a database primary key referring to another table holding all participant
				/// The secondary ID can be used to indicate type
				/// such as indicating the type of participant (player, squad, guild, clan, etc).
				printf("Enter the primary ID of participant A (int): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "0");
				functor->participantADbId.primaryKey = atoi(inputStr);

				printf("Enter the secondary ID of participant A (int): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "1");
				functor->participantADbId.secondaryKey = atoi(inputStr);

				printf("Enter the primary ID of participant B (int): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "2");
				functor->participantBDbId.primaryKey = atoi(inputStr);

				printf("Enter the secondary ID of participant B (int): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "3");
				functor->participantBDbId.secondaryKey = atoi(inputStr);

				// Score is tracked in case you want to check the database for cheaters, or to modify ratings, etc.
				// If you don't care about it, just use any number.
				printf("Enter the score of participant A (float): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "10.0");
				functor->participantAScore = (float) atof(inputStr);

				printf("Enter the score of participant B (float): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "21.0");
				functor->participantBScore = (float) atof(inputStr);

				// The rating of the player before the match started. Elo.h/.cpp is provided as one way to calculate ratings
				printf("Enter the pre-match rating of participant A (float): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "12.0");
				functor->participantAOldRating = (float) atof(inputStr);

				printf("Enter the post-match rating of participant A (float): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "5.0");
				functor->participantANewRating = (float) atof(inputStr);

				printf("Enter the pre-match rating of participant B (float): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "20.0");
				functor->participantBOldRating = (float) atof(inputStr);

				printf("Enter the post-match rating of participant B (float): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "30.0");
				functor->participantBNewRating = (float) atof(inputStr);

				/// Games are identified by a pair of integers the same way participants are
				/// The primary key could be the game title (such as Asteroids) while the secondary key could
				/// Indicate a submode (such as cooperative or deathmatch)
				printf("Enter the primary ID of the game (int): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "12");
				functor->gameDbId.primaryKey = atoi(inputStr);

				printf("Enter the secondary ID of the game (int): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "13");
				functor->gameDbId.secondaryKey = atoi(inputStr);

				/// A string is stored with each match.
				printf("Enter the match notes: ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "Default Match Notes");
				functor->matchNotes=inputStr;

				printf("Store test binary data? (y/n): ");
				gets(inputStr);
				if (inputStr[0]!='n')
				{
					const char * data = "Default Match Binary Data";
					functor->matchBinaryDataLength=(int) strlen(data)+1;
					// As before, calling a function in the external code in case it was built to a DLL
					functor->matchBinaryData = SubmitMatch_PostgreSQLImpl::AllocBytes(functor->matchBinaryDataLength);
					memcpy(functor->matchBinaryData, data, functor->matchBinaryDataLength);
				}

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				rankingServer.PushFunctor(functor);
			}
			else if (ch=='r')
			{
				// We could do new and delete, but this ensures the class is allocated and deallocated in the same DLL, should we use one.
				// By default deallocation takes place in HandleResult()
				GetRatingForParticipant_PostgreSQLImpl *functor = GetRatingForParticipant_PostgreSQLImpl::Alloc();

				printf("Get the most recent rating for a participant\n");

				printf("Enter the primary ID of participant A (int): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "0");
				functor->participantDbId.primaryKey = atoi(inputStr);

				printf("Enter the secondary ID of participant A (int): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "1");
				functor->participantDbId.secondaryKey = atoi(inputStr);

				printf("Enter the primary ID of the game (int): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "12");
				functor->gameDbId.primaryKey = atoi(inputStr);

				printf("Enter the secondary ID of the game (int): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "13");
				functor->gameDbId.secondaryKey = atoi(inputStr);

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				rankingServer.PushFunctor(functor);

			}
			else if (ch=='a')
			{
				// We could do new and delete, but this ensures the class is allocated and deallocated in the same DLL, should we use one.
				// By default deallocation takes place in HandleResult()
				GetRatingForParticipants_PostgreSQLImpl *functor = GetRatingForParticipants_PostgreSQLImpl::Alloc();

				printf("Get the most recent rating for all participants\n");

				printf("Enter the primary ID of the game (int): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "12");
				functor->gameDbId.primaryKey = atoi(inputStr);

				printf("Enter the secondary ID of the game (int): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "13");
				functor->gameDbId.secondaryKey = atoi(inputStr);

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				rankingServer.PushFunctor(functor);
			}
			else if (ch=='h')
			{
				// We could do new and delete, but this ensures the class is allocated and deallocated in the same DLL, should we use one.
				// By default deallocation takes place in HandleResult()
				GetHistoryForParticipant_PostgreSQLImpl *functor = GetHistoryForParticipant_PostgreSQLImpl::Alloc();

				printf("Get the match history for a participant\n");

				printf("Enter the primary ID of participant A (int): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "0");
				functor->participantDbId.primaryKey = atoi(inputStr);

				printf("Enter the secondary ID of participant A (int): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "1");
				functor->participantDbId.secondaryKey = atoi(inputStr);

				printf("Enter the primary ID of the game (int): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "12");
				functor->gameDbId.primaryKey = atoi(inputStr);

				printf("Enter the secondary ID of the game (int): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "13");
				functor->gameDbId.secondaryKey = atoi(inputStr);

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				rankingServer.PushFunctor(functor);
			}
		}

		// Causes Functor::HandleResult calls. If this is forgotten you won't get processing result calls.
		ft.CallResultHandlers();

		Sleep(30);
	}
}
