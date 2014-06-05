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
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "LinuxStrings.h"

char * FormatString(const char *format, ...)
{
	static int textIndex=0;
	static char text[4][8096];
	va_list ap;
	va_start(ap, format);

	if (++textIndex==4)
		textIndex=0;
	_vsnprintf(text[textIndex], 8096, format, ap);
	va_end(ap);
	text[textIndex][8096-1]=0;

	return text[textIndex];
}

char * FormatStringTS(char *output, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	_vsnprintf(output, 512, format, ap);
	va_end(ap);
	return output;
}
