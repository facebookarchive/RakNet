/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

// serverIP should be ip address of patcher
// pathToGame should be something like "C:\Games\mygame", whatever the installation path was
// gameName should be patcherHostSubdomainURL found in AutopatcherServer_SelfScaling
// patchImmediately should be 1
// portToStartOn should be 0
// serverPort should be 60000, see LISTEN_PORT_TCP_PATCHER in AutopatcherServer_SelfScaling
// fullScan should be '1' to fully scan all files. 0 to use the last patch.

// Common includes
#include <stdio.h>
#include <stdlib.h>
#include "Kbhit.h"

#include "GetTime.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "StringCompressor.h"
#include "PacketizedTCP.h"
#include "RakNetSocket2.h"

// Client only includes
#include "FileListTransferCBInterface.h"
#include "FileListTransfer.h"
#include "AutopatcherClient.h"
#include "AutopatcherPatchContext.h"
#include "Gets.h"
#include "RakSleep.h"
#include "CloudClient.h"

void GetServers(RakNet::CloudClient *cloudClient, RakNet::RakNetGUID serverGuid);
void GetClientSubscription(RakNet::CloudClient *cloudClient, RakNet::RakNetGUID serverGuid);
void UploadInstanceToCloud(RakNet::CloudClient *cloudClient, RakNet::RakNetGUID serverGuid);

#define CLOUD_CLIENT_PRIMARY_KEY "SelfScaling_Patcher_PK"

class TestCB : public RakNet::AutopatcherClientCBInterface
{
public:
	virtual bool OnFile(OnFileStruct *onFileStruct)
	{
		if (onFileStruct->context.op==PC_HASH_1_WITH_PATCH || onFileStruct->context.op==PC_HASH_2_WITH_PATCH)
			printf("Patched: ");
		else if (onFileStruct->context.op==PC_WRITE_FILE)
			printf("Written: ");
		else if (onFileStruct->context.op==PC_ERROR_FILE_WRITE_FAILURE)
			printf("Write Failure: ");
		else if (onFileStruct->context.op==PC_ERROR_PATCH_TARGET_MISSING)
			printf("Patch target missing: ");
		else if (onFileStruct->context.op==PC_ERROR_PATCH_APPLICATION_FAILURE)
			printf("Patch process failure: ");
		else if (onFileStruct->context.op==PC_ERROR_PATCH_RESULT_CHECKSUM_FAILURE)
			printf("Patch checksum failure: ");
		else if (onFileStruct->context.op==PC_NOTICE_WILL_COPY_ON_RESTART)
			printf("Copy pending restart: ");
		else if (onFileStruct->context.op==PC_NOTICE_FILE_DOWNLOADED)
			printf("Downloaded: ");
		else if (onFileStruct->context.op==PC_NOTICE_FILE_DOWNLOADED_PATCH)
			printf("Downloaded Patch: ");
		else
			RakAssert(0);


		printf("%i. (100%%) %i/%i %s %ib / %ib\n", onFileStruct->setID, onFileStruct->fileIndex+1, onFileStruct->numberOfFilesInThisSet,
			onFileStruct->fileName, onFileStruct->byteLengthOfThisFile,
			onFileStruct->byteLengthOfThisSet);

		// Return false for the file data to be deallocated automatically
		return false;
	}

	virtual void OnFileProgress(FileProgressStruct *fps)
	{
		printf("Downloading: %i. (%i%%) %i/%i %s %ib/%ib %ib/%ib total\n", fps->onFileStruct->setID,
			(int) (100.0*(double)fps->onFileStruct->bytesDownloadedForThisFile/(double)fps->onFileStruct->byteLengthOfThisFile),
			fps->onFileStruct->fileIndex+1, fps->onFileStruct->numberOfFilesInThisSet, fps->onFileStruct->fileName,
			fps->onFileStruct->bytesDownloadedForThisFile,
			fps->onFileStruct->byteLengthOfThisFile,			
			fps->onFileStruct->bytesDownloadedForThisSet,
			fps->onFileStruct->byteLengthOfThisSet
			);
	}

