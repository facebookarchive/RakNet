#include "SQLiteClientLoggerPlugin.h"
#include "MessageIdentifiers.h"
#include "PacketizedTCP.h"
#include "GetTime.h"

static const char COLUMN_NAMES_DELIMITER=',';
static const int MAX_COLUMN_NAMES_LENGTH=512;

using namespace RakNet;

SQLiteClientLoggerPlugin* SQLiteClientLoggerPlugin::logger;


SQLiteClientLoggerPlugin::SQLiteClientLoggerPlugin()
{
	logger=this;
	tickCount=0;
	recursiveCheck=false;
	memoryConstraint=0;
}
SQLiteClientLoggerPlugin::~SQLiteClientLoggerPlugin()
{
	if (logger==this)
		logger=0;
}
void SQLiteClientLoggerPlugin::SetServerParameters(const SystemAddress &systemAddress, RakNet::RakString _dbIdentifier)
{
	serverAddress=systemAddress;
	dbIdentifier=_dbIdentifier;
}
void SQLiteClientLoggerPlugin::SetMemoryConstraint(unsigned int constraint)
{
	memoryConstraint=constraint;
}
void SQLiteClientLoggerPlugin::IncrementAutoTickCount(void)
{
	tickCount++;
}
SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const ParameterListHelper &parameterList )
{
	if (recursiveCheck==true)
		return SQLLR_RECURSION;
	recursiveCheck=true;

	RakNet::BitStream bitStream;
	SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, parameterList.paramCount);
