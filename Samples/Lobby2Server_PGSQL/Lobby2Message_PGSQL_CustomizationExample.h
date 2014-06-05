/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

// You can extend Lobby2 with your own messages, either overriding existing messages
// or adding new ones
// In both cases, you'll need to create a custom class factory to handle this.

#include "Lobby2Message_PGSQL.h"

// --------------------------------------------------------------
// Override an existing message (Platform_Startup) to do user-custom behaviors
// Requires:
// 1. New class factory to create this message, instead of the old one
// --------------------------------------------------------------
namespace RakNet
{

class Platform_Startup_Overridden : public Platform_Startup_PGSQL
{
	virtual bool ServerDBImpl( Lobby2ServerCommand *command, void *databaseInterface )
	{
		printf("Platform_Startup_Overridden");
		return false;
	}
};

// --------------------------------------------------------------
// Create an entirely new message
// Requires:
// 1. New class factory to create this message, in addition to the old ones
// 2. New custom callback handler to process this message's result
// 3. New enumeration list, which appends your messages to the old list
// --------------------------------------------------------------

// New enumeration list
enum Lobby2MessageID_Custom
{
	L2MID_MyCustomMessage=L2MID_COUNT,
};

// Forward declaration(s) of my new message types
struct MyCustomMessage;

// New custom callback handler
struct Lobby2CustomizedHandler : public Lobby2Callbacks
{
	virtual void MessageResult(MyCustomMessage *message);
	virtual void ExecuteDefaultResult(Lobby2Message *message) {message->DebugPrintf();}
};


// Macro to make things easier, customized for our new callback handler
#define __L2_MSG_MY_CUSTOM_IMPL(__NAME__) \
	virtual void CallCallback(Lobby2Callbacks *cb) {((Lobby2CustomizedHandler*)cb)->MessageResult(this);}; \
	virtual Lobby2MessageID GetID(void) const {return (Lobby2MessageID) L2MID_##__NAME__;} \
	virtual const char* GetName(void) const {return #__NAME__;} \
	virtual void DebugMsg(RakNet::RakString &out) const {out.Set(#__NAME__ " result=%s\n", Lobby2ResultCodeDescription::ToEnglish(resultCode));};


// The new message
struct MyCustomMessage : public Lobby2Message
{
	__L2_MSG_MY_CUSTOM_IMPL(MyCustomMessage)
		
	virtual bool RequiresAdmin(void) const {return false;}
	virtual bool RequiresRankingPermission(void) const {return false;}
	virtual bool CancelOnDisconnect(void) const {return true;}
	virtual bool RequiresLogin(void) const {return false;}
	virtual void Serialize( bool writeToBitstream, bool serializeOutput, RakNet::BitStream *bitStream );
	virtual bool PrevalidateInput(void) {return true;}
	virtual bool ServerDBImpl( Lobby2ServerCommand *command, void *databaseInterface )
	{
		printf("MyCustomMessage");
		return false;
	}
};

// The new class factory
struct Lobby2MessageFactory_Customized : public Lobby2MessageFactory_PGSQL
{
	virtual Lobby2Message *Alloc(Lobby2MessageID id)
	{
		switch (id)
		{
		case L2MID_Platform_Startup:
			return RakNet::OP_NEW<Platform_Startup_Overridden>(__FILE__, __LINE__); 
		case L2MID_MyCustomMessage:
			return RakNet::OP_NEW<MyCustomMessage>(__FILE__, __LINE__); 
		}
		return Lobby2MessageFactory_PGSQL::Alloc(id);
	}
};

} // end namespace