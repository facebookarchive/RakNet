/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "Lobby2Client.h"
#include "RakAssert.h"
#include "MessageIdentifiers.h"

using namespace RakNet;

Lobby2Client::Lobby2Client()
{
	serverAddress=RakNet::UNASSIGNED_SYSTEM_ADDRESS;
}
Lobby2Client::~Lobby2Client()
{

}
void Lobby2Client::SetServerAddress(SystemAddress addr)
{
	serverAddress=addr;
}
SystemAddress Lobby2Client::GetServerAddress(void) const
{
	return serverAddress;
}
void Lobby2Client::SendMsg(Lobby2Message *msg)
{
	// Callback must be ready to receive reply
	RakAssert(callbacks.Size());
	msg->resultCode=L2RC_PROCESSING;

	RakNet::BitStream bs;
	bs.Write((MessageID)ID_LOBBY2_SEND_MESSAGE);
	bs.Write((MessageID)msg->GetID());
	msg->Serialize(true,false,&bs);
	SendUnified(&bs,packetPriority, RELIABLE_ORDERED, orderingChannel, serverAddress, false);
}
void Lobby2Client::SendMsgAndDealloc(Lobby2Message *msg)
{
	SendMsg(msg);
	msgFactory->Dealloc(msg);
}
PluginReceiveResult Lobby2Client::OnReceive(Packet *packet)
{
	RakAssert(packet);

	switch (packet->data[0]) 
	{
	case ID_LOBBY2_SEND_MESSAGE:
		OnMessage(packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	}

	return RR_CONTINUE_PROCESSING;
}
void Lobby2Client::OnClosedConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
	(void)systemAddress;
	(void)rakNetGUID;
	(void)lostConnectionReason;
//	if (systemAddress==serverAddress)
//		ClearIgnoreList();
}
void Lobby2Client::OnShutdown(void)
{
//	ClearIgnoreList();
}
void Lobby2Client::OnMessage(Packet *packet)
{
	RakNet::BitStream bs(packet->data,packet->length,false);
	bs.IgnoreBytes(1); // ID_LOBBY2_SEND_MESSAGE
	MessageID msgId;
	bs.Read(msgId);
	Lobby2MessageID lobby2MessageID = (Lobby2MessageID) msgId;
	Lobby2Message *lobby2Message = msgFactory->Alloc(lobby2MessageID);
	if (lobby2Message)
	{
		lobby2Message->Serialize(false,true,&bs);
		if (lobby2Message->ClientImpl(this))
		{
			for (unsigned long i=0; i < callbacks.Size(); i++)
			{
				if (lobby2Message->callbackId==(uint32_t)-1 || lobby2Message->callbackId==callbacks[i]->callbackId)
					lobby2Message->CallCallback(callbacks[i]);
			}
		}
		msgFactory->Dealloc(lobby2Message);
	}
	else
	{
		RakAssert("Lobby2Client::OnMessage lobby2Message==0" && 0);
	}
}
/*
void Lobby2Client::AddToIgnoreList(RakNet::RakString user)
{
	ignoreList.Insert(user,user,false);
}
void Lobby2Client::RemoveFromIgnoreList(RakNet::RakString user)
{
	ignoreList.RemoveIfExists(user);
}
void Lobby2Client::SetIgnoreList(DataStructures::List<RakNet::RakString> users)
{
	for (unsigned int i=0; i < users.Size(); i++)
		ignoreList.Insert(users[i],users[i],false);
}
bool Lobby2Client::IsInIgnoreList(RakNet::RakString user) const
{
	return ignoreList.HasData(user);
}
void Lobby2Client::ClearIgnoreList(void)
{
	ignoreList.Clear(_FILE_AND_LINE_);
}
const DataStructures::OrderedList<RakNet::RakString, RakNet::RakString>* Lobby2Client::GetIgnoreList(void) const
{
	return &ignoreList;
}
*/