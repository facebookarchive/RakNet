#include "SQLite3ClientPlugin.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"

using namespace RakNet;

void SQLite3PluginResultInterface_Printf::_sqlite3_exec(
	RakNet::RakString inputStatement,
	unsigned int queryId,
	RakNet::RakString dbIdentifier,
	const SQLite3Table &table,
	RakNet::RakString errorMsg)
{

	if (errorMsg.IsEmpty()==false)
	{
		printf("Error for query: %s\n", inputStatement.C_String());
		printf("%s\n", errorMsg.C_String());
		return;
	}

	unsigned int idx;
	for (idx=0; idx < table.columnNames.Size(); idx++)
		printf("%s ", table.columnNames[idx].C_String());
	printf("\n");
	if (table.rows.Size()==0)
	{
		printf("<NO ROWS>\n");
	}
	else
	{
		for (idx=0; idx < table.rows.Size(); idx++)
		{
			unsigned int idx2;
			for (idx2=0; idx2 < table.rows[idx]->entries.Size(); idx2++)
				printf("%s ", table.rows[idx]->entries[idx2].C_String());
			printf("\n");
		}
	}
}
void SQLite3PluginResultInterface_Printf::OnUnknownDBIdentifier(
	RakNet::RakString inputStatement,
	unsigned int queryId,
	RakNet::RakString dbIdentifier)
{
	printf("Unknown DB %s\n", dbIdentifier.C_String());
}
SQLite3ClientPlugin::SQLite3ClientPlugin()
{
	nextQueryId=0;
}
SQLite3ClientPlugin::~SQLite3ClientPlugin()
{
}
void SQLite3ClientPlugin::AddResultHandler(SQLite3PluginResultInterface *res)
{
	resultHandlers.Push(res,_FILE_AND_LINE_);
}
void SQLite3ClientPlugin::RemoveResultHandler(SQLite3PluginResultInterface *res)
{
	unsigned int idx = resultHandlers.GetIndexOf(res);
	if (idx!=-1)
		resultHandlers.RemoveAtIndex(idx);
}
void SQLite3ClientPlugin::ClearResultHandlers(void)
{
	resultHandlers.Clear(true,_FILE_AND_LINE_);
}
unsigned int SQLite3ClientPlugin::_sqlite3_exec(RakNet::RakString dbIdentifier, RakNet::RakString inputStatement,
										  PacketPriority priority, PacketReliability reliability, char orderingChannel, const SystemAddress &systemAddress)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)ID_SQLite3_EXEC);
	bsOut.Write(nextQueryId);
	bsOut.Write(dbIdentifier);
	bsOut.Write(inputStatement);
	bsOut.Write(true);
	SendUnified(&bsOut, priority,reliability,orderingChannel,systemAddress,false);
	++nextQueryId;
	return nextQueryId-1;
}

PluginReceiveResult SQLite3ClientPlugin::OnReceive(Packet *packet)
{
	switch (packet->data[0])
	{
	case ID_SQLite3_EXEC:
		{
			unsigned int queryId;
			RakNet::RakString dbIdentifier;
			RakNet::RakString inputStatement;
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));
			bsIn.Read(queryId);
			bsIn.Read(dbIdentifier);
			bsIn.Read(inputStatement);
			bool isRequest;
			bsIn.Read(isRequest);
			if (isRequest)
			{
				// Server code
				RakAssert(0);
			}
			else
			{
				// Client code
				RakNet::RakString errorMsgStr;
				SQLite3Table inputTable;		

				// Read it
				bsIn.Read(errorMsgStr);
				inputTable.Deserialize(&bsIn);

				unsigned int idx;
				for (idx=0; idx < resultHandlers.Size(); idx++)
					resultHandlers[idx]->_sqlite3_exec(inputStatement, queryId, dbIdentifier, inputTable,errorMsgStr);
			}

			return RR_STOP_PROCESSING_AND_DEALLOCATE;
		}
		break;
	case ID_SQLite3_UNKNOWN_DB:
		{
			unsigned int queryId;
			RakNet::RakString dbIdentifier;
			RakNet::RakString inputStatement;
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));
			bsIn.Read(queryId);
			bsIn.Read(dbIdentifier);
			bsIn.Read(inputStatement);
			unsigned int idx;
			for (idx=0; idx < resultHandlers.Size(); idx++)
				resultHandlers[idx]->OnUnknownDBIdentifier(inputStatement, queryId, dbIdentifier);
		}
	}

	return RR_CONTINUE_PROCESSING;
}
