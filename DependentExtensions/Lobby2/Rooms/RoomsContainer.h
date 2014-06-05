/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __LOBBY_ROOM_H
#define __LOBBY_ROOM_H

#include "DS_Map.h"
#include "DS_Table.h"
#include "RoomsErrorCodes.h"
#include "DS_List.h"
#include "RakNetTypes.h"
#include "IntervalTimer.h"
#include "RoomTypes.h"

namespace RakNet
{
class ProfanityFilter;
class Room;
class PerGameRoomList;
class PerGameRoomsContainer;
class BitStream;
typedef unsigned int RoomID;
struct QuickJoinUser;
struct RoomMember;
class AllGamesRoomsContainer;

class RoomsParticipant
{
public:
	RoomsParticipant() {room=0; inQuickJoin=false;}
	~RoomsParticipant() {}
	Room * GetRoom(void) const {return room;}
	void SetPerGameRoomsContainer(PerGameRoomsContainer *p) {perGameRoomsContainer=p;}
	void SetRoom(Room *_room) {room=_room; inQuickJoin=false;}
	void SetInQuickJoin(bool b) {inQuickJoin=b; if (b) room=0;}

	// Name is used for persistent invites and bans. Name should be unique among all participants or else the invites and bans will be applied to the wrong players
	RakNet::RakString GetName(void) const {return name;}
	void SetName(const char *str) {name = str;}
	void SetSystemAddress(const SystemAddress &sa) {systemAddress=sa;}
	SystemAddress GetSystemAddress(void) const {return systemAddress;}
	void SetGUID(RakNetGUID g) {guid=g;}
	RakNetGUID GetGUID(void) const {return guid;}

	PerGameRoomsContainer *GetPerGameRoomsContainer(void) const {return perGameRoomsContainer;}
	bool GetInQuickJoin(void) const {return inQuickJoin;}
protected:
	RakNet::RakString name;
	SystemAddress systemAddress;
	RakNetGUID guid;
	Room *room;
	bool inQuickJoin;
	PerGameRoomsContainer *perGameRoomsContainer;
};

typedef RakNet::RakString GameIdentifier;

enum RoomLockState
{
	// Anyone can join or leave
	RLS_NOT_LOCKED,
	// Anyone can join as spectator or become spectator. New players are not allowed. You cannot leave spectator.
	RLS_PLAYERS_LOCKED,
	// No new players are allowed, and you cannot toggle spectator
	RLS_ALL_LOCKED
};

enum ParticipantCanJoinRoomResult
{
	PCJRR_SUCCESS,
	PCJRR_BANNED,
	PCJRR_NO_PUBLIC_SLOTS,
	PCJRR_NO_PUBLIC_OR_RESERVED_SLOTS,
	PCJRR_NO_SPECTATOR_SLOTS,
	PCJRR_LOCKED,
	PCJRR_SLOT_ALREADY_USED,
};

struct Slots
{
	Slots();
	~Slots();
	unsigned int publicSlots;
	unsigned int reservedSlots;
	unsigned int spectatorSlots;
	unsigned int GetTotalSlots(void) const {return publicSlots+reservedSlots+spectatorSlots;}
	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	RoomsErrorCode Validate(void) const;
};

struct InvitedUser
{
	InvitedUser() {room=0; roomId=0; invitedAsSpectator=false;}
	Room *room;
	RoomID roomId;
	RakNet::RakString invitorName;
	SystemAddress invitorSystemAddress;
	RakNet::RakString target;
	RakNet::RakString subject;
	RakNet::RakString body;
	bool invitedAsSpectator;

	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};

struct BannedUser
{
	RakNet::RakString target;
	RakNet::RakString reason;
	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};

struct RemoveUserResult
{
	RemoveUserResult();
	~RemoveUserResult();

	// Why return a deleted pointer?
//	RoomsParticipant *removedUser;
	bool removedFromQuickJoin;
	bool removedFromRoom;
	SystemAddress removedUserAddress;
	RakNet::RakString removedUserName;

	// Following members only apply if removedFromRoom==true
	Room *room;
	RoomID roomId;
	bool gotNewModerator; // If you were the moderator before, this is true
	DataStructures::List<InvitedUser> clearedInvitations; // If invitations were cleared when you leave, these are the invitations
	bool roomDestroyed; // Up to caller to deallocate

