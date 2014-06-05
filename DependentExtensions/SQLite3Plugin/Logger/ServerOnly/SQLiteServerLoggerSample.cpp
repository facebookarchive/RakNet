#include "RakPeerInterface.h"
#include "SQLiteServerLoggerPlugin.h"
#include "BitStream.h"
#include "RakSleep.h"

#include "Kbhit.h"
#include "GetTime.h"
#include "PacketizedTCP.h"


int main(void)
{
	printf("Demonstration of SQLiteServerLoggerPlugin.\n");

	RakNet::PacketizedTCP packetizedTCP;
	RakNet::SQLiteServerLoggerPlugin loggerPlugin;
// 	printf("Enable DXT compression (y/n)? ");
// 	loggerPlugin.SetEnableDXTCompression(getche()=='y');
	loggerPlugin.SetEnableDXTCompression(true);
	loggerPlugin.SetSessionManagementMode(RakNet::SQLiteServerLoggerPlugin::CREATE_SHARED_NAMED_DB_HANDLE, true, "");

	/*
//	printf("Enter path to DB file to create, or enter for memory.\n");
	char filePath[256];
	Gets(filePath,sizeof(filePath));
	filePath[0]=0;
	if (filePath[0]==0)
		strcpy(filePath, "C:\\EchoChamber\\logger.sqlite");
	sqlite3 *database;
	if (sqlite3_open_v2(filePath, &database, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0)!=SQLITE_OK)
		return 1;
	loggerPlugin.AddDBHandle("loggerDb", database);
	*/
	packetizedTCP.AttachPlugin(&loggerPlugin);
	packetizedTCP.Start(38123,8);
	printf("\nStarted.\n");
	bool quit=false;
	bool isProcessing=false;

	RakNet::SQLiteServerLoggerPlugin::ProcessingStatus processingStatusNew;
	RakNet::SQLiteServerLoggerPlugin::ProcessingStatus processingStatusOld;
	memset(&processingStatusOld,0,sizeof(processingStatusOld));

	RakNet::SystemAddress sa;
	while (quit==false || isProcessing==true)
	{
		RakNet::Packet *p;
		for (p = packetizedTCP.Receive(); p; packetizedTCP.DeallocatePacket(p), p = packetizedTCP.Receive())
		{
			;
		}
		sa = packetizedTCP.HasNewIncomingConnection();
		if (sa!=RakNet::UNASSIGNED_SYSTEM_ADDRESS)
			printf("New incoming connection from %s\n", sa.ToString(true));
		sa = packetizedTCP.HasLostConnection();
		if (sa!=RakNet::UNASSIGNED_SYSTEM_ADDRESS)
			printf("Lost connection from %s\n", sa.ToString(true));
		sa = packetizedTCP.HasFailedConnectionAttempt();
		sa = packetizedTCP.HasCompletedConnectionAttempt();
		RakSleep(0);

		if (kbhit())
		{
			if (getch()=='q')
			{
				printf("Quitting as soon as threads finish.\n");
				packetizedTCP.Stop();
				quit=true;
			}
		}

		loggerPlugin.GetProcessingStatus(&processingStatusNew);
		if (memcmp(&processingStatusNew,&processingStatusOld,sizeof(processingStatusOld))!=0)
		{
			printf("buffered=%i cpuWait=%i cpuDo=%i cpuDone=%i sqlWait=%i sqlDo=%i sqlDone=%i\n",
				processingStatusNew.packetsBuffered, 
				processingStatusNew.cpuPendingProcessing,processingStatusNew.cpuNumThreadsWorking,processingStatusNew.cpuProcessedAwaitingDeallocation,
				processingStatusNew.sqlPendingProcessing,processingStatusNew.sqlNumThreadsWorking,processingStatusNew.sqlProcessedAwaitingDeallocation
				);
			memcpy(&processingStatusOld,&processingStatusNew,sizeof(processingStatusOld));
		}

		if (processingStatusNew.cpuNumThreadsWorking==processingStatusNew.cpuPendingProcessing==processingStatusNew.cpuProcessedAwaitingDeallocation==
			processingStatusNew.packetsBuffered==processingStatusNew.sqlNumThreadsWorking==processingStatusNew.sqlPendingProcessing==processingStatusNew.sqlProcessedAwaitingDeallocation==0)
			isProcessing=false;
		else
			isProcessing=true;
	}

	loggerPlugin.CloseAllSessions();


	return 1;
}
