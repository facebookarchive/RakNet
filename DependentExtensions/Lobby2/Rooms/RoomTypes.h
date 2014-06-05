/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "DS_Table.h"

#ifndef __ROOM_TYPES_H
#define __ROOM_TYPES_H

enum RoomMemberMode
{
	/// The owner of the room, who is also a player in the room. The owner cannot be a spectator
	RMM_MODERATOR,

	/// The room member is a player in a public slot
	RMM_PUBLIC,

	/// The room member is a player in a reserved slot
	RMM_RESERVED,

	/// The room member is a spectator in a public slot.
	RMM_SPECTATOR_PUBLIC,

	/// The room member is a spectator in a reserved slot.
	RMM_SPECTATOR_RESERVED,

	/// Used as a query flag - join any slot that is playable (reserved or public)
	RMM_ANY_PLAYABLE,

	/// Used as a query flag - join any slot that is for a spectator (reserved or public)
	RMM_ANY_SPECTATOR,
};

const char *RoomMemberModeToEnum(RoomMemberMode e);

struct DefaultRoomColumns
{
	enum
	{
		TC_TITLE_NAME,
		TC_TITLE_ID,
		TC_ROOM_NAME,
		TC_ROOM_ID,
		TC_TOTAL_SLOTS,
		TC_TOTAL_PUBLIC_PLUS_RESERVED_SLOTS,
		TC_USED_SLOTS,
		TC_USED_PUBLIC_PLUS_RESERVED_SLOTS,
		TC_REMAINING_SLOTS,
		TC_REMAINING_PUBLIC_PLUS_RESERVED_SLOTS,
		TC_TOTAL_PUBLIC_SLOTS,
		TC_TOTAL_RESERVED_SLOTS,
		TC_TOTAL_SPECTATOR_SLOTS,
		TC_USED_PUBLIC_SLOTS,
		TC_USED_RESERVED_SLOTS,
		TC_USED_SPECTATOR_SLOTS,
		TC_REMAINING_PUBLIC_SLOTS,
		TC_REMAINING_RESERVED_SLOTS,
		TC_REMAINING_SPECTATOR_SLOTS,
		TC_CREATION_TIME,
		TC_DESTROY_ON_MODERATOR_LEAVE,
		TC_LOBBY_ROOM_PTR,
		TC_TABLE_COLUMNS_COUNT
	} columnId;

	const char *columnName;
	DataStructures::Table::ColumnType columnType;

	static const char *GetColumnName(int columnId);
	static int GetColumnIndex(const char *columnName);
	static DataStructures::Table::ColumnType GetColumnType(int columnId);
	static bool HasColumnName(const char *columnName);
	static void AddDefaultColumnsToTable(DataStructures::Table *table);
	static bool HasDefaultColumns(DataStructures::Table *table);
};

#endif
