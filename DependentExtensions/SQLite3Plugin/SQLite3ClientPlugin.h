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
///



#ifndef ___SQLITE_3_CLIENT_PLUGIN_H
#define ___SQLITE_3_CLIENT_PLUGIN_H

#include "RakNetTypes.h"
#include "Export.h"
#include "PluginInterface2.h"
#include "PacketPriority.h"
#include "SocketIncludes.h"
#include "DS_Multilist.h"
#include "RakString.h"
#include "SQLite3PluginCommon.h"

class RakPeerInterface;

namespace RakNet
{

/// \brief Handles results of calls to SQLite3Plugin::_sqlite3_exec()
/// Results from calling SQLite3Plugin::_sqlite3_exec() are handled in this callback.
/// You should implement the callback, and let the plugin know about it via SQLite3Plugin::AddResultHandler()
/// Be sure to call SQLite3Plugin::RemoveResultHandler() or SQLite3Plugin::ClearResultHandlers() if you delete the callback
/// \ingroup SQL_LITE_3_PLUGIN
class SQLite3PluginResultInterface
{
public:

	/// Query executed, possibly returning data or an error message
	///
	/// \param[out] inputStatement Passed to SQLite3Plugin::_sqlite3_exec
	/// \param[out] queryId Returned from SQLite3Plugin::_sqlite3_exec
	/// \param[out] dbIdentifier Passed to SQLite3Plugin::_sqlite3_exec
	/// \param[out] table Result of call to _sqlite3_exec, should that statement return a result
	/// \param[out] errorMsg If _sqlite3_exec failed, then the error message is here, and table will be empty
	/// \ingroup SQL_LITE_3_PLUGIN
	virtual void _sqlite3_exec(
		RakNet::RakString inputStatement,
		unsigned int queryId,
		RakNet::RakString dbIdentifier,
		const SQLite3Table &table,
		RakNet::RakString errorMsg)=0;

	/// dbIdentifier is unknown on the remote system
	///
	/// \param[out] inputStatement Passed to SQLite3Plugin::_sqlite3_exec
	/// \param[out] queryId Returned from SQLite3Plugin::_sqlite3_exec
	/// \param[out] dbIdentifier Passed to SQLite3Plugin::_sqlite3_exec
	/// \ingroup SQL_LITE_3_PLUGIN
	virtual void OnUnknownDBIdentifier(
		RakNet::RakString inputStatement,
		unsigned int queryId,
		RakNet::RakString dbIdentifier)=0;
};

/// Sample callback implementation that just prints to the screen the results
/// \ingroup SQL_LITE_3_PLUGIN
class SQLite3PluginResultInterface_Printf : public SQLite3PluginResultInterface
{
	virtual void _sqlite3_exec(
		RakNet::RakString inputStatement,
		unsigned int queryId,
		RakNet::RakString dbIdentifier,
		const SQLite3Table &table,
		RakNet::RakString errorMsg);

	virtual void OnUnknownDBIdentifier(
		RakNet::RakString inputStatement,
		unsigned int queryId,
		RakNet::RakString dbIdentifier);
};

/// SQLite version 3 supports remote calls via networked file handles, but not over the regular internet
/// This plugin will serialize calls to and results from sqlite3_exec
/// That's all it does - any remote system can execute SQL queries.
/// Intended as a starting platform to derive from for more advanced functionality (security over who can query, etc).
/// Compatible as a plugin with both RakPeerInterface and PacketizedTCP
/// \ingroup SQL_LITE_3_PLUGIN
class RAK_DLL_EXPORT SQLite3ClientPlugin : public PluginInterface2
{
public:
	SQLite3ClientPlugin();
	virtual ~SQLite3ClientPlugin();

	/// Add an interface to get callbacks for results
	/// Up to user to make sure the pointer is valid through the lifetime of use
	void AddResultHandler(SQLite3PluginResultInterface *res);
	void RemoveResultHandler(SQLite3PluginResultInterface *res);
	void ClearResultHandlers(void);

	/// Execute a statement on the remote system
	/// \note Don't forget to escape your input strings. RakString::SQLEscape() is available for this.
	/// \param[in] dbIdentifier Which database to use, added with AddDBHandle()
	/// \param[in] inputStatement SQL statement to execute
	/// \param[in] priority See RakPeerInterface::Send()
	/// \param[in] reliability See RakPeerInterface::Send()
	/// \param[in] orderingChannel See RakPeerInterface::Send()
	/// \param[in] systemAddress See RakPeerInterface::Send()
	/// \return Query ID. Will be returned in _sqlite3_exec
	unsigned int _sqlite3_exec(RakNet::RakString dbIdentifier, RakNet::RakString inputStatement,
		PacketPriority priority, PacketReliability reliability, char orderingChannel, const SystemAddress &systemAddress);

	/// \internal For plugin handling
	virtual PluginReceiveResult OnReceive(Packet *packet);

	// List of result handlers added with AddResultHandler()
	DataStructures::List<SQLite3PluginResultInterface *> resultHandlers;
	// Each query returns a numeric id if you want it. This tracks what id to assign next. Increments sequentially.
	unsigned int nextQueryId;
};

}

#endif
