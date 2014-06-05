/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "Lobby2Server.h"
#include "RakAssert.h"
#include "MessageIdentifiers.h"

//#define __INTEGRATE_LOBBY2_WITH_ROOMS_PLUGIN

#ifdef __INTEGRATE_LOBBY2_WITH_ROOMS_PLUGIN
#include "RoomsPlugin.h"
#endif

using namespace RakNet;

int Lobby2Server::UserCompByUsername( const RakString &key, Lobby2Server::User * const &data )
{
	if (key < data->userName)
		return -1;
	if (key==data->userName)
		return 0;
	return 1;
}

Lobby2Server::Lobby2Server()
{
	DataStructures::OrderedList<SystemAddress, SystemAddress>::IMPLEMENT_DEFAULT_COMPARISON();
	DataStructures::OrderedList<RakString, RakString>::IMPLEMENT_DEFAULT_COMPARISON();
	roomsPlugin=0;
	roomsPluginAddress=UNASSIGNED_SYSTEM_ADDRESS;
}
Lobby2Server::~Lobby2Server()
{
	Clear();
}
void Lobby2Server::SendMsg(Lobby2Message *msg, const DataStructures::List<SystemAddress> &recipients)
{
	RakNet::BitStream bs;
	bs.Write((MessageID)ID_LOBBY2_SEND_MESSAGE);
	bs.Write((MessageID)msg->GetID());
	msg->Serialize(true,true,&bs);
	SendUnifiedToMultiple(&bs,packetPriority, RELIABLE_ORDERED, orderingChannel, recipients);
}
void Lobby2Server::Update(void)
{
	while (threadActionQueue.Size())
	{
		threadActionQueueMutex.Lock();
		if (threadActionQueue.Size())
		{
			ThreadAction ta = threadActionQueue.Pop();
			threadActionQueueMutex.Unlock();
			if (ta.action==L2MID_Client_Logoff)
			{
				OnLogoff(&ta.command, false);
			}
			else if (ta.action==L2MID_Client_Login)
			{
				OnLogin(&ta.command, false);
			}
			else if (ta.action==L2MID_Client_ChangeHandle)
			{
				OnChangeHandle(&ta.command, false);
			}
		}
		else
		{
			threadActionQueueMutex.Unlock();
			break;
		}
	}

	if (threadPool.HasOutputFast() && threadPool.HasOutput())
	{
		Lobby2ServerCommand c = threadPool.GetOutput();
		c.lobby2Message->ServerPostDBMemoryImpl(this, c.callingUserName);
		if (c.returnToSender)
		{
			for (unsigned long i=0; i < callbacks.Size(); i++)
			{
				if (c.lobby2Message->callbackId==(uint32_t)-1 || c.lobby2Message->callbackId==callbacks[i]->callbackId)
					c.lobby2Message->CallCallback(callbacks[i]);
			}


			RakNet::BitStream bs;
			bs.Write((MessageID)ID_LOBBY2_SEND_MESSAGE);
			bs.Write((MessageID)c.lobby2Message->GetID());
			c.lobby2Message->Serialize(true,true,&bs);
			// Have the ID to send to, but not the address. The ID came from the thread, such as notifying another user
			if (c.callerSystemAddresses.Size()==0)
			{
				unsigned int i;
				if (c.callerUserId!=0)
				{
					for (i=0; i < users.Size(); i++)
					{
						if (users[i]->callerUserId==c.callerUserId)
						{
							c.callerSystemAddresses=users[i]->systemAddresses;
							c.callerGuids=users[i]->guids;		

							/*
							if (c.requiredConnectionAddress!=UNASSIGNED_SYSTEM_ADDRESS)
							{
								// This message refers to another user that has to be logged on for it to be sent
								bool objectExists;
								unsigned int idx;
								idx = users.GetIndexFromKey(c.callerSystemAddress,&objectExists);
								if (objectExists==false)
								{
									if (c.deallocMsgWhenDone)
										RakNet::OP_DELETE(c.lobby2Message, __FILE__, __LINE__);
									return;
								}
							}
							*/
							break;
						}
					}
				}
				if (c.callerSystemAddresses.Size()==0 && c.callingUserName.IsEmpty()==false)
				{
					for (i=0; i < users.Size(); i++)
					{
						if (users[i]->callerUserId==c.callerUserId)
						{
							c.callerSystemAddresses=users[i]->systemAddresses;
							c.callerGuids=users[i]->guids;
							break;
						}
					}
				}
			}
			else
			{
				bool objectExists;
				unsigned int idx;
				idx = users.GetIndexFromKey(c.callingUserName,&objectExists);
				if (objectExists && 
					c.callingUserName.IsEmpty()==false &&
					users[idx]->userName!=c.callingUserName)
				{
					// Different user, same IP address. Abort the send.
					if (c.deallocMsgWhenDone)
						RakNet::OP_DELETE(c.lobby2Message, __FILE__, __LINE__);
					return;
				}
			}
			SendUnifiedToMultiple(&bs,packetPriority, RELIABLE_ORDERED, orderingChannel, c.callerSystemAddresses);
		}
		if (c.deallocMsgWhenDone)
			RakNet::OP_DELETE(c.lobby2Message, __FILE__, __LINE__);
	}
}
PluginReceiveResult Lobby2Server::OnReceive(Packet *packet)
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
void Lobby2Server::OnShutdown(void)
{
	Clear();
}
void Lobby2Server::OnClosedConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
	(void)rakNetGUID;
	(void)lostConnectionReason;

	unsigned int index = GetUserIndexBySystemAddress(systemAddress);

	// If systemAddress is a user, then notify his friends about him logging off
	if (index!=-1)
	{
		bool found=false;
		User *user = users[index];
		for (unsigned int i=0; i < user->systemAddresses.Size(); i++)
		{
			if (user->systemAddresses[i]==systemAddress)
			{
				found=true;
				user->systemAddresses.RemoveAtIndexFast(i);
				break;
			}
		}

		if (found && user->systemAddresses.Size()==0)
		{
			// Log this logoff due to closed connection
			Lobby2Message *lobby2Message = msgFactory->Alloc(L2MID_Client_Logoff);
			Lobby2ServerCommand command;
			command.lobby2Message=lobby2Message;
			command.deallocMsgWhenDone=true;
			command.returnToSender=true;
			command.callerUserId=users[index]->callerUserId;
			command.server=this;
			ExecuteCommand(&command);

			RemoveUser(index);
		}		
	}
}
void Lobby2Server::OnMessage(Packet *packet)
{
	RakNet::BitStream bs(packet->data,packet->length,false);
	bs.IgnoreBytes(1); // ID_LOBBY2_SEND_MESSAGE
	MessageID msgId;
	bs.Read(msgId);
	Lobby2MessageID lobby2MessageID = (Lobby2MessageID) msgId;
	unsigned int index;
	Lobby2Message *lobby2Message = msgFactory->Alloc(lobby2MessageID);
	if (lobby2Message)
	{
		lobby2Message->Serialize(false,false,&bs);
		Lobby2ServerCommand command;
		command.lobby2Message=lobby2Message;
		command.deallocMsgWhenDone=true;
		command.returnToSender=true;
		index=GetUserIndexBySystemAddress(packet->systemAddress);
		if (index!=-1)
		{
			command.callingUserName=users[index]->userName;
			command.callerUserId=users[index]->callerUserId;
		}
		else
		{
			if (lobby2Message->RequiresLogin())
			{
				RakNet::BitStream bs;
				bs.Write((MessageID)ID_LOBBY2_SEND_MESSAGE);
				bs.Write((MessageID)lobby2Message->GetID());
				lobby2Message->resultCode=L2RC_NOT_LOGGED_IN;
				lobby2Message->Serialize(true,true,&bs);
				SendUnified(&bs,packetPriority, RELIABLE_ORDERED, orderingChannel, packet->systemAddress, false);
				return;
			}
			command.callerUserId=0;
		}
		command.callerSystemAddresses.Push(packet->systemAddress,__FILE__,__LINE__);
		command.callerGuids.Push(packet->guid,__FILE__,__LINE__);
		command.server=this;
		ExecuteCommand(&command);
	}
	else
	{
		RakNet::BitStream out;
		out.Write((MessageID)ID_LOBBY2_SERVER_ERROR);
		out.Write((unsigned char) L2SE_UNKNOWN_MESSAGE_ID);
		out.Write((unsigned int) msgId);
		SendUnified(&bs,packetPriority, RELIABLE_ORDERED, orderingChannel, packet->systemAddress, false);
	}
}
void Lobby2Server::Clear(void)
{
	ClearAdminAddresses();
	ClearRankingAddresses();
	ClearUsers();
	ClearConnections();

	threadPool.StopThreads();
	RakAssert(threadPool.NumThreadsWorking()==0);

	unsigned i;
	Lobby2ServerCommand c;
	for (i=0; i < threadPool.InputSize(); i++)
	{
		c = threadPool.GetInputAtIndex(i);
		if (c.deallocMsgWhenDone && c.lobby2Message)
			RakNet::OP_DELETE(c.lobby2Message, __FILE__, __LINE__);
	}
	threadPool.ClearInput();
	for (i=0; i < threadPool.OutputSize(); i++)
	{
		c = threadPool.GetOutputAtIndex(i);
		if (c.deallocMsgWhenDone && c.lobby2Message)
			RakNet::OP_DELETE(c.lobby2Message, __FILE__, __LINE__);
	}
	threadPool.ClearOutput();

	threadActionQueueMutex.Lock();
	threadActionQueue.Clear(__FILE__, __LINE__);
	threadActionQueueMutex.Unlock();
}
void Lobby2Server::AddAdminAddress(SystemAddress addr)
{
	adminAddresses.Insert(addr,addr,false, __FILE__, __LINE__ );
}
bool Lobby2Server::HasAdminAddress(const DataStructures::List<SystemAddress> &addresses)
{
	if (addresses.Size()==0)
		return true;

	unsigned int j;
	for (j=0; j < addresses.Size(); j++)
	{
		if (adminAddresses.HasData(addresses[j]))
			return true;
	}
	return false;
}
void Lobby2Server::RemoveAdminAddress(SystemAddress addr)
{
	adminAddresses.RemoveIfExists(addr);
}
void Lobby2Server::ClearAdminAddresses(void)
{
	adminAddresses.Clear(false, __FILE__, __LINE__);
}
void Lobby2Server::AddRankingAddress(SystemAddress addr)
{
	rankingAddresses.Insert(addr,addr,false, __FILE__, __LINE__ );
}
bool Lobby2Server::HasRankingAddress(const DataStructures::List<SystemAddress> &addresses)
{
	if (addresses.Size()==0)
		return true;

	unsigned int j;
	for (j=0; j < addresses.Size(); j++)
	{
		if (rankingAddresses.HasData(addresses[j]))
			return true;
	}
	return false;
}
void Lobby2Server::RemoveRankingAddress(SystemAddress addr)
{
	rankingAddresses.RemoveIfExists(addr);
}
void Lobby2Server::ClearRankingAddresses(void)
{
	rankingAddresses.Clear(false, __FILE__, __LINE__);
}
void Lobby2Server::ExecuteCommand(Lobby2ServerCommand *command)
{
	//RakNet::BitStream out;
	if (command->lobby2Message->PrevalidateInput()==false)
	{
		SendMsg(command->lobby2Message, command->callerSystemAddresses);
		if (command->deallocMsgWhenDone)
			msgFactory->Dealloc(command->lobby2Message);
		return;
	}

	if (command->lobby2Message->RequiresAdmin() && HasAdminAddress(command->callerSystemAddresses)==false)
	{
		command->lobby2Message->resultCode=L2RC_REQUIRES_ADMIN;
		SendMsg(command->lobby2Message, command->callerSystemAddresses);
		//SendUnifiedToMultiple(&out,packetPriority, RELIABLE_ORDERED, orderingChannel, command->callerSystemAddresses);
		if (command->deallocMsgWhenDone)
			msgFactory->Dealloc(command->lobby2Message);
		return;
	}

	if (command->lobby2Message->RequiresRankingPermission() && HasRankingAddress(command->callerSystemAddresses)==false)
	{
		command->lobby2Message->resultCode=L2RC_REQUIRES_ADMIN;
		SendMsg(command->lobby2Message, command->callerSystemAddresses);
		//SendUnifiedToMultiple(&out,packetPriority, RELIABLE_ORDERED, orderingChannel, command->callerSystemAddresses);
		if (command->deallocMsgWhenDone)
			msgFactory->Dealloc(command->lobby2Message);
		return;
	}

	if (command->lobby2Message->ServerPreDBMemoryImpl(this, command->callingUserName)==true)
	{
		SendMsg(command->lobby2Message, command->callerSystemAddresses);
		if (command->deallocMsgWhenDone)
			msgFactory->Dealloc(command->lobby2Message);
		return;
	}

	command->server=this;
	AddInputCommand(*command);
}
void Lobby2Server::SetRoomsPlugin(RoomsPlugin *rp)
{
	roomsPlugin=rp;
	roomsPluginAddress=UNASSIGNED_SYSTEM_ADDRESS;
}
void Lobby2Server::SetRoomsPluginAddress(SystemAddress address)
{
	roomsPluginAddress=address;
	roomsPlugin=0;
}
void Lobby2Server::ClearUsers(void)
{
	unsigned int i;
	for (i=0; i < users.Size(); i++)
		RakNet::OP_DELETE(users[i], __FILE__, __LINE__);
	users.Clear(false, __FILE__, __LINE__);
}
void Lobby2Server::LogoffFromRooms(User *user)
{
	// Remove from the room too
#if defined(__INTEGRATE_LOBBY2_WITH_ROOMS_PLUGIN)
	// Tell the rooms plugin about the logoff event
	if (roomsPlugin)
	{
		roomsPlugin->LogoffRoomsParticipant(user->userName, UNASSIGNED_SYSTEM_ADDRESS);
	}
	else if (roomsPluginAddress!=UNASSIGNED_SYSTEM_ADDRESS)
	{
		RakNet::BitStream bs;
		RoomsPlugin::SerializeLogoff(user->userName,&bs);
		SendUnified(&bs,packetPriority, RELIABLE_ORDERED, orderingChannel, roomsPluginAddress, false);
	}
#endif

}
void Lobby2Server::SendRemoteLoginNotification(RakNet::RakString handle, const DataStructures::List<SystemAddress>& recipients)
{
	Notification_Client_RemoteLogin notification;
	notification.handle=handle;
	notification.resultCode=L2RC_SUCCESS;
	SendMsg(&notification, recipients);
}
void Lobby2Server::OnLogin(Lobby2ServerCommand *command, bool calledFromThread)
{
	if (calledFromThread)
	{
		ThreadAction ta;
		ta.action=L2MID_Client_Login;
		ta.command=*command;
		threadActionQueueMutex.Lock();
		threadActionQueue.Push(ta, __FILE__, __LINE__ );
		threadActionQueueMutex.Unlock();
		return;
	}

	bool objectExists;
	unsigned int insertionIndex = users.GetIndexFromKey(command->callingUserName, &objectExists);
	if (objectExists)
	{
		User * user = users[insertionIndex];
		if (user->allowMultipleLogins==false)
		{
			SendRemoteLoginNotification(user->userName, user->systemAddresses);
			LogoffFromRooms(user);

			// Already logged in from this system address.
			// Delete the existing entry, which will be reinserted.
			RakNet::OP_DELETE(user,_FILE_AND_LINE_);
			users.RemoveAtIndex(insertionIndex);
		}
		else
		{
			if (user->systemAddresses.GetIndexOf(command->callerSystemAddresses[0])==(unsigned int) -1)
			{
				// Just add system address and guid already in use to the list for this user
				user->systemAddresses.Push(command->callerSystemAddresses[0], __FILE__, __LINE__);
				user->guids.Push(command->callerGuids[0], __FILE__, __LINE__);
			}
			return;
		}
	}
	else
	{
		// Different username, from the same IP address or RakNet instance
		unsigned int idx2 = GetUserIndexByGUID(command->callerGuids[0]);
		unsigned int idx3 = GetUserIndexBySystemAddress(command->callerSystemAddresses[0]);
		if (idx2!=(unsigned int) -1)
		{
			User * user = users[idx2];
			if (user->allowMultipleLogins==true)
				return;
			SendRemoteLoginNotification(user->userName, user->systemAddresses);
			LogoffFromRooms(user);

			RakNet::OP_DELETE(user,__FILE__,__LINE__);
			users.RemoveAtIndex(idx2);

			insertionIndex = users.GetIndexFromKey(command->callingUserName, &objectExists);
		}
		else if (idx3!=(unsigned int) -1)
		{
			User * user = users[idx3];
			if (user->allowMultipleLogins==true)
				return;
			SendRemoteLoginNotification(user->userName, user->systemAddresses);
			LogoffFromRooms(user);

			RakNet::OP_DELETE(user,__FILE__,__LINE__);
			users.RemoveAtIndex(idx3);

			insertionIndex = users.GetIndexFromKey(command->callingUserName, &objectExists);
		}
	}


	User *user = RakNet::OP_NEW<User>( __FILE__, __LINE__ );
	user->userName=command->callingUserName;
	user->systemAddresses=command->callerSystemAddresses;
	user->guids=command->callerGuids;
	user->callerUserId=command->callerUserId;
	user->allowMultipleLogins=((Client_Login*)command->lobby2Message)->allowMultipleLogins;
	users.InsertAtIndex(user, insertionIndex, __FILE__, __LINE__ );

#if defined(__INTEGRATE_LOBBY2_WITH_ROOMS_PLUGIN)
	// Tell the rooms plugin about the login event
	if (roomsPlugin)
	{
		roomsPlugin->LoginRoomsParticipant(user->userName, user->systemAddresses[0], user->guids[0], UNASSIGNED_SYSTEM_ADDRESS);
	}
	else if (roomsPluginAddress!=UNASSIGNED_SYSTEM_ADDRESS)
	{
		RakNet::BitStream bs;
		RoomsPlugin::SerializeLogin(user->userName,user->systemAddresses[0], user->guids[0], &bs);
		SendUnified(&bs,packetPriority, RELIABLE_ORDERED, orderingChannel, roomsPluginAddress, false);
	}
#endif
}
void Lobby2Server::OnLogoff(Lobby2ServerCommand *command, bool calledFromThread)
{
	if (calledFromThread)
	{
		ThreadAction ta;
		ta.action=L2MID_Client_Logoff;
		ta.command=*command;
		threadActionQueueMutex.Lock();
		threadActionQueue.Push(ta, __FILE__, __LINE__ );
		threadActionQueueMutex.Unlock();
		return;
	}
	RemoveUser(command->callingUserName);
}
void Lobby2Server::OnChangeHandle(Lobby2ServerCommand *command, bool calledFromThread)
{
	if (calledFromThread)
	{
		ThreadAction ta;
		ta.action=L2MID_Client_ChangeHandle;
		ta.command=*command;
		threadActionQueueMutex.Lock();
		threadActionQueue.Push(ta, __FILE__, __LINE__ );
		threadActionQueueMutex.Unlock();
		return;
	}

	unsigned int i;
	RakNet::RakString oldHandle;
	for (i=0; i < users.Size(); i++)
	{
		if (users[i]->callerUserId==command->callerUserId)
		{
			oldHandle=users[i]->userName;
			users[i]->userName=command->callingUserName;
			break;
		}
	}

	if (oldHandle.IsEmpty())
		return;

#if defined(__INTEGRATE_LOBBY2_WITH_ROOMS_PLUGIN)
	// Tell the rooms plugin about the handle change
	if (roomsPlugin)
	{
		roomsPlugin->ChangeHandle(oldHandle, command->callingUserName);
	}
	else if (roomsPluginAddress!=UNASSIGNED_SYSTEM_ADDRESS)
	{
		RakNet::BitStream bs;
		RoomsPlugin::SerializeChangeHandle(oldHandle,command->callingUserName,&bs);
		SendUnified(&bs,packetPriority, RELIABLE_ORDERED, orderingChannel, roomsPluginAddress, false);
	}
#endif
}
void Lobby2Server::RemoveUser(RakString userName)
{
	bool objectExists;
	unsigned int index = users.GetIndexFromKey(userName, &objectExists);
	if (objectExists)
		RemoveUser(index);
}
void Lobby2Server::RemoveUser(unsigned int index)
{
	User *user = users[index];

	Lobby2ServerCommand command;
	Notification_Friends_StatusChange *notification = (Notification_Friends_StatusChange *) GetMessageFactory()->Alloc(L2MID_Notification_Friends_StatusChange);
	notification->otherHandle=user->userName;
	notification->op=Notification_Friends_StatusChange::FRIEND_LOGGED_OFF;
	notification->resultCode=L2RC_SUCCESS;
	command.server=this;
	command.deallocMsgWhenDone=true;
	command.lobby2Message=notification;
	command.callerUserId=user->callerUserId;
	command.callingUserName=user->userName;
	ExecuteCommand(&command);

	unsigned i;
	i=0;
	threadPool.LockInput();
	while (i < threadPool.InputSize())
	{
		Lobby2ServerCommand command;
		command = threadPool.GetInputAtIndex(i);
		if (command.lobby2Message->CancelOnDisconnect()&& command.callerSystemAddresses.Size()>0 && user->systemAddresses.GetIndexOf(command.callerSystemAddresses[0])!=(unsigned int)-1)
		{
			if (command.deallocMsgWhenDone)
				RakNet::OP_DELETE(command.lobby2Message, __FILE__, __LINE__);
			threadPool.RemoveInputAtIndex(i);
		}
		else
			i++;
	}
	threadPool.UnlockInput();
	LogoffFromRooms(user);

	RakNet::OP_DELETE(user,__FILE__,__LINE__);
	users.RemoveAtIndex(index);

}
unsigned int Lobby2Server::GetUserIndexBySystemAddress(SystemAddress systemAddress) const
{
	unsigned int idx1,idx2;
	for (idx1=0; idx1 < users.Size(); idx1++)
	{
		for (idx2=0; idx2 < users[idx1]->systemAddresses.Size(); idx2++)
		{
			if (users[idx1]->systemAddresses[idx2]==systemAddress)
				return idx1;
		}
	}
	return (unsigned int) -1;
}
unsigned int Lobby2Server::GetUserIndexByGUID(RakNetGUID guid) const
{
	unsigned int idx1,idx2;
	for (idx1=0; idx1 < users.Size(); idx1++)
	{
		for (idx2=0; idx2 < users[idx1]->guids.Size(); idx2++)
		{
			if (users[idx1]->guids[idx2]==guid)
				return idx1;
		}
	}
	return (unsigned int) -1;
}
unsigned int Lobby2Server::GetUserIndexByUsername(RakNet::RakString userName) const
{
	unsigned int idx;
	bool objectExists;
	idx = users.GetIndexFromKey(userName,&objectExists);
	if (objectExists)
		return idx;
	return (unsigned int) -1;
}
void Lobby2Server::StopThreads(void)
{
	threadPool.StopThreads();
}
void Lobby2Server::SetConfigurationProperties(ConfigurationProperties c)
{
	configurationProperties=c;
}
const Lobby2Server::ConfigurationProperties *Lobby2Server::GetConfigurationProperties(void) const
{
	return &configurationProperties;
}
void Lobby2Server::GetUserOnlineStatus(UsernameAndOnlineStatus &userInfo) const
{
	unsigned int idx = GetUserIndexByUsername(userInfo.handle);
	if (idx!=-1)
	{
		userInfo.isOnline=true;
		userInfo.presence=users[idx]->presence;
	}
	else
	{
		userInfo.isOnline=false;
		userInfo.presence.status=Lobby2Presence::NOT_ONLINE;
		userInfo.presence.isVisible=false;
	}
}
void Lobby2Server::SetPresence(const RakNet::Lobby2Presence &presence, RakNet::RakString userHandle)
{
	unsigned int index = GetUserIndexByUsername(userHandle);

	if (index!=-1)
	{
		User *user = users[index];
		user->presence=presence;

		// Push notify presence update to friends
		Lobby2ServerCommand command;
		Notification_Friends_PresenceUpdate *notification = (Notification_Friends_PresenceUpdate *) GetMessageFactory()->Alloc(L2MID_Notification_Friends_PresenceUpdate);
		notification->newPresence=presence;
		notification->otherHandle=user->userName;
		notification->resultCode=L2RC_SUCCESS;
		command.server=this;
		command.deallocMsgWhenDone=true;
		command.lobby2Message=notification;
		command.callerUserId=user->callerUserId;
		command.callingUserName=user->userName;
		ExecuteCommand(&command);
	}
}
void Lobby2Server::GetPresence(RakNet::Lobby2Presence &presence, RakNet::RakString userHandle)
{
	unsigned int userIndex = GetUserIndexByUsername(userHandle);
	if (userIndex!=-1)
	{
		presence=users[userIndex]->presence;
	}
	else
	{
		presence.status=Lobby2Presence::NOT_ONLINE;
	}
}
void Lobby2Server::SendUnifiedToMultiple( const RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const DataStructures::List<SystemAddress> systemAddresses )
{
	for (unsigned int i=0; i < systemAddresses.Size(); i++)
		SendUnified(bitStream,priority,reliability,orderingChannel,systemAddresses[i],false);
}