	virtual PatchContext ApplyPatchBase(const char *oldFilePath, char **newFileContents, unsigned int *newFileSize, char *patchContents, unsigned int patchSize, uint32_t patchAlgorithm)
	{
		if (patchAlgorithm==0)
		{
			return ApplyPatchBSDiff(oldFilePath, newFileContents, newFileSize, patchContents, patchSize);
		}
		else
		{
			char WORKING_DIRECTORY[MAX_PATH];
			GetTempPath(MAX_PATH, WORKING_DIRECTORY);
			if (WORKING_DIRECTORY[strlen(WORKING_DIRECTORY)-1]=='\\' || WORKING_DIRECTORY[strlen(WORKING_DIRECTORY)-1]=='/')
				WORKING_DIRECTORY[strlen(WORKING_DIRECTORY)-1]=0;

			char buff[128];
			RakNet::TimeUS time = RakNet::GetTimeUS();
#if defined(_WIN32)
			sprintf(buff, "%I64u", time);
#else
			sprintf(buff, "%llu", (long long unsigned int) time);
#endif

			char pathToPatch1[MAX_PATH], pathToPatch2[MAX_PATH];
			sprintf(pathToPatch1, "%s/patchClient_%s.tmp", WORKING_DIRECTORY, buff);
			FILE *fpPatch = fopen(pathToPatch1, "wb");
			if (fpPatch==0)
				return PC_ERROR_PATCH_TARGET_MISSING;
			fwrite(patchContents, 1, patchSize, fpPatch);
			fclose(fpPatch);

			// Invoke xdelta
			// See https://code.google.com/p/xdelta/wiki/CommandLineSyntax
			char commandLine[512];
			_snprintf(commandLine, sizeof(commandLine)-1, "-d -f -s %s %s/patchClient_%s.tmp %s/newFile_%s.tmp", oldFilePath, WORKING_DIRECTORY, buff, WORKING_DIRECTORY, buff);
			commandLine[511]=0;
			
			SHELLEXECUTEINFO shellExecuteInfo;
			shellExecuteInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			shellExecuteInfo.fMask = SEE_MASK_NOASYNC | SEE_MASK_NO_CONSOLE;
			shellExecuteInfo.hwnd = NULL;
			shellExecuteInfo.lpVerb = "open";
			shellExecuteInfo.lpFile = "xdelta3-3.0.6-win32.exe";
			shellExecuteInfo.lpParameters = commandLine;
			shellExecuteInfo.lpDirectory = NULL;
			shellExecuteInfo.nShow = SW_SHOWNORMAL;
			shellExecuteInfo.hInstApp = NULL;
			ShellExecuteEx(&shellExecuteInfo);

			// // ShellExecute is blocking, but if it writes a file to disk that file is not always immediately accessible after it returns. And this only happens in release, and only when not running in the debugger
			// ShellExecute(NULL, "open", "xdelta3-3.0.6-win32.exe", commandLine, NULL, SW_SHOWNORMAL);

			sprintf(pathToPatch2, "%s/newFile_%s.tmp", WORKING_DIRECTORY, buff);
			fpPatch = fopen(pathToPatch2, "r+b");
			RakNet::TimeUS stopWaiting = time + 60000000;
			while (fpPatch==0 && RakNet::GetTimeUS() < stopWaiting)
			{
				RakSleep(1000);
				fpPatch = fopen(pathToPatch2, "r+b");
			}
			if (fpPatch==0)
			{
				printf("\nERROR: Could not open %s.\nerr=%i (%s)\narguments=%s\n", pathToPatch2, errno, strerror(errno), commandLine);
				return PC_ERROR_PATCH_TARGET_MISSING;
			}

			fseek(fpPatch, 0, SEEK_END);
			*newFileSize = ftell(fpPatch);
			fseek(fpPatch, 0, SEEK_SET);
			*newFileContents = (char*) rakMalloc_Ex(*newFileSize, _FILE_AND_LINE_);
			fread(*newFileContents, 1, *newFileSize, fpPatch);
			fclose(fpPatch);

			int unlinkRes1 = _unlink(pathToPatch1);
			int unlinkRes2 = _unlink(pathToPatch2);
			while ((unlinkRes1!=0 || unlinkRes2!=0) && RakNet::GetTimeUS() < stopWaiting)
			{
				RakSleep(1000);
				if (unlinkRes1!=0)
					unlinkRes1 = _unlink(pathToPatch1);
				if (unlinkRes2!=0)
					unlinkRes2 = _unlink(pathToPatch2);
			}

			if (unlinkRes1!=0)
				printf("\nWARNING: unlink %s failed.\nerr=%i (%s)\n", pathToPatch1, errno, strerror(errno));
			if (unlinkRes2!=0)
				printf("\nWARNING: unlink %s failed.\nerr=%i (%s)\n", pathToPatch2, errno, strerror(errno));

			return PC_WRITE_FILE;
		}
	}

} transferCallback;

