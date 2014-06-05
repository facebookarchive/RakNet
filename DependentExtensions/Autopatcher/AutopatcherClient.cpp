/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "AutopatcherClient.h"
#include "DirectoryDeltaTransfer.h"
#include "FileList.h"
#include "StringCompressor.h"
#include "RakPeerInterface.h"
#include "FileListTransfer.h"
#include "FileListTransferCBInterface.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "AutopatcherPatchContext.h"
#include "ApplyPatch.h"
#include "FileOperations.h"
//#include "DR_SHA1.h"
#include <stdio.h>
#include "FileOperations.h"
#include "RakAssert.h"
#include "ThreadPool.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

using namespace RakNet;

#include "SuperFastHash.h"
static const unsigned HASH_LENGTH=4;

#define COPY_ON_RESTART_EXTENSION ".patched.tmp"

// -----------------------------------------------------------------

PatchContext AutopatcherClientCBInterface::ApplyPatchBase(const char *oldFilePath, char **newFileContents, unsigned int *newFileSize, char *patchContents, unsigned int patchSize, uint32_t patchAlgorithm)
{
	return ApplyPatchBSDiff(oldFilePath, newFileContents, newFileSize, patchContents, patchSize);
}

PatchContext AutopatcherClientCBInterface::ApplyPatchBSDiff(const char *oldFilePath, char **newFileContents, unsigned int *newFileSize, char *patchContents, unsigned int patchSize)
{
	FILE *fp;
	fp=fopen(oldFilePath, "rb");
	if (fp==0)
		return PC_ERROR_PATCH_TARGET_MISSING;

	fseek(fp, 0, SEEK_END);
	unsigned int prePatchLength = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *prePatchFile = (char*) rakMalloc_Ex(prePatchLength, _FILE_AND_LINE_);
	fread(prePatchFile, prePatchLength, 1, fp);
	fclose(fp);

	bool result = ApplyPatch(prePatchFile, prePatchLength, newFileContents, newFileSize, patchContents, patchSize);
	rakFree_Ex(prePatchFile, _FILE_AND_LINE_);

	if (result==false)
		return PC_ERROR_PATCH_APPLICATION_FAILURE;
	
	return PC_WRITE_FILE;
}

// -----------------------------------------------------------------

struct AutopatcherClientThreadInfo
{
	FileListTransferCBInterface::OnFileStruct onFileStruct;
	char applicationDirectory[512];
	PatchContext result;
	// unsigned prePatchLength;
	// char *prePatchFile;