//	int i;
	//for (i=0; i < parameterList.paramCount; i++)
	//	parameterList.parms[i].Serialize(&bitStream);
	if (parameterList.paramCount>=1)
		parameterList.p0.Serialize(&bitStream);
	if (parameterList.paramCount>=2)
		parameterList.p1.Serialize(&bitStream);
	if (parameterList.paramCount>=3)
		parameterList.p2.Serialize(&bitStream);
	if (parameterList.paramCount>=4)
		parameterList.p3.Serialize(&bitStream);
	if (parameterList.paramCount>=5)
		parameterList.p4.Serialize(&bitStream);
	if (parameterList.paramCount>=6)
		parameterList.p5.Serialize(&bitStream);
	if (parameterList.paramCount>=7)
		parameterList.p6.Serialize(&bitStream);
	if (parameterList.paramCount>=8)
		parameterList.p7.Serialize(&bitStream);
	if (parameterList.paramCount>=9)
		parameterList.p8.Serialize(&bitStream);
	if (parameterList.paramCount>=10)
		parameterList.p9.Serialize(&bitStream);
	if (parameterList.paramCount>=11)
		parameterList.p10.Serialize(&bitStream);
	if (parameterList.paramCount>=12)
		parameterList.p11.Serialize(&bitStream);
	if (parameterList.paramCount>=13)
		parameterList.p12.Serialize(&bitStream);
	if (parameterList.paramCount>=14)
		parameterList.p13.Serialize(&bitStream);
	if (parameterList.paramCount>=15)
		parameterList.p14.Serialize(&bitStream);


	if (memoryConstraint!=0 && tcpInterface)
	{
		if (tcpInterface->GetOutgoingDataBufferSize(serverAddress)+bitStream.GetNumberOfBytesUsed()>=memoryConstraint)
		{
			recursiveCheck=false;
			return SQLLR_WOULD_EXCEED_MEMORY_CONSTRAINT;
		}
	}

	SendUnified(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
	recursiveCheck=false;
	return SQLLR_OK;
}
/*
SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line )
{
	RakNet::BitStream bitStream;
	SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 0);
	SendUnified(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
	return SQLLR_OK;
}
SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1 )
{
	RakNet::BitStream bitStream;
	SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 1);
	p1->Serialize(&bitStream);
	SendUnified(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
	return SQLLR_OK;
}
SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2 )
{
	RakNet::BitStream bitStream;
	SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 2);
	p1->Serialize(&bitStream);
	p2->Serialize(&bitStream);
	SendUnified(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
	return SQLLR_OK;
}
SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3 )
{
	RakNet::BitStream bitStream;
	SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 3);
	p1->Serialize(&bitStream);
	p2->Serialize(&bitStream);
	p3->Serialize(&bitStream);
	SendUnified(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
	return SQLLR_OK;
}
SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4 )
{
	RakNet::BitStream bitStream;
	SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 4);
	p1->Serialize(&bitStream);
	p2->Serialize(&bitStream);
	p3->Serialize(&bitStream);
	p4->Serialize(&bitStream);
	SendUnified(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
	return SQLLR_OK;
}
SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5 )
{
	RakNet::BitStream bitStream;
	SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 5);
	p1->Serialize(&bitStream);
	p2->Serialize(&bitStream);
	p3->Serialize(&bitStream);
	p4->Serialize(&bitStream);
	p5->Serialize(&bitStream);
	SendUnified(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
	return SQLLR_OK;
}
SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6 )
{
	RakNet::BitStream bitStream;
	SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 6);
	p1->Serialize(&bitStream);
	p2->Serialize(&bitStream);
	p3->Serialize(&bitStream);
	p4->Serialize(&bitStream);
	p5->Serialize(&bitStream);
	p6->Serialize(&bitStream);
	SendUnified(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
	return SQLLR_OK;
}
SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6, const LogParameter *p7 )
{
	RakNet::BitStream bitStream;
	SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 7);
	p1->Serialize(&bitStream);
	p2->Serialize(&bitStream);
	p3->Serialize(&bitStream);
	p4->Serialize(&bitStream);
	p5->Serialize(&bitStream);
	p6->Serialize(&bitStream);
	p7->Serialize(&bitStream);
	SendUnified(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
	return SQLLR_OK;
}
SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6, const LogParameter *p7, const LogParameter *p8 )
{
	RakNet::BitStream bitStream;
	SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 8);
	p1->Serialize(&bitStream);
	p2->Serialize(&bitStream);
	p3->Serialize(&bitStream);
	p4->Serialize(&bitStream);
	p5->Serialize(&bitStream);
	p6->Serialize(&bitStream);
	p7->Serialize(&bitStream);
	p8->Serialize(&bitStream);
	SendUnified(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
	return SQLLR_OK;
}
SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6, const LogParameter *p7, const LogParameter *p8, const LogParameter *p9 )
{
	RakNet::BitStream bitStream;
	SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 9);
	p1->Serialize(&bitStream);
	p2->Serialize(&bitStream);
	p3->Serialize(&bitStream);
	p4->Serialize(&bitStream);
	p5->Serialize(&bitStream);
	p6->Serialize(&bitStream);
	p7->Serialize(&bitStream);
	p8->Serialize(&bitStream);
	p9->Serialize(&bitStream);
	SendUnified(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
	return SQLLR_OK;
}
SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6, const LogParameter *p7, const LogParameter *p8, const LogParameter *p9, const LogParameter *p10 )
{
	RakNet::BitStream bitStream;
	SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 10);
	p1->Serialize(&bitStream);
	p2->Serialize(&bitStream);
	p3->Serialize(&bitStream);
	p4->Serialize(&bitStream);
	p5->Serialize(&bitStream);
	p6->Serialize(&bitStream);
	p7->Serialize(&bitStream);
	p8->Serialize(&bitStream);
	p9->Serialize(&bitStream);
	p10->Serialize(&bitStream);
	SendUnified(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
	return SQLLR_OK;
}
SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6, const LogParameter *p7, const LogParameter *p8, const LogParameter *p9, const LogParameter *p10, const LogParameter *p11 )
{
	RakNet::BitStream bitStream;
	SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 11);
	p1->Serialize(&bitStream);
	p2->Serialize(&bitStream);
	p3->Serialize(&bitStream);
	p4->Serialize(&bitStream);
	p5->Serialize(&bitStream);
	p6->Serialize(&bitStream);
	p7->Serialize(&bitStream);
	p8->Serialize(&bitStream);
	p9->Serialize(&bitStream);
	p10->Serialize(&bitStream);
	p11->Serialize(&bitStream);
	SendUnified(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
	return SQLLR_OK;
}
SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6, const LogParameter *p7, const LogParameter *p8, const LogParameter *p9, const LogParameter *p10, const LogParameter *p11, const LogParameter *p12  )
{
	RakNet::BitStream bitStream;
	SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 12);
	p1->Serialize(&bitStream);
	p2->Serialize(&bitStream);
	p3->Serialize(&bitStream);
	p4->Serialize(&bitStream);
	p5->Serialize(&bitStream);
	p6->Serialize(&bitStream);
	p7->Serialize(&bitStream);
	p8->Serialize(&bitStream);
	p9->Serialize(&bitStream);
	p10->Serialize(&bitStream);
	p11->Serialize(&bitStream);
	p12->Serialize(&bitStream);
	SendUnified(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
	return SQLLR_OK;
}
*/
SQLLogResult SQLiteClientLoggerPlugin::CheckQuery(bool isFunction, const char *tableName, const char *columnNames, int numParameters)
{
	(void) isFunction;

	if (recursiveCheck==true)
		return SQLLR_RECURSION;

	if (tableName==0 || tableName[0]==0)
		return SQLLR_TABLE_NAME_BLANK;
	if (isFunction==true)
		return SQLLR_OK;

	if (columnNames==0 || columnNames[0]==0)
	{
		if (numParameters==0)
			return SQLLR_OK;
		return SQLLR_TABLE_DESCRIPTOR_FORMAT_WRONG_PARAMETER_COUNT;
	}

	recursiveCheck=true;

	if (dbIdentifier.IsEmpty())
	{
		recursiveCheck=false;
		return SQLLR_NO_DATABASE_SET;
	}

	unsigned int parameterCount=1;
	unsigned int columnNamesIndex=0;
	for (columnNamesIndex=1; columnNamesIndex < 512 && columnNames[columnNamesIndex]; columnNamesIndex++)
	{
		if (columnNames[columnNamesIndex]==COLUMN_NAMES_DELIMITER)
		{
			if (columnNames[columnNamesIndex-1]==COLUMN_NAMES_DELIMITER)
			{
				recursiveCheck=false;
				return SQLLR_TABLE_DESCRIPTOR_FORMAT_INVALID_SYNTAX;
			}
			parameterCount++;
		}
	}

	recursiveCheck=false;
	if (columnNamesIndex==MAX_COLUMN_NAMES_LENGTH)
	{
		return SQLLR_COLUMN_NAMES_NOT_TERMINATED;
	}

	if (parameterCount!=numParameters)
	{
		return SQLLR_TABLE_DESCRIPTOR_FORMAT_WRONG_PARAMETER_COUNT;
	}

	return SQLLR_OK;
}

