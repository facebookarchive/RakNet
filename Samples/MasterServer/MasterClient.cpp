/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "MasterClient.h"
#include "RakPeerInterface.h"
#include "PacketEnumerations.h"
#include "RakNetworkFactory.h"
#include "StringCompressor.h"
#include "GetTime.h"
#include <cstring>

// Uncomment this define for debugging printfs
#define _SHOW_MASTER_SERVER_PRINTF
#ifdef _SHOW_MASTER_SERVER_PRINTF
#include <cstdio>
#endif

MasterClient::MasterClient()
{
}

MasterClient::~MasterClient()
{
	ClearServerList();
}

bool MasterClient::Connect(char* host, int masterServerPort)
{
	localServer.Clear();
	listServer=serverListed=localServerModified=false;
	localServer.connectionIdentifier.port=rakPeer->GetInternalID().port;
	ruleIdentifierList.Reset();

	return rakPeer->Connect(host, masterServerPort, 0, 0);
}

void MasterClient::Disconnect(void)
{
	if (IsConnected())
		DelistServer();

	rakPeer->Disconnect(100);
}

bool MasterClient::IsConnected(void)
{
	unsigned short numberOfSystems;
	rakPeer->GetConnectionList(0, &numberOfSystems);
	return numberOfSystems==1;
}

void MasterClient::AddQueryRule(char *ruleIdentifier)
{
	if (ruleIdentifier && IsReservedRuleIdentifier(ruleIdentifier)==false)
		stringCompressor->EncodeString(ruleIdentifier, 256, &ruleIdentifierList);
}
void MasterClient::ClearQueryRules(void)
{
	ruleIdentifierList.Reset();
}
void MasterClient::QueryMasterServer(void)
{
	BitStream outgoingBitStream;
	// Request to the master server for the list of servers that contain at least one of the specified keys
	outgoingBitStream.Write((unsigned char)ID_QUERY_MASTER_SERVER);
	if (ruleIdentifierList.GetNumberOfBitsUsed()>0)
		outgoingBitStream.WriteBits(ruleIdentifierList.GetData(), ruleIdentifierList.GetNumberOfBitsUsed(), false);
    rakPeer->Send(&outgoingBitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, true);
}

void MasterClient::PingServers(void)
{
	unsigned serverIndex;

	for (serverIndex=0; serverIndex < gameServerList.serverList.Size(); serverIndex++)
	{
		rakPeer->Ping((char*)rakPeer->PlayerIDToDottedIP(gameServerList.serverList[serverIndex]->connectionIdentifier),
			gameServerList.serverList[serverIndex]->connectionIdentifier.port, false);
	}
}

void MasterClient::Update(RakPeerInterface *peer)
{
	BitStream outgoingBitStream;

	if (listServer && ((serverListed && localServerModified) || (serverListed==false)))
	{
		outgoingBitStream.Write((unsigned char)ID_MASTER_SERVER_SET_SERVER);
		SerializeServer(&localServer, &outgoingBitStream);
		rakPeer->Send(&outgoingBitStream, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, true);
		serverListed=true;
		localServerModified=false;
	}
}

bool MasterClient::OnReceive(RakPeerInterface *peer, Packet *packet)
{
	switch(packet->data[0])
	{
	case ID_NO_FREE_INCOMING_CONNECTIONS:
		OnMasterServerFull();
		return false; // Do not absorb packet
	case ID_DISCONNECTION_NOTIFICATION:
		OnLostConnection();
		return false; // Do not absorb packet
	case ID_CONNECTION_LOST:
		OnLostConnection();
		return false; // Do not absorb packet
	case ID_MODIFIED_PACKET:
		OnModifiedPacket();
		return false;
	case ID_CONNECTION_ATTEMPT_FAILED:
		OnConnectionAttemptFailed();
		return false; // Do not absorb packet
	case ID_MASTER_SERVER_UPDATE_SERVER:
		HandleServerListResponse(packet, false);
		return true; // Absorb packet
	case ID_MASTER_SERVER_SET_SERVER:
		HandleServerListResponse(packet, true);
		return true; // Absorb packet
	case ID_PONG:
		HandlePong(packet);
		return false; // Absorb packet
	case ID_RELAYED_CONNECTION_NOTIFICATION:
		HandleRelayedConnectionNotification(packet);
		return true; // Absorb packet
	}

	return 0;
}

