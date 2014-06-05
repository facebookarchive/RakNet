/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "RakNetworkFactory.h"
#include "GetTime.h"
#include "RakPeerInterface.h"
#include "PacketEnumerations.h"
#include "RakNetStatistics.h"
#include "DirectoryDeltaTransfer.h"
#include "FileListTransfer.h"
#include <cstdio>
#include <stdlib.h>
#include <conio.h>
#include "FileList.h"
#include "DataCompressor.h"
#include "FileListTransferCBInterface.h"

#ifdef _WIN32
#include <windows.h> // Sleep
#else
#include <unistd.h> // usleep
#endif

class TestCB : public FileListTransferCBInterface
{
public:
	void OnFile(
		unsigned fileIndex,
		char *filename,
		unsigned char *fileData,
		unsigned compressedTransmissionLength,
		unsigned finalDataLength,
		unsigned short setID,
		unsigned setCount,	
		unsigned setTotalCompressedTransmissionLength,
		unsigned setTotalFinalLength)
	{
        printf("%i. %i/%i %s %ib->%ib / %ib->%ib\n", setID, fileIndex, setCount, filename, compressedTransmissionLength, finalDataLength, setTotalCompressedTransmissionLength, setTotalFinalLength);
	}
} transferCallback;


int main(void)
{
	char ch;
	RakPeerInterface *rakPeer;

	// directoryDeltaTransfer is the main plugin that does the work for this sample.
	DirectoryDeltaTransfer directoryDeltaTransfer;
	// The fileListTransfer plugin is used by the DirectoryDeltaTransfer plugin and must also be registered (you could use this yourself too if you wanted, of course).
	FileListTransfer fileListTransfer;

	rakPeer = RakNetworkFactory::GetRakPeerInterface();
	rakPeer->AttachPlugin(&directoryDeltaTransfer);
	rakPeer->AttachPlugin(&fileListTransfer);
	directoryDeltaTransfer.SetFileListTransferPlugin(&fileListTransfer);

	printf("This sample demonstrates the plugin to incrementally transfer compressed\n");
	printf("deltas of directories.  In essence, it's a simple autopatcher.\n");
	printf("Unlike the full autopatcher, it has no dependencies.  It is suitable for\n");
	printf("patching from non-dedicated servers at runtime.\n");
	printf("Difficulty: Intermediate\n\n");

	printf("Enter listen port, or hit enter to choose automatically\n");
	unsigned short localPort;
	char str[256];
	gets(str);
	if (str[0]==0)
		localPort=60000;
	else
		localPort=atoi(str);
	if (rakPeer->Initialize(8,localPort,30,0)==false)
	{
		RakNetworkFactory::DestroyRakPeerInterface(rakPeer);
		printf("RakNet initialize failed.  Possibly duplicate port.\n");
		return 1;
	}
	rakPeer->SetMaximumIncomingConnections(8);

	printf("Commands:\n");
	printf("(S)et application directory.\n");
	printf("(A)dd allowed uploads from subdirectory.\n");
	printf("(D)ownload from subdirectory.\n");
	printf("(C)lear allowed uploads.\n");
	printf("C(o)nnect to another system.\n");
	printf("(Q)uit.\n");

	Packet *p;
	while (1)
	{
		// Process packets
		p=rakPeer->Receive();
		while (p)
		{
			if (p->data[0]==ID_NEW_INCOMING_CONNECTION)
				printf("ID_NEW_INCOMING_CONNECTION\n");
			else if (p->data[0]==ID_CONNECTION_REQUEST_ACCEPTED)
				printf("ID_CONNECTION_REQUEST_ACCEPTED\n");

			rakPeer->DeallocatePacket(p);
			p=rakPeer->Receive();
		}
		

		if (kbhit())
		{
			ch=getch();
			if (ch=='s')
			{
				printf("Enter application directory\n");
				gets(str);
				if (str[0]==0)
					strcpy(str, "C:/RakNet");
				directoryDeltaTransfer.SetApplicationDirectory(str);
				printf("This directory will be prefixed to upload and download subdirectories.\n");
			}
			else if (ch=='a')
			{
				printf("Enter uploads subdirectory\n");
				gets(str);
				directoryDeltaTransfer.AddUploadsFromSubdirectory(str);
				printf("%i files for upload.\n", directoryDeltaTransfer.GetNumberOfFilesForUpload());
			}
			else if (ch=='d')
			{
				char subdir[256];
				char outputSubdir[256];
				printf("Enter remote subdirectory to download from.\n");
				printf("This directory may be any uploaded directory, or a subdir therein.\n");
				gets(subdir);
				printf("Enter subdirectory to output to.\n");
				gets(outputSubdir);
                
				unsigned short setId;
				setId=directoryDeltaTransfer.DownloadFromSubdirectory(subdir, outputSubdir, true, rakPeer->GetPlayerIDFromIndex(0), &transferCallback, HIGH_PRIORITY, 0);
				if (setId==(unsigned short)-1)
					printf("Download failed.  Host unreachable.\n");
				else
					printf("Downloading set %i\n", setId);
			}
			else if (ch=='c')
			{
				directoryDeltaTransfer.ClearUploads();
				printf("Uploads cleared.\n");
			}
			else if (ch=='o')
			{
				char host[256];
				printf("Enter host IP: ");
				gets(host);
				if (host[0]==0)
					strcpy(host, "127.0.0.1");
				unsigned short remotePort;
				printf("Enter host port: ");
				gets(str);
				if (str[0]==0)
					remotePort=60000;
				else
					remotePort=atoi(str);
				rakPeer->Connect(host, remotePort, 0, 0);
				printf("Connecting.\n");
			}
			else if (ch=='q')
			{
				printf("Bye!\n");
				break;
			}
		}
	}

	RakNetworkFactory::DestroyRakPeerInterface(rakPeer);

	return 0;
}
