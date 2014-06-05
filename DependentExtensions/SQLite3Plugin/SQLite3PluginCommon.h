/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __SQL_LITE_3_PLUGIN_COMMON_H
#define __SQL_LITE_3_PLUGIN_COMMON_H

#include "DS_Multilist.h"
#include "RakString.h"
#include "BitStream.h"

/// \defgroup SQL_LITE_3_PLUGIN SQLite3Plugin
/// \brief Code to transmit SQLite3 commands across the network
/// \details
/// \ingroup PLUGINS_GROUP

/// Contains a result row, which is just an array of strings
/// \ingroup SQL_LITE_3_PLUGIN
struct SQLite3Row
{
	DataStructures::List<RakNet::RakString> entries;
};

/// Contains a result table, which is an array of column name strings, followed by an array of SQLite3Row
/// \ingroup SQL_LITE_3_PLUGIN
struct SQLite3Table
{
	SQLite3Table();
	~SQLite3Table();
	void Serialize(RakNet::BitStream *bitStream);
	void Deserialize(RakNet::BitStream *bitStream);

	DataStructures::List<RakNet::RakString> columnNames;
	DataStructures::List<SQLite3Row*> rows;
};

#endif
