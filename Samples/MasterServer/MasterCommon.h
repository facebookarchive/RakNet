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

#ifndef __MASTER_COMMON_H
#define __MASTER_COMMON_H

#include "DS_List.h"
#include "NetworkTypes.h"
#include "BitStream.h"
#include "PluginInterface.h"
using namespace RakNet;

class RakPeerInterface;
struct Packet;

// IP, Port, Ping - case sensitive!
#define NUMBER_OF_DEFAULT_MASTER_SERVER_KEYS 3
// If we ping NUMBER_OF_MISSED_PINGS_TO_DROP without ever a response, that server is dropped from the list.
// This includes the last ping, so actually NUMBER_OF_MISSED_PINGS_TO_DROP-1 would be truly missed
#define NUMBER_OF_MISSED_PINGS_TO_DROP 4
// KEEP_ALIVE_PING_FREQUENCY is how often to ping servers to make sure they are active
#define KEEP_ALIVE_PING_FREQUENCY 20000
// How many ms must pass per connection before we check average bytes for a flood attack
#define FLOOD_ATTACK_CHECK_DELAY 5000
// How many bytes per ms someone has to send on average before they are banned.
#define FLOOD_ATTACK_BYTES_PER_MS 2.0f


struct GameServerRule;
struct GameServer;

/// \defgroup MASTER_SERVER_GROUP MasterServer
/// \ingroup PLUGINS_GROUP

/// \ingroup MASTER_SERVER_GROUP
/// \brief Just a utility class.
struct GameServerList
{
public:
	GameServerList();
	~GameServerList();
	void Clear(void);
	void SortOnKey(char *key, bool ascending);
	void QuickSort(int low, int high, bool ascending);
	int Partition(int low, int high, bool ascending);
	int GetIndexByPlayerID(PlayerID playerID);

	DataStructures::List<GameServer*> serverList;
};

/// \ingroup MASTER_SERVER_GROUP
class MasterCommon : public PluginInterface
{
public:
	MasterCommon();

	// ---------------------------------------------------
	// BROWSER FUNCTIONS
	// ---------------------------------------------------
	// Sorting function
	// ruleIdentifier is a string used by you previously when adding rules via PostRule
	// It can also be "IP" "Port" or "Ping"
	// Set ascending to true to sort from low to high.  Otherwise sorts from high to low.
	void SortServerListOnKey(char *ruleIdentifier, bool ascending);

	// serverIndex should be from 0 to GetServerListSize()-1
	// ruleIdentifier is a string used by you previously when adding rules via PostRule
	// It can also be "IP" "Port" or "Ping".
	// identifier found will return true if the specified rule is found AND you are reading the
	// correct type.
	// GetServerListRuleAsInt should be used for int values.
	// GetServerListRuleAsString should be used for string values
	unsigned int GetServerListSize(void);
	int GetServerListRuleAsInt(int serverIndex, char *ruleIdentifier, bool *identifierFound);
	const char* GetServerListRuleAsString(int serverIndex, char *ruleIdentifier, bool *identifierFound);

protected:
	void OnAttach(RakPeerInterface *peer);

	// Delete all elements from the server list
	void ClearServerList(void);
	// Returns true if a rule is reserved
	bool IsReservedRuleIdentifier(char *ruleIdentifier);
	void HandlePong(Packet *packet);
	// Adds or updates the specified rule to the specified server.
	// Returns true if the server has been changed.  False if we are adding a rule that is already the same
	bool UpdateServerRule(GameServer *gameServer, char *ruleIdentifier, char *stringData, int intData);
	// Remove the specified rule from the server.
	// Returns true if the rule was removed.
	bool RemoveServerRule(GameServer *gameServer, char *ruleIdentifier);
	// Encode a playerID to a bitstream
	void SerializePlayerID(PlayerID *playerID, BitStream *outputBitStream);
	// Encode a rule to a bitstream
	void SerializeRule(GameServerRule *gameServerRule, BitStream *outputBitStream);
	// Decode a playerID from a bitstream
	void DeserializePlayerID(PlayerID *playerID, BitStream *inputBitStream);
	// Decode a rule from a bitstream
	GameServerRule *DeserializeRule(BitStream *inputBitStream);
	// Encode a server to a bitstream
	void SerializeServer(GameServer *gameServer, BitStream *outputBitStream);
	// Create a server from a bitstream
	GameServer *DeserializeServer(BitStream *inputBitStream);
	// Add the default rules to a server (ip, port, ping)
	void AddDefaultRulesToServer(GameServer *gameServer, PlayerID playerID);
	// Update one server based on the information in another
	void UpdateServer(GameServer *destination, GameServer *source, bool deleteSingleRules);
	// Add the specified server to the list of servers - or if the server already exists
	// Update the existing server and delete the server passed
	// deleteSingleRules means if a match is found and a rule exists in the old
	// server but not the new, then delete that rule.
	// Returns the new or updated server
	GameServer * UpdateServerList(GameServer *gameServer, bool deleteSingleRules, bool *newServerAdded);

	RakPeerInterface *rakPeer;
	GameServerList gameServerList;
};

/// \ingroup MASTER_SERVER_GROUP
struct GameServerRule
{
	GameServerRule();
	~GameServerRule();

	char *key;
	// stringValue and intValue are mutually exclusive
	char *stringValue;
	int intValue;
};

/// \ingroup MASTER_SERVER_GROUP
struct GameServer
{
	GameServer();
	~GameServer();
	void Clear(void);
	bool FindKey(char *key);
	int keyIndex;
	int numberOfKeysFound;
	RakNetTime lastUpdateTime;
	PlayerID connectionIdentifier; // The game server
	PlayerID originationId; // Only used by the server - the master client PlayerID
	int failedPingResponses;
	RakNetTime nextPingTime;

	// When inserting rules, don't forget that IP and ping should always be added.
	// These are required for any game server
	DataStructures::List<GameServerRule*> serverRules;
};


#endif
