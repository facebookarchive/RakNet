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
/// \brief Contains PHPDirectoryServer2, a client for communicating with a HTTP list of game servers
///
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.jenkinssoftware.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __PHP_DIRECTORY_SERVER_2
#define __PHP_DIRECTORY_SERVER_2

#include "Export.h"
#include "RakString.h"
#include "HTTPConnection.h"
#include "RakNetTypes.h"
#include "DS_Queue.h"
#include "DS_Table.h"
#include "DS_Map.h"

namespace RakNet {

struct SystemAddress;

enum HTTPReadResult
{
	HTTP_RESULT_GOT_TABLE,
	HTTP_RESULT_EMPTY
};

/// \brief Use PHPDirectoryServer2 as a C++ client to DirectoryServer.php
///
/// PHPDirectoryServer2 works with the HTTPConnection class (which works with the TCPInterface class) in order to communiate with DirectoryServer.php found under Samples/PHPDirectoryServer2
class RAK_DLL_EXPORT PHPDirectoryServer2
{
public:
    PHPDirectoryServer2();
	virtual ~PHPDirectoryServer2();

	/// Associate PHPDirectoryServer2 with the HTTPConnection class it will communicate through
	/// \param[in] _http The instance of HTTP connection we will communicate through
	/// \param[in] _path The path to the PHP file on the remote server. For example, if the path is mysite.com/raknet/DirectoryServer.php then you would enter raknet/DirectoryServer.php
	void Init(HTTPConnection *_http, const char *_path);

    /// Set a parameter (these are passed to the server)
	/// To delete a column, just pass an empty string for value
	/// Store the game name and port with UploadTable, rather than SetField, as these columns are required and use reserved column names
	/// \param[in] columnName The name of the column to store
	/// \param[in] value What value to hold for the uploaded row (only one row can be uploaded at a time)
	void SetField(RakNet::RakString columnName, RakNet::RakString value);

	/// Returns the number of fields set with SetField()
	unsigned int GetFieldCount(void) const;

	/// Returns a field set with SetField()
	/// \param[in] index The 0 based index into the field list
	/// \param[out] columnName The \a columnName parameter passed to SetField()
	/// \param[out] value The \a value parameter passed to SetField()
	void GetField(unsigned int index, RakNet::RakString &columnName, RakNet::RakString &value);

	/// Set all parameters at once from a table
	/// \param[in] table A table containing the values you want to send. Note that all values are stored as strings in PHP
	void SetFields(DataStructures::Table *table);

	/// Clear all fields
	void ClearFields(void);

	/// Upload the values set with SetFields() or SetField()
	/// On success:
	/// 1. HTTPConnection::HasRead() will return true.
	/// 2. Pass the value returned by HTTPConnection::Read() to PHPDirectoryServer2::ProcessHTTPRead().
	/// 3. The return value of PHPDirectoryServer2::ProcessHTTPRead() will be HTTP_RESULT_EMPTY		
	/// \param[in] uploadPassword The upload password set in the PHP page itself when you first uploaded and viewed it in the webpage.
	/// \param[in] gameName Every entry must have a game name. Pass it here.
	/// \param[in] gamePort Every entry must have a game port. Pass it here. The IP address will be stored automatically, or you can manually set it by passing a field named _System_Address
	/// \param[in] autoRepost Tables must be uploaded every 60 seconds or they get dropped. Set autoRepost to true to automatically reupload the most recent table.
	void UploadTable(RakNet::RakString uploadPassword, RakNet::RakString gameName, unsigned short gamePort, bool autoRepost);

	/// Send a download request to the PHP server.
	/// On success:
	/// 1. HTTPConnection::HasRead() will return true.
	/// 2. Pass the value returned by HTTPConnection::Read() to PHPDirectoryServer2::ProcessHTTPRead().
	/// 3. The return value of PHPDirectoryServer2::ProcessHTTPRead() will be HTTP_RESULT_GOT_TABLE or HTTP_RESULT_EMPTY
	/// 4. On HTTP_RESULT_GOT_TABLE, use GetLastDownloadedTable() to read the results.
	/// \param[in] downloadPassword The download password set in the PHP page itself when you first uploaded and viewed it in the webpage.
	void DownloadTable(RakNet::RakString downloadPassword);

	/// Same as calling DownloadTable immediately followed by UploadTable, except only the download result is returned
	/// \param[in] uploadPassword The upload password set in the PHP page itself when you first uploaded and viewed it in the webpage.
	/// \param[in] downloadPassword The download password set in the PHP page itself when you first uploaded and viewed it in the webpage.
	/// \param[in] gameName Every entry must have a game name. Pass it here.
	/// \param[in] gamePort Every entry must have a game port. Pass it here. The IP address will be stored automatically, or you can manually set it by passing a field named _System_Address
	/// \param[in] autoRepost Tables must be uploaded every 60 seconds or they get dropped. Set autoRepost to true to automatically reupload the most recent table.
	void UploadAndDownloadTable(RakNet::RakString uploadPassword, RakNet::RakString downloadPassword, RakNet::RakString gameName, unsigned short gamePort, bool autoRepost);

	/// When HTTPConnection::ProcessDataPacket() returns true, and not an error, pass HTTPConnection::Read() to this function
	/// The message will be parsed into DataStructures::Table, and a copy stored internally which can be retrieved by GetLastDownloadedTable();
	/// \param[in] packetData Returned from HTTPInterface::Read()
	/// \return One of the values for HTTPReadResult
	HTTPReadResult ProcessHTTPRead(RakNet::RakString httpRead);

	/// Returns the last value returned from ProcessHTTPString
	/// Default columns are "__GAME_NAME", "__GAME_PORT", "_System_Address"
	/// \return The table created by parsing httpString
	const DataStructures::Table *GetLastDownloadedTable(void) const;

	/// Call this periodically - it will handle connection states and refreshing updates to the server
	void Update(void);
    
private:
    HTTPConnection *http;
	RakNet::RakString pathToPHP;
    
	RakNet::RakString gameNameParam;
	unsigned short gamePortParam;

	void SendOperation(void);
	void PushColumnsAndValues(DataStructures::List<RakNet::RakString> &columns, DataStructures::List<RakNet::RakString> &values);

	DataStructures::Table lastDownloadedTable;
	DataStructures::Map<RakNet::RakString, RakNet::RakString> fields;
	RakNet::RakString currentOperation;
	RakNet::TimeMS nextRepost;

};

} // namespace RakNet

#endif

