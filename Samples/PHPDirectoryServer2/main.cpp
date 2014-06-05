/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

/// \file
/// \brief This file is a sample for using HTTPConnection and PHPDirectoryServer2


#include "TCPInterface.h"
#include "HTTPConnection.h"
#include "PHPDirectoryServer2.h"
#include "RakSleep.h"
#include "RakString.h"
#include "GetTime.h"
#include "DS_Table.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "Gets.h"
#include "Getche.h"

using namespace RakNet;

// Allocate rather than create on the stack or the RakString mutex crashes on shutdown
TCPInterface *tcp;
HTTPConnection *httpConnection;
PHPDirectoryServer2 *phpDirectoryServer2;

enum ReadResultEnum
{
	RR_EMPTY_TABLE,
	RR_READ_TABLE,
	RR_TIMEOUT,
};

ReadResultEnum ReadResult(RakNet::RakString &httpResult)
{
	RakNet::TimeMS endTime=RakNet::GetTimeMS()+10000;
	httpResult.Clear();
	while (RakNet::GetTimeMS()<endTime)
	{
		Packet *packet = tcp->Receive();
		if(packet)
		{
			httpConnection->ProcessTCPPacket(packet);
			tcp->DeallocatePacket(packet);
		}

		if (httpConnection->HasRead())
		{
			httpResult = httpConnection->Read();
			// Good response, let the PHPDirectoryServer2 class handle the data
			// If resultCode is not an empty string, then we got something other than a table
			// (such as delete row success notification, or the message is for HTTP only and not for this class).
			HTTPReadResult readResult = phpDirectoryServer2->ProcessHTTPRead(httpResult);

			if (readResult==HTTP_RESULT_GOT_TABLE)
			{
				//printf("RR_READ_TABLE\n");
				return RR_READ_TABLE;
			}
			else if (readResult==HTTP_RESULT_EMPTY)
			{
				//printf("HTTP_RESULT_EMPTY\n");
				return RR_EMPTY_TABLE;
			}
		}

		// Update our two classes so they can do time-based updates
		httpConnection->Update();
		phpDirectoryServer2->Update();

		// Prevent 100% cpu usage
		RakSleep(30);
	}

	return RR_TIMEOUT;
}

bool HaltOnUnexpectedResult(ReadResultEnum result, ReadResultEnum expected)
{
	if (result!=expected)
	{
		printf("TEST FAILED. Expected ");

		switch (expected)
		{

		case RR_EMPTY_TABLE:
			printf("no results");
			break;
		case RR_TIMEOUT:
			printf("timeout");
			break;
		case RR_READ_TABLE:
			printf("to download result");
			break;
		}
		
		switch (result)
		{
		case RR_EMPTY_TABLE:
			printf(". No results were downloaded");
			break;
		case RR_READ_TABLE:
			printf(". Got a result");
			break;
		case RR_TIMEOUT:
			printf(". Timeout");
			break;
		}
		printf("\n");
		return true;
	}

	return false;
}

