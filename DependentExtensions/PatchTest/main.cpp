/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "ApplyPatch.h"
#include "CreatePatch.h"
#include <stdio.h>
#include <string.h>
/*
// Test from memory
void main(void)
{
	char source[256], destination[256];
	strcpy(source, "The quick red fox jumped over the lazy brown dog.");
	strcpy(destination, "A story: The quick red fox jumped over the lazy brown dog.  The dog then ripped the fox's freaking head off.");

	char *patch;
	unsigned patchSize;
	if (CreatePatch(source, (unsigned int)strlen(source), destination, (unsigned int)strlen(destination), &patch, &patchSize)==false)
	{
		printf("CreatePatch failed!\n");
	}

	char *patchedSource;
	unsigned patchedSourceSize;
	if (ApplyPatch(source, (unsigned int)strlen(source), &patchedSource, &patchedSourceSize, patch, patchSize)==false)
	{
		printf("ApplyPatch failed!\n");
	}

	if (patchedSourceSize!=(unsigned int)strlen(destination))
		printf("Patched source file does not match length of destination\n");
	else if (memcmp(patchedSource, destination, patchedSourceSize)!=0)
		printf("Patched source does not match destination\n");
	else
		printf("Success!\n");
}
*/

// Test from files
extern int TestPatchInMemory(int argc,char *argv[]);
extern int TestDiffInMemory(int argc,char *argv[]);
extern int DIFF_main(int argc,char *argv[]);
extern int PATCH_main(int argc,char * argv[]);
#include <conio.h>

void main(void)
{
	printf("(M)ine or (T)heirs?\n");
	if (getch()=='m')
	{
		char *argv[4];
		argv[1]="Descent1.dll.bak";
		argv[2]="Descent2.dll.bak";
		argv[3]="my_patch";
		if (TestDiffInMemory(4,argv)==0)
		{
			printf("TestDiffInMemory Failed.\n");
			return;
		}

		argv[1]="Descent1.dll.bak";
		argv[2]="Descent2_patched_my.dll.bak";
		argv[3]="my_patch";
		if (TestPatchInMemory(4, argv)==0)
		{
			printf("TestPatchInMemory Failed.\n");
			return;
		}

		printf("Success.\n");
	}
	else
	{
		char *argv[4];
		argv[1]="Descent1.dll.bak";
		argv[2]="Descent2.dll.bak";
		argv[3]="their_patch";
		DIFF_main(4,argv);

		argv[1]="Descent1.dll.bak";
		argv[2]="Descent2_patched.dll.bak";
		argv[3]="their_patch";
		PATCH_main(4, argv);

		printf("Success.\n");
	}
}
