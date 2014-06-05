/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "RakPeerInterface.h"
#include "BitStream.h"
#include "GetTime.h"
#include "RakSleep.h"
#include "Gets.h"
#include "MessageIdentifiers.h"
#include "Kbhit.h"
#include "ReplicaManager3.h"
#include "RakPeerInterface.h"
#include "NetworkIDManager.h"

using namespace RakNet;

// Purpose: UDP network communication
// Required?: Yes
RakPeerInterface *rakPeer;

// Purpose: Game object replication
// Required?: No, but manages object replication automatically. Some form of this is required for almost every game
ReplicaManager3 *replicaManager3;

// Purpose: Lookup game objects given ID. Used by ReplicaManager3
// Required?: Required to use ReplicaManager3, and some form of this is required for almost every game
NetworkIDManager *networkIDManager;

bool runningAsServer;

class User : public Replica3
{
public:
	User() {score=0;}
	virtual ~User() {}
	virtual void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const {allocationIdBitstream->Write("User");}
	virtual RM3ConstructionState QueryConstruction(RakNet::Connection_RM3 *destinationConnection, ReplicaManager3 *replicaManager3)	{
		return QueryConstruction_ServerConstruction(destinationConnection, runningAsServer);
	}
	virtual bool QueryRemoteConstruction(RakNet::Connection_RM3 *sourceConnection) {return QueryRemoteConstruction_ServerConstruction(sourceConnection, runningAsServer);}
	static void SerializeToBitStream(User *user, BitStream *bitStream)
	{
		bitStream->Write(user->score);
		bitStream->Write(user->username);
		bitStream->Write(user->guid);
	}
	static void DeserializeToBitStream(User *user, BitStream *bitStream)
	{
		bitStream->Read(user->score);
		bitStream->Read(user->username);
		bitStream->Read(user->guid);
	}
	virtual void SerializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection) {
		SerializeToBitStream(this, constructionBitstream);
	}
	virtual bool DeserializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection) {
		DeserializeToBitStream(this, constructionBitstream);
		if (guid==rakPeer->GetMyGUID())
			printf("My user created with name %s and score %i\n", username.C_String(), score);
		else
			printf("Another user created with name %s and score %i\n", username.C_String(), score);
		return true;
	}

	virtual void SerializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *destinationConnection) {}
	virtual bool DeserializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *sourceConnection) {return true;}
	virtual RakNet::RM3ActionOnPopConnection QueryActionOnPopConnection(RakNet::Connection_RM3 *droppedConnection) const {
		if (runningAsServer)
			return QueryActionOnPopConnection_Server(droppedConnection);
		else
			return QueryActionOnPopConnection_Client(droppedConnection);}
	virtual void DeallocReplica(RakNet::Connection_RM3 *sourceConnection) {delete this;}
	virtual RakNet::RM3QuerySerializationResult QuerySerialization(RakNet::Connection_RM3 *destinationConnection) {return QuerySerialization_ServerSerializable(destinationConnection, runningAsServer);}
	virtual RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters) {
		SerializeToBitStream(this, &serializeParameters->outputBitstream[0]);
		return RM3SR_BROADCAST_IDENTICALLY;
	}
	virtual void Deserialize(RakNet::DeserializeParameters *deserializeParameters) {
		if (deserializeParameters->bitstreamWrittenTo[0])
			DeserializeToBitStream(this, &deserializeParameters->serializationBitstream[0]);
	}

	static void DeleteUserWithGuid(RakNetGUID guid)
	{
		for (unsigned int i=0; i < replicaManager3->GetReplicaCount(); i++)
		{
			User *u = (User *) replicaManager3->GetReplicaAtIndex(i);
			if (u->guid==guid)
			{
				u->BroadcastDestruction();
				replicaManager3->Dereference(u);
				delete u;
				break;
			}
		}
	}
	virtual void PreDestruction(RakNet::Connection_RM3 *sourceConnection) {
		if (guid==rakPeer->GetMyGUID())
			printf("My user destroyed with name %s and score %i\n", username.C_String(), score);
		else
			printf("Another user destroyed with name %s and score %i\n", username.C_String(), score);
	}

	int score;
	RakString username;
	RakNetGUID guid;
};

// Required by ReplicaManager3. Acts as a class factory for Replica3 derived instances
class SampleConnectionRM3 : public Connection_RM3
{
public:
	SampleConnectionRM3(const SystemAddress &_systemAddress, RakNetGUID _guid) : Connection_RM3(_systemAddress, _guid) {}
	virtual ~SampleConnectionRM3() {}
	virtual Replica3 *AllocReplica(RakNet::BitStream *allocationIdBitstream, ReplicaManager3 *replicaManager3)
	{
		RakString objectType;
		// Types are written by WriteAllocationID()
		allocationIdBitstream->Read(objectType);
		if (objectType=="User") return new User;
		RakAssert("Unknown type in AllocReplica" && 0);
		return 0;
	}
};

