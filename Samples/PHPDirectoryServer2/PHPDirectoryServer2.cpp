/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

/// \file
/// \brief Contains WebGameList, a client for communicating with a HTTP list of game servers
///
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.jenkinssoftware.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free Software Foundation

#include "PHPDirectoryServer2.h"
#include "HTTPConnection.h"
#include "RakSleep.h"
#include "RakString.h"
#include "RakNetTypes.h"
#include "GetTime.h"
#include "RakAssert.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "Itoa.h"

// Column with this header contains the name of the game, passed to UploadTable()
static const char *GAME_NAME_COMMAND="__GAME_NAME";
// Column with this header contains the port of the game, passed to UploadTable()
static const char *GAME_PORT_COMMAND="__GAME_PORT";
// Column with this header contains the IP address of the game, passed to UploadTable()
static const char *SYSTEM_ADDRESS_COMMAND="_System_Address";
// Returned from the PHP server indicating when this row was last updated.
static const char *LAST_UPDATE_COMMAND="__SEC_AFTER_EPOCH_SINCE_LAST_UPDATE";

using namespace RakNet;
using namespace DataStructures;

PHPDirectoryServer2::PHPDirectoryServer2()
    : nextRepost(0)
{
    Map<RakString, RakString>::IMPLEMENT_DEFAULT_COMPARISON();
}
PHPDirectoryServer2::~PHPDirectoryServer2()
{
}
void PHPDirectoryServer2::Init(HTTPConnection *_http, const char *_path)
{
	http=_http;
	pathToPHP=_path;
}

void PHPDirectoryServer2::SetField( RakNet::RakString columnName, RakNet::RakString value )
{
	if (columnName.IsEmpty())
		return;

	if (columnName==GAME_NAME_COMMAND ||
		columnName==GAME_PORT_COMMAND ||
		columnName==LAST_UPDATE_COMMAND)
	{
		RakAssert("PHPDirectoryServer2::SetField attempted to set reserved column name" && 0);
		return;
	}

    fields.Set(columnName, value);
}
unsigned int PHPDirectoryServer2::GetFieldCount(void) const
{
	return fields.Size();
}
void PHPDirectoryServer2::GetField(unsigned int index, RakNet::RakString &columnName, RakNet::RakString &value)
{
	RakAssert(index < fields.Size());
	columnName=fields.GetKeyAtIndex(index);
	value=fields[index];
}
void PHPDirectoryServer2::SetFields(DataStructures::Table *table)
{
	ClearFields();

	unsigned columnIndex, rowIndex;
	DataStructures::Table::Row *row;
	
	for (rowIndex=0; rowIndex < table->GetRowCount(); rowIndex++)
	{
		row = table->GetRowByIndex(rowIndex, 0);
		for (columnIndex=0; columnIndex < table->GetColumnCount(); columnIndex++)
		{
			SetField( table->ColumnName(columnIndex), row->cells[columnIndex]->ToString(table->GetColumnType(columnIndex)) );
		}
	}
}

void PHPDirectoryServer2::ClearFields(void)
{
	fields.Clear();
	nextRepost=0;
}

void PHPDirectoryServer2::UploadTable(RakNet::RakString uploadPassword, RakNet::RakString gameName, unsigned short gamePort, bool autoRepost)
{
	gameNameParam=gameName;
	gamePortParam=gamePort;
	currentOperation="";
	currentOperation="?query=upload&uploadPassword=";
	currentOperation+=uploadPassword;
	SendOperation();

	if (autoRepost)
		nextRepost=RakNet::GetTimeMS()+50000;
	else
		nextRepost=0;
}
void PHPDirectoryServer2::DownloadTable(RakNet::RakString downloadPassword)
{
	currentOperation="?query=download&downloadPassword=";
	currentOperation+=downloadPassword;
	SendOperation();
}
void PHPDirectoryServer2::UploadAndDownloadTable(RakNet::RakString uploadPassword, RakNet::RakString downloadPassword, RakNet::RakString gameName, unsigned short gamePort, bool autoRepost)
{
	gameNameParam=gameName;
	gamePortParam=gamePort;
	currentOperation="?query=upDown&downloadPassword=";
	currentOperation+=downloadPassword;
	currentOperation+="&uploadPassword=";
	currentOperation+=uploadPassword;

	SendOperation();

	if (autoRepost)
		nextRepost=RakNet::GetTimeMS()+50000;
	else
		nextRepost=0;
}

