/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __CRASH_REPORTER_H
#define __CRASH_REPORTER_H

// This is a crash reporter that will send a minidump by email on unhandled exceptions
// This normally only runs if you are not currently debugging.
// To send reports while debugging (mostly to test this class), define _DEBUG_CRASH_REPORTER
// and put your code in a try/except block such as
//
// extern void DumpMiniDump(PEXCEPTION_POINTERS excpInfo);
//
// void main(void)
//{
//__try 
//{
//	RunGame();
//}
//__except(DumpMiniDump(GetExceptionInformation()),EXCEPTION_EXECUTE_HANDLER) 
//{
//}
//}

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
//

// To use:
// #include "DbgHelp.h"
// Link with Dbghelp.lib ws2_32.lib

namespace RakNet {

// Possible actions to take on a crash.  If you want to restart the app as well, see the CrashRelauncher sample.
enum CrashReportAction
{
	// Send an email (mutually exclusive with AOC_EMAIL_WITH_ATTACHMENT)
	AOC_EMAIL_NO_ATTACHMENT=1,

	// Send an email and attach the minidump (mutually exclusive with AOC_EMAIL_NO_ATTACHMENT)
	AOC_EMAIL_WITH_ATTACHMENT=2,

	// Write the minidump to disk in a specified directory
	AOC_WRITE_TO_DISK=4,

	// In silent mode there are no prompts.  This is useful for an unmonitored application.
	AOC_SILENT_MODE=8
};

/// Holds all the parameters to CrashReporter::Start
struct CrashReportControls
{
	// Bitwise OR of CrashReportAction values to determine what to do on a crash.
	int actionToTake;

	// Used to generate the dump filename.  Required with AOC_EMAIL_WITH_ATTACHMENT or AOC_WRITE_TO_DISK
	char appName[128];
	char appVersion[128];

	// Used with AOC_WRITE_TO_DISK .  Path to write to.  Not the filename, just the path. Empty string means the current directory.
	char pathToMinidump[260];

	// Required with AOC_EMAIL_* & AOC_SILENT_MODE . The SMTP server to send emails from.
	char SMTPServer[128];

	// Required with AOC_EMAIL_* & AOC_SILENT_MODE . The account name to send emails with (probably your email address).
	char SMTPAccountName[64];

	// Required with AOC_EMAIL_* & AOC_SILENT_MODE . What to put in the sender field of the email.
	char emailSender[64];

	// Required with AOC_EMAIL_* .  What to put in the subject of the email.
	char emailSubjectPrefix[128];

	// Required with AOC_EMAIL_* as long as you are NOT in AOC_SILENT_MODE . What to put in the body of the email.
	char emailBody[1024];

	// Required with AOC_EMAIL_* . Who to send the email to.
	char emailRecipient[64];

	// Required with AOC_EMAIL_* . What password to use to send the email under TLS, if required
	char emailPassword[64];

	// How much memory to write. MiniDumpNormal is the least but doesn't seem to give correct globals. MiniDumpWithDataSegs gives more.
	// Include "DbgHelp.h" for these enumerations.
	int minidumpType;
};

/// \brief On an unhandled exception, will save a minidump and email it.
/// A minidump can be opened in visual studio to give the callstack and local variables at the time of the crash.
/// It has the same amount of information as if you crashed while debugging in the relevant mode.  So Debug tends to give
/// accurate stacks and info while Release does not.
///
/// Minidumps are only accurate for the code as it was compiled at the date of the release.  So you should label releases in source control
/// and put that label number in the 'appVersion' field.
class CrashReporter
{
public:
	static void Start(CrashReportControls *input);
	static CrashReportControls controls;
};

} // namespace RakNet

#endif
