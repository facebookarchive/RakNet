/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "MasterCommon.h"
#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include <cstring>
#include "GetTime.h"
#include "StringCompressor.h"
#include "BitStream.h"
using namespace RakNet;

// For debugging
#include <cstdio>

GameServerRule::GameServerRule()
{
	key=0;
	stringValue=0;
	intValue=-1;
}
GameServerRule::~GameServerRule()
{
	if (key)
		delete [] key;
	if (stringValue)
		delete [] stringValue;
}

GameServer::GameServer()
{
	connectionIdentifier=UNASSIGNED_PLAYER_ID;
	nextPingTime=0;
	failedPingResponses=0;
	lastUpdateTime=RakNet::GetTime();
}
GameServer::~GameServer()
{
	Clear();
}
void GameServer::Clear()
{
	unsigned i;
	for (i=0; i < serverRules.Size(); i++)
		delete serverRules[i];
	serverRules.Clear();
}
bool GameServer::FindKey(char *key)
{
	unsigned i;
	for (i=0; i < serverRules.Size(); i++)
		if (strcmp(serverRules[i]->key, key)==0)
		{
			keyIndex=i;
			return true;
		}

	keyIndex=-1;
	return false;
}

GameServerList::GameServerList()
{
}
GameServerList::~GameServerList()
{
	Clear();
}
void GameServerList::Clear(void)
{
	unsigned i;
	for (i=0; i < serverList.Size(); i++)
		delete serverList[i];
	serverList.Clear();
}
void GameServerList::SortOnKey(char *key, bool ascending)
{
	unsigned i;
	// Set keyindex
	for (i=0; i < serverList.Size(); i++)
		serverList[i]->FindKey(key);

	QuickSort(0, serverList.Size()-1,ascending);
}


void GameServerList::QuickSort(int low, int high, bool ascending)
{
	int pivot;
	if ( high > low )
	{
		pivot = Partition( low, high, ascending);
		QuickSort( low, pivot-1,ascending);
		QuickSort( pivot+1, high,ascending);
	}
}
int EQ(GameServer *left, GameServer *right)
{
	if (left->keyIndex==-1 || right->keyIndex==-1)
		return true;
	
	if (left->serverRules[left->keyIndex]->stringValue)
		return strcmp(left->serverRules[left->keyIndex]->stringValue,right->serverRules[right->keyIndex]->stringValue)==0;
	else
		return left->serverRules[left->keyIndex]->intValue == right->serverRules[right->keyIndex]->intValue;

}
int LT(GameServer *left, GameServer *right)
{
	if (left->keyIndex==-1)
		return true;
	if (right->keyIndex==-1)
		return false;

	if (left->serverRules[left->keyIndex]->stringValue)
		return strcmp(left->serverRules[left->keyIndex]->stringValue,right->serverRules[right->keyIndex]->stringValue) < 0;
	else
		return left->serverRules[left->keyIndex]->intValue < right->serverRules[right->keyIndex]->intValue;
}
int LTEQ(GameServer *left, GameServer *right)
{
	return LT(left, right) || EQ(left, right);
}
int GT(GameServer *left, GameServer *right)
{
	if (left->keyIndex==-1)
		return false;
	if (right->keyIndex==-1)
		return true;

	if (left->serverRules[left->keyIndex]->stringValue)
		return strcmp(left->serverRules[left->keyIndex]->stringValue,right->serverRules[right->keyIndex]->stringValue) > 0;
	else
		return left->serverRules[left->keyIndex]->intValue > right->serverRules[right->keyIndex]->intValue;
}
int GTEQ(GameServer *left, GameServer *right)
{
	return GT(left, right) || EQ(left, right);
}
int GameServerList::Partition(int low, int high, bool ascending)
{
	int left, right, pivot;
	GameServer *pivot_item, *temp;
	pivot_item = serverList[low];
	pivot = left = low;
	right = high;
	while ( left < right )
	{
		if (ascending)
		{
			/* Move left while item < pivot */
			while( LTEQ(serverList[left], pivot_item) && left < high) left++;
			/* Move right while item > pivot */
			while( GT(serverList[right], pivot_item) && right > 0) right--;
			if ( left < right )
			{
				temp=serverList[left];
				serverList[left]=serverList[right];
				serverList[right]=temp;
			}
		}
		else
		{
			while( GTEQ(serverList[left], pivot_item) && left < high) left++;
			while( LT(serverList[right], pivot_item) && right > 0) right--;
			if ( left < right )
			{
				temp=serverList[left];
				serverList[left]=serverList[right];
				serverList[right]=temp;
			}
		}
	}

	/* right is final position for the pivot */
	serverList[low] = serverList[right];
	serverList[right] = pivot_item;

	return right;
}

