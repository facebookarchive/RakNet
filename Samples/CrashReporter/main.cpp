/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

// Windows only sample to catch unhandled exceptions and email a minidump.
// The minidump can be opened in visual studio and will show you where the crash occurred and give you the local variable values.
//
// How to use the minidump:
//
// Put the minidump on your harddrive and double click it.  It will open Visual Studio.  It will look for the exe that caused the crash in the directory
// that the program that crashed was running at.  If it can't find this exe, or if it is different, it will look in the current
// directory for that exe.  If it still can't find it, or if it is different, it will load Visual Studio and indicate that it can't find
// the executable module.  No source code will be shown at that point.  However, you can specify modpath=<pathToExeDirectory> in the
// project properties window for "Command Arguments".
// The best solution is copy the .dmp to a directory containing a copy of the exe that crashed.
//
// On load, Visual Studio will look for the .pdb, which it uses to find the source code files and other information.  This is fine as long as the source
// code files on your harddrive match those that were used to create the exe.  If they don't, you will see source code but it will be the wrong code.
// There are three ways to deal with this.
//
// The first way is to change the path to your source code so it won't find the wrong code automatically.
// This will cause the debugger to not find the source code pointed to in the .pdb .  You will be prompted for the location of the correct source code.
//
// The second way is to build the exe on a different path than what you normally program with. For example, when you program you use c:/Working/Mygame
// When you release builds, you do at c:/Version2.2/Mygame .  After a build, you keep the source files, the exe, and the pdb
// on a harddrive at that location.  When you get a crash .dmp, copy it to the same directory as the exe, ( c:/Version2.2/Mygame/bin )
// This way the .pdb will point to the correct sources to begin wtih.
//
// The third way is save build labels or branches in source control and get that version (you only need source code + .exe + .pdb) before debugging.
// After debugging, restore your previous work.

#include "SocketLayer.h"
#include "CrashReporter.h" // This is the only required file for the crash reporter.  You must link in Dbghelp.lib
#include <stdio.h> // Printf, for the sample code
#include "Kbhit.h" // getch, for the sample code
#include <DbgHelp.h>
#include "Gets.h"

void function1(int a)
{
	int *crashPtr=0;
	// Keep crashPtr from getting compiled out
	printf("Now crashing!!!! %p\n", crashPtr);
	// If it crashes here in your debugger that is because you didn't define _DEBUG_CRASH_REPORTER to catch it.
	// The normal mode of the crash handler is to only catch when you are NOT debugging (started through ctrl-f5)
	*crashPtr=10;
}

void RunGame(void)
{
	int a=10;
	int b=20;
	function1(a);
	printf("%i", b);
}

//#define _DEBUG_CRASH_REPORTER

// If you don't plan to debug the crash reporter itself, you can remove _DEBUG_CRASH_REPORTER
#ifdef _DEBUG_CRASH_REPORTER
#include "WindowsIncludes.h"
extern void DumpMiniDump(PEXCEPTION_POINTERS excpInfo);
#endif


void main(void)
{
	printf("Demonstrates the crash reporter.\n");
	printf("This program will prompt you for a variety of actions to take on crash.\n");
	printf("If so desired, it will generate a minidump which can be opened in visual studio\n");
	printf("to debug the crash.\n\n");

	RakNet::CrashReportControls controls;
	controls.actionToTake=0;

	printf("Send an email? (y/n)\n");
	if (getch()=='y')
	{
		printf("Attach the mini-dump to the email? (y/n)\n");
		if (getch()=='y')
			controls.actionToTake|=RakNet::AOC_EMAIL_WITH_ATTACHMENT;
		else
			controls.actionToTake|=RakNet::AOC_EMAIL_NO_ATTACHMENT;
	}
	printf("Write mini-dump to disk? (y/n)\n");
	if (getch()=='y')
		controls.actionToTake|=RakNet::AOC_WRITE_TO_DISK;
	printf("Handle crashes in silent mode (no prompts)? (y/n)\n");
	if (getch()=='y')
		controls.actionToTake|=RakNet::AOC_SILENT_MODE;

	if ((controls.actionToTake & RakNet::AOC_EMAIL_WITH_ATTACHMENT) || (controls.actionToTake & RakNet::AOC_EMAIL_NO_ATTACHMENT))
	{
		if (controls.actionToTake & RakNet::AOC_SILENT_MODE)
		{
			printf("Enter SMTP Server: ");
			Gets(controls.SMTPServer,sizeof(controls.SMTPServer));
			if (controls.SMTPServer[0]==0)
				return;
			printf("Enter SMTP account name: ");
			Gets(controls.SMTPAccountName,sizeof(controls.SMTPAccountName));
			if (controls.SMTPAccountName[0]==0)
				return;
			printf("Enter sender email address: ");
			Gets(controls.emailSender,sizeof(controls.emailSender));
		}

		printf("Enter email recipient email address: ");
		Gets(controls.emailRecipient,sizeof(controls.emailRecipient));
		if (controls.emailRecipient[0]==0)
			return;

		printf("Enter subject prefix, if any: ");
		Gets(controls.emailSubjectPrefix,sizeof(controls.emailSubjectPrefix));

		if ((controls.actionToTake & RakNet::AOC_SILENT_MODE)==0)
		{
			printf("Enter text to write in email body: ");
			Gets(controls.emailBody,sizeof(controls.emailBody));
		}
	}
	
	if (controls.actionToTake & RakNet::AOC_WRITE_TO_DISK)
	{
		printf("Enter disk path to write to (ENTER for current directory): ");
		Gets(controls.pathToMinidump,sizeof(controls.pathToMinidump));
	}

	printf("Enter application name: ");
	Gets(controls.appName,sizeof(controls.appName));
	printf("Enter application version: ");
	Gets(controls.appVersion,sizeof(controls.appVersion));

	// MiniDumpNormal will not give you SocketLayer::I correctly but is small (like 15K)
	// MiniDumpWithDataSegs is much bigger (391K) but does give you SocketLayer::I correctly.
	controls.minidumpType=MiniDumpWithDataSegs;

	// You must call Start before any crashes will be reported.
	RakNet::CrashReporter::Start(&controls);
	printf("Crash reporter started.\n");

// If you don't plan to debug the crash reporter itself, you can remove the __try within _DEBUG_CRASH_REPORTER
#ifdef _DEBUG_CRASH_REPORTER
	__try 
#endif
	{
		RunGame();
	}

// If you don't plan to debug the crash reporter itself, you can remove the DumpMiniDump code within _DEBUG_CRASH_REPORTER
#ifdef _DEBUG_CRASH_REPORTER
	__except(DumpMiniDump(GetExceptionInformation()),EXCEPTION_EXECUTE_HANDLER) 
	{
	}
#endif
}
