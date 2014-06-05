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
/// \brief A sample for the SQLite3Plugin, that creates a table to track connections on the server
/// The SQLite3Plugin is used with SQLite version 3 to transmit over the network calls to sqlite3_exec

#include "RakPeerInterface.h"
#include "SQLite3ServerPlugin.h"
#include "SQLite3ClientPlugin.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "Gets.h"
#include "Kbhit.h"
#include "GetTime.h"

using namespace RakNet;

/// A sample derived implementation that will automatically update the table with all connected systems
class ConnectionStatePlugin : public SQLite3ServerPlugin
{
public:
	ConnectionStatePlugin() {lastTimeRemovedDeadRows=0;}

	// Custom function to create the table we want
	// Assumes the database was already added with AddDBHandle
	bool CreateConnectionStateTable(RakNet::RakString dbIdentifier)
	{
		// dbHandles is a member variable of SQLite3Plugin and contains the mappings of identifiers to sql database pointers
		unsigned int idx = dbHandles.GetIndexOf(dbIdentifier);
		if (idx==(unsigned int)-1)
			return false;

		// Store the identifier for the connection state table for use in OnClosedConnection and OnNewConnection
		connectionStateIdentifier=dbIdentifier;

		// Create the table.
		sqlite3_exec(
			// Pointer to sqlite instance previously added with SQLite3Plugin::AddDBHandle()
			dbHandles[idx].dbHandle,
			// Query
			"CREATE TABLE connectionState(systemAddress varchar(64),"
			"rowCreationTime timestamp DATE DEFAULT (datetime('now','localtime')),"
			"lastRowUpdateTime timestamp DATE DEFAULT (datetime('now','localtime')),"
			"rakNetGUID varchar(64))",
			// Ignore per-row callback, callback parameter, and error message destination
			0,0,0);

		return true;
	}

	// Implemented event callback from base class PluginInterface2
	virtual void OnClosedConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
	{
		// Call down to the base class in case it does anything in the future (right now it does nothing)
		SQLite3ServerPlugin::OnClosedConnection(systemAddress, rakNetGUID, lostConnectionReason);

		// Get the database index associated with the table used for this class
		unsigned int idx = dbHandles.GetIndexOf(connectionStateIdentifier);
		if (idx==(unsigned int)-1)
			return;

		// Remove dropped system by primary key system address
		char systemAddressString[64];
		systemAddress.ToString(true,systemAddressString);
		RakNet::RakString query("DELETE FROM connectionState WHERE systemAddress='%s';",
			RakNet::RakString(systemAddressString).SQLEscape().C_String());
		sqlite3_exec(dbHandles[idx].dbHandle,query.C_String(),0,0,0);
	}

	// Implemented event callback from base class PluginInterface2
	virtual void OnNewConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, bool isIncoming)
	{
		// Call down to the base class in case it does anything in the future (right now it does nothing)
		SQLite3ServerPlugin::OnNewConnection(systemAddress, rakNetGUID, isIncoming);

		// Get the database index associated with the table used for this class
		unsigned int idx = dbHandles.GetIndexOf(connectionStateIdentifier);
		if (idx==(unsigned int)-1)
			return;

		// Store new system's system address and guid. rowCreationTime column is created automatically
		char systemAddressString[64];
		systemAddress.ToString(true,systemAddressString);
		char guidString[128];
		rakNetGUID.ToString(guidString);
		RakNet::RakString query(
			"INSERT INTO connectionState (systemAddress,rakNetGUID) VALUES ('%s','%s');",
			RakNet::RakString(systemAddressString).SQLEscape().C_String(),
			RakNet::RakString(guidString).SQLEscape().C_String());
		sqlite3_exec(dbHandles[idx].dbHandle,query.C_String(),0,0,0);
	}

	// After I wrote this I realized it's not needed. ID_CONNECTION_LOST does it for us :)
	/*
	virtual void Update(void)
	{
		// Call down to the base class or the results of thread processing won't update
		SQLite3Plugin::Update();

		// Once a second, remove all rows whose timestamp has not been updated in the last 30 seconds
		RakNet::TimeMS curTime=RakNet::GetTimeMS();
		if (curTime > lastTimeRemovedDeadRows+1000 || curTime < lastTimeRemovedDeadRows) // < is to check overflow
		{
			lastTimeRemovedDeadRows = curTime;

			// Get the database index associated with the table used for this class
			unsigned int idx = dbHandles.GetIndexOf(connectionStateIdentifier);
			if (idx==(unsigned int)-1)
				return;

			sqlite3_exec(dbHandles[idx].dbHandle,"DELETE FROM connectionState WHERE lastRowUpdateTime<dateTime('now','localtime','-30 seconds');",0,0,0);
		}
	}
	*/

	RakNet::RakString connectionStateIdentifier;
	RakNet::TimeMS lastTimeRemovedDeadRows;
};

