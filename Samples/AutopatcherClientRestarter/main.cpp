/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include <stdio.h>
#if defined(_WIN32)
#include "WindowsIncludes.h" // Sleep and CreateProcess
#include <process.h> // system
#else
#include <unistd.h> // usleep
#include <cstdio>
#include <signal.h>  //kill
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#endif

#include "Gets.h"

#include <iostream>

using namespace std;

// This is a simple tool to take the output of PatchApplication::restartOutputFilename
// Perform file operations while that application is not running, and then relaunch that application
int main(int argc, char **argv)
{
	// Run commands on argv[1] and launch argv[2];
	// Run commands on argv[1] and launch argv[2];
	if (argc!=2)
	{
		printf("Usage: FileContainingCommands\n");
		return 1;
	}

	bool deleteFile=false;
	FILE *fp;
	fp = fopen(argv[1], "rt");
	if (fp==0)
	{
		printf("Error: Cannot open %s\n", argv[1]);
		return 1;
	}

	char buff[256];
	int offset=0;

	if (fgets(buff,255,fp)==0)
		return 1;
	buff[strlen(buff)]=0;
	while (buff[0])
	{
		if (strncmp(buff, "#Sleep ", 7)==0)
		{
			int sleepTime=atoi(buff+7);
#ifdef _WIN32
			Sleep(sleepTime);
#else
			usleep(sleepTime * 1000);
#endif
		}
		else if (strncmp(buff, "#DeleteThisFile", 15)==0)
			deleteFile=true;
		else if (strncmp(buff, "#CreateProcess ", 15)==0)
		{
#ifdef _WIN32
			PROCESS_INFORMATION pi;
			STARTUPINFO si;

			// Set up the start up info struct.
			memset(&si, 0,  sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);

			// Launch the child process.
			if (!CreateProcess(
				NULL,
				buff+15,
				NULL, NULL,
				TRUE,
				CREATE_NEW_CONSOLE,
				NULL, NULL,
				&si,
				&pi))
				return 1;

			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );
#else
            char PathName[255];

            strcpy(PathName, buff+15);

            system(PathName);  //This actually runs the application.
#endif
		}
		else
		{
            system(buff);
		}

		if (fgets(buff,255,fp)==0)
			break;
		buff[strlen(buff)]=0;
	}

	fclose(fp);

	// Done!
	if (deleteFile)
	{
		unlink(argv[1]);
	}
}