/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __LOBBY_2_SERVER_H
#define __LOBBY_2_SERVER_H

#include "Export.h"
#include "RakNetTypes.h"
#include "Lobby2Plugin.h"
#include "DS_OrderedList.h"
#include "ThreadPool.h"
#include "Lobby2Presence.h"

//class PostgreSQLInterface;

namespace RakNet
{
	
struct Lobby2Message;
class RoomsPlugin;

/// Commands are either messages from remote systems, or can be run by the local system
/// \internal
struct Lobby2ServerCommand
{
	Lobby2Message *lobby2Message;
	bool deallocMsgWhenDone;
	bool returnToSender;
	unsigned int callerUserId;
	RakNet::RakString callingUserName;
	DataStructures::List<SystemAddress> callerSystemAddresses;
	DataStructures::List<RakNetGUID> callerGuids;
	//SystemAddress requiredConnectionAddress;
	Lobby2Server *server;
};

/// \brief The base class for the lobby server, without database specific functionality
/// \details This is a plugin which will take incoming messages via Lobby2Client_PC::SendMsg(), process them, and send back the same messages with output and a result code
/// Unlike the first implementation of the lobby server, this is a thin plugin that mostly just sends messages to threads and sends back the results.
/// \ingroup LOBBY_2_SERVER
class RAK_DLL_EXPORT Lobby2Server : public RakNet::Lobby2Plugin, public ThreadDataInterface
{
public:	
	Lobby2Server();
	virtual ~Lobby2Server();
	
	/// \brief Connect to the database \a numWorkerThreads times using the connection string
	/// \param[in] conninfo See the postgre docs
	/// \return True on success, false on failure.
	virtual bool ConnectToDB(const char *conninfo, int numWorkerThreads)=0;
	/// \internal
	virtual void AddInputFromThread(Lobby2Message *msg, unsigned int targetUserId, RakNet::RakString targetUserHandle)=0;
	/// \internal
	virtual void AddOutputFromThread(Lobby2Message *msg, unsigned int targetUserId, RakNet::RakString targetUserHandle)=0;

	/// \brief Lobby2Message encapsulates a user command, containing both input and output data
	/// \details This will serialize and transmit that command
	void SendMsg(Lobby2Message *msg, const DataStructures::List<SystemAddress> &recipients);

	/// \brief Add a command, which contains a message and other data such as who send the message.
	/// \details The command will be processed according to its implemented virtual functions. Most likely it will be processed in a thread to run database commands
	void ExecuteCommand(Lobby2ServerCommand *command);

	/// \brief If Lobby2Message::RequiresAdmin() returns true, the message can only be processed from a remote system if the sender's system address is first added()
	/// \details This is useful if you want to administrate the server remotely
	void AddAdminAddress(SystemAddress addr);
	/// \brief If AddAdminAddress() was previously called with \a addr then this returns true.
	bool HasAdminAddress(const DataStructures::List<SystemAddress> &addresses);
	/// \brief Removes a system address previously added with AddAdminAddress()
	void RemoveAdminAddress(SystemAddress addr);
	/// \brief Removes all system addresses previously added with AddAdminAddress()
	void ClearAdminAddresses(void);

	/// \brief If Lobby2Message::RequiresRankingPermission() returns true, then the system that sent the command must be registered with AddRankingAddress()
	/// \param[in] addr Address to allow
	void AddRankingAddress(SystemAddress addr);

	/// Returns if an address was previously added with AddRankingAddress()
	/// \param[in] addr Address to check
	bool HasRankingAddress(const DataStructures::List<SystemAddress> &addresses);

	/// Removes an addressed added with AddRankingAddress()
	/// \param[in] addr Address to check
	void RemoveRankingAddress(SystemAddress addr);

	/// \brief Clears all addresses added with AddRankingAddress()
	void ClearRankingAddresses(void);
	
	/// \brief To use RoomsPlugin and Lobby2Server together, register RoomsPlugin with this funcrtion
	/// \details The rooms plugin does not automatically handle users logging in and logging off, or renaming users.
	/// You can have Lobby2Server manage this for you by calling SetRoomsPlugin() with a pointer to the rooms plugin, if it is on the local system.
	/// This will call RoomsPlugin::LoginRoomsParticipant() and RoomsPlugin::LogoffRoomsParticipant() as the messages L2MID_Client_Login and L2MID_Client_Logoff arrive
	/// This will use the pointer to RoomsPlugin directly. Setting this will disable SetRoomsPluginAddress()
	/// \note This is an empty function. You must #define __INTEGRATE_LOBBY2_WITH_ROOMS_PLUGIN and link with RoomsPlugin.h to use it()
	void SetRoomsPlugin(RoomsPlugin *rp);

	/// \brief This is similar to SetRoomsPlugin(), except the plugin is on another system.
	/// \details This will set the system address of that system to send the login and logoff commands to.
	/// For this function to work, you must first call RoomsPlugin::AddLoginServerAddress() so that RoomsPlugin will accept the incoming login and logoff messages.
	/// \note This is an empty function. You must #define __INTEGRATE_LOBBY2_WITH_ROOMS_PLUGIN and link with RoomsPlugin.h to use it()
	void SetRoomsPluginAddress(SystemAddress address);

