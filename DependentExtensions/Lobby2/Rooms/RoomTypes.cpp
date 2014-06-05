/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "RoomTypes.h"

const char *RoomMemberModeToEnum(RoomMemberMode e)
{
	switch (e)
	{
	case RMM_MODERATOR:
		return "RMM_MODERATOR";
	case RMM_PUBLIC:
		return "RMM_PUBLIC";
	case RMM_RESERVED:
		return "RMM_RESERVED";
	case RMM_SPECTATOR_PUBLIC:
		return "RMM_SPECTATOR_PUBLIC";
	case RMM_SPECTATOR_RESERVED:
		return "RMM_SPECTATOR_RESERVED";
	case RMM_ANY_PLAYABLE:
		return "RMM_ANY_PLAYABLE";
	case RMM_ANY_SPECTATOR:
		return "RMM_ANY_SPECTATOR";
	}
	return "Error in RoomMemberModeToEnum";
}

static DefaultRoomColumns defaultRoomColumns[DefaultRoomColumns::TC_TABLE_COLUMNS_COUNT] =
{
	{DefaultRoomColumns::TC_TITLE_NAME, "Title name", DataStructures::Table::STRING},
	{DefaultRoomColumns::TC_TITLE_ID, "Title id", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_ROOM_NAME, "Room name", DataStructures::Table::STRING},
	{DefaultRoomColumns::TC_ROOM_ID, "Room id", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_TOTAL_SLOTS, "Total slots", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_TOTAL_PUBLIC_PLUS_RESERVED_SLOTS, "Total Public plus reserved slots", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_USED_SLOTS, "Used slots", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_USED_PUBLIC_PLUS_RESERVED_SLOTS, "Used public plus reserved slots", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_REMAINING_SLOTS, "Remaining slots", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_REMAINING_PUBLIC_PLUS_RESERVED_SLOTS, "Remaining public plus reserved slots", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_TOTAL_PUBLIC_SLOTS, "Total public slots", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_TOTAL_RESERVED_SLOTS, "Total reserved slots", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_TOTAL_SPECTATOR_SLOTS, "Total spectator slots", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_USED_PUBLIC_SLOTS, "Used public slots", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_USED_RESERVED_SLOTS, "Used reserved slots", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_USED_SPECTATOR_SLOTS, "Used spectator slots", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_REMAINING_PUBLIC_SLOTS, "Remaining public slots", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_REMAINING_RESERVED_SLOTS, "Remaining reserved slots", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_REMAINING_SPECTATOR_SLOTS, "Remaining spectator slots", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_CREATION_TIME, "Creation time", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_DESTROY_ON_MODERATOR_LEAVE, "Destroy on moderator leave", DataStructures::Table::NUMERIC},
	{DefaultRoomColumns::TC_LOBBY_ROOM_PTR, "Lobby room ptr [Internal]", DataStructures::Table::POINTER},
};

const char *DefaultRoomColumns::GetColumnName(int columnId) {return defaultRoomColumns[columnId].columnName;}
DataStructures::Table::ColumnType DefaultRoomColumns::GetColumnType(int columnId) {return defaultRoomColumns[columnId].columnType;}
bool DefaultRoomColumns::HasColumnName(const char *columnName)
{
	unsigned i;
	for (i=0; i < TC_TABLE_COLUMNS_COUNT; i++)
		if (strcmp(columnName,GetColumnName(i))==0)
			return true;
	return false;
}
int DefaultRoomColumns::GetColumnIndex(const char *columnName)
{
	unsigned i;
	for (i=0; i < TC_TABLE_COLUMNS_COUNT; i++)
		if (strcmp(columnName,GetColumnName(i))==0)
			return i;
	return -1;
}
void DefaultRoomColumns::AddDefaultColumnsToTable(DataStructures::Table *table)
{
	unsigned i;
	for (i=0; i < DefaultRoomColumns::TC_TABLE_COLUMNS_COUNT; i++)
		table->AddColumn(DefaultRoomColumns::GetColumnName(i), DefaultRoomColumns::GetColumnType(i));
}
bool DefaultRoomColumns::HasDefaultColumns(DataStructures::Table *table)
{
	unsigned i;
	for (i=0; i < DefaultRoomColumns::TC_TABLE_COLUMNS_COUNT; i++)
	{
		if (table->ColumnIndex(DefaultRoomColumns::GetColumnName(i))!=-1)
			return true;
	}
	return false;
}
