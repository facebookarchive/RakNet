/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "MySQLInterface.h"
#include "RakAssert.h"
#include "BitStream.h"
#include "FormatString.h"
#include "LinuxStrings.h"
#include <errmsg.h>


#ifdef _WIN32
#elif defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)

#else
#include <stdlib.h>//atoi
#endif

#ifdef _WIN32
#include <Winsock2.h>
#endif
#include "mysql.h"

MySQLInterface::MySQLInterface()
{
	mySqlConnection=0;
    lastError[0]=0;
}

MySQLInterface::~MySQLInterface()
{
    Disconnect();
}

bool MySQLInterface::Connect(const char *host,
							 const char *user,
							 const char *passwd,
							 const char *db,
							 unsigned int port,
							 const char *unix_socket,
							 unsigned long clientflag)
{
	if (IsConnected())
		return false;

	_host=host;
	_user=user;
	_passwd=passwd;
	_db=db;
	_port=port;
	_unix_socket=unix_socket;
	_clientflag=clientflag;


	mySqlConnection = mysql_init(0);
	return mysql_real_connect (mySqlConnection, host, user, passwd, db, port, unix_socket, clientflag) != 0;
}

void MySQLInterface::Disconnect(void)
{
	if (IsConnected())
	{
		mysql_close(mySqlConnection);
		mySqlConnection = 0;
	}
}

bool MySQLInterface::IsConnected(void) const
{
	return mySqlConnection != 0;
}

void MySQLInterface::Commit(void)
{
	mysql_query (mySqlConnection, "COMMIT;");
}

void MySQLInterface::Rollback(void)
{
    mysql_query (mySqlConnection, "ROLLBACK");
}

bool MySQLInterface::ExecuteBlockingCommand(const char *command)
{
	int queryResult;
	if ((queryResult=mysql_query(mySqlConnection, command)) != 0)
    {
        strcpy (lastError, mysql_error (mySqlConnection));
	    return false;
	}

	return true;    
}

bool MySQLInterface::ExecuteBlockingCommand(const char *command, MYSQL_RES **result, bool rollbackOnFailure)
{
	if (!ExecuteBlockingCommand (command))
		return false;

	*result = mysql_store_result (mySqlConnection);
	return *result != 0;
}
char *MySQLInterface::GetLocalTimestamp(void)
{
	static char resultString[512];

	MYSQL_RES *result;
	if (ExecuteBlockingCommand("SELECT LOCALTIMESTAMP", &result, false))
	{
	    MYSQL_ROW row = mysql_fetch_row (result);
	    if (row [0])
			sprintf(resultString,"%s\n", row [0]);
		else
			resultString[0]=0;

	    mysql_free_result(result);
	}
	else
		resultString[0]=0;

	return (char*)resultString;

}

bool MySQLInterface::ExecuteQueryReadInt (const char * query, int *value)
{
	MYSQL_RES * result=0;
	if (!ExecuteBlockingCommand(query, &result, false))
	{
		mysql_free_result(result);
		return false;
	}
    
	MYSQL_ROW row = mysql_fetch_row (result);
	if (row == 0 || mysql_num_fields (result) != 1)
	{
		mysql_free_result(result);
		return false;
	}

	*value = atoi(row [0]);
	mysql_free_result(result);

	return true;	
}


const char* MySQLInterface::GetLastError(void) const
{               
    return lastError;
}

RakNet::RakString MySQLInterface::GetEscapedString(const char *input) const
{
	unsigned long len = (unsigned long) strlen(input);
	char *fn = new char [len*2+1];
	mysql_real_escape_string(mySqlConnection, fn, input, len);
	RakNet::RakString output;
	// Use assignment so it doesn't parse printf escape strings
	output = fn;
	delete [] fn;
	return output;
}