void DownloadTable()
{
	phpDirectoryServer2->DownloadTable("a");
}
void UploadTable(RakNet::RakString gameName, unsigned short gamePort)
{
	phpDirectoryServer2->UploadTable("a", gameName, gamePort, false);
}
void UploadAndDownloadTable(RakNet::RakString gameName, unsigned short gamePort)
{
	phpDirectoryServer2->UploadAndDownloadTable("a", "a", gameName, gamePort, false);
}
bool PassTestOnEmptyDownloadedTable()
{
	const DataStructures::Table *games = phpDirectoryServer2->GetLastDownloadedTable();

	if (games->GetRowCount()==0)
	{
		printf("Test passed.\n");
		return true;
	}
	printf("TEST FAILED. Empty table should have been downloaded.\n");
	return false;
}
bool VerifyDownloadMatchesUpload(int requiredRowCount, int testRowIndex)
{
	const DataStructures::Table *games = phpDirectoryServer2->GetLastDownloadedTable();
	if (games->GetRowCount()!=(unsigned int)requiredRowCount)
	{
		printf("TEST FAILED. Expected %i result rows, got %i\n", requiredRowCount, games->GetRowCount());
		return false;
	}
	RakNet::RakString columnName;
	RakNet::RakString value;
	unsigned int i;
	DataStructures::Table::Row *row = games->GetRowByIndex(testRowIndex,NULL);
	const DataStructures::List<DataStructures::Table::ColumnDescriptor>& columns = games->GetColumns();
	unsigned int colIndex;
	// +4 comes from automatic fields
	// _GAME_PORT
	// _GAME_NAME
	// _SYSTEM_ADDRESS
	// __SEC_AFTER_EPOCH_SINCE_LAST_UPDATE
	if (phpDirectoryServer2->GetFieldCount()+4!=games->GetColumnCount())
	{
		printf("TEST FAILED. Expected %i columns, got %i\n", phpDirectoryServer2->GetFieldCount()+4, games->GetColumnCount());
		printf("Expected columns:\n");
		for (colIndex=0; colIndex < phpDirectoryServer2->GetFieldCount(); colIndex++)
		{
			phpDirectoryServer2->GetField(colIndex, columnName, value);
			printf("%i. %s\n", colIndex+1, columnName.C_String());
		}
		printf("%i. _GAME_PORT\n", colIndex++);
		printf("%i. _GAME_NAME\n", colIndex++);
		printf("%i. _System_Address\n", colIndex++);
		printf("%i. __SEC_AFTER_EPOCH_SINCE_LAST_UPDATE\n", colIndex++);

		printf("Got columns:\n");
		for (colIndex=0; colIndex < columns.Size(); colIndex++)
		{
			printf("%i. %s\n", colIndex+1, columns[colIndex].columnName);
		}

		return false;
	}
	for (i=0; i < phpDirectoryServer2->GetFieldCount(); i++)
	{
		phpDirectoryServer2->GetField(i, columnName, value);
		for (colIndex=0; colIndex < columns.Size(); colIndex++)
		{
			if (strcmp(columnName.C_String(), columns[colIndex].columnName)==0)
				break;
		}
		if (colIndex==columns.Size())
		{
			printf("TEST FAILED. Expected column with name %s\n", columnName.C_String());
			return false;
		}

		if (strcmp(value.C_String(), row->cells[colIndex]->c)!=0)
		{
			printf("TEST FAILED. Expected row with value '%s' at index %i for column %s. Got '%s'.\n", value.C_String(), i, columnName.C_String(), row->cells[colIndex]->c);
			return false;
		}
	}

	printf("Test passed.\n");
	return true;
}
void PrintHttpResult(RakNet::RakString httpResult)
{
	printf("--- Last result read ---\n");
	printf("%s", httpResult.C_String());
}
void PrintFieldColumns(void)
{
	unsigned int colIndex;
	RakNet::RakString columnName;
	RakNet::RakString value;
	for (colIndex=0; colIndex < phpDirectoryServer2->GetFieldCount(); colIndex++)
	{
		phpDirectoryServer2->GetField(colIndex, columnName, value);
		printf("%i. %s\n", colIndex+1, columnName.C_String());
	}
}
bool RunTest()
{
	RakNet::RakString httpResult;
	ReadResultEnum rr;
	char ch[32];
	printf("Warning, table must be clear before starting the test.\n");
	printf("Press enter to start\n");
	Gets(ch,sizeof(ch));

	printf("*** Testing initial table is empty.\n");
	// Table should start emptyF
	DownloadTable();
	if (HaltOnUnexpectedResult(rr=ReadResult(httpResult), RR_EMPTY_TABLE))
		{PrintHttpResult(httpResult); return false;}
	if (PassTestOnEmptyDownloadedTable()==false)
		{PrintHttpResult(httpResult); return false;}

	printf("*** Downloading again, to ensure download does not modify the table.\n");
	// Downloading should not modify the table
	DownloadTable();
	if (HaltOnUnexpectedResult(rr=ReadResult(httpResult), RR_EMPTY_TABLE))
		{PrintHttpResult(httpResult); return false;}
	if (PassTestOnEmptyDownloadedTable()==false)
		{PrintHttpResult(httpResult); return false;}

	printf("*** Testing upload.\n");
	// Upload values likely to mess up PHP
	phpDirectoryServer2->SetField("TestField1","0");
	phpDirectoryServer2->SetField("TestField2","");
	phpDirectoryServer2->SetField("TestField3"," ");
	phpDirectoryServer2->SetField("TestField4","!@#$%^&*(");
	phpDirectoryServer2->SetField("TestField5","A somewhat big long string as these things typically go.\nIt even has a linebreak!");
	phpDirectoryServer2->SetField("TestField6","=");
	phpDirectoryServer2->UploadTable("a", "FirstGameUpload", 80, false);
	if (HaltOnUnexpectedResult(rr=ReadResult(httpResult), RR_EMPTY_TABLE))
		{PrintHttpResult(httpResult); return false;}
	if (PassTestOnEmptyDownloadedTable()==false)
		{PrintHttpResult(httpResult); return false;}

	printf("*** Testing download, should match upload exactly.\n");
	// Download what we just uploaded
	DownloadTable();
	if (HaltOnUnexpectedResult(rr=ReadResult(httpResult), RR_READ_TABLE))
		{PrintHttpResult(httpResult); return false;}
	// Check results
	if (VerifyDownloadMatchesUpload(1,0)==false)
		{PrintHttpResult(httpResult); return false;}

	printf("*** Testing that download works twice in a row.\n");
	// Make sure download works twice
	DownloadTable();
	if (HaltOnUnexpectedResult(rr=ReadResult(httpResult), RR_READ_TABLE))
		{PrintHttpResult(httpResult); return false;}
	// Check results
	if (VerifyDownloadMatchesUpload(1,0)==false)
		{PrintHttpResult(httpResult); return false;}

	printf("*** Testing reuploading a game to modify fields.\n");
	// Modify fields
	phpDirectoryServer2->SetField("TestField1","zero");
	phpDirectoryServer2->SetField("TestField2","empty");
	phpDirectoryServer2->SetField("TestField3","space");
	phpDirectoryServer2->SetField("TestField4","characters");
	phpDirectoryServer2->SetField("TestField5","A shorter string");
	phpDirectoryServer2->SetField("TestField6","Test field 6");
	phpDirectoryServer2->UploadTable("a", "FirstGameUpload", 80, false);
	if (HaltOnUnexpectedResult(rr=ReadResult(httpResult), RR_EMPTY_TABLE))
	{PrintHttpResult(httpResult); return false;}
	if (PassTestOnEmptyDownloadedTable()==false)
		{PrintHttpResult(httpResult); return false;}

	printf("*** Testing that downloading returns modified fields.\n");
	// Download what we just uploaded
	DownloadTable();
	if (HaltOnUnexpectedResult(rr=ReadResult(httpResult), RR_READ_TABLE))
		{PrintHttpResult(httpResult); return false;}
	// Check results
	if (VerifyDownloadMatchesUpload(1,0)==false)
		{PrintHttpResult(httpResult); return false;}

	printf("*** Testing that downloading works twice.\n");
	// Make sure download works twice
	DownloadTable();
	if (HaltOnUnexpectedResult(rr=ReadResult(httpResult), RR_READ_TABLE))
		{PrintHttpResult(httpResult); return false;}
	// Check results
	if (VerifyDownloadMatchesUpload(1,0)==false)
		{PrintHttpResult(httpResult); return false;}

	printf("*** Testing upload of a second game.\n");
	// Upload another game
	phpDirectoryServer2->SetField("TestField1","0");
	phpDirectoryServer2->SetField("TestField2","");
	phpDirectoryServer2->SetField("TestField3"," ");
	phpDirectoryServer2->SetField("TestField4","Game two characters !@#$%^&*(");
	phpDirectoryServer2->UploadTable("a", "SecondGameUpload", 80, false);
	if (HaltOnUnexpectedResult(rr=ReadResult(httpResult), RR_EMPTY_TABLE))
	{PrintHttpResult(httpResult); return false;}
	if (PassTestOnEmptyDownloadedTable()==false)
		{PrintHttpResult(httpResult); return false;}
	RakNet::TimeMS startTime = RakNet::GetTimeMS();

	printf("*** Testing 20 repeated downloads.\n");
	//printf("Field columns\n");
	//PrintFieldColumns();

	// Download repeatedly
	unsigned int downloadCount=0;
	while (downloadCount < 20)
	{
		printf("*** (%i) Downloading 'FirstGameUpload'\n", downloadCount+1);
		// Download again (First game)
		DownloadTable();
		if (HaltOnUnexpectedResult(rr=ReadResult(httpResult), RR_READ_TABLE))
			{PrintHttpResult(httpResult); return false;}
		// Check results
		// DOn't have this stored anymore
//		if (VerifyDownloadMatchesUpload(2,0)==false)
//			{PrintHttpResult(httpResult); return false;}

		printf("*** (%i) Downloading 'SecondGameUpload'\n", downloadCount+1);
		// Download again (second game)
		DownloadTable();
		if (HaltOnUnexpectedResult(rr=ReadResult(httpResult), RR_READ_TABLE))
			{PrintHttpResult(httpResult); return false;}
		// Check results
		if (VerifyDownloadMatchesUpload(2,1)==false)
			{PrintHttpResult(httpResult); return false;}

		downloadCount++;

		RakSleep(1000);
	}

	printf("*** Waiting for 70 seconds to have elapsed...\n");
	RakSleep(70000 - (RakNet::GetTimeMS()-startTime));


	printf("*** Testing that table is now clear.\n");
	// Table should be cleared
	DownloadTable();
	if (HaltOnUnexpectedResult(rr=ReadResult(httpResult), RR_EMPTY_TABLE))
		{PrintHttpResult(httpResult); return false;}
	if (PassTestOnEmptyDownloadedTable()==false)
		{PrintHttpResult(httpResult); return false;}

	printf("*** Testing upload and download. No games should be downloaded.\n");
	phpDirectoryServer2->ClearFields();
	phpDirectoryServer2->SetField("TestField1","NULL");
	UploadAndDownloadTable("FirstGameUpload", 80);
	if (HaltOnUnexpectedResult(rr=ReadResult(httpResult), RR_EMPTY_TABLE))
		{PrintHttpResult(httpResult); return false;}
	if (PassTestOnEmptyDownloadedTable()==false)
		{PrintHttpResult(httpResult); return false;}

	printf("*** Testing upload and download. One game should be downloaded.\n");
	UploadAndDownloadTable("ThirdGameUpload", 80);
	if (HaltOnUnexpectedResult(rr=ReadResult(httpResult), RR_READ_TABLE))
		{PrintHttpResult(httpResult); return false;}
	if (VerifyDownloadMatchesUpload(1,0)==false)
		{PrintHttpResult(httpResult); return false;}

	return true;
}

