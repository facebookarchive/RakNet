/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __LOBBY_2_PLUGIN_H
#define __LOBBY_2_PLUGIN_H

#include "Lobby2Message.h"
#include "PluginInterface2.h"
#include "PacketPriority.h"
#include "RakPeerInterface.h"

/// \defgroup LOBBY_2_GROUP Lobby2Plugin
/// \brief SQL based lobby system, with support for users, friends, clans, emails, ranking, and a message board
/// \details
/// \ingroup PLUGINS_GROUP

/// \defgroup LOBBY_2_COMMANDS Lobby2Commands
/// \brief Commands that can be sent to Lobby2Server from Lobby2Client
/// \details
/// \ingroup LOBBY_2_GROUP

/// \defgroup LOBBY_2_NOTIFICATIONS Lobby2Notifications
/// \brief Callbacks that the Lobby2System will send to you
/// \details
/// \ingroup LOBBY_2_GROUP

/// \defgroup LOBBY_2_SERVER Lobby2Server
/// \brief Runs the server modules that asynchronously processes Lobby2Message
/// \details
/// \ingroup LOBBY_2_GROUP

/// \defgroup LOBBY_2_CLIENT Lobby2Client
/// \brief Sends commands to Lobby2Server
/// \details
/// \ingroup LOBBY_2_GROUP


namespace RakNet
{

/// \ingroup LOBBY_2_GROUP
enum ServerErrors
{
	// Class factory could not create a message of the given type
	// Followed by 4 bytes, with the message number
	L2SE_UNKNOWN_MESSAGE_ID,
	// Client is trying to run a function that requires admin access. Use Lobby2Server::AddAdminAddress() to add this client.
	L2SE_REQUIRES_ADMIN,
};

struct Lobby2MessageFactory;

/// \brief Both Lobby2Server and Lobby2Client derive from this class
/// \details 
/// \ingroup LOBBY_2_GROUP
class RAK_DLL_EXPORT Lobby2Plugin : public PluginInterface2
{
public:
	Lobby2Plugin();
	virtual ~Lobby2Plugin();
	
	/// \brief Ordering channel to send messages on
	/// \param[in] oc The ordering channel
	void SetOrderingChannel(char oc);

	/// \brief Send priority to send messages on
	/// \param[in] pp The packet priority
	void SetSendPriority(PacketPriority pp);

	/// \brief Creates messages from message IDs
	/// \details Server should get a factory that creates messages with database functionality.<BR>
	/// Client can use the base class
	/// \param[in] f Class factory instance, which should remain valid for the scope of the plugin
	void SetMessageFactory(Lobby2MessageFactory *f);

	/// \brief Returns whatever was passed to SetMessageFactory()
	Lobby2MessageFactory* GetMessageFactory(void) const;

	/// \brief Set the callback to receive the results of operations via SendMsg()
	virtual void SetCallbackInterface(Lobby2Callbacks *cb);	

	/// \brief You can have more than one callback to get called from the results of operations via SendMsg()
	virtual void AddCallbackInterface(Lobby2Callbacks *cb);	

	/// \brief Removes a callback added with AddCallbackInterface();
	virtual void RemoveCallbackInterface(Lobby2Callbacks *cb);

	/// \brief Removes all callbacks added with AddCallbackInterface();
	virtual void ClearCallbackInterfaces();
	
protected:
		
	char orderingChannel;
	PacketPriority packetPriority;	
	Lobby2MessageFactory *msgFactory;

	DataStructures::List<Lobby2Callbacks*> callbacks;
};

}; // namespace RakNet

#endif