void SQLiteClientLoggerPlugin::SerializeHeader(RakNet::BitStream *bitStream, bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, unsigned char parameterCount ) const
{
	bitStream->Write((MessageID) ID_SQLLITE_LOGGER);
	bitStream->Write(dbIdentifier);
	bitStream->Write(tableName);
	bitStream->Write(line);
	bitStream->Write(file);
	bitStream->Write(tickCount);
	bitStream->Write(RakNet::GetTimeMS());
	bitStream->Write(isFunctionCall);
	bitStream->Write(parameterCount);
	if (isFunctionCall==false && parameterCount>=1)
	{
		int stringIndices[64];
		int strIndex=0;
		stringIndices[strIndex++]=0;
		char columnNamesCopy[MAX_COLUMN_NAMES_LENGTH];
		RakAssert(strlen(columnNames)<MAX_COLUMN_NAMES_LENGTH);
		strncpy(columnNamesCopy, columnNames, MAX_COLUMN_NAMES_LENGTH);
		columnNamesCopy[MAX_COLUMN_NAMES_LENGTH-1]=0;
		for (int i=0; columnNamesCopy[i]; i++)
		{
			if (columnNamesCopy[i]==COLUMN_NAMES_DELIMITER)
			{
				columnNamesCopy[i]=0;
				stringIndices[strIndex++]=i+1;
			}
		}
		RakAssert(strIndex==parameterCount);
		for (int i=0; i < parameterCount; i++)
		{
			bitStream->Write((char*)columnNamesCopy + stringIndices[i]);
		}
	}
}
void SQLiteClientLoggerPlugin::Update(void)
{
	SQLite3ClientPlugin::Update();
}
