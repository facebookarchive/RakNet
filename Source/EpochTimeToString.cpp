/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "FormatString.h"
#include "EpochTimeToString.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
// localtime
#include <time.h>
#include "LinuxStrings.h"

char * EpochTimeToString(long long time)
{
	static int textIndex=0;
	static char text[4][64];

	if (++textIndex==4)
		textIndex=0;

	struct tm * timeinfo;
	time_t t = time;
	timeinfo = localtime ( &t );
	strftime (text[textIndex],64,"%c.",timeinfo);

	/*
	time_t
	// Copied from the docs
	struct tm *newtime;
	newtime = _localtime64(& time);
	asctime_s( text[textIndex], sizeof(text[textIndex]), newtime );

	while (text[textIndex][0] && (text[textIndex][strlen(text[textIndex])-1]=='\n' || text[textIndex][strlen(text[textIndex])-1]=='\r'))
		text[textIndex][strlen(text[textIndex])-1]=0;
		*/

	return text[textIndex];
}