	QuickJoinUser *qju;
	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};


struct RoomMemberDescriptor
{
	RakNet::RakString name;
	RoomMemberMode roomMemberMode;
	bool isReady;
	// Filled externally
	SystemAddress systemAddress;
	RakNetGUID guid;

	void FromRoomMember(RoomMember *roomMember);
	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};

struct NetworkedRoomCreationParameters
{
	NetworkedRoomCreationParameters() {hiddenFromSearches=false; destroyOnModeratorLeave=false; autoLockReadyStatus=false; inviteToRoomPermission=INVITE_MODE_ANYONE_CAN_INVITE; inviteToSpectatorSlotPermission=INVITE_MODE_ANYONE_CAN_INVITE; clearInvitesOnNewModerator=false;}
	// Checked by Validate
	Slots slots;
	bool hiddenFromSearches;
	bool destroyOnModeratorLeave;
	bool autoLockReadyStatus; // When everyone is ready and (the room is full or the room is locked), don't allow users to set unready.
	enum SendInvitePermission
	{
		INVITE_MODE_ANYONE_CAN_INVITE,
		INVITE_MODE_MODERATOR_CAN_INVITE,
		INVITE_MODE_PUBLIC_SLOTS_CAN_INVITE,
		INVITE_MODE_RESERVED_SLOTS_CAN_INVITE,
		INVITE_MODE_SPECTATOR_SLOTS_CAN_INVITE,
		INVITE_MODE_MODERATOR_OR_PUBLIC_SLOTS_CAN_INVITE,
		INVITE_MODE_MODERATOR_OR_PUBLIC_OR_RESERVED_SLOTS_CAN_INVITE,
	} inviteToRoomPermission, inviteToSpectatorSlotPermission;
	bool clearInvitesOnNewModerator; // Leave or change
	RakNet::RakString roomName;

	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	static const char *SendInvitePermissionToEnum(SendInvitePermission e);
};
struct RoomDescriptor
{
	DataStructures::List<RoomMemberDescriptor> roomMemberList;
	DataStructures::List<BannedUser> banList;
	RoomLockState roomLockState;
	RoomID lobbyRoomId;
	bool autoLockReadyStatus;
	bool hiddenFromSearches;
	NetworkedRoomCreationParameters::SendInvitePermission inviteToRoomPermission;
	NetworkedRoomCreationParameters::SendInvitePermission inviteToSpectatorSlotPermission;
	DataStructures::Table roomProperties;

	DataStructures::Table::Cell *GetProperty(const char* columnName)
	{
		return roomProperties.GetRowByIndex(0,0)->cells[roomProperties.ColumnIndex(columnName)];
	}
	DataStructures::Table::Cell *GetProperty(int index)
	{
		return roomProperties.GetRowByIndex(0,0)->cells[index];
	}
	
	void Clear(void)
	{
		roomMemberList.Clear(false, _FILE_AND_LINE_);
		banList.Clear(false, _FILE_AND_LINE_);
		roomProperties.Clear();
	}
	void FromRoom(Room *room, AllGamesRoomsContainer *agrc);
	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};

struct JoinedRoomResult
{
	JoinedRoomResult() {roomOutput=0; acceptedInvitor=0; agrc=0; joiningMember=0;}
	~JoinedRoomResult() {}
	Room* roomOutput;
	RoomDescriptor roomDescriptor;
	RoomsParticipant* acceptedInvitor;
	RakNet::RakString acceptedInvitorName;
	SystemAddress acceptedInvitorAddress;
	RoomsParticipant* joiningMember;
	RakNet::RakString joiningMemberName;
	SystemAddress joiningMemberAddress;
	RakNetGUID joiningMemberGuid;

	// Needed to serialize
	AllGamesRoomsContainer *agrc;

	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream );
};


struct RoomCreationParameters
{
	RoomCreationParameters();
	~RoomCreationParameters();

	NetworkedRoomCreationParameters networkedRoomCreationParameters;

	// Not checked
	RoomsParticipant* firstUser;
	GameIdentifier gameIdentifier;

	// Output parameters:
	// Was the room created?
	bool createdRoom;
	Room *roomOutput;