void OutputBody(HTTPConnection& http, const char *path, const char *data, TCPInterface& tcp);

void TestPHPDirectoryServer(int argc, char **argv)
{
	printf("PHP Directory server 2.\n");
	printf("Similar to lightweight database, but uses common shared webservers.\n");
	printf("Set columns and one row for your game, and upload it to a\nviewable and downloadable webpage.\n");
	printf("Difficulty: Intermediate\n\n");

// 	tcp = RakNet::OP_NEW<TCPInterface>(_FILE_AND_LINE_);
// 	httpConnection = RakNet::OP_NEW<HTTPConnection>(_FILE_AND_LINE_);
// 	phpDirectoryServer2 = RakNet::OP_NEW<PHPDirectoryServer2>(_FILE_AND_LINE_);



//	RakNet::TimeMS lastTouched = 0;



	char website[256];
	char pathToPHP[256];
	if (argc==3)
	{
		strcpy(website, argv[1]);
		strcpy(pathToPHP, argv[2]);
	}
	else
	{
		printf("Enter website, e.g. jenkinssoftware.com:\n");
		Gets(website,sizeof(website));
		if (website[0]==0)
			strcpy(website, "jenkinssoftware.com");

		printf("Enter path to DirectoryServer.php, e.g. raknet/DirectoryServer.php:\n");
		Gets(pathToPHP,sizeof(pathToPHP));
		if (pathToPHP[0]==0)
			strcpy(pathToPHP, "/raknet/DirectoryServer.php");
	}

	if (website[strlen(website)-1]!='/' && pathToPHP[0]!='/')
	{
		memmove(pathToPHP+1, pathToPHP, strlen(pathToPHP)+1);
		pathToPHP[0]='/';
	}

	// This creates an HTTP connection using TCPInterface. It allows you to Post messages to and parse messages from webservers.
	// The connection attempt is asynchronous, and is handled automatically as HTTPConnection::Update() is called
	httpConnection->Init(tcp, website);

	// This adds specific parsing functionality to HTTPConnection, in order to communicate with DirectoryServer.php
	phpDirectoryServer2->Init(httpConnection, pathToPHP);

	if (RunTest())
	{
		printf("All tests passed.\n");
	}

	char str[256];
	do 
	{
		printf("\nPress q to quit.\n");
		Gets(str, sizeof(str));
	} while (str[0]!='q');

	// The destructor of each of these references the other, so delete in this order
	RakNet::OP_DELETE(phpDirectoryServer2,_FILE_AND_LINE_);
	RakNet::OP_DELETE(httpConnection,_FILE_AND_LINE_);
	RakNet::OP_DELETE(tcp,_FILE_AND_LINE_);
}