	// postPatchFile is passed in PC_NOTICE_WILL_COPY_ON_RESTART
	char *postPatchFile;
	unsigned postPatchLength;
	AutopatcherClientCBInterface *cbInterface;
};
// -----------------------------------------------------------------
AutopatcherClientThreadInfo* AutopatcherClientWorkerThread(AutopatcherClientThreadInfo* input, bool *returnOutput, void* perThreadData)
{
	char fullPathToDir[1024];
	*returnOutput=true;

	strcpy(fullPathToDir, input->applicationDirectory);
	strcat(fullPathToDir, input->onFileStruct.fileName);
	if (input->onFileStruct.context.op==PC_WRITE_FILE)
	{
		if (WriteFileWithDirectories(fullPathToDir, (char*)input->onFileStruct.fileData, input->onFileStruct.byteLengthOfThisFile)==false)
		{
			char newDir[1024];
			strcpy(newDir, fullPathToDir);
			strcat(newDir, COPY_ON_RESTART_EXTENSION);
			if (WriteFileWithDirectories(newDir, (char*)input->onFileStruct.fileData, input->onFileStruct.byteLengthOfThisFile))
			{
				input->result=PC_NOTICE_WILL_COPY_ON_RESTART;
			}
			else
			{
				input->result=PC_ERROR_FILE_WRITE_FAILURE;
			}
		}
		else
		{
			input->result=(PatchContext) input->onFileStruct.context.op;
		}
	}
	else
	{
		RakAssert(input->onFileStruct.context.op==PC_HASH_1_WITH_PATCH || input->onFileStruct.context.op==PC_HASH_2_WITH_PATCH);

//		CSHA1 sha1;
		

		//				printf("apply patch %i bytes\n", byteLengthOfThisFile-SHA1_LENGTH);
		//				for (int i=0; i < byteLengthOfThisFile-SHA1_LENGTH; i++)
		//					printf("%i ", fileData[SHA1_LENGTH+i]);
		//				printf("\n");
		int hashMultiplier;
		if (input->onFileStruct.context.op==PC_HASH_1_WITH_PATCH)
			hashMultiplier=1;
		else
			hashMultiplier=2; // else op==PC_HASH_2_WITH_PATCH

		PatchContext result = input->cbInterface->ApplyPatchBase(fullPathToDir, &input->postPatchFile, &input->postPatchLength, (char*)input->onFileStruct.fileData+HASH_LENGTH*hashMultiplier, input->onFileStruct.byteLengthOfThisFile-HASH_LENGTH*hashMultiplier, input->onFileStruct.context.flnc_extraData2);
		if (result == PC_ERROR_PATCH_APPLICATION_FAILURE || input->result==PC_ERROR_PATCH_TARGET_MISSING)
		{
			input->result=result;
			return input;
		}

		unsigned int hash = SuperFastHash(input->postPatchFile, input->postPatchLength);
		if (RakNet::BitStream::DoEndianSwap())
			RakNet::BitStream::ReverseBytesInPlace((unsigned char*) &hash, sizeof(hash));

		//if (memcmp(sha1.GetHash(), input->onFileStruct.fileData, HASH_LENGTH)!=0)

		if (memcmp(&hash, input->onFileStruct.fileData+HASH_LENGTH*(hashMultiplier-1), HASH_LENGTH)!=0)
		{
			input->result=PC_ERROR_PATCH_RESULT_CHECKSUM_FAILURE;
		}
		else
		{
			// Write postPatchFile over the existing file
			if (WriteFileWithDirectories(fullPathToDir, (char*)input->postPatchFile, input->postPatchLength)==false)
			{
				char newDir[1024];
				strcpy(newDir, fullPathToDir);
				strcat(newDir, COPY_ON_RESTART_EXTENSION);
				if (WriteFileWithDirectories(newDir, (char*)input->postPatchFile, input->postPatchLength))
				{
					input->result=PC_NOTICE_WILL_COPY_ON_RESTART;
				}
				else
				{
					input->result=PC_ERROR_FILE_WRITE_FAILURE;
				}
			}
			else
			{
				input->result=(PatchContext)input->onFileStruct.context.op;
			}
		}
	}

	return input;
}
// -----------------------------------------------------------------
namespace RakNet
{
class AutopatcherClientCallback : public FileListTransferCBInterface
{
public:
	ThreadPool<AutopatcherClientThreadInfo*,AutopatcherClientThreadInfo*> threadPool;
	char applicationDirectory[512];
	AutopatcherClientCBInterface *onFileCallback;
	AutopatcherClient *client;
	bool downloadComplete;
	bool canDeleteUser;