int GameServerList::GetIndexByPlayerID(PlayerID playerID)
{
	int i;
	for (i=0; i < (int)serverList.Size(); i++)
	{
		if (serverList[i]->connectionIdentifier==playerID)
			return i;
	}
	return -1;
}

MasterCommon::MasterCommon()
{
//	rakPeer = RakNetworkFactory::GetRakPeerInterface();
}
void MasterCommon::ClearServerList(void)
{
	gameServerList.Clear();
}
void MasterCommon::SortServerListOnKey(char *ruleIdentifier, bool ascending)
{
	gameServerList.SortOnKey(ruleIdentifier, ascending);
}
unsigned int MasterCommon::GetServerListSize(void)
{
	return gameServerList.serverList.Size();
}
int MasterCommon::GetServerListRuleAsInt(int serverIndex, char *ruleIdentifier, bool *identifierFound)
{
	int keyIndex;
	if (serverIndex >= (int)gameServerList.serverList.Size())
	{
		*identifierFound=false;
		return -1;
	}

	gameServerList.serverList[serverIndex]->FindKey(ruleIdentifier);
	keyIndex=gameServerList.serverList[serverIndex]->keyIndex;
	if (keyIndex==-1)
	{
		*identifierFound=false;
		return -1;
	}

	if (gameServerList.serverList[serverIndex]->serverRules[keyIndex]->stringValue)
	{
		*identifierFound=false;
		return -1;
	}

	*identifierFound=true;
	return gameServerList.serverList[serverIndex]->serverRules[keyIndex]->intValue;
}
const char* MasterCommon::GetServerListRuleAsString(int serverIndex, char *ruleIdentifier, bool *identifierFound)
{
	int keyIndex;
	if (serverIndex >= (int)gameServerList.serverList.Size())
	{
		*identifierFound=false;
		return "serverIndex out of bounds";
	}

	gameServerList.serverList[serverIndex]->FindKey(ruleIdentifier);
	keyIndex=gameServerList.serverList[serverIndex]->keyIndex;
	if (keyIndex==-1)
	{
		*identifierFound=false;
		return "Server does not contain specified rule";
	}

	if (gameServerList.serverList[serverIndex]->serverRules[keyIndex]->stringValue==0)
	{
		*identifierFound=false;
		return "Server rule is not a string. Use GetServerListRuleAsInt";
	}

	*identifierFound=true;
	return gameServerList.serverList[serverIndex]->serverRules[keyIndex]->stringValue;
}

bool MasterCommon::IsReservedRuleIdentifier(char *ruleIdentifier)
{
	if (strcmp(ruleIdentifier, "Ping")==0 || 
		strcmp(ruleIdentifier, "IP")==0 || 
		strcmp(ruleIdentifier, "Port")==0)
		return true;

	return false;
}