	/// \brief Server configuration properties, to customize how the server runs specific commands
	struct ConfigurationProperties
	{
		ConfigurationProperties() {requiresEmailAddressValidationToLogin=false; requiresTitleToLogin=false; accountRegistrationRequiredAgeYears=0;}

		/// \brief If true, Client_Login will fail with Client_Login_EMAIL_ADDRESS_NOT_VALIDATED unless the email address registered with Client_RegisterAccount is verified with the command System_SetEmailAddressValidated
		bool requiresEmailAddressValidationToLogin;

		/// \brief If true Client_Login::titleName and Client_Login::titleSecretKey must be previously registered with System_CreateTitle or Client_Login will fail with L2RC_Client_Login_BAD_TITLE_OR_TITLE_SECRET_KEY
		bool requiresTitleToLogin;

		/// \brief If true, Client_RegisterAccount::cdKey must be previously registered with CDKey_Add or Client_RegisterAccount will fail with L2RC_Client_RegisterAccount_REQUIRES_CD_KEY or a related error message
		bool accountRegistrationRequiresCDKey;

		/// \brief The minimum age needed to register accounts. If Client_RegisterAccount::createAccountParameters::ageInDays is less than this, then the account registration will fail with L2RC_Client_RegisterAccount_REQUIRED_AGE_NOT_MET
		/// \details Per-title age requirements can be handled client-side with System_CreateTitle::requiredAge and System_GetTitleRequiredAge
		unsigned int accountRegistrationRequiredAgeYears;
	};

	/// \brief Set the desired configuration properties. This is read during runtime from threads.
	void SetConfigurationProperties(ConfigurationProperties c);
	/// \brief Get the previously set configuration properties.
	const ConfigurationProperties *GetConfigurationProperties(void) const;

	/// Set the presence of a logged in user
	/// \param[in] presence Presence info of this user
	void SetPresence(const RakNet::Lobby2Presence &presence, RakNet::RakString userHandle);

	/// Get the presence of a logged in user, by handle
	/// \param[out] presence Presence info of requested user
	/// \param[in] userHandle Handle of the user
	void GetPresence(RakNet::Lobby2Presence &presence, RakNet::RakString userHandle);

	/// \internal Lets the plugin know that a user has logged on, so this user can be tracked and the message forwarded to RoomsPlugin
	void OnLogin(Lobby2ServerCommand *command, bool calledFromThread);
	/// \internal Lets the plugin know that a user has logged off, so this user can be tracked and the message forwarded to RoomsPlugin
	void OnLogoff(Lobby2ServerCommand *command, bool calledFromThread);
	/// \internal Lets the plugin know that a user has been renamed, so this user can be tracked and the message forwarded to RoomsPlugin
	void OnChangeHandle(Lobby2ServerCommand *command, bool calledFromThread);

	/// \internal
	struct User
	{
		DataStructures::List<SystemAddress> systemAddresses;
		DataStructures::List<RakNetGUID> guids;
		unsigned int callerUserId;
		RakNet::RakString userName;
		Lobby2Presence presence;
		bool allowMultipleLogins;
	};

	/// \internal
	static int UserCompByUsername( const RakString &key, Lobby2Server::User * const &data );

	/// \internal
	struct ThreadAction
	{
		Lobby2MessageID action;
		Lobby2ServerCommand command;
	};

	const DataStructures::OrderedList<RakString, User*, Lobby2Server::UserCompByUsername>& GetUsers(void) const {return users;}
	void GetUserOnlineStatus(UsernameAndOnlineStatus &userInfo) const;
	

protected:

	void Update(void);
	PluginReceiveResult OnReceive(Packet *packet);
	void OnClosedConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );
	void OnShutdown(void);
	void OnMessage(Packet *packet);
	void Clear(void);
	void ClearUsers(void);
	unsigned int GetUserIndexBySystemAddress(SystemAddress systemAddress) const;
	unsigned int GetUserIndexByGUID(RakNetGUID guid) const;
	unsigned int GetUserIndexByUsername(RakNet::RakString userName) const;
	void StopThreads(void);
	void SendRemoteLoginNotification(RakNet::RakString handle, const DataStructures::List<SystemAddress>& recipients);

	/// \internal
	void RemoveUser(RakString userName);
	/// \internal
	void RemoveUser(unsigned int index);
	void LogoffFromRooms(User *user);

	virtual void* PerThreadFactory(void *context)=0;
	virtual void PerThreadDestructor(void* factoryResult, void *context)=0;
	virtual void AddInputCommand(Lobby2ServerCommand command)=0;
	virtual void ClearConnections(void) {};

	DataStructures::OrderedList<SystemAddress, SystemAddress> adminAddresses;
	DataStructures::OrderedList<SystemAddress, SystemAddress> rankingAddresses;
	DataStructures::OrderedList<RakString, User*, Lobby2Server::UserCompByUsername> users;
	RoomsPlugin *roomsPlugin;
	SystemAddress roomsPluginAddress;
	ThreadPool<Lobby2ServerCommand,Lobby2ServerCommand> threadPool;
	SimpleMutex connectionPoolMutex;
	ConfigurationProperties configurationProperties;
	DataStructures::Queue<ThreadAction> threadActionQueue;
	SimpleMutex threadActionQueueMutex;

	//DataStructures::List<PostgreSQLInterface *> connectionPool;
	void SendUnifiedToMultiple( const RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const DataStructures::List<SystemAddress> systemAddresses );
};
	
}

#endif