	// May return REC_ROOM_CREATION_PARAMETERS_* or REC_SUCCESS
	RoomsErrorCode Validate(
		const DataStructures::List<RakNet::RakString> &otherRoomNames,
		ProfanityFilter *profanityFilter) const;
};

struct RoomMember
{
	RoomMember();
	~RoomMember();
	RoomsParticipant* roomsParticipant;
	RoomMemberMode roomMemberMode;
	RakNet::TimeMS joinTime;
	bool isReady;
	// Internal - set to false when a new member is added. When the other members have been told about this member, it is set to true
	bool newMemberNotificationProcessed;
};
struct KickedUser
{
	RoomsParticipant* roomsParticipant;
	RakNet::RakString reason;
};

struct RoomQuery
{
	RoomQuery();
	~RoomQuery();

	// Point to an externally allocated array of FilterQuery, or use the helper functions below to use a static array (not threadsafe to use the static array)
	DataStructures::Table::FilterQuery *queries;
	// Size of the queries array
	unsigned int numQueries;
	// Not used
	bool queriesAllocated;

	// Helper functions
	// Easier to use, but not threadsafe
	void AddQuery_NUMERIC(const char *columnName, double numericValue, DataStructures::Table::FilterQueryType op=DataStructures::Table::QF_EQUAL);
	void AddQuery_STRING(const char *columnName, const char *charValue, DataStructures::Table::FilterQueryType op=DataStructures::Table::QF_EQUAL);
	void AddQuery_BINARY(const char *columnName, const char *input, int inputLength, DataStructures::Table::FilterQueryType op=DataStructures::Table::QF_EQUAL);
	void AddQuery_POINTER(const char *columnName, void *ptr, DataStructures::Table::FilterQueryType op=DataStructures::Table::QF_EQUAL);
	RoomsErrorCode Validate(void);

	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);

	/// \internal
	void SetQueriesToStatic(void);

private:
	static DataStructures::Table::FilterQuery fq[32];
	static DataStructures::Table::Cell cells[32];
	void SetupNextQuery(const char *columnName,DataStructures::Table::FilterQueryType op);
};


struct NetworkedQuickJoinUser
{
	NetworkedQuickJoinUser() {timeout=60000; minimumPlayers=2;}

	// How long to wait for
	RakNet::TimeMS timeout;
	// What queries to join the room on.
	RoomQuery query;
	// Minimum number of slots to join
	int minimumPlayers;

	void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};

struct QuickJoinUser
{
	QuickJoinUser();
	~QuickJoinUser();

	NetworkedQuickJoinUser networkedQuickJoinUser;

	// Total amount of time spent waiting
	RakNet::TimeMS totalTimeWaiting;

	// Which user
	RoomsParticipant* roomsParticipant;
	static int SortByTotalTimeWaiting( QuickJoinUser* const &key, QuickJoinUser* const &data );
	static int SortByMinimumSlots( QuickJoinUser* const &key, QuickJoinUser* const &data );
};

int RoomPriorityComp( Room * const &key, Room * const &data );

// PerGameRoomsContainer, mapped by game id
class AllGamesRoomsContainer
{
	public:
	AllGamesRoomsContainer();
	~AllGamesRoomsContainer();
	static void UnitTest(void);

	RoomsErrorCode CreateRoom(RoomCreationParameters *roomCreationParameters,
		ProfanityFilter *profanityFilter);

	// Enters a room based on the search queries. If no rooms are available to join, will create a room instead
	RoomsErrorCode EnterRoom(RoomCreationParameters *roomCreationParameters, 
		RoomMemberMode roomMemberMode, 
		ProfanityFilter *profanityFilter,
		RoomQuery *query,
		JoinedRoomResult *joinRoomResult);

	// Attempts to join a room by search query filters
	// Returns REC_JOIN_BY_FILTER_*
	RoomsErrorCode JoinByFilter(GameIdentifier gameIdentifier, RoomMemberMode roomMemberMode, RoomsParticipant* roomsParticipant, RoomID lastRoomJoined, RoomQuery *query, JoinedRoomResult *joinRoomResult);

	// Add a new title to host games with
	RoomsErrorCode AddTitle(GameIdentifier gameIdentifier);

	// Get all pending invites to you
	RoomsErrorCode GetInvitesToParticipant(RoomsParticipant* roomsParticipant, DataStructures::List<InvitedUser*> &invites);

