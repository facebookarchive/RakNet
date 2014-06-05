/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __LOBBY_2_CLIENT_H
#define __LOBBY_2_CLIENT_H

#include "Lobby2Plugin.h"
#include "DS_OrderedList.h"

namespace RakNet
{

struct Lobby2Message;

/// \brief Class used to send commands to Lobby2Server
/// \details The lobby system works by sending implementations of Lobby2Message from Lobby2Client to Lobby2Server, and getting the results via Lobby2Client::SetCallbackInterface()<BR>
/// The client itself is a thin shell that does little more than call Serialize on the messages.<BR>
/// To use:<BR>
/// <OL>
/// <LI>Call Lobby2Client::SetServerAddress() after connecting to the system running Lobby2Server.
/// <LI>Instantiate an instance of RakNet::Lobby2MessageFactory and register it with RakNet::Lobby2Plugin::SetMessageFactory() (the base class of Lobby2Client)
/// <LI>Call messageFactory.Alloc(command); where command is one of the Lobby2MessageID enumerations.
/// <LI>Instantiate a (probably derived) instance of Lobby2Callbacks and register it with Lobby2Client::SetCallbackInterface()
/// <LI>Cast the returned structure, fill in the input parameters, and call Lobby2Client::SendMsg() to send this command to the server.
/// <LI>Wait for the result of the operation to be sent to your callback. The message will contain the original input parameters, possibly output parameters, and Lobby2Message::resultCode will be filled in.
/// </OL>
/// \ingroup LOBBY_2_CLIENT
class RAK_DLL_EXPORT Lobby2Client : public RakNet::Lobby2Plugin
{
public:	
	Lobby2Client();
	virtual ~Lobby2Client();
	
	/// \brief Set the address of the server. When you call SendMsg() the packet will be sent to this address.
	void SetServerAddress(SystemAddress addr);

	// \brief Return whatever was passed to SetServerAddress()
	SystemAddress GetServerAddress(void) const;

	/// \brief Send a command to the server
	/// \param[in] msg The message that represents the command
	/// \param[in] callbackId Which callback, registered with SetCallbackInterface() or AddCallbackInterface(), should process the result. -1 for all
	virtual void SendMsg(Lobby2Message *msg);

	/// \brief Same as SendMsg()
	/// Also calls Dealloc on the message factory
	virtual void SendMsgAndDealloc(Lobby2Message *msg);

	// Let the user do this if they want. Not all users may want ignore lists
	/*
	/// For convenience, the list of ignored users is populated when Client_GetIgnoreList, Client_StopIgnore, and Client_StartIgnore complete.
	/// Client_GetIgnoreList is sent to us from the server automatically on login.
	/// The main reason this is here is so if you use RoomsPlugin as a client, you can check this list to filter out chat messages from ignored users.
	/// This is just a list of strings for you to read - it does NOT actually perform the ignore operation.
	virtual void AddToIgnoreList(RakNet::RakString user);
	virtual void RemoveFromIgnoreList(RakNet::RakString user);
	virtual void SetIgnoreList(DataStructures::List<RakNet::RakString> users);
	virtual bool IsInIgnoreList(RakNet::RakString user) const;
	void ClearIgnoreList(void);
	const DataStructures::OrderedList<RakNet::RakString, RakNet::RakString>* GetIgnoreList(void) const;
	DataStructures::OrderedList<RakNet::RakString, RakNet::RakString> ignoreList;
	*/

protected:

	PluginReceiveResult OnReceive(Packet *packet);
	void OnClosedConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );
	void OnShutdown(void);
	void OnMessage(Packet *packet);

	SystemAddress serverAddress;
	Lobby2Callbacks *callback;
};
	
}

#endif