	AutopatcherClientCallback(void)
	{
		threadPool.StartThreads(1,0);
		canDeleteUser=false;
		downloadComplete=false;
	}
	virtual ~AutopatcherClientCallback(void)
	{
		StopThreads();
	}
	void StopThreads(void)
	{
		threadPool.StopThreads();
		RakAssert(threadPool.NumThreadsWorking()==0);

		unsigned i;
		AutopatcherClientThreadInfo* info;
		for (i=0; i < threadPool.InputSize(); i++)
		{
			info = threadPool.GetInputAtIndex(i);
//			if (info->prePatchFile)
//				rakFree_Ex(info->prePatchFile, _FILE_AND_LINE_ );
			if (info->postPatchFile)
				rakFree_Ex(info->postPatchFile, _FILE_AND_LINE_ );
			if (info->onFileStruct.fileData)
				rakFree_Ex(info->onFileStruct.fileData, _FILE_AND_LINE_ );
			RakNet::OP_DELETE(info, _FILE_AND_LINE_);
		}
		threadPool.ClearInput();
		for (i=0; i < threadPool.OutputSize(); i++)
		{
			info = threadPool.GetOutputAtIndex(i);
//			if (info->prePatchFile)
//				rakFree_Ex(info->prePatchFile, _FILE_AND_LINE_ );
			if (info->postPatchFile)
				rakFree_Ex(info->postPatchFile, _FILE_AND_LINE_ );
			if (info->onFileStruct.fileData)
				rakFree_Ex(info->onFileStruct.fileData, _FILE_AND_LINE_ );
			RakNet::OP_DELETE(info, _FILE_AND_LINE_);
		}
		threadPool.ClearOutput();
	}
	// Update is run in the user thread
	virtual bool Update(void)
	{
		if (threadPool.HasOutputFast() && threadPool.HasOutput())
		{
			AutopatcherClientThreadInfo *threadInfo = threadPool.GetOutput();
			threadInfo->onFileStruct.context.op=threadInfo->result;
			switch (threadInfo->result)
			{
				case PC_NOTICE_WILL_COPY_ON_RESTART:
				{
					client->CopyAndRestart(threadInfo->onFileStruct.fileName);
					if (threadInfo->onFileStruct.context.op==PC_WRITE_FILE)
					{
						// Regular file in use but we can write the temporary file.  Restart and copy it over the existing
						onFileCallback->OnFile(&threadInfo->onFileStruct);
					}
					else
					{
						// Regular file in use but we can write the temporary file.  Restart and copy it over the existing
						rakFree_Ex(threadInfo->onFileStruct.fileData, _FILE_AND_LINE_ );
						threadInfo->onFileStruct.fileData=threadInfo->postPatchFile;
						onFileCallback->OnFile(&threadInfo->onFileStruct);
						threadInfo->onFileStruct.fileData=0;
					}
				}
				break;
				case PC_ERROR_FILE_WRITE_FAILURE:
				{
					if (threadInfo->onFileStruct.context.op==PC_WRITE_FILE)
					{
						onFileCallback->OnFile(&threadInfo->onFileStruct);
					}
					else
					{
						rakFree_Ex(threadInfo->onFileStruct.fileData, _FILE_AND_LINE_ );
						threadInfo->onFileStruct.fileData=threadInfo->postPatchFile;
						threadInfo->onFileStruct.byteLengthOfThisFile=threadInfo->postPatchLength;
						onFileCallback->OnFile(&threadInfo->onFileStruct);
						threadInfo->onFileStruct.fileData=0;
					}					
				}
				break;
				case PC_ERROR_PATCH_TARGET_MISSING:
				{
					onFileCallback->OnFile(&threadInfo->onFileStruct);
					client->Redownload(threadInfo->onFileStruct.fileName);
				}
				break;
				case PC_ERROR_PATCH_APPLICATION_FAILURE:
				{
					// Failure - signal class and download this file.
					onFileCallback->OnFile(&threadInfo->onFileStruct);
					client->Redownload(threadInfo->onFileStruct.fileName);
				}
				break;
				case PC_ERROR_PATCH_RESULT_CHECKSUM_FAILURE:
				{
					// Failure - signal class and download this file.
					onFileCallback->OnFile(&threadInfo->onFileStruct);
					client->Redownload(threadInfo->onFileStruct.fileName);
				}
				break;
				default:
				{
					if (threadInfo->onFileStruct.context.op==PC_WRITE_FILE)
					{
						onFileCallback->OnFile(&threadInfo->onFileStruct);
					}
					else
					{
						rakFree_Ex(threadInfo->onFileStruct.fileData, _FILE_AND_LINE_ );
						threadInfo->onFileStruct.fileData=threadInfo->postPatchFile;
						onFileCallback->OnFile(&threadInfo->onFileStruct);
						threadInfo->onFileStruct.fileData=0;
					}
				}
				break;
			}

//			if (threadInfo->prePatchFile)
//				rakFree_Ex(threadInfo->prePatchFile, _FILE_AND_LINE_ );
			if (threadInfo->postPatchFile)
				rakFree_Ex(threadInfo->postPatchFile, _FILE_AND_LINE_ );
			if (threadInfo->onFileStruct.fileData)
				rakFree_Ex(threadInfo->onFileStruct.fileData, _FILE_AND_LINE_ );
			RakNet::OP_DELETE(threadInfo, _FILE_AND_LINE_);
		}

		// If both input and output are empty, we are done.
		if (onFileCallback->Update()==false)
			canDeleteUser=true;

		if ( downloadComplete &&
			canDeleteUser &&
			threadPool.IsWorking()==false)
		{
			// Stop threads before calling OnThreadCompletion, in case the other thread starts a new instance of this thread.
			StopThreads();
			client->OnThreadCompletion();
			return false;
		}

		return true;
	}
	virtual bool OnDownloadComplete(DownloadCompleteStruct *dcs)
	{
		downloadComplete=true;
		if (onFileCallback->OnDownloadComplete(dcs)==false)
		{
			canDeleteUser=true;
		}
		return true;
	};
	virtual void OnDereference(void)
	{
		onFileCallback->OnDereference();
		StopThreads();
	}
	virtual bool OnFile(OnFileStruct *onFileStruct)
	{
		AutopatcherClientThreadInfo *inStruct = RakNet::OP_NEW<AutopatcherClientThreadInfo>( _FILE_AND_LINE_ );
		memset(inStruct,0,sizeof(AutopatcherClientThreadInfo));
//		inStruct->prePatchFile=0;
		inStruct->postPatchFile=0;
		inStruct->cbInterface=onFileCallback;
		memcpy(&(inStruct->onFileStruct), onFileStruct, sizeof(OnFileStruct));
		strcpy(inStruct->applicationDirectory,applicationDirectory);
		if (onFileStruct->context.op==PC_HASH_1_WITH_PATCH || onFileStruct->context.op==PC_HASH_2_WITH_PATCH)
			onFileStruct->context.op=PC_NOTICE_FILE_DOWNLOADED_PATCH;
		else
			onFileStruct->context.op=PC_NOTICE_FILE_DOWNLOADED;
		onFileCallback->OnFile(onFileStruct);
		threadPool.AddInput(AutopatcherClientWorkerThread, inStruct);

		// Return false means don't delete OnFileStruct::data
		return false;
	}
	virtual void OnFileProgress(FileProgressStruct *fps)
	{
		char fullPathToDir[1024];

		if (fps->onFileStruct->fileName)
		{
			strcpy(fullPathToDir, applicationDirectory);
			strcat(fullPathToDir, fps->onFileStruct->fileName);
			onFileCallback->OnFileProgress(fps);
		}
	}
};
}
AutopatcherClient::AutopatcherClient()
{
	serverId=UNASSIGNED_SYSTEM_ADDRESS;
	serverIdIndex=-1;
	applicationDirectory[0]=0;
	fileListTransfer=0;
    priority=HIGH_PRIORITY;
	orderingChannel=0;
	serverDate=0;
	userCB=0;
	processThreadCompletion=false;
}
AutopatcherClient::~AutopatcherClient()
{
	Clear();
}
void AutopatcherClient::Clear(void)
{
	if (fileListTransfer)
		fileListTransfer->RemoveReceiver(serverId);
	serverId=UNASSIGNED_SYSTEM_ADDRESS;
	setId=(unsigned short)-1;
	redownloadList.Clear();
	copyAndRestartList.Clear();
}
void AutopatcherClient::SetUploadSendParameters(PacketPriority _priority, char _orderingChannel)
{
	priority=_priority;
	orderingChannel=_orderingChannel;
}
void AutopatcherClient::SetFileListTransferPlugin(FileListTransfer *flt)
{
	fileListTransfer=flt;
}
double AutopatcherClient::GetServerDate(void) const
{
	return serverDate;
}
void AutopatcherClient::CancelDownload(void)
{
	fileListTransfer->CancelReceive(setId);
	Clear();
}
void AutopatcherClient::OnThreadCompletion(void)
{
	processThreadCompletionMutex.Lock();
	processThreadCompletion=true;
	processThreadCompletionMutex.Unlock();
}
bool AutopatcherClient::IsPatching(void) const
{
	return fileListTransfer->IsHandlerActive(setId);
}
bool AutopatcherClient::PatchApplication(const char *_applicationName, const char *_applicationDirectory, double lastUpdateDate, SystemAddress host, AutopatcherClientCBInterface *onFileCallback, const char *restartOutputFilename, const char *pathToRestartExe)
{
    RakAssert(applicationName);
	RakAssert(applicationDirectory);
	RakAssert(pathToRestartExe);
	RakAssert(restartOutputFilename);

//	if (rakPeerInterface->GetIndexFromSystemAddress(host)==-1)
//		return false;
	if (IsPatching())
		return false; // Already in the middle of patching.

	strcpy(applicationDirectory, _applicationDirectory);
	FileList::FixEndingSlash(applicationDirectory);
	strcpy(applicationName, _applicationName);
	serverId=host;
	patchComplete=false;
	userCB=onFileCallback;
	strcpy(copyOnRestartOut, restartOutputFilename);
	strcpy(restartExe, pathToRestartExe);
	processThreadCompletionMutex.Lock();
	processThreadCompletion=false;
	processThreadCompletionMutex.Unlock();

	RakNet::BitStream outBitStream;
	outBitStream.Write((unsigned char)ID_AUTOPATCHER_GET_CHANGELIST_SINCE_DATE);
	StringCompressor::Instance()->EncodeString(applicationName, 512, &outBitStream);
	outBitStream.Write(lastUpdateDate);
    SendUnified(&outBitStream, priority, RELIABLE_ORDERED, orderingChannel, host, false);
	return true;
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void AutopatcherClient::Update(void)
{
	processThreadCompletionMutex.Lock();
	if (processThreadCompletion)
	{
		processThreadCompletion=false;
		processThreadCompletionMutex.Unlock();

		fileListTransfer->RemoveReceiver(serverId);

		// If redownload list, process it
		if (redownloadList.fileList.Size())
		{
			RakNet::BitStream outBitStream;
			AutopatcherClientCallback *transferCallback;
			transferCallback = RakNet::OP_NEW<AutopatcherClientCallback>( _FILE_AND_LINE_ );
			strcpy(transferCallback->applicationDirectory, applicationDirectory);
			transferCallback->onFileCallback=userCB;
			transferCallback->client=this;
			setId = fileListTransfer->SetupReceive(transferCallback, true, serverId);

			// Ask for patches for the files in the list that are different from what we have.
			outBitStream.Write((unsigned char)ID_AUTOPATCHER_GET_PATCH);
			outBitStream.Write(setId);
			double lastUpdateData=0;
			outBitStream.Write(lastUpdateData);
			StringCompressor::Instance()->EncodeString(applicationName, 512, &outBitStream);
			redownloadList.Serialize(&outBitStream);
			SendUnified(&outBitStream, priority, RELIABLE_ORDERED, orderingChannel, serverId, false);
			redownloadList.Clear();
		}
		else if (copyAndRestartList.fileList.Size())
		{
			Packet *p = AllocatePacketUnified(1);
			p->bitSize=p->length*8;
			p->data[0]=ID_AUTOPATCHER_RESTART_APPLICATION;
			p->systemAddress=serverId;
			p->systemAddress.systemIndex=serverIdIndex;
			PushBackPacketUnified(p,false);

			FILE *fp;
			fp = fopen(copyOnRestartOut, "wt");
			RakAssert(fp);
			if (fp)
			{
				fprintf(fp, "#Sleep 1000\n");
				unsigned i;
				for (i=0; i < copyAndRestartList.fileList.Size(); i++)
				{
#ifdef _WIN32
					fprintf(fp, "del /q \"%s%s\"\n", applicationDirectory, copyAndRestartList.fileList[i].filename.C_String());
					RakString sourceFn = copyAndRestartList.fileList[i].filename;
					RakString bareFilename = sourceFn;
					bareFilename.StartAfterLastCharacter('/');
					fprintf(fp, "rename \"%s%s%s\" \"%s\"\n", applicationDirectory, bareFilename.C_String(), COPY_ON_RESTART_EXTENSION, copyAndRestartList.fileList[i].filename.C_String());
#else
					fprintf(fp, "rm -f \"%s%s\"\n", applicationDirectory, copyAndRestartList.fileList[i].filename.C_String());
					fprintf(fp, "mv \"%s%s%s\" \"%s\"\n", applicationDirectory, copyAndRestartList.fileList[i].filename.C_String(), COPY_ON_RESTART_EXTENSION, copyAndRestartList.fileList[i].filename.C_String());
#endif
				}
#ifdef _WIN32
				fprintf(fp, "#CreateProcess \"%s\"\n", restartExe);
#else
				fprintf(fp, "chmod +x \"%s\"\n", restartExe);
				fprintf(fp, "#CreateProcess \"%s\" &\n", restartExe);
#endif
				fprintf(fp, "#DeleteThisFile\n");
				fclose(fp);
			}
		}
		else
		{
			Packet *p = AllocatePacketUnified(1);
			p->bitSize=p->length*8;
			p->data[0]=ID_AUTOPATCHER_FINISHED;
			p->systemAddress=serverId;
			p->systemAddress.systemIndex=serverIdIndex;
			PushBackPacketUnified(p,false);
		}
	}
	else
	{
		processThreadCompletionMutex.Unlock();
	}
}
void AutopatcherClient::OnClosedConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
	if (systemAddress==serverId)
		Clear();
}
PluginReceiveResult AutopatcherClient::OnReceive(Packet *packet)
{
	switch (packet->data[0]) 
	{
	case ID_AUTOPATCHER_CREATION_LIST:
		return OnCreationList(packet);
	case ID_AUTOPATCHER_DELETION_LIST:
		OnDeletionList(packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR:
	case ID_AUTOPATCHER_CANNOT_DOWNLOAD_ORIGINAL_UNMODIFIED_FILES:
		fileListTransfer->RemoveReceiver(serverId);
		Clear();
		return RR_CONTINUE_PROCESSING;
	case ID_AUTOPATCHER_FINISHED_INTERNAL:
		return OnDownloadFinishedInternal(packet);
	case ID_AUTOPATCHER_FINISHED:
		return OnDownloadFinished(packet);
	}
	return RR_CONTINUE_PROCESSING;
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void AutopatcherClient::OnShutdown(void)
{
	// TODO
}

PluginReceiveResult AutopatcherClient::OnCreationList(Packet *packet)
{
	RakAssert(fileListTransfer);
	if (packet->systemAddress!=serverId)
		return RR_STOP_PROCESSING_AND_DEALLOCATE;

	RakNet::BitStream inBitStream(packet->data, packet->length, false);
	RakNet::BitStream outBitStream;
	FileList remoteFileList, missingOrChanged;
	inBitStream.IgnoreBits(8);
	if (remoteFileList.Deserialize(&inBitStream)==false)
		return RR_STOP_PROCESSING_AND_DEALLOCATE;

	inBitStream.Read(serverDate);
	double patchApplicationLastUpdateDate;
	inBitStream.Read(patchApplicationLastUpdateDate);

	// Go through the list of hashes.  For each file we already have, remove it from the list.
	remoteFileList.ListMissingOrChangedFiles(applicationDirectory, &missingOrChanged, true, false);

	if (missingOrChanged.fileList.Size()==0)
	{
		packet->data[0]=ID_AUTOPATCHER_FINISHED;
		return RR_CONTINUE_PROCESSING; // Pass to user
	}

	// Prepare the transfer plugin to get a file list.
	AutopatcherClientCallback *transferCallback;
	transferCallback = RakNet::OP_NEW<AutopatcherClientCallback>( _FILE_AND_LINE_ );
	strcpy(transferCallback->applicationDirectory, applicationDirectory);
	transferCallback->onFileCallback=userCB;
	transferCallback->client=this;
	setId = fileListTransfer->SetupReceive(transferCallback, true, packet->systemAddress);

	// Ask for patches for the files in the list that are different from what we have.
	outBitStream.Write((unsigned char)ID_AUTOPATCHER_GET_PATCH);
	outBitStream.Write(setId);
	outBitStream.Write(patchApplicationLastUpdateDate);
	StringCompressor::Instance()->EncodeString(applicationName, 512, &outBitStream);
	missingOrChanged.Serialize(&outBitStream);
	SendUnified(&outBitStream, priority, RELIABLE_ORDERED, orderingChannel, packet->systemAddress, false);

	return RR_STOP_PROCESSING_AND_DEALLOCATE; // Absorb this message
}
void AutopatcherClient::OnDeletionList(Packet *packet)
{
	if (packet->systemAddress!=serverId)
		return;

	RakNet::BitStream inBitStream(packet->data, packet->length, false);
	RakNet::BitStream outBitStream;
	inBitStream.IgnoreBits(8);
	FileList fileList;
	if (fileList.Deserialize(&inBitStream)==false)
		return;
	fileList.DeleteFiles(applicationDirectory);
}
PluginReceiveResult AutopatcherClient::OnDownloadFinished(Packet *packet)
{
	RakNet::BitStream inBitStream(packet->data, packet->length, false);
	inBitStream.IgnoreBits(8);
	// This may have been created internally, with no serverDate written (line 469 or so)
	if (inBitStream.GetNumberOfUnreadBits()>7)
	{
		inBitStream.Read(serverDate);
	}
	serverId=packet->systemAddress;
	serverIdIndex=packet->systemAddress.systemIndex;

	return RR_CONTINUE_PROCESSING;
}
PluginReceiveResult AutopatcherClient::OnDownloadFinishedInternal(Packet *packet)
{
	RakNet::BitStream inBitStream(packet->data, packet->length, false);
	inBitStream.IgnoreBits(8);
	serverId=packet->systemAddress;
	serverIdIndex=packet->systemAddress.systemIndex;
	inBitStream.Read(serverDate);

	return RR_STOP_PROCESSING_AND_DEALLOCATE;
}
void AutopatcherClient::CopyAndRestart(const char *filePath)
{
	// We weren't able to write applicationDirectory + filePath so we wrote applicationDirectory + filePath + COPY_ON_RESTART_EXTENSION instead
	copyAndRestartList.AddFile(filePath,filePath, 0, 0, 0, FileListNodeContext(0,0,0,0));
}
void AutopatcherClient::Redownload(const char *filePath)
{
	redownloadList.AddFile(filePath,filePath, 0, 0, 0, FileListNodeContext(0,0,0,0));
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