void TestGet(void)
{
	printf("This is NOT a reliable way to download from a website. Use libcurl instead.\n");
	httpConnection->Init(tcp, "jenkinssoftware.com");
	httpConnection->Get("/trivia/ranking.php?t=single&places=6&top");
	while (1)
	{
		Packet *packet = tcp->Receive();
		if(packet)
		{
			//printf((char*) packet->data);
			httpConnection->ProcessTCPPacket(packet);
			tcp->DeallocatePacket(packet);
		}
		httpConnection->Update();

		if (httpConnection->IsBusy()==false)
		{
			RakString fileContents = httpConnection->Read();
			printf(fileContents.C_String());
			getche();
			return;
		}
		// Prevent 100% cpu usage
		RakSleep(30);
	}
}

int main(int argc, char **argv)
{
	printf("PHP Directory server 2.\n");
	printf("Similar to lightweight database, but uses common shared webservers.\n");
	printf("Set columns and one row for your game, and upload it to a\nviewable and downloadable webpage.\n");
	printf("Difficulty: Intermediate\n\n");

	tcp = RakNet::OP_NEW<TCPInterface>(__FILE__,__LINE__);
	httpConnection = RakNet::OP_NEW<HTTPConnection>(__FILE__,__LINE__);
	phpDirectoryServer2 = RakNet::OP_NEW<PHPDirectoryServer2>(__FILE__,__LINE__);



	//    RakNetTime lastTouched = 0;

	// Start the TCP thread. This is used for general TCP communication, whether it is for webpages, sending emails, or telnet
	tcp->Start(0, 64);

	TestPHPDirectoryServer(argc,argv);

	//TestGet();

	return 0;
}