void MasterCommon::AddDefaultRulesToServer(GameServer *gameServer, PlayerID playerID)
{
	GameServerRule *gameServerRule;

	// Every server has NUMBER_OF_DEFAULT_MASTER_SERVER_KEYS keys by default: IP, port, and ping
	gameServerRule = new GameServerRule;
	gameServerRule->key=new char[strlen("IP")+1];
	strcpy(gameServerRule->key, "IP");
	gameServerRule->stringValue=new char[22]; // Should be enough to hold an IP address
	strncpy(gameServerRule->stringValue, rakPeer->PlayerIDToDottedIP(playerID), 21);
	gameServerRule->stringValue[21]=0;
	gameServer->serverRules.Insert(gameServerRule);

	gameServerRule = new GameServerRule;
	gameServerRule->key=new char[strlen("Port")+1];
	strcpy(gameServerRule->key, "Port");
	gameServerRule->intValue=playerID.port;
	gameServer->serverRules.Insert(gameServerRule);

	gameServerRule = new GameServerRule;
	gameServerRule->key=new char[strlen("Ping")+1];
	strcpy(gameServerRule->key, "Ping");
	gameServerRule->intValue=9999;
	gameServer->serverRules.Insert(gameServerRule);
}
void MasterCommon::HandlePong(Packet *packet)
{
	// Find the server specified by packet
	int serverIndex;
	unsigned int pingTime;

	serverIndex=gameServerList.GetIndexByPlayerID(packet->playerId);
	if (serverIndex>=0)
	{
		gameServerList.serverList[serverIndex]->failedPingResponses=0;
		if (gameServerList.serverList[serverIndex]->FindKey("Ping"))
		{
			RakNet::BitStream ptime( packet->data+1, sizeof(unsigned int), false);
			ptime.Read(pingTime);
			gameServerList.serverList[serverIndex]->serverRules[gameServerList.serverList[serverIndex]->keyIndex]->intValue=pingTime;
			#ifdef _SHOW_MASTER_SERVER_PRINTF
			printf("Got pong. Ping=%i\n", pingTime);
			#endif
		}
#ifdef _DEBUG
		else
			// No ping key!
			assert(0);
#endif
	}
}
bool MasterCommon::UpdateServerRule(GameServer *gameServer, char *ruleIdentifier, char *stringData, int intData)
{
	GameServerRule *gameServerRule;
	gameServer->lastUpdateTime=RakNet::GetTime();

	// Add the rule to our local server.  If it changes the local server, set a flag so we upload the
	// local server on the next update.
	if (gameServer->FindKey(ruleIdentifier))
	{
		// Is the data the same?
		if (gameServer->serverRules[gameServer->keyIndex]->stringValue)
		{
			if (stringData==0)
			{
				// No string.  Delete the string and use int data instead
				delete [] gameServer->serverRules[gameServer->keyIndex]->stringValue;
				gameServer->serverRules[gameServer->keyIndex]->stringValue=0;
				gameServer->serverRules[gameServer->keyIndex]->intValue=intData;
				return true;
			}
			else if (strcmp(gameServer->serverRules[gameServer->keyIndex]->stringValue, stringData)!=0)
			{
				// Different string
				delete [] gameServer->serverRules[gameServer->keyIndex]->stringValue;
				gameServer->serverRules[gameServer->keyIndex]->stringValue = new char [strlen(stringData)+1];
				strcpy(gameServer->serverRules[gameServer->keyIndex]->stringValue, stringData);
				return true;
			}
		}
		else
		{
			if (stringData)
			{
				// Has a string where there is currently none
				gameServer->serverRules[gameServer->keyIndex]->stringValue = new char [strlen(stringData)+1];
				strcpy(gameServer->serverRules[gameServer->keyIndex]->stringValue, stringData);
				gameServer->serverRules[gameServer->keyIndex]->intValue=-1;
				return true;
			}
			else if (gameServer->serverRules[gameServer->keyIndex]->intValue!=intData)
			{
				// Different int value
				gameServer->serverRules[gameServer->keyIndex]->intValue=intData;
				return true;
			}
		}
	}
	else
	{
		// No such key.  Add a new one.
		gameServerRule = new GameServerRule;
		gameServerRule->key=new char[strlen(ruleIdentifier)+1];
		strcpy(gameServerRule->key, ruleIdentifier);
		if (stringData)
		{
			gameServerRule->stringValue=new char[strlen(stringData)+1];
			strcpy(gameServerRule->stringValue, stringData);
		}
		else
		{
			gameServerRule->intValue=intData;
		}
		gameServer->serverRules.Insert(gameServerRule);
		return true;
	}

	return false;
}
bool MasterCommon::RemoveServerRule(GameServer *gameServer, char *ruleIdentifier)
{
	if (gameServer->FindKey(ruleIdentifier))
	{
		delete gameServer->serverRules[gameServer->keyIndex];
		gameServer->serverRules.RemoveAtIndex(gameServer->keyIndex);
		return true;
	}

	return false;
}

