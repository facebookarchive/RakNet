/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "PostgreSQLInterface.h"
#include "VariadicSQLParser.h"

// libpq-fe.h is part of PostgreSQL which must be installed on this computer to use the PostgreRepository
#include "libpq-fe.h"

#ifdef _CONSOLE_1
#include "Console1Includes.h"
#elif defined(_WIN32)
#include <winsock2.h> // htonl
#elif defined(_CONSOLE_2)
#include "Console2Includes.h"
#else
#include <arpa/inet.h>
#endif

// alloca
#ifdef _COMPATIBILITY_1
#elif defined(_WIN32)
#include <malloc.h>
#else
//#include <stdlib.h>
#endif

#include "RakString.h"
#include "RakAssert.h"
#include "BitStream.h"
#include "FormatString.h"
#include "LinuxStrings.h"

#define PQEXECPARAM_FORMAT_TEXT		0
#define PQEXECPARAM_FORMAT_BINARY	1

PostgreSQLInterface::PostgreSQLInterface()
{
	pgConn=0;
	isConnected=false;
	lastError[0]=0;
	pgConnAllocatedHere=false;
}
PostgreSQLInterface::~PostgreSQLInterface()
{
	if (isConnected && pgConnAllocatedHere)
		PQfinish(pgConn);
}
bool PostgreSQLInterface::Connect(const char *conninfo)
{
	if (isConnected==false)
	{
		_conninfo=conninfo;

		pgConn=PQconnectdb(conninfo);
		ConnStatusType status = PQstatus(pgConn);
		isConnected=status==CONNECTION_OK;
		if (isConnected==false)
		{
			PQfinish(pgConn);
			return false;
		}
		pgConnAllocatedHere=true;
	}

	return isConnected;
}
void PostgreSQLInterface::AssignConnection(PGconn *_pgConn)
{
	pgConnAllocatedHere=false;
	pgConn=_pgConn;
	ConnStatusType status = PQstatus(pgConn);
	isConnected=status==CONNECTION_OK;
}
PGconn *PostgreSQLInterface::GetPGConn(void) const
{
	return pgConn;
}
void PostgreSQLInterface::Disconnect(void)
{
	if (isConnected)
	{
		PQfinish(pgConn);
		isConnected=false;
	}
}
void PostgreSQLInterface::Rollback(void)
{
	PGresult *result = PQexec(pgConn, "ROLLBACK;");
	PQclear(result);
}
bool PostgreSQLInterface::IsResultSuccessful(PGresult *result, bool rollbackOnFailure)
{
	if (result==0)
		return false;

	bool success=false;
	ExecStatusType execStatus = PQresultStatus(result);
	strcpy(lastError,PQresultErrorMessage(result));
	switch (execStatus)
	{
	case PGRES_COMMAND_OK:
		success=true;
		break;
	case PGRES_EMPTY_QUERY:
		break;
	case PGRES_TUPLES_OK:
		success=true;
		break;
	case PGRES_COPY_OUT:
		break;
	case PGRES_COPY_IN:
		break;
	case PGRES_BAD_RESPONSE:
		break;
	case PGRES_NONFATAL_ERROR:
		break;
	case PGRES_FATAL_ERROR:
		if (rollbackOnFailure)
			Rollback();
		break;
	}
	return success;
}
bool PostgreSQLInterface::ExecuteBlockingCommand(const char *command, PGresult **result, bool rollbackOnFailure)
{
	*result = PQexec(pgConn, command);
	return IsResultSuccessful(*result, rollbackOnFailure);
}
bool PostgreSQLInterface::PQGetValueFromBinary(int *output, PGresult *result, unsigned int rowIndex, const char *columnName)
{
	return PQGetValueFromBinary(output,result,(int) rowIndex,columnName);
}
bool PostgreSQLInterface::PQGetValueFromBinary(int *output, PGresult *result, int rowIndex, const char *columnName)
{
	int columnIndex = PQfnumber(result, columnName); if (columnIndex==-1) return false;
	char *binaryData = PQgetvalue(result, rowIndex, columnIndex);
	if (binaryData==0)
		return false;
	if (binaryData)
	{
		RakAssert(PQgetlength(result, rowIndex, columnIndex)==sizeof(int));
		memcpy(output, binaryData, sizeof(int));
		EndianSwapInPlace((char*)output, sizeof(int));
		return true;
	}
	return false;
}
bool PostgreSQLInterface::PQGetValueFromBinary(unsigned int *output, PGresult *result, int rowIndex, const char *columnName)
{
	int columnIndex = PQfnumber(result, columnName); if (columnIndex==-1) return false;
	char *binaryData = PQgetvalue(result, rowIndex, columnIndex);
	if (binaryData==0 || PQgetlength(result, rowIndex, columnIndex)==0)
		return false;	
	if (binaryData)
	{
		RakAssert(PQgetlength(result, rowIndex, columnIndex)==sizeof(unsigned int));
		memcpy(output, binaryData, sizeof(unsigned int));
		EndianSwapInPlace((char*)output, sizeof(unsigned int));
		return true;
	}
	return false;
}
bool PostgreSQLInterface::PQGetValueFromBinary(long long *output, PGresult *result, int rowIndex, const char *columnName)
{
	int columnIndex = PQfnumber(result, columnName); if (columnIndex==-1) return false;
	char *binaryData = PQgetvalue(result, rowIndex, columnIndex);
	if (binaryData==0)
		return false;
	if (binaryData)
	{
		RakAssert(PQgetlength(result, rowIndex, columnIndex)==sizeof(long long));
		memcpy(output, binaryData, sizeof(long long));
		EndianSwapInPlace((char*)output, sizeof(long long));
		return true;
	}
	return false;
}
bool PostgreSQLInterface::PQGetValueFromBinary(float *output, PGresult *result, int rowIndex, const char *columnName)
{
	int columnIndex = PQfnumber(result, columnName); if (columnIndex==-1) return false;
	char *binaryData = PQgetvalue(result, rowIndex, columnIndex);
	if (binaryData==0)
		return false;
	int len = PQgetlength(result, rowIndex, columnIndex);
	RakAssert(len==sizeof(float));
	RakAssert(binaryData);
	if (binaryData)
	{
		memcpy(output, binaryData, sizeof(float));
		EndianSwapInPlace((char*)output, sizeof(float));
		return true;
	}
	return false;
}
bool PostgreSQLInterface::PQGetValueFromBinary(double *output, PGresult *result, int rowIndex, const char *columnName)
{
	int columnIndex = PQfnumber(result, columnName); if (columnIndex==-1) return false;
	char *binaryData = PQgetvalue(result, rowIndex, columnIndex);
	if (binaryData==0)
		return false;
	int len = PQgetlength(result, rowIndex, columnIndex);
	RakAssert(len==sizeof(double));
	RakAssert(binaryData);
	if (binaryData)
	{
		memcpy(output, binaryData, sizeof(double));
		EndianSwapInPlace((char*)output, sizeof(double));
		return true;
	}
	return false;
}
bool PostgreSQLInterface::PQGetValueFromBinary(bool *output, PGresult *result, int rowIndex, const char *columnName)
{
	int columnIndex = PQfnumber(result, columnName); if (columnIndex==-1) return false;
	char *binaryData = PQgetvalue(result, rowIndex, columnIndex);
	if (binaryData==0)
		return false;
	*output = binaryData[0]!=0;
	
	// Seems to return 1 byte only
	/*
	int tempResult=0;
//	int len = PQgetlength(result, rowIndex, columnIndex);
	// RakAssert(len==sizeof(int)); // bug here, returns 0, but is an int
	RakAssert(binaryData);
	if (binaryData)
	{
		memcpy(&tempResult, binaryData, sizeof(int));
		EndianSwapInPlace((char*)&tempResult, sizeof(int));
		*output = tempResult!=0;
		return true;
	}
	*/
	
	return true;
}
bool PostgreSQLInterface::PQGetValueFromBinary(RakNet::RakString *output, PGresult *result, int rowIndex, const char *columnName)
{
	int columnIndex = PQfnumber(result, columnName); if (columnIndex==-1) return false;
	char *gv;
	gv = PQgetvalue(result, rowIndex, columnIndex);
	if (gv && gv[0])
		*output=gv;
	else
		output->Clear();
	return output->IsEmpty()==false;
}
bool PostgreSQLInterface::PQGetValueFromBinary(char **output, unsigned int *outputLength, PGresult *result, int rowIndex, const char *columnName)
{
	return PQGetValueFromBinary(output, (int*) outputLength,result, rowIndex,columnName);
}
bool PostgreSQLInterface::PQGetValueFromBinary(char **output, int *outputLength, PGresult *result, int rowIndex, const char *columnName)
{
	int columnIndex = PQfnumber(result, columnName);
	if (columnIndex!=-1)
	{
		*outputLength=PQgetlength(result, rowIndex, columnIndex);
		if (*outputLength==0)
		{
			*output=0;
			return false;
		}
		else
		{
			*output = (char*) rakMalloc_Ex(*outputLength,_FILE_AND_LINE_);
			memcpy(*output, PQgetvalue(result, rowIndex, columnIndex), *outputLength);
			return true;
		}
	}
	else
		return false;
	
}
void PostgreSQLInterface::EndianSwapInPlace(char* data, int dataLength)
{
	static bool alreadyNetworkOrder=(htonl(12345) == 12345);
	if (alreadyNetworkOrder)
		return;
	int i;
	char tmp;
	for (i=0; i < dataLength/2; i++)
	{
		tmp=data[i];
		data[i]=data[dataLength-1-i];
		data[dataLength-1-i]=tmp;
	}
}

