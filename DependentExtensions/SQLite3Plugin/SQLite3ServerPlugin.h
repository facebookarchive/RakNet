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
/// \brief Contains code to call sqlite3_exec over a network that does not support shared file handles.


#ifndef ___SQLITE_3_SERVER_PLUGIN_H
#define ___SQLITE_3_SERVER_PLUGIN_H

/// \brief Control if SQLite3 statements execute in a thread
/// \details sqlite3_exec is blocking and will therefore block other operations in the same program<BR>
/// If defined, sqlite3_exec executes in a thread so that doesn't happen<BR>
/// If the only thing this system is doing is running SQLite, then you'll get marginally better performance by commenting it out.
/// \ingroup SQL_LITE_3_PLUGIN
#define SQLite3_STATEMENT_EXECUTE_THREADED

#include "RakNetTypes.h"
#include "Export.h"
#include "PluginInterface2.h"
#include "PacketPriority.h"
#include "SocketIncludes.h"
#include "DS_Multilist.h"
#include "RakString.h"
#include "sqlite3.h"
#include "SQLite3PluginCommon.h"

#ifdef SQLite3_STATEMENT_EXECUTE_THREADED
#include "ThreadPool.h"
#endif

class RakPeerInterface;

namespace RakNet
{

/// \brief Exec SQLLite commands over the network
/// \details SQLite version 3 supports remote calls via networked file handles, but not over the regular internet<BR>
/// This plugin will serialize calls to and results from sqlite3_exec<BR>
/// That's all it does - any remote system can execute SQL queries.<BR>
/// Intended as a starting platform to derive from for more advanced functionality (security over who can query, etc).<BR>
/// Compatible as a plugin with both RakPeerInterface and PacketizedTCP
/// \ingroup SQL_LITE_3_PLUGIN
class RAK_DLL_EXPORT SQLite3ServerPlugin : public PluginInterface2
{
public:
	SQLite3ServerPlugin();
	virtual ~SQLite3ServerPlugin();

	/// Associate identifier with dbHandle, so when we get calls to operate on identifier, we use dbhandle
	/// If SQLite3_STATEMENT_EXECUTE_THREADED is defined, will start the execution thread the first time a dbHandle is added.
	/// \return true on success, false on dbIdentifier empty, or already in use
	virtual bool AddDBHandle(RakNet::RakString dbIdentifier, sqlite3 *dbHandle, bool dbAutoCreated=false);

	/// Stop using a dbHandle, lookup either by identifier or by pointer.
	/// If SQLite3_STATEMENT_EXECUTE_THREADED is defined, do not call this while processing commands, since the commands run in a thread and might be using the dbHandle
	/// Call before closing the handle or else SQLite3Plugin won't know that it was closed, and will continue using it
	void RemoveDBHandle(RakNet::RakString dbIdentifier, bool alsoCloseConnection=false);
	void RemoveDBHandle(sqlite3 *dbHandle, bool alsoCloseConnection=false);

	/// \internal For plugin handling
	virtual PluginReceiveResult OnReceive(Packet *packet);
	virtual void OnAttach(void);
	virtual void OnDetach(void);

	/// \internal
	struct NamedDBHandle
	{
		RakNet::RakString dbIdentifier;
		sqlite3 *dbHandle;
		bool dbAutoCreated;
		RakNet::TimeMS whenCreated;
	};

#ifdef SQLite3_STATEMENT_EXECUTE_THREADED
	virtual void Update(void);
	/// \internal
	struct SQLExecThreadInput
	{
		SQLExecThreadInput() {data=0; packet=0;}
		char *data;
		unsigned int length;
		SystemAddress sender;
		RakNet::TimeMS whenMessageArrived;
		sqlite3 *dbHandle;
		RakNet::Packet *packet;
	};

	/// \internal
	struct SQLExecThreadOutput
	{
		SQLExecThreadOutput() {data=0; packet=0;}
		char *data;
		unsigned int length;
		SystemAddress sender;
		RakNet::Packet *packet;
	};
#endif // SQLite3_STATEMENT_EXECUTE_THREADED

protected:
	virtual void StopThreads(void);

	// List of databases added with AddDBHandle()
	DataStructures::Multilist<ML_ORDERED_LIST, NamedDBHandle, RakNet::RakString> dbHandles;

#ifdef SQLite3_STATEMENT_EXECUTE_THREADED
	// The point of the sqlThreadPool is so that SQL queries, which are blocking, happen in the thread and don't slow down the rest of the application
	// The sqlThreadPool has a queue for incoming processing requests.  As systems disconnect their pending requests are removed from the list.
	ThreadPool<SQLExecThreadInput, SQLExecThreadOutput> sqlThreadPool;
#endif
};

};

extern bool operator<( const DataStructures::MLKeyRef<RakNet::RakString> &inputKey, const RakNet::SQLite3ServerPlugin::NamedDBHandle &cls );
extern bool operator>( const DataStructures::MLKeyRef<RakNet::RakString> &inputKey, const RakNet::SQLite3ServerPlugin::NamedDBHandle &cls );
extern bool operator==( const DataStructures::MLKeyRef<RakNet::RakString> &inputKey, const RakNet::SQLite3ServerPlugin::NamedDBHandle &cls );

#endif