	RoomsErrorCode RemoveUser(RoomsParticipant* roomsParticipant, RemoveUserResult *removeMemberResult);

	// ROOMS OPERATIONS, implicit room
	RoomsErrorCode SendInvite(RoomsParticipant* roomsParticipant, RoomsParticipant* inviteeId, bool inviteToSpectatorSlot, RakNet::RakString subject, RakNet::RakString body);
	RoomsErrorCode AcceptInvite(RoomID roomId, Room **room, RoomsParticipant* roomsParticipant, RakNet::RakString inviteSender);
	RoomsErrorCode StartSpectating(RoomsParticipant* roomsParticipant);
	RoomsErrorCode StopSpectating(RoomsParticipant* roomsParticipant);
	RoomsErrorCode GrantModerator(RoomsParticipant* roomsParticipant, RoomsParticipant *newModerator, DataStructures::List<InvitedUser> &clearedInvites);
	RoomsErrorCode ChangeSlotCounts(RoomsParticipant* roomsParticipant, Slots slots);
	RoomsErrorCode SetCustomRoomProperties(RoomsParticipant* roomsParticipant, DataStructures::Table *table);
	RoomsErrorCode ChangeRoomName(RoomsParticipant* roomsParticipant, RakNet::RakString newRoomName, ProfanityFilter *profanityFilter);
	RoomsErrorCode SetHiddenFromSearches(RoomsParticipant* roomsParticipant, bool _hiddenFromSearches);
	RoomsErrorCode SetDestroyOnModeratorLeave(RoomsParticipant* roomsParticipant, bool destroyOnModeratorLeave);
	RoomsErrorCode SetReadyStatus(RoomsParticipant* roomsParticipant, bool isReady);
	RoomsErrorCode GetReadyStatus( RoomID roomId, Room **room, DataStructures::List<RoomsParticipant*> &readyUsers, DataStructures::List<RoomsParticipant*> &unreadyUsers);
	RoomsErrorCode SetRoomLockState(RoomsParticipant* roomsParticipant, RoomLockState _roomLockState);
	RoomsErrorCode GetRoomLockState(RoomID roomId, Room **room, RoomLockState *roomLockState);
	RoomsErrorCode AreAllMembersReady(RoomID roomId, Room **room, bool *allReady);
	RoomsErrorCode KickMember(RoomsParticipant* roomsParticipant, RoomsParticipant *kickedParticipant, RakNet::RakString reason);
	RoomsErrorCode UnbanMember(RoomsParticipant* roomsParticipant, RakNet::RakString name);
	RoomsErrorCode GetBanReason( RoomID lobbyRoomId, Room **room, RakNet::RakString name, RakNet::RakString *reason);
	RoomsErrorCode LeaveRoom(RoomsParticipant* roomsParticipant, RemoveUserResult *removeUserResult);
	//RoomsErrorCode GetKickReason(RoomsParticipant* roomsParticipant, RakNet::RakString *kickReason);


	void GetRoomProperties(RoomID roomId, Room **room, DataStructures::Table *table);

	// Quick join algorithm:
	//
	// -- ROOM JOIN --
	//
	// For all rooms:
	// 1. Clear all quickJoinWorkingList from all rooms
	// For all quick join members
	// 2. Use RoomPrioritySort to get all rooms they can potentially join
	// 3. For each of these rooms, record that this member can potentially join by storing a copy of the pointer into quickJoinWorkingList, if minimumPlayers => total room slots
	// For all rooms:
	// 4. For each room where there are enough potential quick join members to fill the room, join all those members at once. Remove these members from the quick join list. Go to 1.
	//
	// -- ROOM CREATE --
	//
	// 5. Sort quick join members by minimumPlayers, excluding members where minimumPlayers > total number of quick join members
	// From greatest minimumPlayers to least
	// 6. If the current member created a room, find out how many subsequent members would join based on the custom filter
	// 7. If this satisfies minimumPlayers, have that user create a room and those subsequent members join.
	// 
	// -- EXPIRE
	//
	// 5. Remove from list if timeout has expired.
	// 6. Return results of operation (List<timeoutExpired>, List<joinedARoom>, List<RoomsThatWereJoined>
	//
	// Returns false if processing skipped due to optimization timer
	RoomsErrorCode ProcessQuickJoins( 
		DataStructures::List<QuickJoinUser*> &timeoutExpired,
		DataStructures::List<JoinedRoomResult> &joinedRoomMembers,
		DataStructures::List<QuickJoinUser*> &dereferencedPointers,
		RakNet::TimeMS elapsedTime);