// Required by ReplicaManager3. Acts as a class factory for Connection_RM3 derived instances
class SampleRM3 : public ReplicaManager3
{
public:
	SampleRM3() {}
	virtual ~SampleRM3() {}
	virtual Connection_RM3* AllocConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID) const {return new SampleConnectionRM3(systemAddress,rakNetGUID);}
	virtual void DeallocConnection(Connection_RM3 *connection) const {delete connection;}
};

int serverMain(void);
int clientMain(void);

int main(void)
{
	rakPeer=RakNet::RakPeerInterface::GetInstance();
	networkIDManager = NetworkIDManager::GetInstance();
	replicaManager3=new SampleRM3;

	// ---------------------------------------------------------------------------------------------------------------------
	// Attach plugins
	// ---------------------------------------------------------------------------------------------------------------------
	rakPeer->AttachPlugin(replicaManager3);

	// Tell ReplicaManager3 which networkIDManager to use for object lookup, used for automatic serialization
	replicaManager3->SetNetworkIDManager(networkIDManager);

	printf("(S)erver or (C)lient?\n");
	char ch = getch();
	if (ch=='s' || ch=='S')
		return serverMain();
	else
		return clientMain();
}

int serverMain(void)
{
	runningAsServer=true;

	RakNet::SocketDescriptor sd;
	sd.port=60000;
	StartupResult sr = rakPeer->Startup(8,&sd,1);
	RakAssert(sr==RAKNET_STARTED);
	rakPeer->SetMaximumIncomingConnections(8);
	rakPeer->SetTimeoutTime(30000,RakNet::UNASSIGNED_SYSTEM_ADDRESS);

	printf("Waiting for connections\n");

	Packet *packet;
	while (1)
	{
		for (packet = rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet = rakPeer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_NEW_INCOMING_CONNECTION:
					printf("ID_NEW_INCOMING_CONNECTION from %s. guid=%s.\n", packet->systemAddress.ToString(true), packet->guid.ToString());
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
					printf("ID_CONNECTION_REQUEST_ACCEPTED from %s,guid=%s\n", packet->systemAddress.ToString(true), packet->guid.ToString());
				break;
			case ID_CONNECTION_LOST:
				printf("ID_CONNECTION_LOST from %s,guid=%s\n", packet->systemAddress.ToString(true), packet->guid.ToString());
				User::DeleteUserWithGuid(packet->guid);
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				printf("ID_DISCONNECTION_NOTIFICATION from %s,guid=%s\n", packet->systemAddress.ToString(true), packet->guid.ToString());
				User::DeleteUserWithGuid(packet->guid);
				break;
			// Login
			case ID_USER_PACKET_ENUM:
				{
					BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(MessageID));
					RakString username;
					bsIn.Read(username);
					printf("Accepting login from %s\n", username.C_String());

					User *newUser = new User;
					newUser->score=0;
					newUser->guid=packet->guid;
					newUser->username = username;
					replicaManager3->Reference(newUser);
				}
				break;
			}
		}
		RakSleep(30);
	}
}

int clientMain(void)
{
	runningAsServer=false;

	RakNet::SocketDescriptor sd;
	sd.port=0;
	StartupResult sr = rakPeer->Startup(1,&sd,1);
	RakAssert(sr==RAKNET_STARTED);
	rakPeer->SetTimeoutTime(30000,RakNet::UNASSIGNED_SYSTEM_ADDRESS);

	rakPeer->Connect("127.0.0.1", 60000, 0, 0);
	printf("Connecting to server...\n");

	Packet *packet;
	while (1)
	{
		for (packet = rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet = rakPeer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_NEW_INCOMING_CONNECTION:
				printf("ID_NEW_INCOMING_CONNECTION from %s. guid=%s.\n", packet->systemAddress.ToString(true), packet->guid.ToString());
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				{
					printf("ID_CONNECTION_REQUEST_ACCEPTED from %s,guid=%s\n", packet->systemAddress.ToString(true), packet->guid.ToString());
					printf("Logging in...\n");
					BitStream bsOut;
					// Login
					bsOut.WriteCasted<MessageID>(ID_USER_PACKET_ENUM);
					RakString username("User %s", rakPeer->GetMyGUID().ToString());
					bsOut.Write(username);
					rakPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->guid, false);
				}
				break;
			case ID_CONNECTION_LOST:
				printf("ID_CONNECTION_LOST from %s,guid=%s\n", packet->systemAddress.ToString(true), packet->guid.ToString());
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				printf("ID_DISCONNECTION_NOTIFICATION from %s,guid=%s\n", packet->systemAddress.ToString(true), packet->guid.ToString());
				break;
			}
		}
		RakSleep(30);
	}
}