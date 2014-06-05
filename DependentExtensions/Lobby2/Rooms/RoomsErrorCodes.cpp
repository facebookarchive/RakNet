/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "RoomsErrorCodes.h"
#include "RakAssert.h"

using namespace RakNet;

static RoomsErrorCodeDescription errorCodeDescriptions[ROOMS_ERROR_CODES_COUNT] =
{
	{REC_SUCCESS, "REC_SUCCESS", "Success."},
	{REC_USERNAME_IS_EMPTY, "REC_USERNAME_IS_EMPTY", "Username is empty."},
	{REC_NOT_LOGGED_IN, "REC_NOT_LOGGED_IN", "Not logged in."},
	{REC_ADD_TO_ROOM_NO_RESERVED_OR_PUBLIC, "REC_ADD_TO_ROOM_NO_RESERVED_OR_PUBLIC", "Failed to enter the room. Room has no reserved or public slots available to join."},
	{REC_ADD_TO_ROOM_NO_PUBLIC, "REC_ADD_TO_ROOM_NO_PUBLIC", "Failed to enter the room. Room has no public slots available to join."},
	{REC_ADD_TO_ROOM_NO_SPECTATOR, "REC_ADD_TO_ROOM_NO_SPECTATOR", "Failed to enter the room. Room has no spectator slots available to join."},
	{REC_ADD_TO_ROOM_ALREADY_IN_THIS_ROOM, "REC_ADD_TO_ROOM_ALREADY_IN_THIS_ROOM", "Failed to enter the room. You are already in this room."},
	{REC_ADD_TO_ROOM_ALREADY_IN_ANOTHER_ROOM, "REC_ADD_TO_ROOM_ALREADY_IN_ANOTHER_ROOM", "Failed to enter the room. You are already in a different room. Leave your existing room first."},
	{REC_ADD_TO_ROOM_KICKED_OUT_OF_ROOM, "REC_ADD_TO_ROOM_KICKED_OUT_OF_ROOM", "Failed to enter the room. You have been banned from this room."},
	{REC_CHANGE_MEMBER_TYPE_NO_SLOTS, "REC_CHANGE_MEMBER_TYPE_NO_SLOTS", "The room does not have free slots of the desired type."},
	{REC_SEARCH_BY_FILTER_UNKNOWN_TITLE, "REC_SEARCH_BY_FILTER_UNKNOWN_TITLE", "Unknown title (Programmer error)."},
	{REC_JOIN_BY_FILTER_UNKNOWN_TITLE, "REC_JOIN_BY_FILTER_UNKNOWN_TITLE", "Room join failed. Unknown title (Programmer error)."},
	{REC_JOIN_BY_FILTER_NO_ROOMS, "REC_JOIN_BY_FILTER_NO_ROOMS", "Room join failed. There are no rooms, or no rooms that meet your search requirements."},
	{REC_JOIN_BY_FILTER_CURRENTLY_IN_A_ROOM, "REC_JOIN_BY_FILTER_CURRENTLY_IN_A_ROOM", "Room join failed. You are already in a room."},
	{REC_JOIN_BY_FILTER_CURRENTLY_IN_QUICK_JOIN, "REC_JOIN_BY_FILTER_CURRENTLY_IN_QUICK_JOIN", "Room join failed. You are currently in quick join. Leave quick join first."},
	{REC_JOIN_BY_FILTER_CANNOT_JOIN_AS_MODERATOR, "REC_JOIN_BY_FILTER_CANNOT_JOIN_AS_MODERATOR", "Room join failed. You cannot join an existing room as a moderator. Create a new room instead."},
	{REC_JOIN_BY_FILTER_ROOM_LOCKED, "REC_JOIN_BY_FILTER_ROOM_LOCKED", "Room join failed. The room is locked to new players."},
	{REC_JOIN_BY_FILTER_BANNED, "REC_JOIN_BY_FILTER_BANNED", "Room join failed. You are banned from this room."},
	{REC_JOIN_BY_FILTER_NO_SLOTS, "REC_JOIN_BY_FILTER_NO_SLOTS", "Room join failed. The room is full."},
	{REC_JOIN_BY_QUICK_JOIN_CANNOT_JOIN_AS_MODERATOR, "REC_JOIN_BY_QUICK_JOIN_CANNOT_JOIN_AS_MODERATOR", "Quick join failed. You cannot join an existing room as a moderator. Create a new room instead."},
	{REC_JOIN_BY_QUICK_JOIN_ROOM_LOCKED, "REC_JOIN_BY_QUICK_JOIN_ROOM_LOCKED", "Quick join failed. All rooms are locked to new players."},
	{REC_JOIN_BY_QUICK_JOIN_BANNED, "REC_JOIN_BY_QUICK_JOIN_BANNED", "Quick join failed. You are banned from the only available rooms."},
	{REC_JOIN_BY_QUICK_JOIN_NO_SLOTS, "REC_JOIN_BY_QUICK_JOIN_NO_SLOTS", "Quick join failed. All rooms are full."},
	{REC_ADD_TO_QUICK_JOIN_CURRENTLY_IN_A_ROOM, "REC_ADD_TO_QUICK_JOIN_CURRENTLY_IN_A_ROOM", "Failed to quick join. You are currently in a room. Leave your existing room first."},
	{REC_ADD_TO_QUICK_JOIN_UNKNOWN_TITLE, "REC_ADD_TO_QUICK_JOIN_UNKNOWN_TITLE", "Failed to quick join. Unknown title (Programmer error)."},
	{REC_ADD_TO_QUICK_JOIN_ALREADY_THERE, "REC_ADD_TO_QUICK_JOIN_ALREADY_THERE", "Quick join is already in progress."},
	{REC_ADD_TO_QUICK_JOIN_INVALID_TIMEOUT_TOO_LOW, "REC_ADD_TO_QUICK_JOIN_INVALID_TIMEOUT_TOO_LOW", "Failed to quick join. Tiebout is below the minimum threshold."},
	{REC_ADD_TO_QUICK_JOIN_INVALID_TIMEOUT_TOO_HIGH, "REC_ADD_TO_QUICK_JOIN_INVALID_TIMEOUT_TOO_HIGH", "Failed to quick join. Timeout is above the minimum threshold."},
	{REC_ADD_TO_QUICK_JOIN_MINIMUM_SLOTS_TOO_LOW, "REC_ADD_TO_QUICK_JOIN_MINIMUM_SLOTS_TOO_LOW", "Failed to quick join. Must have at least one slot for other players."},
	{REC_ADD_TO_QUICK_JOIN_MINIMUM_SLOTS_TOO_HIGH, "REC_ADD_TO_QUICK_JOIN_MINIMUM_SLOTS_TOO_HIGH", "Failed to quick join. Too many player slots."},
	{REC_REMOVE_FROM_QUICK_UNKNOWN_TITLE, "REC_REMOVE_FROM_QUICK_UNKNOWN_TITLE", "Failed to leave quick join. Unknown title (Programmer error)."},
	{REC_REMOVE_FROM_QUICK_JOIN_NOT_THERE, "REC_REMOVE_FROM_QUICK_JOIN_NOT_THERE", "Failed to leave quick join. You are not in quick join to begin with."},
	{REC_CREATE_ROOM_UNKNOWN_TITLE, "REC_CREATE_ROOM_UNKNOWN_TITLE", "Failed to create a room. Unknown title (Programmer error)."},
	{REC_CREATE_ROOM_CURRENTLY_IN_QUICK_JOIN, "REC_CREATE_ROOM_CURRENTLY_IN_QUICK_JOIN", "Failed to create a room. You are currently in quick join. Leave quick join first."},
	{REC_CREATE_ROOM_CURRENTLY_IN_A_ROOM, "REC_CREATE_ROOM_CURRENTLY_IN_A_ROOM", "Failed to create a room. You are already in a room."},
	{REC_ROOM_CREATION_PARAMETERS_EMPTY_ROOM_NAME, "REC_ROOM_CREATION_PARAMETERS_EMPTY_ROOM_NAME", "You must specify a room name."},
	{REC_ROOM_CREATION_PARAMETERS_RESERVED_QUICK_JOIN_ROOM_NAME, "REC_ROOM_CREATION_PARAMETERS_RESERVED_QUICK_JOIN_ROOM_NAME", "Invalid room creation parameters. The room name is reserved for quick join matches."},
	{REC_ROOM_CREATION_PARAMETERS_ROOM_NAME_HAS_PROFANITY, "REC_ROOM_CREATION_PARAMETERS_ROOM_NAME_HAS_PROFANITY", "The desired room name cannot contain profanity."},
	{REC_ROOM_CREATION_PARAMETERS_ROOM_NAME_IN_USE, "REC_ROOM_CREATION_PARAMETERS_ROOM_NAME_IN_USE", "The desired room name is already in use."},
	{REC_ROOM_CREATION_PARAMETERS_NO_PLAYABLE_SLOTS, "REC_ROOM_CREATION_PARAMETERS_NO_PLAYABLE_SLOTS", "Invalid room creation parameters. The room must have at least one playable slot."},
	{REC_SET_ROOM_PROPERTIES_UNKNOWN_ROOM, "REC_SET_ROOM_PROPERTIES_UNKNOWN_ROOM", "Unknown room."},
	{REC_LEAVE_ROOM_UNKNOWN_ROOM_ID, "REC_LEAVE_ROOM_UNKNOWN_ROOM_ID", "Failed to leave a room. Your room no longer exists."},
	{REC_LEAVE_ROOM_CURRENTLY_IN_QUICK_JOIN, "REC_LEAVE_ROOM_CURRENTLY_IN_QUICK_JOIN", "Failed to leave a room. You are currently in quick join. Leave quick join first."},
	{REC_LEAVE_ROOM_NOT_IN_ROOM, "REC_LEAVE_ROOM_NOT_IN_ROOM", "You are not currently in a room."},
	{REC_ENTER_ROOM_UNKNOWN_TITLE, "REC_ENTER_ROOM_UNKNOWN_TITLE", "Failed to enter a room. Unknown title (Programmer error)."},
	{REC_ENTER_ROOM_CURRENTLY_IN_QUICK_JOIN, "REC_ENTER_ROOM_CURRENTLY_IN_QUICK_JOIN", "Failed to enter a room. You are currently in quick join. Leave quick join first."},
	{REC_ENTER_ROOM_CURRENTLY_IN_A_ROOM, "REC_ENTER_ROOM_CURRENTLY_IN_A_ROOM", "Failed to enter a room. You are already in a room."},
	{REC_PROCESS_QUICK_JOINS_UNKNOWN_TITLE, "REC_PROCESS_QUICK_JOINS_UNKNOWN_TITLE", "Unknown title (Programmer error)."},
	{REC_ROOM_QUERY_TOO_MANY_QUERIES, "REC_ROOM_QUERY_TOO_MANY_QUERIES,", "Failed to process room query. Too many queries."},
	{REC_ROOM_QUERY_INVALID_QUERIES_POINTER, "REC_ROOM_QUERY_INVALID_QUERIES_POINTER", "Failed to process room query. NULL query pointer."},
	{REC_SEND_INVITE_UNKNOWN_ROOM_ID, "REC_SEND_INVITE_UNKNOWN_ROOM_ID", "Failed to send room invite. Your room no longer exists."},
	{REC_SEND_INVITE_INVITEE_ALREADY_INVITED, "REC_SEND_INVITE_INVITEE_ALREADY_INVITED", "User was already invited to the room."},
	{REC_SEND_INVITE_CANNOT_PERFORM_ON_SELF, "REC_SEND_INVITE_CANNOT_PERFORM_ON_SELF", "Cannot invite yourself."},
	{REC_SEND_INVITE_INVITOR_ONLY_MODERATOR_CAN_INVITE, "REC_SEND_INVITE_INVITOR_ONLY_MODERATOR_CAN_INVITE", "Failed to send room invite. Room settings only allows the moderator to invite."},
	{REC_SEND_INVITE_INVITOR_LACK_INVITE_PERMISSIONS, "REC_SEND_INVITE_INVITOR_LACK_INVITE_PERMISSIONS", "Failed to send room invite. Room settings does not allow invites to the desired slot type."},
	{REC_SEND_INVITE_INVITOR_NOT_IN_ROOM, "REC_SEND_INVITE_INVITOR_NOT_IN_ROOM", "Failed to send room invite. You are not in the room you are trying to invite to."},
	{REC_SEND_INVITE_NO_SLOTS, "REC_SEND_INVITE_NO_SLOTS", "Failed to send room invite. The room is full."},
	{REC_SEND_INVITE_INVITEE_ALREADY_IN_THIS_ROOM, "REC_SEND_INVITE_INVITEE_ALREADY_IN_THIS_ROOM", "Failed to send room invite. This member is already in the room."},
	{REC_SEND_INVITE_INVITEE_BANNED, "REC_SEND_INVITE_INVITEE_BANNED", "Failed to send room invite. The target member was banned from the room."},
	{REC_SEND_INVITE_RECIPIENT_NOT_ONLINE, "REC_SEND_INVITE_RECIPIENT_NOT_ONLINE", "Failed to send room invite. The target member is not online."},
	{REC_SEND_INVITE_ROOM_LOCKED, "REC_SEND_INVITE_ROOM_LOCKED", "Failed to send room invite. The room is locked to players of the intended slot."},
	{REC_ACCEPT_INVITE_UNKNOWN_ROOM_ID, "REC_ACCEPT_INVITE_UNKNOWN_ROOM_ID", "Failed to accept room invite. Your room no longer exists."},
	{REC_ACCEPT_INVITE_CURRENTLY_IN_A_ROOM, "REC_ACCEPT_INVITE_CURRENTLY_IN_A_ROOM", "Failed to accept room invite. You are already in a room. Leave the room first."},
	{REC_ACCEPT_INVITE_CURRENTLY_IN_QUICK_JOIN, "REC_ACCEPT_INVITE_CURRENTLY_IN_QUICK_JOIN", "Failed to accept room invite. You are currently in quick join. Leave quick join first."},
	{REC_ACCEPT_INVITE_BANNED, "REC_ACCEPT_INVITE_BANNED", "Failed to accept room invite. The moderator has banned you from the room."},
	{REC_ACCEPT_INVITE_NO_SLOTS, "REC_ACCEPT_INVITE_NO_SLOTS", "Failed to accept room invite. The room is full for the specified slot type."},
	{REC_ACCEPT_INVITE_ROOM_LOCKED, "REC_ACCEPT_INVITE_ROOM_LOCKED", "Failed to accept room invite. The room has been locked to the specified slot type."},
	{REC_ACCEPT_INVITE_NO_SUCH_INVITE, "REC_ACCEPT_INVITE_NO_SUCH_INVITE", "Failed to accept room invite. You have no pending invites to this room."},
	{REC_SLOTS_VALIDATION_NO_PLAYABLE_SLOTS, "REC_SLOTS_VALIDATION_NO_PLAYABLE_SLOTS", "Invalid room slots. The room must have at least one playable slot."},
	{REC_SLOTS_VALIDATION_NEGATIVE_PUBLIC_SLOTS, "REC_SLOTS_VALIDATION_NEGATIVE_PUBLIC_SLOTS", "Invalid room slots. Public slots cannot be negative."},
	{REC_SLOTS_VALIDATION_NEGATIVE_RESERVED_SLOTS, "REC_SLOTS_VALIDATION_NEGATIVE_RESERVED_SLOTS", "Invalid room slots. Reserved slots cannot be negative."},
	{REC_SLOTS_VALIDATION_NEGATIVE_SPECTATOR_SLOTS, "REC_SLOTS_VALIDATION_NEGATIVE_SPECTATOR_SLOTS", "Invalid room slots. Spectator slots cannot be negative."},
	{REC_START_SPECTATING_UNKNOWN_ROOM_ID, "REC_START_SPECTATING_UNKNOWN_ROOM_ID", "Failed to spectate. Your room no longer exists."},
	{REC_START_SPECTATING_ALREADY_SPECTATING, "REC_START_SPECTATING_ALREADY_SPECTATING", "You are already spectating."},
	{REC_START_SPECTATING_NO_SPECTATOR_SLOTS_AVAILABLE, "REC_START_SPECTATING_NO_SPECTATOR_SLOTS_AVAILABLE", "Failed to spectate. No spectator slots available."},
	{REC_START_SPECTATING_NOT_IN_ROOM, "REC_START_SPECTATING_NOT_IN_ROOM", "Failed to spectate. Your room no longer exists."},
	{REC_START_SPECTATING_REASSIGN_MODERATOR_BEFORE_SPECTATE, "REC_START_SPECTATING_REASSIGN_MODERATOR_BEFORE_SPECTATE", "The moderator cannot spectate wthout first granting moderator to another player."},
	{REC_START_SPECTATING_ROOM_LOCKED, "REC_START_SPECTATING_ROOM_LOCKED", "Failed to spectate. The room has been locked."},
	{REC_STOP_SPECTATING_UNKNOWN_ROOM_ID, "REC_STOP_SPECTATING_UNKNOWN_ROOM_ID", "Failed to stop spectating. Your room no longer exists."},
	{REC_STOP_SPECTATING_NOT_IN_ROOM, "REC_STOP_SPECTATING_NOT_IN_ROOM", "Failed to stop spectating. You are not in a room."},
	{REC_STOP_SPECTATING_NOT_CURRENTLY_SPECTATING, "REC_STOP_SPECTATING_NOT_CURRENTLY_SPECTATING", "Failed to stop spectating. You are not currently spectating."},
	{REC_STOP_SPECTATING_NO_SLOTS, "REC_STOP_SPECTATING_NO_SLOTS", "Failed to stop spectating. All player slots are full."},
	{REC_STOP_SPECTATING_ROOM_LOCKED, "REC_STOP_SPECTATING_ROOM_LOCKED", "Failed to stop spectating. The room has been locked."},
	{REC_GRANT_MODERATOR_UNKNOWN_ROOM_ID, "REC_GRANT_MODERATOR_UNKNOWN_ROOM_ID", "Failed to grant moderator to another player. Your room no longer exists."},
	{REC_GRANT_MODERATOR_NEW_MODERATOR_NOT_ONLINE, "REC_GRANT_MODERATOR_NEW_MODERATOR_NOT_ONLINE", "Failed to grant moderator to another player. The new moderator is not online."},
	{REC_GRANT_MODERATOR_NOT_IN_ROOM, "REC_GRANT_MODERATOR_NOT_IN_ROOM", "Failed to grant moderator to another player. You are not in a room."},
	{REC_GRANT_MODERATOR_NEW_MODERATOR_NOT_IN_ROOM, "REC_GRANT_MODERATOR_NEW_MODERATOR_NOT_IN_ROOM", "Failed to grant moderator to another player. The new moderator is not in the room."},
	{REC_GRANT_MODERATOR_CANNOT_PERFORM_ON_SELF, "REC_GRANT_MODERATOR_CANNOT_PERFORM_ON_SELF", "You are already the moderator."},
	{REC_GRANT_MODERATOR_MUST_BE_MODERATOR_TO_GRANT_MODERATOR, "REC_GRANT_MODERATOR_MUST_BE_MODERATOR_TO_GRANT_MODERATOR", "Failed to grant moderator to another player. You must be moderator to do this."},
	{REC_GRANT_MODERATOR_NEW_MODERATOR_NOT_IN_PLAYABLE_SLOT, "REC_GRANT_MODERATOR_NEW_MODERATOR_NOT_IN_PLAYABLE_SLOT", "Failed to grant moderator to another player. The new moderator must be in a playable slot (not spectating)."},
	{REC_CHANGE_SLOT_COUNTS_UNKNOWN_ROOM_ID, "REC_CHANGE_SLOT_COUNTS_UNKNOWN_ROOM_ID", "Failed to change slot counts. Your room no longer exists."},
	{REC_CHANGE_SLOT_COUNTS_NOT_IN_ROOM, "REC_CHANGE_SLOT_COUNTS_NOT_IN_ROOM", "Failed to change slot counts. You are not in a room."},
	{REC_CHANGE_SLOT_COUNTS_MUST_BE_MODERATOR, "REC_CHANGE_SLOT_COUNTS_MUST_BE_MODERATOR", "Failed to change slot counts. You must be moderator to do this."},
	{REC_SET_CUSTOM_ROOM_PROPERTIES_UNKNOWN_ROOM_ID, "REC_SET_CUSTOM_ROOM_PROPERTIES_UNKNOWN_ROOM_ID", "Failed to set room properties. Your room no longer exists."},
	{REC_SET_CUSTOM_ROOM_PROPERTIES_CONTAINS_DEFAULT_COLUMNS, "REC_SET_CUSTOM_ROOM_PROPERTIES_CONTAINS_DEFAULT_COLUMNS", "Failed to set custom room properties. Custom properties cannot contain default columns. Use the provided functions for this."},
	{REC_SET_CUSTOM_ROOM_PROPERTIES_NOT_IN_ROOM, "REC_SET_CUSTOM_ROOM_PROPERTIES_NOT_IN_ROOM", "Failed to set room properties. You are not in a room."},
	{REC_SET_CUSTOM_ROOM_PROPERTIES_MUST_BE_MODERATOR, "REC_SET_CUSTOM_ROOM_PROPERTIES_MUST_BE_MODERATOR", "Failed to set room properties. You must be moderator to do this."},
	{REC_GET_ROOM_PROPERTIES_EMPTY_ROOM_NAME_AND_NOT_IN_A_ROOM, "REC_GET_ROOM_PROPERTIES_EMPTY_ROOM_NAME_AND_NOT_IN_A_ROOM", "Failed to get room properties. The room name is empty."},
	{REC_GET_ROOM_PROPERTIES_UNKNOWN_ROOM_NAME, "REC_GET_ROOM_PROPERTIES_UNKNOWN_ROOM_NAME", "Failed to get room properties. Named room does not exist."},
	{REC_CHANGE_ROOM_NAME_UNKNOWN_ROOM_ID, "REC_CHANGE_ROOM_NAME_UNKNOWN_ROOM_ID", "Failed to change the room's name. Your room no longer exists."},
	{REC_CHANGE_ROOM_NAME_NOT_IN_ROOM, "REC_CHANGE_ROOM_NAME_NOT_IN_ROOM", "Failed to change the room's name. You are not in a room."},
	{REC_CHANGE_ROOM_NAME_MUST_BE_MODERATOR, "REC_CHANGE_ROOM_NAME_MUST_BE_MODERATOR", "Failed to change the room's name. You must be moderator to do this."},
	{REC_CHANGE_ROOM_NAME_HAS_PROFANITY, "REC_CHANGE_ROOM_NAME_HAS_PROFANITY", "Failed to change the room's name. The new name contains profanity."},
	{REC_CHANGE_ROOM_NAME_EMPTY_ROOM_NAME, "REC_CHANGE_ROOM_NAME_EMPTY_ROOM_NAME", "Failed to change the room's name. The new name is empty."},
	{REC_CHANGE_ROOM_NAME_NAME_ALREADY_IN_USE, "REC_CHANGE_ROOM_NAME_NAME_ALREADY_IN_USE", "Failed to change the room's name. The new name is already in use."},
	{REC_SET_HIDDEN_FROM_SEARCHES_UNKNOWN_ROOM_ID, "REC_SET_HIDDEN_FROM_SEARCHES_UNKNOWN_ROOM_ID", "Failed to set the room hidden from searches. Your room no longer exists."},
	{REC_SET_HIDDEN_FROM_SEARCHES_NOT_IN_ROOM, "REC_SET_HIDDEN_FROM_SEARCHES_NOT_IN_ROOM", "Failed to set the room hidden from searches. You are not in a room."},
	{REC_SET_HIDDEN_FROM_SEARCHES_MUST_BE_MODERATOR, "REC_SET_HIDDEN_FROM_SEARCHES_MUST_BE_MODERATOR", "Failed to set the room hidden from searches. You must be moderator to do this."},
	{REC_SET_DESTROY_ON_MODERATOR_LEAVE_UNKNOWN_ROOM_ID, "REC_SET_DESTROY_ON_MODERATOR_LEAVE_UNKNOWN_ROOM_ID", "Failed to set the room to be destroyed on moderator leave. Your room no longer exists."},
	{REC_SET_DESTROY_ON_MODERATOR_LEAVE_NOT_IN_ROOM, "REC_SET_DESTROY_ON_MODERATOR_LEAVE_NOT_IN_ROOM", "Failed to set the room to be destroyed on moderator leave. You are not in a room."},
	{REC_SET_DESTROY_ON_MODERATOR_LEAVE_MUST_BE_MODERATOR, "REC_SET_DESTROY_ON_MODERATOR_LEAVE_MUST_BE_MODERATOR", "Failed to set the room to be destroyed on moderator leave. You must be moderator to do this."},
	{REC_SET_READY_STATUS_UNKNOWN_ROOM_ID, "REC_SET_READY_STATUS_UNKNOWN_ROOM_ID", "Failed to set ready status. Your room no longer exists."},
	{REC_SET_READY_STATUS_NOT_IN_ROOM, "REC_SET_READY_STATUS_NOT_IN_ROOM,", "Failed to set ready status. You are not in a room."},
	{REC_SET_READY_STATUS_NOT_IN_PLAYABLE_SLOT, "REC_SET_READY_STATUS_NOT_IN_PLAYABLE_SLOT", "Failed to set ready status. You are currently spectating. Only players can set ready status."},
	{REC_SET_READY_STATUS_AUTO_LOCK_ALL_PLAYERS_READY, "REC_SET_READY_STATUS_AUTO_LOCK_ALL_PLAYERS_READY", "Failed to unready. The room is locked. Leave the room if you do not want to play."},
	{REC_GET_READY_STATUS_NOT_IN_ROOM, "REC_GET_READY_STATUS_NOT_IN_ROOM", "Failed to get ready status for your existing room. You are not in a room."},
	{REC_GET_READY_STATUS_UNKNOWN_ROOM_ID, "REC_GET_READY_STATUS_UNKNOWN_ROOM_ID,", "Failed to get ready status. Your room no longer exists."},
	{REC_SET_ROOM_LOCK_STATE_UNKNOWN_ROOM_ID, "REC_SET_ROOM_LOCK_STATE_UNKNOWN_ROOM_ID", "Failed to set room lock state. Your room no longer exists."},
	{REC_SET_ROOM_LOCK_STATE_NOT_IN_ROOM, "REC_SET_ROOM_LOCK_STATE_NOT_IN_ROOM", "Failed to set room lock state. You are not in a room."},
	{REC_SET_ROOM_LOCK_STATE_MUST_BE_MODERATOR, "REC_SET_ROOM_LOCK_STATE_MUST_BE_MODERATOR", "Failed to set room lock state. You must be moderator to do this."},
	{REC_SET_ROOM_LOCK_STATE_BAD_ENUMERATION_VALUE, "REC_SET_ROOM_LOCK_STATE_BAD_ENUMERATION_VALUE", "Failed to set room lock state. Bad enumeration (programmer error)."},
	{REC_GET_ROOM_LOCK_STATE_UNKNOWN_ROOM_ID, "REC_GET_ROOM_LOCK_STATE_UNKNOWN_ROOM_ID", "Failed to get room lock state. Your room no longer exists."},
	{REC_GET_ROOM_LOCK_STATE_NOT_IN_ROOM, "REC_GET_ROOM_LOCK_STATE_NOT_IN_ROOM", "Failed to get room lock state for your existing room. You are not in a room."},
	{REC_ARE_ALL_MEMBERS_READY_UNKNOWN_ROOM_ID, "REC_ARE_ALL_MEMBERS_READY_UNKNOWN_ROOM_ID", "Failed to check member ready status. Your room no longer exists."},
	{REC_ARE_ALL_MEMBERS_READY_NOT_IN_ROOM, "REC_ARE_ALL_MEMBERS_READY_NOT_IN_ROOM", "Failed to check member ready status for your existing room. You are not in a room."},
	{REC_KICK_MEMBER_UNKNOWN_ROOM_ID, "REC_KICK_MEMBER_UNKNOWN_ROOM_ID", "Failed to kick member. Your room no longer exists."},
	{REC_KICK_MEMBER_NOT_IN_ROOM, "REC_KICK_MEMBER_NOT_IN_ROOM", "Failed to kick member. You are not in a room."},
	{REC_KICK_MEMBER_TARGET_NOT_ONLINE, "REC_KICK_MEMBER_TARGET_NOT_ONLINE", "Failed to kick member. The target member is not online."},
	{REC_KICK_MEMBER_TARGET_NOT_IN_YOUR_ROOM, "REC_KICK_MEMBER_TARGET_NOT_IN_YOUR_ROOM", "Failed to kick member. Member is no longer in the room."},
	{REC_KICK_MEMBER_MUST_BE_MODERATOR, "REC_KICK_MEMBER_MUST_BE_MODERATOR", "Failed to kick member. You must be moderator to do this."},
	{REC_KICK_MEMBER_CANNOT_PERFORM_ON_SELF, "REC_KICK_MEMBER_CANNOT_PERFORM_ON_SELF", "Cannot kick yourself."},
	{REC_GET_KICK_REASON_UNKNOWN_ROOM_ID, "REC_GET_KICK_REASON_UNKNOWN_ROOM_ID", "Failed to get kick reason. Your room no longer exists."},
	{REC_GET_KICK_REASON_NOT_KICKED, "REC_GET_KICK_REASON_NOT_KICKED", "Specified user has not been kicked."},
	{REC_REMOVE_USER_NOT_IN_ROOM, "REC_REMOVE_USER_NOT_IN_ROOM", "Failed to remove user from room. User is not in room."},
	{REC_ADD_TITLE_ALREADY_IN_USE, "REC_ADD_TITLE_ALREADY_IN_USE", "Failed to add a title. This title is already in use."},
	{REC_UNBAN_MEMBER_UNKNOWN_ROOM_ID, "REC_UNBAN_MEMBER_UNKNOWN_ROOM_ID", "Failed to unban member. Your room no longer exists."},
	{REC_UNBAN_MEMBER_NOT_IN_ROOM, "REC_UNBAN_MEMBER_NOT_IN_ROOM", "Failed to unban member. You are not in a room."},
	{REC_UNBAN_MEMBER_MUST_BE_MODERATOR, "REC_UNBAN_MEMBER_MUST_BE_MODERATOR", "Failed to unban member. You must be moderator to do this."},
	{REC_UNBAN_MEMBER_NOT_BANNED, "REC_UNBAN_MEMBER_NOT_BANNED", "Specified member is not banned."},
	{REC_GET_BAN_REASON_UNKNOWN_ROOM_ID, "REC_GET_BAN_REASON_UNKNOWN_ROOM_ID", "Failed to get ban reason. Room no longer exists."},
	{REC_GET_BAN_REASON_NOT_BANNED, "REC_GET_BAN_REASON_NOT_BANNED", "This user is not banned."},
	{REC_CHANGE_HANDLE_NEW_HANDLE_IN_USE, "REC_CHANGE_HANDLE_NEW_HANDLE_IN_USE", "The handle you are changing to is already in use."},
	{REC_CHANGE_HANDLE_CONTAINS_PROFANITY, "REC_CHANGE_HANDLE_CONTAINS_PROFANITY", "The handle you are changing to contains profanity."},
	{REC_CHAT_USER_NOT_IN_ROOM, "REC_CHAT_USER_NOT_IN_ROOM", "You must be in a room to send room chat messages."},
	{REC_CHAT_RECIPIENT_NOT_ONLINE, "REC_CHAT_RECIPIENT_NOT_ONLINE,", "Chat recipient is not online."},
	{REC_CHAT_RECIPIENT_NOT_IN_ANY_ROOM, "REC_CHAT_RECIPIENT_NOT_IN_ANY_ROOM", "Chat recipient is not in a room."},
	{REC_CHAT_RECIPIENT_NOT_IN_YOUR_ROOM, "REC_CHAT_RECIPIENT_NOT_IN_YOUR_ROOM", "Chat recipient is not in your room."},
	{REC_BITSTREAM_USER_NOT_IN_ROOM, "REC_BITSTREAM_USER_NOT_IN_ROOM", "You must be in a room to send bitstream messages."},
	{REC_BITSTREAM_RECIPIENT_NOT_ONLINE, "REC_BITSTREAM_RECIPIENT_NOT_ONLINE", "Bitstream recipient is not online."},
	{REC_BITSTREAM_RECIPIENT_NOT_IN_ANY_ROOM, "REC_BITSTREAM_RECIPIENT_NOT_IN_ANY_ROOM", "Bitstream recipient is not in a room."},
	{REC_BITSTREAM_RECIPIENT_NOT_IN_YOUR_ROOM, "REC_BITSTREAM_RECIPIENT_NOT_IN_YOUR_ROOM", "Bitstream recipient is not in your room."},

};

const char *RoomsErrorCodeDescription::ToEnglish(RoomsErrorCode result)
{
	RakAssert(errorCodeDescriptions[result].errorCode==result);
	return errorCodeDescriptions[result].englishDesc;
}
const char *RoomsErrorCodeDescription::ToEnum(RoomsErrorCode result)
{
	RakAssert(errorCodeDescriptions[result].errorCode==result);
	return errorCodeDescriptions[result].enumDesc;
}
void RoomsErrorCodeDescription::Validate(void)
{
	int i;
	for (i=0; i < ROOMS_ERROR_CODES_COUNT; i++)
	{
		RakAssert(errorCodeDescriptions[i].errorCode==i);
	}
}