	// Quick join - Store a list of all members waiting to quick join.
	// Quick join ends when
	// 1. An existing room can be fully populated using waiting quick join members.
	// 2. Enough quick join members are waiting that a new room can be created with the number of members >= minimumPlayers for all members
	// It also ends if timeToWaitMS expires.
	// Returns REC_ADD_TO_QUICK_JOIN_*
	// Passed pointer is stored on REC_SUCCESS, allocate, and do not deallocate unless not successful
	RoomsErrorCode AddUserToQuickJoin(GameIdentifier gameIdentifier, QuickJoinUser *quickJoinMember);

	// Returns REC_REMOVE_FROM_QUICK_JOIN_*
	RoomsErrorCode RemoveUserFromQuickJoin(RoomsParticipant* roomsParticipant, QuickJoinUser **qju);

	// Is this user in quick join?
	bool IsInQuickJoin(RoomsParticipant* roomsParticipant);

	// Get all rooms for a certain title
	static int RoomsSortByName( Room* const &key, Room* const &data );
	RoomsErrorCode SearchByFilter( GameIdentifier gameIdentifier, RoomsParticipant* roomsParticipant, RoomQuery *roomQuery, DataStructures::OrderedList<Room*, Room*, RoomsSortByName> &roomsOutput, bool onlyJoinable );

	// Deallocate a room
	void DestroyRoomIfDead(Room *room);

	// If a handle changes, you have to tell the system here. Otherwise ban and invite names will be out of synch
	// System does not verify that the handle is not currently in use since it does not necessarily know about all online players
	// This is an invariant up to the caller to uphold. Failure to do so will result in the wrong players being banned or invited
	void ChangeHandle(RakNet::RakString oldHandle, RakNet::RakString newHandle);

	unsigned int GetPropertyIndex(RoomID lobbyRoomId, const char *propertyName) const;

	DataStructures::Map<GameIdentifier, PerGameRoomsContainer*> perGamesRoomsContainers;

	Room * GetRoomByLobbyRoomID(RoomID lobbyRoomID);
	Room * GetRoomByName(RakNet::RakString roomName);

protected:
	RoomID nextRoomId;
};

class PerGameRoomsContainer
{
public:
	PerGameRoomsContainer();
	~PerGameRoomsContainer();

	// Has pointer column to class Room
	DataStructures::Table roomsTable;

	// Members that are waiting to quick join	
	DataStructures::List<QuickJoinUser*> quickJoinList;

	static int RoomsSortByTimeThenTotalSlots( Room* const &key, Room* const &data );
				
	protected:

	RoomsErrorCode CreateRoom(RoomCreationParameters *roomCreationParameters,
		ProfanityFilter *profanityFilter,
		RoomID lobbyRoomId,
		bool validate);
	RoomsErrorCode LeaveRoom(RoomsParticipant* roomsParticipant, bool *gotNewModerator);
	RoomsErrorCode JoinByFilter(RoomMemberMode roomMemberMode, RoomsParticipant* roomsParticipant, RoomID lastRoomJoined, RoomQuery *query, JoinedRoomResult *joinRoomResult);
	RoomsErrorCode AddUserToQuickJoin(QuickJoinUser *quickJoinMember);
	RoomsErrorCode RemoveUserFromQuickJoin(RoomsParticipant* roomsParticipant, QuickJoinUser **qju);
	bool IsInQuickJoin(RoomsParticipant* roomsParticipant);
	unsigned int GetQuickJoinIndex(RoomsParticipant* roomsParticipant);
	void GetRoomNames(DataStructures::List<RakNet::RakString> &roomNames);
	void GetAllRooms(DataStructures::List<Room*> &rooms);
	// Looks for a particular room that has a particular ID
	Room * GetRoomByLobbyRoomID(RoomID lobbyRoomID);
	Room * GetRoomByName(RakNet::RakString roomName);
	RoomsErrorCode GetInvitesToParticipant(RoomsParticipant* roomsParticipant, DataStructures::List<InvitedUser*> &invites);
	bool DestroyRoomIfDead(Room *room);
	void ChangeHandle(RakNet::RakString oldHandle, RakNet::RakString newHandle);