int main(void)
{
	printf("Demonstration of SQLite3Plugin.\n");
	printf("Allows you to send SQL queries to a remote system running SQLite3.\n");
	printf("System is a basis from which to add more functionality (security, etc.)\n");
	printf("Difficulty: Intermediate\n\n");

	RakNet::RakPeerInterface *rakClient=RakNet::RakPeerInterface::GetInstance();
	RakNet::RakPeerInterface *rakServer=RakNet::RakPeerInterface::GetInstance();
	// Client just needs the base class to do sends
	RakNet::SQLite3ClientPlugin sqlite3ClientPlugin;
	// Server uses our sample derived class to track logins
	ConnectionStatePlugin sqlite3ServerPlugin;
	// Default result handler to print what happens on the client
	SQLite3PluginResultInterface_Printf sqlite3ResultHandler;
	rakClient->AttachPlugin(&sqlite3ClientPlugin);
	rakServer->AttachPlugin(&sqlite3ServerPlugin);
	sqlite3ClientPlugin.AddResultHandler(&sqlite3ResultHandler);

	// Create a database, and tell the plugin about it
	sqlite3 *database;
	// Here :memory: means create the database in memory only.
	// Normally the first parameter refers to a path on the disk to the database file
	if (sqlite3_open_v2(":memory:", &database, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0)!=SQLITE_OK)
		return 1;

	static const char* DATABASE_IDENTIFIER="ConnectionStateDBInMemory";
	sqlite3ServerPlugin.AddDBHandle(DATABASE_IDENTIFIER, database);
	sqlite3ServerPlugin.CreateConnectionStateTable(DATABASE_IDENTIFIER);

	// Start and connect RakNet as usual
	RakNet::SocketDescriptor socketDescriptor(10000,0);
	if (rakServer->Startup(1,&socketDescriptor, 1)!=RAKNET_STARTED)
	{
		printf("Start call failed!\n");
		return 0;
	}
	rakServer->SetMaximumIncomingConnections(1);
	socketDescriptor.port=0;
	rakClient->Startup(1, &socketDescriptor, 1);
	if (rakClient->Connect("127.0.0.1", 10000, 0, 0)!=RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		printf("Connect call failed\n");
		return 0;
	}

	// Wait for the connection to complete
	RakSleep(500);

	
	printf("Enter QUIT to quit, anything else is sent as a query.\n");
	while (1)
	{
		if (kbhit())
		{
			printf("Enter query: ");
			char query[512];
			Gets(query,sizeof(query));
			if (stricmp(query, "QUIT")==0)
			{
				printf("Bye\n");
				break;
			}
			else
			{
				// Send a query to the database through RakNet
				// Result will be printed through SQLite3PluginResultInterface_Printf
				sqlite3ClientPlugin._sqlite3_exec(DATABASE_IDENTIFIER, query, HIGH_PRIORITY, RELIABLE_ORDERED, 0, rakClient->GetSystemAddressFromIndex(0));

			}
		}

		rakClient->DeallocatePacket(rakClient->Receive());
		rakServer->DeallocatePacket(rakServer->Receive());

		RakSleep(30);
	}
	
	rakClient->Shutdown(100,0);
	rakServer->Shutdown(100,0);
	
	RakNet::RakPeerInterface::DestroyInstance(rakClient);
	RakNet::RakPeerInterface::DestroyInstance(rakServer);

	sqlite3_close(database);

	return 0;
}
