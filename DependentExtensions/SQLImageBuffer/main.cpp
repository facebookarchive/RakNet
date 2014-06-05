/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "RakPeerInterface.h"
#include "RakSleep.h"
#include "RakThread.h"
#include "RakNetworkFactory.h"
#include <stdio.h>
#include "../SQLite3Plugin/ServerOnly/sqlite3.h"
#include "GetTime.h"
#include "RakString.h"
#include "jpeglib.h"

using namespace RakNet;

struct my_error_mgr {
	struct jpeg_error_mgr pub;	/* "public" fields */
};

METHODDEF(void) my_error_exit (j_common_ptr cinfo);

//
//	to handle fatal errors.
//	the original JPEG code will just exit(0). can't really
//	do that in Windows....
//

METHODDEF(void) my_error_exit (j_common_ptr cinfo)
{
}

void main(void)
{
	printf("Add images to SQLite database and read back to measure performance.\n");

	// Create a database, and tell the plugin about it
	sqlite3 *database;
	// Here :memory: means create the database in memory only.
	// Normally the first parameter refers to a path on the disk to the database file
	if (sqlite3_open_v2("C:\\EchoChamber\\sqliteDb", &database, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0)!=SQLITE_OK)
		return;

	char *errorMsg;
	sqlite3_exec(database, "CREATE TABLE Images (imageId_pk integer PRIMARY KEY, imageData bytea);", 0, 0, &errorMsg);
	if (errorMsg)
	{
		printf(errorMsg);
		sqlite3_free(errorMsg);
		return;
	}

	printf("Opening image\n");
	const char *imgPath1="c:\\temp\\img1.jpg";
	const char *imgPath2="c:\\temp\\img2.jpg";
	FILE *fp = fopen(imgPath1, "rb");
	unsigned fileLength1;
	fseek(fp, 0, SEEK_END);
	fileLength1 = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *fileBuff1 = (char *) malloc(fileLength1);
	fread(fileBuff1, 1, fileLength1, fp);
	fclose(fp);

	fp = fopen(imgPath2, "rb");
	unsigned fileLength2;
	fseek(fp, 0, SEEK_END);
	fileLength2 = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *fileBuff2 = (char *) malloc(fileLength2);
	fread(fileBuff2, 1, fileLength2, fp);
	fclose(fp);

	static const int numInsertions=100;
	printf("Adding image to database %i times\n", numInsertions);

	sqlite3_stmt *statement;
	if (sqlite3_prepare_v2(
		database, 
		"INSERT INTO Images (imageData) VALUES (?);",
		-1,
		&statement,
		0
		)!=SQLITE_OK)
		return;


	unsigned int i;
	int rc;
	for (i=0; i < numInsertions; i++)
	{
		if ((i&1)==0)
		{
			if (sqlite3_bind_blob(statement, 1, fileBuff1, fileLength1, 0)!=SQLITE_OK)
				return;
		}
		else
		{
			if (sqlite3_bind_blob(statement, 1, fileBuff2, fileLength2, 0)!=SQLITE_OK)
				return;
		}

		rc = sqlite3_step(statement);
		if (rc!=SQLITE_DONE && rc!=SQLITE_OK)
			return;

		sqlite3_reset(statement);
	}

	sqlite3_finalize(statement);


	// 4 animations 25 FPS = 100 images per second
	if (sqlite3_prepare_v2(
		database, 
		"SELECT imageData FROM Images;",
		-1,
		&statement,
		0
		)!=SQLITE_OK)
		return;

	// Execute first time
	printf("Reading 100 images\n");
	RakNetTimeMS startTime=RakNet::GetTimeMS();
	rc = sqlite3_step(statement);
	RakNetTimeMS endTime=RakNet::GetTimeMS();
	printf("Statement execution took %i milliseconds\n", endTime-startTime);
	printf("Processing row ");
	unsigned int rowCount=1;
	while (rc==SQLITE_ROW)
	{
		const void *readImageDataFromDb = sqlite3_column_blob(statement, 0);
		int lengthOfImageData = sqlite3_column_bytes(statement, 0);


		struct jpeg_decompress_struct cinfo;
		struct my_error_mgr jerr;
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = my_error_exit;
		jpeg_create_decompress(&cinfo);
		jpeg_memory_src(&cinfo, (const JOCTET *) readImageDataFromDb, lengthOfImageData);
		(void) jpeg_read_header(&cinfo, TRUE);
		JDIMENSION width = cinfo.image_width;
		JDIMENSION height = cinfo.image_height;
		jpeg_destroy_decompress(&cinfo);

		printf("%i ", rowCount++);

		rc = sqlite3_step(statement);
	}
	if (rc==SQLITE_ERROR)
		return;
	RakNetTimeMS endTime2=RakNet::GetTimeMS();

	sqlite3_reset(statement);
	printf("\n");
	printf("Processing took %i milliseconds\n", endTime2-endTime);
	printf("Total time is %i milliseconds\n", endTime2-startTime);

	sqlite3_finalize(statement);

	free(fileBuff1);
	free(fileBuff2);
	sqlite3_close(database);

	printf("Press enter to quit\n");
	char str[256];
	gets(str);
	return;
}