	unsigned ProcessQuickJoins( DataStructures::List<QuickJoinUser*> &timeoutExpired,
		DataStructures::List<JoinedRoomResult> &joinedRooms,
		DataStructures::List<QuickJoinUser*> &dereferencedPointers,
		RakNet::TimeMS elapsedTime,
		RoomID startingRoomId);

	// Sort an input list of rooms
	// Rooms are sorted by time created (longest is higher priority). If within one minute, then subsorted by total playable slot count (lower is higher priority).
	// When using EnterRoom or JoinByFilter, record the last roomOutput joined, and try to avoid rejoining the same roomOutput just left
	void RoomPrioritySort( RoomsParticipant* roomsParticipant, RoomQuery *roomQuery, DataStructures::OrderedList<Room*, Room*, RoomsSortByTimeThenTotalSlots> &roomsOutput );
	
	RoomsErrorCode SearchByFilter( RoomsParticipant* roomsParticipant, RoomQuery *roomQuery, DataStructures::OrderedList<Room*, Room*, AllGamesRoomsContainer::RoomsSortByName> &roomsOutput, bool onlyJoinable );

	friend class AllGamesRoomsContainer;
	IntervalTimer nextQuickJoinProcess;
};

// Holds all the members of a particular roomOutput
class Room
{
	public:
		Room( RoomID _roomId, RoomCreationParameters *roomCreationParameters, DataStructures::Table::Row *_row, RoomsParticipant* roomsParticipant );
		~Room();
		RoomsErrorCode SendInvite(RoomsParticipant* roomsParticipant, RoomsParticipant* inviteeId, bool inviteToSpectatorSlot, RakNet::RakString subject, RakNet::RakString body);
		RoomsErrorCode AcceptInvite(RoomsParticipant* roomsParticipant, RakNet::RakString inviteSender);
		RoomsErrorCode StartSpectating(RoomsParticipant* roomsParticipant);
		RoomsErrorCode StopSpectating(RoomsParticipant* roomsParticipant);
		RoomsErrorCode GrantModerator(RoomsParticipant* roomsParticipant, RoomsParticipant *newModerator, DataStructures::List<InvitedUser> &clearedInvites);
		RoomsErrorCode ChangeSlotCounts(RoomsParticipant* roomsParticipant, Slots slots);
		RoomsErrorCode SetCustomRoomProperties(RoomsParticipant* roomsParticipant, DataStructures::Table *table);
		RoomsErrorCode ChangeRoomName(RoomsParticipant* roomsParticipant, RakNet::RakString newRoomName, ProfanityFilter *profanityFilter);
		RoomsErrorCode SetHiddenFromSearches(RoomsParticipant* roomsParticipant, bool _hiddenFromSearches);
		RoomsErrorCode SetDestroyOnModeratorLeave(RoomsParticipant* roomsParticipant, bool destroyOnModeratorLeave);
		RoomsErrorCode SetReadyStatus(RoomsParticipant* roomsParticipant, bool isReady);
		RoomsErrorCode GetReadyStatus(DataStructures::List<RoomsParticipant*> &readyUsers, DataStructures::List<RoomsParticipant*> &unreadyUsers);
		RoomsErrorCode SetRoomLockState(RoomsParticipant* roomsParticipant, RoomLockState _roomLockState);
		RoomsErrorCode GetRoomLockState(RoomLockState *_roomLockState);
		RoomsErrorCode AreAllMembersReady(unsigned int exceptThisIndex, bool *allReady);
		RoomsErrorCode KickMember(RoomsParticipant* roomsParticipant, RoomsParticipant *kickedParticipant, RakNet::RakString reason);
		RoomsErrorCode UnbanMember(RoomsParticipant* roomsParticipant, RakNet::RakString name);
		RoomsErrorCode GetBanReason(RakNet::RakString name, RakNet::RakString *reason);
		RoomsErrorCode LeaveRoom(RoomsParticipant* roomsParticipant, RemoveUserResult *removeUserResult);
		//RoomsErrorCode GetKickReason(RoomsParticipant* roomsParticipant, RakNet::RakString *kickReason);
		
