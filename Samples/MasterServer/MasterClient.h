/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

// TOPOLOGY
// Connect to master server
// Stay connected
// When an information query comes in, parse it and write out to a bitstream.
// When all bitstreams are written, send that back to the master server.

#ifndef __MASTER_CLIENT_H
#define __MASTER_CLIENT_H

#include "MasterCommon.h"
#include "BitStream.h"

/// \ingroup MasterServer
/// \brief implements the master client
class MasterClient : public MasterCommon
{
public:
	MasterClient();
	~MasterClient();

	// ---------------------------------------------------
	// NETWORKING FUNCTIONS
	// ---------------------------------------------------
	// We must be connected to call any of the following functions.
	// Host is the master server IP
	// masterServerPort is the master server port
	// masterClientPort is the port the master client should use (must be different than gamePort)
	// gamePort is the port your game server or game client is using.
	bool Connect(char* host, int masterServerPort);

	// Disconnect and terminate.  Game servers should stay connected to use NAT punch-through.  Game clients,
	// or game servers that do not need NAT punch-through can disconnect.
	void Disconnect(void);

	// Returns true if connected
	bool IsConnected(void);

	// This will tell the master server that we are trying to connect to the indicated game server.
	// The indicated server will then also try to connect to us, bypassing most NATs
	// and increasing the chance of a successful connection.  You should try connecting to the game server
	// at the same time you call this function.
	void ConnectionAttemptNotification(char *serverIP, unsigned short serverPort);

	// ---------------------------------------------------
	// SERVER LISTING FUNCTIONS
	// ---------------------------------------------------
	// List the server.
	void ListServer(void);
	// Remove our server listing from the master server
	void DelistServer(void);
	// Add a rule about our server.  Can be done before or after listing
	void PostRule(char *ruleIdentifier, char *stringData, int intData);
	// Remove a rule about our server
	void RemoveRule(char *ruleIdentifier);
	
	// ---------------------------------------------------
	// SERVER QUERY FUNCTIONS
	// ---------------------------------------------------
	// Adds a rule to look for when we query.  This will update existing servers that contain these rules.
	// Do not query for "Ping", "IP", or "Port" as these are automatically returned.
	void AddQueryRule(char *ruleIdentifier);
	// Clears all rules from our query list.  This will return all servers.
	void ClearQueryRules(void);
	// Query the master server with our rule set.  To get all servers, call ClearQueryRules() after
	// any prior calls to add AddQueryRule to clear the rule set.
	void QueryMasterServer(void);
	// Pings all servers on our list.
	void PingServers(void);

	// ---------------------------------------------------
	// SEE MasterCommon.h FOR BROWSER FUNCTIONS
	// ---------------------------------------------------

	// ---------------------------------------------------
	// OVERRIDABLE EVENTS
	// ---------------------------------------------------
	// Event if we lose the connection to the master server
	void OnLostConnection(void);
	// Couldn't connect
	void OnConnectionAttemptFailed(void);
	// Event if the master server is full when we try to connect
	void OnMasterServerFull(void);
	// Event if a packet was tampered with mid-steram
	void OnModifiedPacket(void);
	// Event if a server was added to the list
	void OnGameServerListAddition(GameServer *newServer);
	// Event if a server has its rules updated (only happens if querying with a rule set)
	void OnGameServerListRuleUpdate(GameServer *updatedServer);
	// Event when we complete a query
	void OnGameServerListQueryComplete(void);
	// Event when a game client wants to connect to our server
	// You should call AdvertiseSystem to the passed IP and port from your game instance
	void OnConnectionRequest(const char *clientIP, unsigned short clientPort);

protected:

	void Update(RakPeerInterface *peer);
	bool OnReceive(RakPeerInterface *peer, Packet *packet);

	void HandleServerListResponse(Packet *packet, bool overwriteExisting);
	void HandleRelayedConnectionNotification(Packet *packet);

	bool listServer, serverListed, localServerModified;
	GameServer localServer;
	BitStream ruleIdentifierList;
};

#endif