void MasterCommon::SerializePlayerID(PlayerID *playerID, BitStream *outputBitStream)
{
	outputBitStream->Write(playerID->binaryAddress);
	outputBitStream->Write(playerID->port);
}
void MasterCommon::SerializeRule(GameServerRule *gameServerRule, BitStream *outputBitStream)
{
	stringCompressor->EncodeString(gameServerRule->key, 256, outputBitStream);

	if (gameServerRule->stringValue)
	{
		outputBitStream->Write(true);
		stringCompressor->EncodeString(gameServerRule->stringValue, 256, outputBitStream);
	}
	else
	{
		outputBitStream->Write(false);
		outputBitStream->WriteCompressed(gameServerRule->intValue);
	}
}
void MasterCommon::DeserializePlayerID(PlayerID *playerID, BitStream *inputBitStream)
{
	*playerID=UNASSIGNED_PLAYER_ID;

	inputBitStream->Read(playerID->binaryAddress);
	inputBitStream->Read(playerID->port);
}
GameServerRule * MasterCommon::DeserializeRule(BitStream *inputBitStream)
{
	char output[256];
	bool isAString;
	GameServerRule *newRule;

	newRule = new GameServerRule;

	stringCompressor->DecodeString(output, 256, inputBitStream);
	if (output[0]==0)
	{
#ifdef _DEBUG
		assert(0);
#endif
		return 0;
	}

	newRule->key = new char [strlen(output)+1];
	strcpy(newRule->key, output);
	if (inputBitStream->Read(isAString)==false)
	{
#ifdef _DEBUG
		assert(0);
#endif
		return 0;
	}

	if (isAString)
	{
		stringCompressor->DecodeString(output, 256, inputBitStream);
		if (output[0]==0)
		{
#ifdef _DEBUG
			assert(0);
#endif
			return 0;
		}
		newRule->stringValue = new char[strlen(output)+1];
		strcpy(newRule->stringValue, output);
	}
	else
	{
		if (inputBitStream->ReadCompressed(newRule->intValue)==false)
		{
#ifdef _DEBUG
			assert(0);
#endif
			return 0;
		}
	}

	return newRule;    
}
void MasterCommon::SerializeServer(GameServer *gameServer, BitStream *outputBitStream)
{
	unsigned serverIndex;
	unsigned short numberOfRulesToWrite;

	numberOfRulesToWrite=0;

	// Find out how many rules to write.
	for (serverIndex=0; serverIndex < gameServer->serverRules.Size(); serverIndex++)
	{
		// We don't write reserved identifiers
		if (IsReservedRuleIdentifier(gameServer->serverRules[serverIndex]->key)==false)
			numberOfRulesToWrite++;
	}

	// Write the server identifier
	SerializePlayerID(&(gameServer->connectionIdentifier), outputBitStream);

	// Write the number of rules
	outputBitStream->WriteCompressed(numberOfRulesToWrite);

	// Write all the rules
	for (serverIndex=0; serverIndex < gameServer->serverRules.Size(); serverIndex++)
	{
		if (IsReservedRuleIdentifier(gameServer->serverRules[serverIndex]->key))
			continue;

		SerializeRule(gameServer->serverRules[serverIndex], outputBitStream);
	}
}
GameServer * MasterCommon::DeserializeServer(BitStream *inputBitStream)
{
	unsigned serverIndex;
	unsigned short numberOfRulesToWrite;
	GameServer *gameServer;
	GameServerRule *gameServerRule;

	gameServer= new GameServer;
	DeserializePlayerID(&(gameServer->connectionIdentifier), inputBitStream);

	// Read the number of rules
	if (inputBitStream->ReadCompressed(numberOfRulesToWrite)==false)
	{
		delete gameServer;
		return 0;
	}
	// Read all the rules
	for (serverIndex=0; serverIndex < numberOfRulesToWrite; serverIndex++)
	{
		gameServerRule = DeserializeRule(inputBitStream);
		if (gameServerRule==0)
		{
			delete gameServer;
			return 0;
		}
		if (IsReservedRuleIdentifier(gameServerRule->key))
			delete gameServerRule;
		else
			gameServer->serverRules.Insert(gameServerRule);
	}
	return gameServer;
}

void MasterCommon::UpdateServer(GameServer *destination, GameServer *source, bool deleteSingleRules)
{
	unsigned sourceRuleIndex,destinationRuleIndex;

	destination->lastUpdateTime=RakNet::GetTime();

	// If (deleteSingleRules) then delete any rules that exist in the old and not in the new
	if (deleteSingleRules)
	{
		destinationRuleIndex=0;
		while (destinationRuleIndex < destination->serverRules.Size())
		{
			if (IsReservedRuleIdentifier(destination->serverRules[destinationRuleIndex]->key)==false &&
				source->FindKey(destination->serverRules[destinationRuleIndex]->key)==false)
			{
				delete destination->serverRules[destinationRuleIndex];
				destination->serverRules.RemoveAtIndex(destinationRuleIndex);
			}
			else
				destinationRuleIndex++;
		}
	}

	// Go through all the rules.
	for (sourceRuleIndex=0; sourceRuleIndex < source->serverRules.Size(); sourceRuleIndex++)
	{
		if (IsReservedRuleIdentifier(source->serverRules[sourceRuleIndex]->key))
			continue;

		// Add any fields that exist in the new and do not exist in the old
		// Update any fields that exist in both
		UpdateServerRule(destination, source->serverRules[sourceRuleIndex]->key, source->serverRules[sourceRuleIndex]->stringValue, source->serverRules[sourceRuleIndex]->intValue);
	}
}

GameServer* MasterCommon::UpdateServerList(GameServer *gameServer, bool deleteSingleRules, bool *newServerAdded)
{
	int searchIndex;

	if (gameServer==0)
	{
#ifdef _DEBUG
		assert(0);
#endif
		return 0;
	}

	// Find the existing game server that matches this port/address.
	searchIndex = gameServerList.GetIndexByPlayerID(gameServer->connectionIdentifier);
	if (searchIndex<0)
	{
		// If not found, then add it to the list.
		AddDefaultRulesToServer(gameServer, gameServer->connectionIdentifier);
		gameServerList.serverList.Insert(gameServer);
		*newServerAdded=true;
		return gameServer;
	}
	else
	{
		// Update the existing server
		UpdateServer(gameServerList.serverList[searchIndex], gameServer, deleteSingleRules);
		delete gameServer;
		*newServerAdded=false;
		return gameServerList.serverList[searchIndex];
	}
}

void MasterCommon::OnAttach(RakPeerInterface *peer)
{
	rakPeer=peer;
}