HTTPReadResult PHPDirectoryServer2::ProcessHTTPRead(RakNet::RakString httpRead)
{
	const char *c = (const char*) httpRead.C_String(); // current position
	HTTPReadResult resultCode=HTTP_RESULT_EMPTY;

	lastDownloadedTable.Clear();


	if (*c=='\n')
		c++;
	char buff[256];
	int buffIndex;
	bool isCommand=true;
	DataStructures::List<RakNet::RakString> columns;
	DataStructures::List<RakNet::RakString> values;
	RakNet::RakString curString;
	bool isComment=false;
	buffIndex=0;
	while(c && *c)
	{
		// 3 is comment
		if (*c=='\003')
		{
			isComment=!isComment;
			c++;
			continue;
		}
		if (isComment)
		{
			c++;
			continue;
		}

		// 1 or 2 separates fields
		// 4 separates rows
		if (*c=='\001')
		{
			if (isCommand)
			{
				buff[buffIndex]=0;
				columns.Push(RakString::NonVariadic(buff), _FILE_AND_LINE_);
				isCommand=false;
				if (buff[0]!=0)
					resultCode=HTTP_RESULT_GOT_TABLE;
			}
			else
			{
				buff[buffIndex]=0;
				values.Push(RakString::NonVariadic(buff), _FILE_AND_LINE_);
				isCommand=true;
			}
			buffIndex=0;
		}
		else if (*c=='\002')
		{
			buff[buffIndex]=0;
			buffIndex=0;
			values.Push(RakString::NonVariadic(buff), _FILE_AND_LINE_);
			isCommand=true;
			PushColumnsAndValues(columns, values);
			columns.Clear(true, _FILE_AND_LINE_);
			values.Clear(true, _FILE_AND_LINE_);

		}
		else
		{
			if (buffIndex<256-1)
				buff[buffIndex++]=*c;
		}
		c++;
	}
	if (buff[0] && columns.Size()==values.Size()+1)
	{
		buff[buffIndex]=0;
		values.Push(RakString::NonVariadic(buff), _FILE_AND_LINE_);
	}

	PushColumnsAndValues(columns, values);

	return resultCode;
}
void PHPDirectoryServer2::PushColumnsAndValues(DataStructures::List<RakNet::RakString> &columns, DataStructures::List<RakNet::RakString> &values)
{
	DataStructures::Table::Row *row=0;

	unsigned int i;
	for (i=0; i < columns.Size() && i < values.Size(); i++)
	{
		if (columns[i].IsEmpty()==false)
		{
			unsigned col = lastDownloadedTable.ColumnIndex(columns[i]);
			if(col == (unsigned)-1)
			{
				col = lastDownloadedTable.AddColumn(columns[i], DataStructures::Table::STRING);
			}

			if (row==0)
			{
				row = lastDownloadedTable.AddRow(lastDownloadedTable.GetAvailableRowId());
			}
			row->UpdateCell(col,values[i].C_String());
		}
	}
}
const DataStructures::Table *PHPDirectoryServer2::GetLastDownloadedTable(void) const
{
	return &lastDownloadedTable;
}
void PHPDirectoryServer2::SendOperation(void)
{
	RakString outgoingMessageBody;
	char buff[64];

	outgoingMessageBody += GAME_PORT_COMMAND;
	outgoingMessageBody += '\001';
	outgoingMessageBody += Itoa(gamePortParam,buff,10);
	outgoingMessageBody += '\001';
	outgoingMessageBody += GAME_NAME_COMMAND;
	outgoingMessageBody += '\001';
	outgoingMessageBody += gameNameParam;

	for (unsigned i = 0; i < fields.Size(); i++)
	{
		RakString value = fields[i];
		value.URLEncode();
		outgoingMessageBody += RakString("\001%s\001%s",
			fields.GetKeyAtIndex(i).C_String(),
			value.C_String());
	}

	RakString postURL;
	postURL+=pathToPHP;
	postURL+=currentOperation;
	http->Post(postURL.C_String(), outgoingMessageBody, "application/x-www-form-urlencoded");

}
void PHPDirectoryServer2::Update(void)
{
	if (http->IsBusy())
		return;


	if (nextRepost==0 || fields.Size()==0)
		return;

	RakNet::TimeMS time = GetTimeMS();

	// Entry deletes itself after 60 seconds, so keep reposting if set to do so
	if (time > nextRepost)
	{
		nextRepost=RakNet::GetTimeMS()+50000;
		SendOperation();
	}
}
