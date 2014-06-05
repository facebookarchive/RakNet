/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

// To compile link with Dbghelp.lib
// The callstack in release is the same as usual, which means it isn't all that accurate.
#ifdef WIN32

#include <stdio.h>
#include "WindowsIncludes.h"
#include <DbgHelp.h>
#include <stdlib.h>
#include <time.h>
#include "SendFileTo.h"
#include "CrashReporter.h"
#include "EmailSender.h"
#include "FileList.h"
#include "FileOperations.h"
#include "SimpleMutex.h"

using namespace RakNet;

CrashReportControls CrashReporter::controls;

// More info at:
// http://www.codeproject.com/debug/postmortemdebug_standalone1.asp
// http://www.codeproject.com/debug/XCrashReportPt3.asp
// http://www.codeproject.com/debug/XCrashReportPt1.asp
// http://www.microsoft.com/msj/0898/bugslayer0898.aspx

LONG ProcessException(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	char appDescriptor[_MAX_PATH];
	if ((CrashReporter::controls.actionToTake & AOC_SILENT_MODE) == 0)
	{
		sprintf(appDescriptor, "%s has crashed.\nGenerate a report?",  CrashReporter::controls.appName);
		if (::MessageBox( NULL, appDescriptor, "Crash Reporter", MB_YESNO )==IDNO)
		{
			return EXCEPTION_CONTINUE_SEARCH;
		}
	}

	char dumpFilepath[_MAX_PATH];
	char dumpFilename[_MAX_PATH];
	sprintf(appDescriptor, "%s %s - %s %s", CrashReporter::controls.appName, CrashReporter::controls.appVersion, __DATE__, __TIME__);

	if ((CrashReporter::controls.actionToTake & AOC_EMAIL_WITH_ATTACHMENT) ||
		(CrashReporter::controls.actionToTake & AOC_WRITE_TO_DISK)
		)
	{
		if (CrashReporter::controls.actionToTake & AOC_WRITE_TO_DISK)
		{
			strcpy(dumpFilepath, CrashReporter::controls.pathToMinidump);
			WriteFileWithDirectories(dumpFilepath,0,0);
			AddSlash(dumpFilepath);
		}
		else
		{
			// Write to a temporary directory if the user doesn't want the dump on the harddrive.
			if (!GetTempPath( _MAX_PATH, dumpFilepath ))
				dumpFilepath[0]=0;
		}
		unsigned i, dumpFilenameLen;
		strcpy(dumpFilename, appDescriptor);
		dumpFilenameLen=(unsigned) strlen(appDescriptor);
		for (i=0; i < dumpFilenameLen; i++)
			if (dumpFilename[i]==':' || dumpFilename[i]=='/' || dumpFilename[i]=='\\')
				dumpFilename[i]='.'; // Remove illegal characters from filename
		strcat(dumpFilepath, dumpFilename);
		strcat(dumpFilepath, ".dmp");

		HANDLE hFile = CreateFile(dumpFilepath,GENERIC_WRITE, FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if (hFile==INVALID_HANDLE_VALUE)
			return EXCEPTION_CONTINUE_SEARCH;

		MINIDUMP_EXCEPTION_INFORMATION eInfo;
		eInfo.ThreadId = GetCurrentThreadId();
		eInfo.ExceptionPointers = ExceptionInfo;
		eInfo.ClientPointers = FALSE;

		if (MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			(MINIDUMP_TYPE)CrashReporter::controls.minidumpType,
			ExceptionInfo ? &eInfo : NULL,
			NULL,
			NULL)==false)
			return EXCEPTION_CONTINUE_SEARCH;

		CloseHandle(hFile);
	}

	char silentModeEmailBody[1024];
	char subject[1204];
	if (CrashReporter::controls.actionToTake & AOC_EMAIL_NO_ATTACHMENT)
	{
		strcpy(subject, CrashReporter::controls.emailSubjectPrefix);
		strcat(subject, appDescriptor);

		if (CrashReporter::controls.actionToTake & AOC_SILENT_MODE)
		{
		sprintf(silentModeEmailBody, "%s%s version %s has crashed.\r\nIt was compiled on %s %s.\r\n", CrashReporter::controls.emailBody, CrashReporter::controls.appName,CrashReporter::controls.appVersion, __DATE__, __TIME__);

			if (CrashReporter::controls.actionToTake & AOC_WRITE_TO_DISK)
				sprintf(silentModeEmailBody+strlen(silentModeEmailBody), "Minidump written to %s \r\n", dumpFilepath);

			// Silently send email with attachment
			EmailSender emailSender;
			emailSender.Send(CrashReporter::controls.SMTPServer,
				25,
				CrashReporter::controls.SMTPAccountName,
				CrashReporter::controls.emailRecipient,
				CrashReporter::controls.emailSender,
				CrashReporter::controls.emailRecipient,
				subject,
				silentModeEmailBody,
				0,
				false,
				CrashReporter::controls.emailPassword);
		}
		else
		{
			CSendFileTo sendFile;
			sendFile.SendMail(0, 0, 0, subject, CrashReporter::controls.emailBody, CrashReporter::controls.emailRecipient);
		}
	}
	else if (CrashReporter::controls.actionToTake & AOC_EMAIL_WITH_ATTACHMENT)
	{
		strcpy(subject, CrashReporter::controls.emailSubjectPrefix);
		strcat(subject, dumpFilename);
		strcat(dumpFilename, ".dmp");

		if (CrashReporter::controls.actionToTake & AOC_SILENT_MODE)
		{
			sprintf(silentModeEmailBody, "%s%s version %s has crashed.\r\nIt was compiled on %s %s.\r\n", CrashReporter::controls.emailBody, CrashReporter::controls.appName,CrashReporter::controls.appVersion, __DATE__, __TIME__);

			if (CrashReporter::controls.actionToTake & AOC_WRITE_TO_DISK)
				sprintf(silentModeEmailBody+strlen(silentModeEmailBody), "Minidump written to %s \r\n", dumpFilepath);

			// Silently send email with attachment
			EmailSender emailSender;
			FileList files;
			files.AddFile(dumpFilepath,dumpFilename,FileListNodeContext(0,0,0,0));
			emailSender.Send(CrashReporter::controls.SMTPServer,
				25,
				CrashReporter::controls.SMTPAccountName,
				CrashReporter::controls.emailRecipient,
				CrashReporter::controls.emailSender,
				CrashReporter::controls.emailRecipient,
				subject,
				silentModeEmailBody,
				&files,
				false,
				CrashReporter::controls.emailPassword);
		}
		else
		{
			CSendFileTo sendFile;
			sendFile.SendMail(0, dumpFilepath, dumpFilename, subject, CrashReporter::controls.emailBody, CrashReporter::controls.emailRecipient);
		}
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI CrashExceptionFilter( struct _EXCEPTION_POINTERS *ExceptionInfo )
{
	// Mutex here due to http://www.jenkinssoftware.com/raknet/forum/index.php?topic=2305.0;topicseen
	static SimpleMutex crashExceptionFilterMutex;
	crashExceptionFilterMutex.Lock();
	LONG retVal = ProcessException(ExceptionInfo); 
	crashExceptionFilterMutex.Unlock();
	return retVal;
}

void DumpMiniDump(PEXCEPTION_POINTERS excpInfo)
{
	if (excpInfo == NULL) 
	{
		// Generate exception to get proper context in dump
		__try 
		{
			RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);
		} 
		__except(DumpMiniDump(GetExceptionInformation()),EXCEPTION_EXECUTE_HANDLER) 
		{
		}
	} 
	else
	{
		ProcessException(excpInfo); 
	}
}

// #define _DEBUG_CRASH_REPORTER

void CrashReporter::Start(CrashReportControls *input)
{
	memcpy(&controls, input, sizeof(CrashReportControls));

#ifndef _DEBUG_CRASH_REPORTER
	SetUnhandledExceptionFilter(CrashExceptionFilter);
#endif
}
#endif //WIN32