char *PostgreSQLInterface::GetLocalTimestamp(void)
{
	static char strRes[512];

	PGresult *result;
	if (ExecuteBlockingCommand("SELECT LOCALTIMESTAMP", &result, false))
	{
		char *ts=PQgetvalue(result, 0, 0);
		if (ts)
		{
			sprintf(strRes,"Local timestamp is: %s\n", ts);
		}
		else
		{
			sprintf(strRes,"Can't read current time\n");
		}
		PQclear(result);
	}
	else
		sprintf(strRes,"Failed to read LOCALTIMESTAMP\n");

	return (char*)strRes;
}
long long PostgreSQLInterface::GetEpoch(void)
{
	PGresult *result;
	long long out;
	result = QueryVariadic("EXTRACT(EPOCH FROM current_timestamp) as out);");
	PostgreSQLInterface::PQGetValueFromBinary(&out, result, 0, "out");
	PQclear(result);
	return out;
}

const char* PostgreSQLInterface::GetLastError(void) const
{
	return (char*)lastError;
}
void PostgreSQLInterface::EncodeQueryInput(const char *colName, unsigned int value, RakNet::RakString &paramTypeStr, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
{
	(void)numParams;
	(void)paramData;
	(void)paramLength;
	(void)paramFormat;

	if (paramTypeStr.IsEmpty()==false)
	{
		paramTypeStr += ", ";
		valueStr += ", ";
	}
	paramTypeStr += colName;
	valueStr += FormatString("%i", value);
}
void PostgreSQLInterface::EncodeQueryUpdate(const char *colName, unsigned int value, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
{
	(void)numParams;
	(void)paramData;
	(void)paramLength;
	(void)paramFormat;

	if (valueStr.IsEmpty()==false)
	{
		valueStr += ", ";
	}
	valueStr += colName;
	valueStr += " = ";
	valueStr += FormatString("%i", value);
}
void PostgreSQLInterface::EncodeQueryInput(const char *colName, int value, RakNet::RakString &paramTypeStr, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
{
	(void)numParams;
	(void)paramData;
	(void)paramLength;
	(void)paramFormat;

	if (paramTypeStr.IsEmpty()==false)
	{
		paramTypeStr += ", ";
		valueStr += ", ";
	}
	paramTypeStr += colName;
	valueStr += FormatString("%i", value);
}
void PostgreSQLInterface::EncodeQueryInput(const char *colName, bool value, RakNet::RakString &paramTypeStr, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
{
	(void)numParams;
	(void)paramData;
	(void)paramLength;
	(void)paramFormat;

	if (paramTypeStr.IsEmpty()==false)
	{
		paramTypeStr += ", ";
		valueStr += ", ";
	}
	paramTypeStr += colName;
	if (value)
		valueStr += "true";
	else
		valueStr += "false";
}
void PostgreSQLInterface::EncodeQueryUpdate(const char *colName, int value, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
{
	(void)numParams;
	(void)paramData;
	(void)paramLength;
	(void)paramFormat;

	if (valueStr.IsEmpty()==false)
	{
		valueStr += ", ";
	}
	valueStr += colName;
	valueStr += " = ";
	valueStr += FormatString("%i", value);
}
void PostgreSQLInterface::EncodeQueryInput(const char *colName, float value, RakNet::RakString &paramTypeStr, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
{
	(void)numParams;
	(void)paramData;
	(void)paramLength;
	(void)paramFormat;

	if (paramTypeStr.IsEmpty()==false)
	{
		paramTypeStr += ", ";
		valueStr += ", ";
	}
	paramTypeStr += colName;
	valueStr += FormatString("%f", value);
}
void PostgreSQLInterface::EncodeQueryUpdate(const char *colName, float value, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
{
	(void)numParams;
	(void)paramData;
	(void)paramLength;
	(void)paramFormat;

	if (valueStr.IsEmpty()==false)
	{
		valueStr += ", ";
	}
	valueStr += colName;
	valueStr += " = ";
	valueStr += FormatString("%f", value);
}
void PostgreSQLInterface::EncodeQueryInput(const char *colName, char *binaryData, int binaryDataLength, RakNet::RakString &paramTypeStr, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat, bool writeEmpty)
{
	if (writeEmpty==false && (binaryData==0 || binaryDataLength==0))
		return;

	if (binaryData==0)
		binaryDataLength=0;

	if (paramTypeStr.IsEmpty()==false)
	{
		paramTypeStr += ", ";
		valueStr += ", ";
	}
	paramTypeStr += colName;
	valueStr+=FormatString("$%i::bytea", numParams+1);

	paramData[numParams]=binaryData;
	paramLength[numParams]=binaryDataLength;
	paramFormat[numParams]=PQEXECPARAM_FORMAT_BINARY;
	numParams++;
}
void PostgreSQLInterface::EncodeQueryUpdate(const char *colName, char *binaryData, int binaryDataLength, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
{
	if (binaryData==0 || binaryDataLength==0)
		return;

	if (binaryData==0)
		binaryDataLength=0;

	if (valueStr.IsEmpty()==false)
	{
		valueStr += ", ";
	}
	valueStr += colName;
	valueStr += " = ";
	valueStr+=FormatString("$%i::bytea", numParams+1);

	paramData[numParams]=binaryData;
	paramLength[numParams]=binaryDataLength;
	paramFormat[numParams]=PQEXECPARAM_FORMAT_BINARY;
	numParams++;
}
void PostgreSQLInterface::EncodeQueryInput(const char *colName, const char *str, RakNet::RakString &paramTypeStr, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat, bool writeEmpty, const char *type)
{
	if (writeEmpty==false && (str==0 || str[0]==0))
		return;

	static char *emptyStr=(char*)"";
	static char *emptyDate=(char*)"01/01/01";

	if (paramTypeStr.IsEmpty()==false)
	{
		paramTypeStr += ", ";
		valueStr += ", ";
	}
	paramTypeStr += colName;
	valueStr+=FormatString("$%i::%s", numParams+1, type);

	if (writeEmpty && (str==0 || str[0]==0))
	{
		if (strcmp(type,"date")==0)
		{
			paramData[numParams]=emptyDate;
			paramLength[numParams]=(int)strlen(emptyDate);
		}
		else
		{
			paramData[numParams]=emptyStr;
			paramLength[numParams]=0;
		}		
	}
	else
	{
		paramData[numParams]=(char*) str;
		paramLength[numParams]=(int) strlen(str);
	}	
	paramFormat[numParams]=PQEXECPARAM_FORMAT_TEXT;
	numParams++;
}
void PostgreSQLInterface::EncodeQueryUpdate(const char *colName, const char *str, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat, const char *type)
{
	if (str==0 || str[0]==0)
		return;

	if (valueStr.IsEmpty()==false)
	{
		valueStr += ", ";
	}
	valueStr += colName;
	valueStr+=" = ";
	valueStr+=FormatString("$%i::%s", numParams+1, type);

	paramData[numParams]=(char*) str;
	paramLength[numParams]=(int) strlen(str);

	paramFormat[numParams]=PQEXECPARAM_FORMAT_TEXT;
	numParams++;
}
void PostgreSQLInterface::EncodeQueryInput(const char *colName, const RakNet::RakString &str, RakNet::RakString &paramTypeStr, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat, bool writeEmpty, const char *type)
{
	EncodeQueryInput(colName, str.C_String(), paramTypeStr, valueStr, numParams, paramData, paramLength, paramFormat, writeEmpty, type);
}
void PostgreSQLInterface::EncodeQueryUpdate(const char *colName, const RakNet::RakString &str, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat, const char *type)
{
	EncodeQueryUpdate(colName, str.C_String(), valueStr, numParams, paramData, paramLength, paramFormat, type);
}
RakNet::RakString PostgreSQLInterface::GetEscapedString(const char *input) const
{
	unsigned long len = (unsigned long) strlen(input);
	char *fn = (char*) rakMalloc_Ex(len*2+1,_FILE_AND_LINE_);
	int error;
	PQescapeStringConn(pgConn, fn, input, len, &error);
	RakNet::RakString output;
	// Use assignment so it doesn't parse printf escape strings
	output = fn;
	rakFree_Ex(fn,_FILE_AND_LINE_);
	return output;
}

PGresult * PostgreSQLInterface::QueryVariadic( const char * input, ... )
{
	RakNet::RakString query;
	PGresult *result;
	DataStructures::List<VariadicSQLParser::IndexAndType> indices;
	if ( input==0 || input[0]==0 )
		return 0;

	// Lookup this query in the stored query table. If it doesn't exist, prepare it.
	RakNet::RakString inputStr;
	inputStr=input;
	unsigned int preparedQueryIndex;
	for (preparedQueryIndex=0; preparedQueryIndex < preparedQueries.Size(); preparedQueryIndex++)
	{
		if (preparedQueries[preparedQueryIndex].StrICmp(inputStr)==0)
			break;
	}

	// Find out how many params there are
	// Find out the type of each param (%f, %s)
	indices.Clear(false, _FILE_AND_LINE_);
	GetTypeMappingIndices( input, indices );

	if (preparedQueryIndex==preparedQueries.Size())
	{
//		if (indices.Size()>0)
//			query += " (";
		RakNet::RakString formatCopy;
		RakNet::RakString insertion;
		formatCopy=input;
		unsigned int i;
		unsigned int indexOffset=0;
		for (i=0; i < indices.Size(); i++)
		{
//			if (i!=0)
//				query += ",";
//			query+=typeMappings[indices[i].typeMappingIndex].type;
			formatCopy.SetChar(indices[i].strIndex+indexOffset, '$');
//			if (i < 9)
//				formatCopy.SetChar(indices[i].strIndex+1, i+1+'0');
//			else
			insertion=RakNet::RakString("%i::%s", i+1, VariadicSQLParser::GetTypeMappingAtIndex(indices[i].typeMappingIndex));
			formatCopy.SetChar(indices[i].strIndex+1+indexOffset, insertion);
			indexOffset+=(unsigned int) insertion.GetLength()-1;
		}
//		if (indices.Size()>0)
//			query += ")";
//		query += " AS ";
		query += formatCopy;
	//	query += ";\n";
		formatCopy+= ";\n";
		result = PQprepare(pgConn, RakNet::RakString("PGSQL_ExecuteVariadic_%i", preparedQueries.Size()), formatCopy.C_String(), indices.Size(), NULL);
		if (IsResultSuccessful(result, false))
		{
			PQclear(result);
			preparedQueries.Insert(inputStr, _FILE_AND_LINE_);
		}
		else
		{
			printf(formatCopy.C_String());
			printf("\n");
			printf(lastError);
			RakAssert(0);
			PQclear(result);
			return 0;
		}
	}

//	char *paramData[512];
//	int paramLength[512];
//	int paramFormat[512];

	va_list argptr;
	va_start(argptr, input);
	char **paramData;
	int *paramLength;
	int *paramFormat;
	ExtractArguments(argptr, indices, &paramData, &paramLength);
	paramFormat=RakNet::OP_NEW_ARRAY<int>(indices.Size(),_FILE_AND_LINE_);
	for (unsigned int i=0; i < indices.Size(); i++)
		paramFormat[i]=PQEXECPARAM_FORMAT_BINARY;
	result = PQexecPrepared(pgConn, RakNet::RakString("PGSQL_ExecuteVariadic_%i", preparedQueryIndex), indices.Size(), paramData, paramLength, paramFormat, PQEXECPARAM_FORMAT_BINARY );
	VariadicSQLParser::FreeArguments(indices, paramData, paramLength);
	RakNet::OP_DELETE_ARRAY(paramFormat,_FILE_AND_LINE_);
	va_end(argptr);

	if (IsResultSuccessful(result, false)==false)
	{
		printf(lastError);
		PQclear(result);
		return 0;
	}
	return result;
}
/*
PGresult * PostgreSQLInterface::QueryVariadic( const char * input, ... )
{
	RakNet::RakString query;
	PGresult *result;
	unsigned int i;
	DataStructures::List<IndexAndType> indices;
	if ( input==0 || input[0]==0 )
		return 0;

	// Find out how many params there are
	// Find out the type of each param (%f, %s)
	GetTypeMappingIndices( input, indices );

	char *paramData[512];
	int paramLength[512];
	int paramFormat[512];

	va_list argptr;
	int variadicArgIndex;
	va_start(argptr, input);
	for (variadicArgIndex=0, i=0; i < indices.Size(); i++, variadicArgIndex++)
	{
		if (typeMappings[indices[i].typeMappingIndex].inputType=='i' ||
			typeMappings[indices[i].typeMappingIndex].inputType=='d')
		{
			int val = va_arg( argptr, int );
			paramData[i]=(char*) &val;
			paramLength[i]=sizeof(val);
			if (RakNet::BitStream::IsNetworkOrder()==false) RakNet::BitStream::ReverseBytesInPlace((unsigned char*) paramData[i], paramLength[i]);
		}
		else if (typeMappings[indices[i].typeMappingIndex].inputType=='s')
		{
			char* val = va_arg( argptr, char* );
			paramData[i]=val;
			paramLength[i]=(int) strlen(val);
		}
		else if (typeMappings[indices[i].typeMappingIndex].inputType=='b')
		{
			bool val = va_arg( argptr, bool );
			paramData[i]=(char*) &val;
			paramLength[i]=sizeof(bool);
			if (RakNet::BitStream::IsNetworkOrder()==false) RakNet::BitStream::ReverseBytesInPlace((unsigned char*) paramData[i], paramLength[i]);
		}
		else if (typeMappings[indices[i].typeMappingIndex].inputType=='f')
		{
			float val = va_arg( argptr, float );
			paramData[i]=(char*) &val;
			paramLength[i]=sizeof(float);
			if (RakNet::BitStream::IsNetworkOrder()==false) RakNet::BitStream::ReverseBytesInPlace((unsigned char*) paramData[i], paramLength[i]);
		}
		else if (typeMappings[indices[i].typeMappingIndex].inputType=='g')
		{
			double val = va_arg( argptr, double );
			paramData[i]=(char*) &val;
			paramLength[i]=sizeof(double);
			if (RakNet::BitStream::IsNetworkOrder()==false) RakNet::BitStream::ReverseBytesInPlace((unsigned char*) paramData[i], paramLength[i]);
		}
		else if (typeMappings[indices[i].typeMappingIndex].inputType=='a')
		{
			char* val = va_arg( argptr, char* );
			paramData[i]=val;
			variadicArgIndex++;
			paramLength[i]=va_arg( argptr, unsigned int );
		}
		paramFormat[i]=PQEXECPARAM_FORMAT_BINARY;
	}

	// Replace each %whatever with $index::type
	RakNet::RakString inputCopy;
	inputCopy=input;
	unsigned int lastIndex=0;
	for (i=0; i < indices.Size(); i++)
	{
		query+=inputCopy.SubStr(lastIndex, indices[i].strIndex-lastIndex);
		query+=RakNet::RakString("$%i::", i+1);
		query+=typeMappings[indices[i].typeMappingIndex].type;
		lastIndex=indices[i].strIndex+2; // +2 is to skip the %whateverCharacter
	}
	query+=inputCopy.SubStr(lastIndex, (unsigned int)-1);

	result = PQexecParams(pgConn, query.C_String(),indices.Size(),0,paramData,paramLength,paramFormat,PQEXECPARAM_FORMAT_BINARY);
	if (IsResultSuccessful(result, false)==false)
	{
		PQclear(result);
		return 0;
	}
	return result;
}
*/
void PostgreSQLInterface::ClearResult(PGresult *result)
{
	PQclear(result);
}