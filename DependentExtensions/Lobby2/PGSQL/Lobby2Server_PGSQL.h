/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __LOBBY_2_SERVER_PGSQL_H
#define __LOBBY_2_SERVER_PGSQL_H

#include "Lobby2Server.h"

class PostgreSQLInterface;

namespace RakNet
{

/// PostgreSQL specific functionality to the lobby server
class RAK_DLL_EXPORT Lobby2Server_PGSQL : public RakNet::Lobby2Server
{
public:	
	Lobby2Server_PGSQL();
	virtual ~Lobby2Server_PGSQL();

	STATIC_FACTORY_DECLARATIONS(Lobby2Server_PGSQL)
	
	/// ConnectTo to the database \a numWorkerThreads times using the connection string
	/// \param[in] conninfo See the postgre docs
	/// \return True on success, false on failure.
	virtual bool ConnectToDB(const char *conninfo, int numWorkerThreads);

	/// Add input to the worker threads, from a thread already running
	virtual void AddInputFromThread(Lobby2Message *msg, unsigned int targetUserId, RakNet::RakString targetUserHandle);
	/// Add output from the worker threads, from a thread already running. This is in addition to the current message, so is used for notifications
	virtual void AddOutputFromThread(Lobby2Message *msg, unsigned int targetUserId, RakNet::RakString targetUserHandle);

protected:

	virtual void AddInputCommand(Lobby2ServerCommand command);
	virtual void* PerThreadFactory(void *context);
	virtual void PerThreadDestructor(void* factoryResult, void *context);
	virtual void ClearConnections(void);
	DataStructures::List<PostgreSQLInterface *> connectionPool;
};
	
}

#endif