		RoomsErrorCode JoinByFilter(RoomsParticipant* roomsParticipant, RoomMemberMode roomMemberMode, JoinedRoomResult *joinRoomResult);
		RoomsErrorCode JoinByQuickJoin(RoomsParticipant* roomsParticipant, RoomMemberMode roomMemberMode, JoinedRoomResult *joinRoomResult);

		bool IsHiddenToParticipant(RoomsParticipant* roomsParticipant) const;

		// Can this user join this roomOutput?
		ParticipantCanJoinRoomResult ParticipantCanJoinAsPlayer( RoomsParticipant* roomsParticipant, bool asSpectator, bool checkHasInvite );
		ParticipantCanJoinRoomResult ParticipantCanJoinRoom( RoomsParticipant* roomsParticipant, bool asSpectator, bool checkHasInvite );

		// Returns true if there are only spectators, or nobody at all
		bool IsRoomDead(void) const;		

		RoomsErrorCode GetInvitesToParticipant(RoomsParticipant* roomsParticipant, DataStructures::List<InvitedUser*> &invites);

		RoomsParticipant* GetModerator(void) const;
		
		//  Gets the roomOutput ID
		RoomID GetID(void) const;

		double GetNumericProperty(RoomID lobbyRoomId, const char *propertyName) const;
		const char *GetStringProperty(RoomID lobbyRoomId, const char *propertyName) const;

		double GetNumericProperty(int index) const;
		const char *GetStringProperty(int index) const;
		void SetNumericProperty(int index, double value);
		void SetStringProperty(int index, const char *value);
				
		// Public for easy access
		DataStructures::List<RoomMember*> roomMemberList;

		DataStructures::List<InvitedUser> inviteList;
		DataStructures::List<BannedUser> banList;

		// Don't store - slow because when removing users I have to iterate through every room
		// DataStructures::List<KickedUser> kickedList;
		
		// Internal
		DataStructures::List<QuickJoinUser*> quickJoinWorkingList;
		
		static void UpdateRowSlots( DataStructures::Table::Row* row, Slots *totalSlots, Slots *usedSlots);

		void ChangeHandle(RakNet::RakString oldHandle, RakNet::RakString newHandle);
protected:
		Room();
		
		// Updates the table row
		RoomsErrorCode RemoveUser(RoomsParticipant* roomsParticipant,RemoveUserResult *removeUserResult);

		bool IsRoomLockedToSpectators(void) const;
		bool IsRoomLockedToPlayers(void) const;

		bool IsInRoom(RoomsParticipant* roomsParticipant) const;
		bool HasInvite(RakNet::RakString roomsParticipant);
		unsigned int GetRoomIndex(RoomsParticipant* roomsParticipant) const;
		unsigned int GetBannedIndex(RakNet::RakString username) const;
		unsigned int GetInviteIndex(RakNet::RakString invitee, RakNet::RakString invitor) const;
		unsigned int GetFirstInviteIndex(RakNet::RakString invitee) const;
		bool AreAllPlayableSlotsFilled(void) const;
		bool HasOpenPublicSlots(void) const;
		bool HasOpenReservedSlots(void) const;
		bool HasOpenSpectatorSlots(void) const;
		void UpdateUsedSlots( void );
		void UpdateUsedSlots( Slots *totalSlots, Slots *usedSlots );
		static void UpdateUsedSlots( DataStructures::Table::Row* tableRow, Slots *totalSlots, Slots *usedSlots );
		Slots GetTotalSlots(void) const;
		void SetTotalSlots(Slots *totalSlots);
		Slots GetUsedSlots(void) const;
		

		RoomLockState roomLockState;
		
		friend struct RoomDescriptor;
		friend class PerGameRoomsContainer;
		friend class AllGamesRoomsContainer;

		RoomID lobbyRoomId;
		DataStructures::Table::Row *tableRow;

		bool autoLockReadyStatus;
		bool hiddenFromSearches;
//		bool destroyOnModeratorLeave;
		bool clearInvitesOnNewModerator;
		NetworkedRoomCreationParameters::SendInvitePermission inviteToRoomPermission, inviteToSpectatorSlotPermission;

		bool roomDestroyed;

};


} // namespace Lobby2

#endif