int main(int argc, char **argv)
{
	if (argc<8)
	{
		printf("Arguments: serverIP, pathToGame, gameName, patchImmediately, localPort, serverPort, fullScan");
		return 0;
	}

	RakNet::SystemAddress TCPServerAddress=RakNet::UNASSIGNED_SYSTEM_ADDRESS;
	RakNet::AutopatcherClient autopatcherClient;
	RakNet::FileListTransfer fileListTransfer;
	RakNet::CloudClient cloudClient;
	autopatcherClient.SetFileListTransferPlugin(&fileListTransfer);
	bool didRebalance=false; // So we only reconnect to a lower load server once, for load balancing

	bool fullScan = argv[7][0]=='1';

	unsigned short localPort;
	localPort=atoi(argv[5]);

	unsigned short serverPort=atoi(argv[6]);

	RakNet::PacketizedTCP packetizedTCP;
	if (packetizedTCP.Start(localPort,1)==false)
	{
		printf("Failed to start TCP. Is the port already in use?");
		return 1;
	}
	packetizedTCP.AttachPlugin(&autopatcherClient);
	packetizedTCP.AttachPlugin(&fileListTransfer);

	RakNet::RakPeerInterface *rakPeer;
	rakPeer = RakNet::RakPeerInterface::GetInstance();
	RakNet::SocketDescriptor socketDescriptor(localPort,0);
	rakPeer->Startup(1,&socketDescriptor, 1);
	rakPeer->AttachPlugin(&cloudClient);
	DataStructures::List<RakNet::RakNetSocket2* > sockets;
	rakPeer->GetSockets(sockets);
	printf("Started on port %i\n", sockets[0]->GetBoundAddress().GetPort());


	char buff[512];
	strcpy(buff, argv[1]);

	rakPeer->Connect(buff, serverPort, 0, 0);

	printf("Connecting...\n");
	char appDir[512];
	strcpy(appDir, argv[2]);
	char appName[512];
	strcpy(appName, argv[3]);

	bool patchImmediately=argc>=5 && argv[4][0]=='1';

	RakNet::Packet *p;
	while (1)
	{
		RakNet::SystemAddress notificationAddress;
		notificationAddress=packetizedTCP.HasCompletedConnectionAttempt();
		if (notificationAddress!=RakNet::UNASSIGNED_SYSTEM_ADDRESS)
		{
			printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
			TCPServerAddress=notificationAddress;
		}
		notificationAddress=packetizedTCP.HasNewIncomingConnection();
		if (notificationAddress!=RakNet::UNASSIGNED_SYSTEM_ADDRESS)
			printf("ID_NEW_INCOMING_CONNECTION\n");
		notificationAddress=packetizedTCP.HasLostConnection();
		if (notificationAddress!=RakNet::UNASSIGNED_SYSTEM_ADDRESS)
			printf("ID_CONNECTION_LOST\n");
		notificationAddress=packetizedTCP.HasFailedConnectionAttempt();
		if (notificationAddress!=RakNet::UNASSIGNED_SYSTEM_ADDRESS)
		{
			printf("ID_CONNECTION_ATTEMPT_FAILED TCP\n");
			autopatcherClient.SetFileListTransferPlugin(0);
			autopatcherClient.Clear();
			packetizedTCP.Stop();
			rakPeer->Shutdown(500,0);
			RakNet::RakPeerInterface::DestroyInstance(rakPeer);
			return 0;
		}


		p=packetizedTCP.Receive();
		while (p)
		{
			if (p->data[0]==ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR)
			{
				char buff[256];
				RakNet::BitStream temp(p->data, p->length, false);
				temp.IgnoreBits(8);
				RakNet::StringCompressor::Instance()->DecodeString(buff, 256, &temp);
				printf("ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR\n");
				printf("%s\n", buff);
				autopatcherClient.SetFileListTransferPlugin(0);
				autopatcherClient.Clear();
				packetizedTCP.Stop();
				rakPeer->Shutdown(500,0);
				RakNet::RakPeerInterface::DestroyInstance(rakPeer);
				return 0;
			}
			else if (p->data[0]==ID_AUTOPATCHER_CANNOT_DOWNLOAD_ORIGINAL_UNMODIFIED_FILES)
			{
				printf("ID_AUTOPATCHER_CANNOT_DOWNLOAD_ORIGINAL_UNMODIFIED_FILES\n");
				autopatcherClient.SetFileListTransferPlugin(0);
				autopatcherClient.Clear();
				packetizedTCP.Stop();
				rakPeer->Shutdown(500,0);
				RakNet::RakPeerInterface::DestroyInstance(rakPeer);
				return 0;
			}			
			else if (p->data[0]==ID_AUTOPATCHER_FINISHED)
			{
				printf("ID_AUTOPATCHER_FINISHED with server time %f\n", autopatcherClient.GetServerDate());
				double srvDate=autopatcherClient.GetServerDate();
				FILE *fp = fopen("srvDate", "wb");
				fwrite(&srvDate,sizeof(double),1,fp);
				fclose(fp);
				autopatcherClient.SetFileListTransferPlugin(0);
				autopatcherClient.Clear();
				packetizedTCP.Stop();
				rakPeer->Shutdown(500,0);
				RakNet::RakPeerInterface::DestroyInstance(rakPeer);
				return 0;
			}
			else if (p->data[0]==ID_AUTOPATCHER_RESTART_APPLICATION)
			{
				printf("ID_AUTOPATCHER_RESTART_APPLICATION");
				autopatcherClient.SetFileListTransferPlugin(0);
				autopatcherClient.Clear();
				packetizedTCP.Stop();
				rakPeer->Shutdown(500,0);
				RakNet::RakPeerInterface::DestroyInstance(rakPeer);
				return 0;
			}
			// Launch \"AutopatcherClientRestarter.exe autopatcherRestart.txt\"\nQuit this application immediately after to unlock files.\n");

			packetizedTCP.DeallocatePacket(p);
			p=packetizedTCP.Receive();
		}

		p=rakPeer->Receive();
		while (p)
		{
			if (p->data[0]==ID_CONNECTION_REQUEST_ACCEPTED)
			{
				// UploadInstanceToCloud(&cloudClient, p->guid);
				// GetClientSubscription(&cloudClient, p->guid);
				GetServers(&cloudClient, p->guid);
				break;
			}
			else if (p->data[0]==ID_CONNECTION_ATTEMPT_FAILED)
			{
				printf("ID_CONNECTION_ATTEMPT_FAILED UDP\n");
				autopatcherClient.SetFileListTransferPlugin(0);
				autopatcherClient.Clear();
				packetizedTCP.Stop();
				rakPeer->Shutdown(500,0);
				RakNet::RakPeerInterface::DestroyInstance(rakPeer);
				return 0;
			}
			else if (p->data[0]==ID_CLOUD_GET_RESPONSE)
			{
				RakNet::CloudQueryResult cloudQueryResult;
				cloudClient.OnGetReponse(&cloudQueryResult, p);
				unsigned int rowIndex;
				const bool wasCallToGetServers=cloudQueryResult.cloudQuery.keys[0].primaryKey=="CloudConnCount";
				printf("\n");
				if (wasCallToGetServers)
					printf("Downloaded server list. %i servers.\n", cloudQueryResult.rowsReturned.Size());

				unsigned short connectionsOnOurServer=65535;
				unsigned short lowestConnectionsServer=65535;
				RakNet::SystemAddress lowestConnectionAddress;

				for (rowIndex=0; rowIndex < cloudQueryResult.rowsReturned.Size(); rowIndex++)
				{
					RakNet::CloudQueryRow *row = cloudQueryResult.rowsReturned[rowIndex];
					if (wasCallToGetServers)
					{
						unsigned short connCount;
						RakNet::BitStream bsIn(row->data, row->length, false);
						bsIn.Read(connCount);
						printf("%i. Server found at %s with %i connections\n", rowIndex+1, row->serverSystemAddress.ToString(true), connCount);

						unsigned short connectionsExcludingOurselves;
						if (row->serverGUID==p->guid)
							connectionsExcludingOurselves=connCount-1;
						else
							connectionsExcludingOurselves=connCount;

						// Find the lowest load server (optional)
						if (p->guid==row->serverGUID)
						{
							connectionsOnOurServer=connectionsExcludingOurselves;
						}
						else if (connectionsExcludingOurselves < lowestConnectionsServer)
						{
							lowestConnectionsServer=connectionsExcludingOurselves;
							lowestConnectionAddress=row->serverSystemAddress;
						}
					}
				}


				// Do load balancing by reconnecting to lowest load server (optional)
				if (didRebalance==false && wasCallToGetServers)
				{
					if (cloudQueryResult.rowsReturned.Size()>0 && connectionsOnOurServer>lowestConnectionsServer)
					{
						printf("Reconnecting to lower load server %s\n", lowestConnectionAddress.ToString(false));

						rakPeer->CloseConnection(p->guid, true);
						// Wait for the thread to close, otherwise will immediately get back ID_CONNECTION_ATTEMPT_FAILED because no free outgoing connection slots
						// Alternatively, just call Startup() with 2 slots instead of 1
						RakSleep(500);

						rakPeer->Connect(lowestConnectionAddress.ToString(false), lowestConnectionAddress.GetPort(), 0, 0);

						// TCP Connect to new IP address
						packetizedTCP.Connect(lowestConnectionAddress.ToString(false),serverPort,false);
					}
					else
					{
						// TCP Connect to original IP address
						packetizedTCP.Connect(buff,serverPort,false);
					}

					didRebalance=true;
				}

				cloudClient.DeallocateWithDefaultAllocator(&cloudQueryResult);
			}

			rakPeer->DeallocatePacket(p);
			p=rakPeer->Receive();
		}

		if (TCPServerAddress!=RakNet::UNASSIGNED_SYSTEM_ADDRESS && patchImmediately==true)
		{
			patchImmediately=false;
			char restartFile[512];
			strcpy(restartFile, appDir);
			strcat(restartFile, "/autopatcherRestart.txt");

			double lastUpdateDate;

			if (fullScan==false)
			{
				FILE *fp = fopen("srvDate", "rb");
				if (fp)
				{
					fread(&lastUpdateDate, sizeof(lastUpdateDate), 1, fp);
					fclose(fp);
				}
				else
					lastUpdateDate=0;
			}
			else
				lastUpdateDate=0;

			if (autopatcherClient.PatchApplication(appName, appDir, lastUpdateDate, TCPServerAddress, &transferCallback, restartFile, argv[0]))
			{
				printf("Patching process starting.\n");
			}
			else
			{
				printf("Failed to start patching.\n");
				autopatcherClient.SetFileListTransferPlugin(0);
				autopatcherClient.Clear();
				packetizedTCP.Stop();
				rakPeer->Shutdown(500,0);
				RakNet::RakPeerInterface::DestroyInstance(rakPeer);
				return 0;
			}
		}
		RakSleep(30);
	}

	// Dereference so the destructor doesn't crash
	autopatcherClient.SetFileListTransferPlugin(0);

	autopatcherClient.Clear();
	packetizedTCP.Stop();
	rakPeer->Shutdown(500,0);
	RakNet::RakPeerInterface::DestroyInstance(rakPeer);
	return 1;
}
void UploadInstanceToCloud(RakNet::CloudClient *cloudClient, RakNet::RakNetGUID serverGuid)
{
	RakNet::CloudKey cloudKey(CLOUD_CLIENT_PRIMARY_KEY,0);
	RakNet::BitStream bs;
	bs.Write("Hello World"); // This could be anything such as player list, game name, etc.
	cloudClient->Post(&cloudKey, bs.GetData(), bs.GetNumberOfBytesUsed(), serverGuid);
}
void GetClientSubscription(RakNet::CloudClient *cloudClient, RakNet::RakNetGUID serverGuid)
{
	RakNet::CloudQuery cloudQuery;
	cloudQuery.keys.Push(RakNet::CloudKey(CLOUD_CLIENT_PRIMARY_KEY,0),_FILE_AND_LINE_);
	cloudQuery.subscribeToResults=false;
	cloudClient->Get(&cloudQuery, serverGuid);
}
void GetServers(RakNet::CloudClient *cloudClient, RakNet::RakNetGUID serverGuid)
{
	RakNet::CloudQuery cloudQuery;
	cloudQuery.keys.Push(RakNet::CloudKey("CloudConnCount",0),_FILE_AND_LINE_); // CloudConnCount is defined at the top of CloudServerHelper.cpp
	cloudQuery.subscribeToResults=false;
	cloudClient->Get(&cloudQuery, serverGuid);
}