void MasterClient::ConnectionAttemptNotification(char *serverIP, unsigned short serverPort)
{
	if (serverIP==0)
		return;

	BitStream bitStream(23);
	bitStream.Write((unsigned char)ID_RELAYED_CONNECTION_NOTIFICATION);
	bitStream.Write(localServer.connectionIdentifier.port); // Your own game client port
	bitStream.Write(serverPort); // The game server you are connecting to port
	stringCompressor->EncodeString(serverIP, 22, &bitStream); // The game server IP you are connecting to
	rakPeer->Send(&bitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true);
}
void MasterClient::ListServer(void)
{
	listServer=true;
}
void MasterClient::DelistServer(void)
{
	BitStream bitStream;
	listServer=false;
	if (serverListed)
	{
		bitStream.Write((unsigned char)ID_MASTER_SERVER_DELIST_SERVER);
		bitStream.Write(localServer.connectionIdentifier.port);
        rakPeer->Send(&bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, true);
		serverListed=false;
	}
}
void MasterClient::HandleServerListResponse(Packet *packet, bool overwriteExisting)
{
	int serverIndex;
	bool newServerAdded;
	unsigned short numberOfServers;
	GameServer *gameServer;
	RakNetTime currentTime;
	BitStream inputBitStream(packet->data, packet->length, false);
	inputBitStream.IgnoreBits(8*sizeof(unsigned char));
	
	if (inputBitStream.ReadCompressed(numberOfServers)==false)
		return;

	currentTime=RakNet::GetTime();

	for (serverIndex=0; serverIndex < numberOfServers; serverIndex++)
	{
		gameServer = DeserializeServer(&inputBitStream);

		// Find the existing game server that matches this port/address.
		// If not found, then add it to the list.
		// else update it
		// If (overwriteExisting)
		// - Delete any fields that exist in the old and not in the new
		// Add any fields that exist in the new and do not exist in the old
		// Update any fields that exist in both
		// Unset the deletion mark
		gameServer=UpdateServerList(gameServer,overwriteExisting, &newServerAdded);
		if (newServerAdded)
		{
			// Ping the new server
			rakPeer->Ping((char*)rakPeer->PlayerIDToDottedIP(gameServer->connectionIdentifier),
				gameServer->connectionIdentifier.port, false);

			// Returns true if new server updated
			OnGameServerListAddition(gameServer);
		}
		else
		{
			// returns false if an existing server is modified
			OnGameServerListRuleUpdate(gameServer);
		}


	}

	// Any servers that were not updated on the last call to UpdateServerList
	// will have lastUpdateTime time as less than the current time
	// Delete those
	serverIndex=0;
	while (serverIndex < (int) gameServerList.serverList.Size())
	{
		if (gameServerList.serverList[serverIndex]->lastUpdateTime < currentTime)
		{
			delete gameServerList.serverList[serverIndex];
			gameServerList.serverList.RemoveAtIndex(serverIndex);
		}
		else
			serverIndex++;
	}

	OnGameServerListQueryComplete();
}
void MasterClient::HandleRelayedConnectionNotification(Packet *packet)
{
	PlayerID clientSystem;
	BitStream incomingBitStream(packet->data, packet->length, false);
	incomingBitStream.IgnoreBits(8*sizeof(unsigned char));
	incomingBitStream.Read(clientSystem.binaryAddress);
	incomingBitStream.Read(clientSystem.port);
	
	OnConnectionRequest(rakPeer->PlayerIDToDottedIP(clientSystem), clientSystem.port);
}
void MasterClient::PostRule(char *ruleIdentifier, char *stringData, int intData)
{
	if (ruleIdentifier)
	{
		if (IsReservedRuleIdentifier(ruleIdentifier))
			return;

		localServerModified |= UpdateServerRule(&localServer, ruleIdentifier, stringData, intData);
	}	
}

void MasterClient::RemoveRule(char *ruleIdentifier)
{
	if (ruleIdentifier)
		localServerModified |= RemoveServerRule(&localServer, ruleIdentifier);
}

void MasterClient::OnLostConnection(void)
{
#ifdef _SHOW_MASTER_SERVER_PRINTF
	printf("Connection lost.\n");
#endif
}
void MasterClient::OnConnectionAttemptFailed(void)
{
#ifdef _SHOW_MASTER_SERVER_PRINTF
	printf("Connection attempt failed.\n");
#endif
}
void MasterClient::OnMasterServerFull(void)
{
#ifdef _SHOW_MASTER_SERVER_PRINTF
	printf("Server full.\n");
#endif
}
void MasterClient::OnModifiedPacket(void)
{
#ifdef _SHOW_MASTER_SERVER_PRINTF
	printf("Modified packet.\n");
#endif
}
void MasterClient::OnGameServerListAddition(GameServer *newServer)
{
#ifdef _SHOW_MASTER_SERVER_PRINTF
	printf("Server added.\n");
#endif
}
void MasterClient::OnGameServerListRuleUpdate(GameServer *updatedServer)
{
#ifdef _SHOW_MASTER_SERVER_PRINTF
	printf("Rules updated for a server.\n");
#endif
}
void MasterClient::OnGameServerListQueryComplete(void)
{
#ifdef _SHOW_MASTER_SERVER_PRINTF
	printf("Query complete.\n");
#endif
}
// Event when a game client wants to connect to our server
// You should call AdvertiseSystem to the passed IP and port from your game instance
void MasterClient::OnConnectionRequest(const char *clientIP, unsigned short clientPort)
{
#ifdef _SHOW_MASTER_SERVER_PRINTF
	printf("Master client indicates a connection request from %s:%i.\n", clientIP, clientPort);
#endif
	rakPeer->AdvertiseSystem((char*)clientIP, clientPort,0,0);
}
