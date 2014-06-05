/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

/// \file
/// \brief An implementation of the AutopatcherRepositoryInterface to use PostgreSQL to store the relevant data


#include "AutopatcherPostgreRepository.h"
#include "AutopatcherPatchContext.h"
#include "FileList.h"
// libpq-fe.h is part of PostgreSQL which must be installed on this computer to use the PostgreRepository
#include "libpq-fe.h"
#include "CreatePatch.h"
#include "AutopatcherPatchContext.h"
// #include "DR_SHA1.h"
#include <stdlib.h>
#include "LinuxStrings.h"
// localtime
#include <time.h>

static const unsigned HASH_LENGTH=sizeof(unsigned int);

// ntohl
#ifdef _WIN32
#include <Winsock2.h>
#else
#include <netinet/in.h>
#endif

// alloca
#ifdef _COMPATIBILITY_1
#elif defined(_WIN32)
#include <malloc.h>
#else
//#include <stdlib.h>
#endif

#define PQEXECPARAM_FORMAT_TEXT		0
#define PQEXECPARAM_FORMAT_BINARY	1

using namespace RakNet;

AutopatcherPostgreRepository::AutopatcherPostgreRepository()
{
	filePartConnection=0;
}
AutopatcherPostgreRepository::~AutopatcherPostgreRepository()
{
	if (filePartConnection)
		PQfinish(filePartConnection);
}
bool AutopatcherPostgreRepository::CreateAutopatcherTables(void)
{
	if (isConnected==false)
		return false;

	const char *command =
		"BEGIN;"
	"CREATE TABLE Applications ("
		"applicationID serial PRIMARY KEY UNIQUE,"
		"applicationName text NOT NULL UNIQUE,"
		"changeSetID integer NOT NULL DEFAULT 0,"
		"userName text NOT NULL"
		");"
	"CREATE TABLE FileVersionHistory ("
		"fileID serial PRIMARY KEY UNIQUE,"
		"applicationID integer REFERENCES Applications ON DELETE CASCADE,"
		"filename text NOT NULL,"
		"fileLength integer,"
		"content bytea,"
		"contentHash bytea,"
		"patch bytea,"
		"patchAlgorithm integer,"
		"createFile boolean NOT NULL,"
		"modificationDate double precision DEFAULT (EXTRACT(EPOCH FROM now())),"
		"lastSentDate double precision,"
		"timesSent integer NOT NULL DEFAULT 0,"
		"changeSetID integer NOT NULL,"
		"userName text NOT NULL,"
		"CONSTRAINT file_has_data CHECK ( createFile=FALSE OR ((content IS NOT NULL) AND (contentHash IS NOT NULL) AND (fileLength IS NOT NULL) ) )"
		");"
	"CREATE VIEW AutoPatcherView AS SELECT "
		"FileVersionHistory.applicationid,"
		"Applications.applicationName,"
		"FileVersionHistory.fileID,"
		"FileVersionHistory.fileName,"
		"FileVersionHistory.createFile,"
		"FileVersionHistory.fileLength,"
		"FileVersionHistory.changeSetID,"
		"FileVersionHistory.lastSentDate," 
		"FileVersionHistory.modificationDate,"
		"FileVersionHistory.timesSent "
	"FROM (FileVersionHistory JOIN Applications ON "
		"( FileVersionHistory.applicationID = Applications.applicationID )) "
	"ORDER BY Applications.applicationID ASC, FileVersionHistory.fileID ASC;"
	"COMMIT;";

	PGresult *result;
	//sqlCommandMutex.Lock();
	bool res = ExecuteBlockingCommand(command, &result, true);
	//sqlCommandMutex.Unlock();
	PQclear(result);
	return res;
}
bool AutopatcherPostgreRepository2::CreateAutopatcherTables(void)
{
	if (isConnected==false)
		return false;

	const char *command =
		"BEGIN;"
		"CREATE TABLE Applications ("
		"applicationID serial PRIMARY KEY UNIQUE,"
		"applicationName text NOT NULL UNIQUE,"
		"changeSetID integer NOT NULL DEFAULT 0,"
		"userName text NOT NULL"
		");"
		"CREATE TABLE FileVersionHistory ("
		"fileID serial PRIMARY KEY UNIQUE,"
		"applicationID integer REFERENCES Applications ON DELETE CASCADE,"
		"filename text NOT NULL,"
		"fileLength integer,"
		"pathToContent text,"
		"contentHash bytea,"
		"patch bytea,"
		"patchAlgorithm integer,"
		"createFile boolean NOT NULL,"
		"modificationDate double precision DEFAULT (EXTRACT(EPOCH FROM now())),"
		"lastSentDate double precision,"
		"timesSent integer NOT NULL DEFAULT 0,"
		"changeSetID integer NOT NULL,"
		"userName text NOT NULL,"
		"CONSTRAINT file_has_data CHECK ( createFile=FALSE OR ((contentHash IS NOT NULL) AND (fileLength IS NOT NULL) ) )"
		");"
		"CREATE VIEW AutoPatcherView AS SELECT "
		"FileVersionHistory.applicationid,"
		"Applications.applicationName,"
		"FileVersionHistory.fileID,"
		"FileVersionHistory.fileName,"
		"FileVersionHistory.createFile,"
		"FileVersionHistory.fileLength,"
		"FileVersionHistory.changeSetID,"
		"FileVersionHistory.lastSentDate," 
		"FileVersionHistory.modificationDate,"
		"FileVersionHistory.timesSent "
		"FROM (FileVersionHistory JOIN Applications ON "
		"( FileVersionHistory.applicationID = Applications.applicationID )) "
		"ORDER BY Applications.applicationID ASC, FileVersionHistory.fileID ASC;"
		"COMMIT;";

	PGresult *result;
	//sqlCommandMutex.Lock();
	bool res = ExecuteBlockingCommand(command, &result, true);
	//sqlCommandMutex.Unlock();
	PQclear(result);
	return res;
}
bool AutopatcherPostgreRepository::DestroyAutopatcherTables(void)
{
	if (isConnected==false)
		return false;

	const char *command = 
		"BEGIN;"
		"DROP TABLE Applications CASCADE;"
		"DROP TABLE FileVersionHistory CASCADE;"
		"COMMIT;";

	PGresult *result;
	//sqlCommandMutex.Lock();
	bool b = ExecuteBlockingCommand(command, &result, true);
	//sqlCommandMutex.Unlock();
	PQclear(result);
	return b;
}
bool AutopatcherPostgreRepository::AddApplication(const char *applicationName, const char *userName)
{
	char query[512];
	if (strlen(applicationName)>100)
		return false;
	if (strlen(userName)>100)
		return false;

	sprintf(query, "INSERT INTO Applications (applicationName, userName) VALUES ('%s', '%s');", GetEscapedString(applicationName).C_String(), GetEscapedString(userName).C_String());
	PGresult *result;
	//sqlCommandMutex.Lock();
	bool b = ExecuteBlockingCommand(query, &result, false);
	//sqlCommandMutex.Unlock();
	PQclear(result);
	return b;
}
bool AutopatcherPostgreRepository::RemoveApplication(const char *applicationName)
{
	char query[512];
	if (strlen(applicationName)>100)
		return false;
	
	sprintf(query, "DELETE FROM Applications WHERE applicationName='%s';", GetEscapedString(applicationName).C_String());
	PGresult *result;
	//sqlCommandMutex.Lock();
	bool b = ExecuteBlockingCommand(query, &result, false);
	//sqlCommandMutex.Unlock();
	PQclear(result);
	return b;
}

bool AutopatcherPostgreRepository::GetChangelistSinceDate(const char *applicationName, FileList *addedOrModifiedFilesWithHashData, FileList *deletedFiles, double sinceDate)
{
	PGresult *result;
	char query[512];
	if (strlen(applicationName)>100)
		return false;
	RakNet::RakString escapedApplicationName = GetEscapedString(applicationName);
	sprintf(query, "SELECT applicationID FROM applications WHERE applicationName='%s';", escapedApplicationName.C_String());
	//sqlCommandMutex.Lock();
	if (ExecuteBlockingCommand(query, &result, false)==false)
	{
		//sqlCommandMutex.Unlock();
		PQclear(result);
		return false;
	}
	//sqlCommandMutex.Unlock();
	int numRows;
	numRows = PQntuples(result);
	if (numRows==0)
	{
		sprintf(lastError,"ERROR: %s not found in UpdateApplicationFiles\n",applicationName);
		return false;
	}
	char *res;
	res = PQgetvalue(result,0,0);
	int applicationID;
	applicationID=atoi(res);
	PQclear(result);
	if (sinceDate!=0.0)
		sprintf(query, "SELECT DISTINCT ON (filename) filename, fileLength, contentHash, createFile FROM FileVersionHistory WHERE applicationId=%i AND modificationDate > %f ORDER BY filename, fileId DESC;", applicationID, sinceDate);
	else
		sprintf(query, "SELECT DISTINCT ON (filename) filename, fileLength, contentHash, createFile FROM FileVersionHistory WHERE applicationId=%i ORDER BY filename, fileId DESC;", applicationID);

	//sqlCommandMutex.Lock();
	result = PQexecParams(pgConn, query,0,0,0,0,0,PQEXECPARAM_FORMAT_BINARY);
	//sqlCommandMutex.Unlock();
	if (IsResultSuccessful(result, false)==false)
	{
		PQclear(result);
		return false;
	}

	int filenameColumnIndex = PQfnumber(result, "filename");
	int contentHashColumnIndex = PQfnumber(result, "contentHash");
	int createFileColumnIndex = PQfnumber(result, "createFile");
	int fileLengthColumnIndex = PQfnumber(result, "fileLength");
	char *hardDriveFilename;
	char *hardDriveHash;
	char *createFileResult;
	char *fileLengthPtr;
	int rowIndex;
	int fileLength;
	RakAssert(PQfsize(result, fileLengthColumnIndex)==sizeof(fileLength));
	
	numRows = PQntuples(result);

	for (rowIndex=0; rowIndex < numRows; rowIndex++)
	{
		createFileResult = PQgetvalue(result, rowIndex, createFileColumnIndex);
		hardDriveFilename = PQgetvalue(result, rowIndex, filenameColumnIndex);
		if (createFileResult[0]==1)
		{
			hardDriveHash = PQgetvalue(result, rowIndex, contentHashColumnIndex);
			fileLengthPtr = PQgetvalue(result, rowIndex, fileLengthColumnIndex);
			memcpy(&fileLength, fileLengthPtr, sizeof(fileLength));
			fileLength=ntohl(fileLength); // This is asinine...
			addedOrModifiedFilesWithHashData->AddFile(hardDriveFilename, hardDriveFilename, hardDriveHash, HASH_LENGTH, fileLength, FileListNodeContext(0,0,0,0), false);
		}
		else
		{
			deletedFiles->AddFile(hardDriveFilename,hardDriveFilename,0,0,0,FileListNodeContext(0,0,0,0), false);
		}
	}
	
	return true;
}
int AutopatcherPostgreRepository::GetPatches(const char *applicationName, FileList *input, bool allowDownloadOfOriginalUnmodifiedFiles, FileList *patchList)
{
	PGresult *result;
	char query[512];
	if (strlen(applicationName)>100)
		return 0;
	RakNet::RakString escapedApplicationName = GetEscapedString(applicationName);
	sprintf(query, "SELECT applicationID FROM applications WHERE applicationName='%s';", escapedApplicationName.C_String());
	//sqlCommandMutex.Lock();
	if (ExecuteBlockingCommand(query, &result, false)==false)
	{
		//sqlCommandMutex.Unlock();
		PQclear(result);
		return 0;
	}
	//sqlCommandMutex.Unlock();
	int numRows;
	numRows = PQntuples(result);
	if (numRows==0)
	{
		sprintf(lastError,"ERROR: %s not found in UpdateApplicationFiles\n",applicationName);
		return 0;
	}
	char *res;
	res = PQgetvalue(result,0,0);
	int applicationID;
	applicationID=atoi(res);
	PQclear(result);

	// Go through the input list.
	unsigned inputIndex;
	char *userHash, *contentHash;
	RakNet::RakString userFilename;
	// char *content;
//	char *fileId, *fileLength;
	char *patch;
	// int contentLength;
	int patchLength;
//	int contentColumnIndex;
	int contentHashIndex, fileIdIndex, fileLengthIndex;
	const char *outTemp[2];
	int outLengths[2];
	int formats[2];
	PGresult *patchResult;
	for (inputIndex=0; inputIndex < input->fileList.Size(); inputIndex++)
	{
		userHash=input->fileList[inputIndex].data;
		userFilename=input->fileList[inputIndex].filename;

		if (userHash==0)
		{
			// If the user does not have a hash in the input list, get the contents of latest version of this named file and write it to the patch list
		//	sprintf(query, "SELECT DISTINCT ON (filename) content FROM FileVersionHistory WHERE applicationId=%i AND filename=$1::text ORDER BY filename, fileId DESC;", applicationID);
			sprintf(query, "SELECT DISTINCT ON (filename) fileId, fileLength, changeSetID FROM FileVersionHistory WHERE applicationId=%i AND filename=$1::text ORDER BY filename, fileId DESC;", applicationID);
			outTemp[0]=userFilename.C_String();
			outLengths[0]=(int) userFilename.GetLength();
			formats[0]=PQEXECPARAM_FORMAT_BINARY;
			//sqlCommandMutex.Lock();
			result = PQexecParams(pgConn, query,1,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);
			//sqlCommandMutex.Unlock();
			if (IsResultSuccessful(result, false)==false)
			{
				PQclear(result);
				return 0;
			}
			numRows = PQntuples(result);
			if (numRows>0)
			{
			//	content = PQgetvalue(result, 0, 0);
			//	contentLength=PQgetlength(result, 0, 0);
			//	patchList->AddFile(userFilename, content, contentLength, contentLength, FileListNodeContext(PC_WRITE_FILE,0),false);

				int fileIdIndex = PQfnumber(result, "fileId");
				int fileLengthIndex = PQfnumber(result, "fileLength");
				int changeSetIdIndex = PQfnumber(result, "changeSetID");
				int fileId = ntohl(*((int*)PQgetvalue(result, 0, fileIdIndex)));
				int fileLength = ntohl(*((int*)PQgetvalue(result, 0, fileLengthIndex)));
				int changeSetId = ntohl(*((int*)PQgetvalue(result, 0, changeSetIdIndex)));

				if (allowDownloadOfOriginalUnmodifiedFiles==false && changeSetId==0)
				{
					printf("Failure: allowDownloadOfOriginalUnmodifiedFiles==false for %s length %i\n", userFilename.C_String(), fileLength);
					PQclear(result);
					return -1;
				}

				patchList->AddFile(userFilename,userFilename, 0, fileLength, fileLength, FileListNodeContext(PC_WRITE_FILE,fileId,0,0),true);
			}
			PQclear(result);
		}
		else // Assuming the user does have a hash.
		{
			if (input->fileList[inputIndex].dataLengthBytes!=HASH_LENGTH)
				return 0;

			// Get the hash and ID of the latest version of this file, by filename.
			sprintf(query, "SELECT DISTINCT ON (filename) contentHash, fileId, fileLength FROM FileVersionHistory WHERE applicationId=%i AND filename=$1::text ORDER BY filename, fileId DESC;", applicationID);
			outTemp[0]=userFilename.C_String();
			outLengths[0]=(int)userFilename.GetLength();
			formats[0]=PQEXECPARAM_FORMAT_BINARY;
			//sqlCommandMutex.Lock();
			result = PQexecParams(pgConn, query,1,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);
			//sqlCommandMutex.Unlock();
			if (IsResultSuccessful(result, false)==false)
			{
				PQclear(result);
				return 0;
			}
			numRows = PQntuples(result);
			if (numRows>0)
			{
				contentHashIndex = PQfnumber(result, "contentHash");
				fileIdIndex = PQfnumber(result, "fileId");
				fileLengthIndex = PQfnumber(result, "fileLength");
				contentHash = PQgetvalue(result, 0, contentHashIndex);
				int fileIdInt = ntohl(*((int*)PQgetvalue(result, 0, fileIdIndex)));
				int fileLengthInt = ntohl(*((int*)PQgetvalue(result, 0, fileLengthIndex)));
//				fileId = PQgetvalue(result, 0, fileIdIndex);
//				fileLength = PQgetvalue(result, 0, fileLengthIndex);
//				int fileLengthInt = ntohl(*((int*)fileLength));

//				unsigned int hash1 = SuperFastHashFile("C:/temp/RakNet_Icon_Final.psd");
//				unsigned int hash2 = SuperFastHashFile("C:/temp/AutopatcherClient/RakNet_Icon_Final.psd");

				if (memcmp(contentHash, userHash, HASH_LENGTH)!=0)
				{
					// Look up by user hash/filename/applicationID, returning the patch
					sprintf(query, "SELECT patch, patchAlgorithm, fileId FROM FileVersionHistory WHERE applicationId=%i AND filename=$1::text AND contentHash=$2::bytea;", applicationID);
					outTemp[0]=userFilename.C_String();
					outLengths[0]=(int)userFilename.GetLength();
					formats[0]=PQEXECPARAM_FORMAT_TEXT;
					outTemp[1]=userHash;
					outLengths[1]=HASH_LENGTH;
					formats[1]=PQEXECPARAM_FORMAT_BINARY;
					//sqlCommandMutex.Lock();
					patchResult = PQexecParams(pgConn, query,2,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);
					//sqlCommandMutex.Unlock();

					if (IsResultSuccessful(patchResult, false)==false)
					{
						PQclear(patchResult);
						return 0;
					}
					numRows = PQntuples(patchResult);
					if (numRows==0)
					{
						PQclear(patchResult);

			//			outTemp[0]=fileId;
			//			outLengths[0]=PQfsize(result, fileIdIndex);
			//			formats[0]=PQEXECPARAM_FORMAT_BINARY;

						/*
						// Get 32000000 bytes at a time to workaround http://support.microsoft.com/kb/q201213
						int fileIndex=0;
						char *file = RakNet::OP_NEW char[fileLengthInt];
						while (fileIndex < fileLengthInt)
						{
							sprintf(query, "SELECT substring(content from %i for 32000000) FROM FileVersionHistory WHERE fileId=$1::integer;", fileIndex, fileId);
							patchResult = PQexecParams(pgConn, query,1,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);
							if (IsResultSuccessful(patchResult, false)==false)
							{
								PQclear(patchResult);
								return false;
							}
							content = PQgetvalue(patchResult, 0, 0);
							contentLength=PQgetlength(patchResult, 0, 0);
							assert(contentLength==32000000 || contentLength==fileLengthInt-fileIndex);
							memcpy(file+fileIndex,content,contentLength);
							PQclear(patchResult);
							fileIndex+=32000000;
						}
*/

						/*
						sprintf(query, "SELECT content FROM FileVersionHistory WHERE fileId=%i", fileIdInt);
						patchResult = PQexecParams(pgConn, query,0,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);
						if (IsResultSuccessful(patchResult, false)==false)
						{
							PQclear(patchResult);
							return false;
						}
						char *content = PQgetvalue(patchResult, 0, 0);
						int contentLength=PQgetlength(patchResult, 0, 0);
						unsigned int hash1 = SuperFastHashFile("C:/temp/RakNet_Icon_Final.psd");
						unsigned int hash2 = SuperFastHash(content, contentLength);
						*/

						// No patch, add the file
						patchList->AddFile(userFilename,userFilename, 0, fileLengthInt, fileLengthInt, FileListNodeContext(PC_WRITE_FILE,fileIdInt,0,0), true);
//						patchList->AddFile(userFilename, file, fileLengthInt, contentLength, FileListNodeContext(PC_WRITE_FILE,0), true);
//						RakNet::OP_DELETE_ARRAY file;
					}
					else
					{
						int patchColumnIndex = PQfnumber(patchResult, "patch");
						int patchAlgorithmColumnIndex = PQfnumber(patchResult, "patchAlgorithm");


						// Otherwise, write the hash of the new version and then write the patch to get to that version.
						// 
						// int patchAlgorithm = ntohl(*((int*)PQgetvalue(result, 0, patchAlgorithmColumnIndex)));

						const char *tv = PQgetvalue(patchResult, 0, patchAlgorithmColumnIndex);
						int patchAlgorithm;
						if (tv)
							patchAlgorithm = ntohl(*((int*)tv));
						else
							patchAlgorithm = 0;

						int patchFileIdColumnIndex = PQfnumber(patchResult, "fileId");
						const char *tv2 = PQgetvalue(patchResult, 0, patchFileIdColumnIndex);
						int patchIdInt = ntohl(*((int*)tv2));

						patchLength=PQgetlength(patchResult, 0, patchColumnIndex);

						bool useReference = patchLength > 1048576;
						char *temp;

						if (useReference==false)
							temp = RakNet::OP_NEW_ARRAY<char>(patchLength + HASH_LENGTH, _FILE_AND_LINE_ );
						else
							temp = 0;

						if (temp!=0)
						{
							patch = PQgetvalue(patchResult, 0, patchColumnIndex);
							memcpy(temp, contentHash, HASH_LENGTH);
							memcpy(temp+HASH_LENGTH, patch, patchLength);
							//						int len;
							//						assert(PQfsize(result, fileLengthIndex)==sizeof(fileLength));
							//						memcpy(&len, fileLength, sizeof(len));
							//						len=ntohl(len);
							//	printf("send patch %i bytes\n", patchLength);
							//	for (int i=0; i < patchLength; i++)
							//		printf("%i ", patch[i]);
							//	printf("\n");

							patchList->AddFile(userFilename,userFilename, temp, HASH_LENGTH+patchLength, fileLengthInt, FileListNodeContext(PC_HASH_1_WITH_PATCH,0,patchAlgorithm,0),false );
						}
						else
						{
							patchList->AddFile(userFilename,userFilename, temp, HASH_LENGTH+patchLength, fileLengthInt, FileListNodeContext(PC_HASH_1_WITH_PATCH,fileIdInt,patchAlgorithm,patchIdInt),true );
						}
						PQclear(patchResult);
						RakNet::OP_DELETE_ARRAY(temp, _FILE_AND_LINE_);
					}
				}
				else
				{
					// else if the hash of this file matches what the user has, the user has the latest version.  Done.
				}
			}
			else
			{
				// else if there is no such file, skip this file.
			}

			PQclear(result);
		}
	}

	return 1;
}
bool AutopatcherPostgreRepository::GetMostRecentChangelistWithPatches(RakNet::RakString &applicationName, FileList *patchedFiles, FileList *addedFiles, FileList *addedOrModifiedFileHashes, FileList *deletedFiles, double *priorRowPatchTime, double *mostRecentRowPatchTime)
{
	PGresult *result;
	char query[1024];
	if (applicationName.GetLength()>100)
		return false;

	(*priorRowPatchTime)=0;
	(*mostRecentRowPatchTime)=0;

	const char *outTemp[2];
	int outLengths[2];
	int formats[2];

	if (applicationName.GetLength()==0)
	{
		strcpy(query, 		
			"SELECT tbl1.applicationName, tbl4.* FROM  "
			"(SELECT applicationID, applicationName FROM Applications) as tbl1, "
			"(SELECT tbl2.applicationId, tbl2.fileId, tbl2.fileName, tbl2.fileLength, tbl2.contentHash, tbl2.createFile, tbl2.changeSetId, tbl2.content, tbl3.patch, tbl3.patchAlgorithm, tbl3.contentHash as priorHash FROM  "
			"(SELECT * From FileVersionHistory WHERE modificationDate=(select MAX(modificationDate) from FileVersionHistory) AND changeSetId!=0 ) as tbl2 "
			"LEFT OUTER JOIN "
			"(SELECT patch, patchAlgorithm, fileName, contentHash FROM FileVersionHistory WHERE  "
			"(changeSetId = (SELECT changeSetId FROM FileVersionHistory WHERE modificationDate=(select MAX(modificationDate) from FileVersionHistory) LIMIT 1) - 1 )) as tbl3 "
			"ON tbl2.filename=tbl3.filename) as tbl4; "
			);

		result = PQexecParams(pgConn, query,0,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);

		if (IsResultSuccessful(result, true)==false)
		{
			PQclear(result);
			return false;
		}
	}
	else
	{
		strcpy(query, 
			"SELECT tbl2.fileId, tbl2.fileName, tbl2.fileLength, tbl2.contentHash, tbl2.createFile, tbl2.changeSetId, tbl2.content, tbl3.patch, tbl3.patchAlgorithm, tbl3.contentHash as priorHash FROM  "
			"(SELECT * From FileVersionHistory WHERE changeSetId=(SELECT MAX(changeSetId) FROM Applications WHERE applicationName=$1::text)-1 AND changeSetId!=0 "
			") as tbl2 "
			"LEFT OUTER JOIN  "
			"(SELECT fileName, patch, patchAlgorithm, contentHash From FileVersionHistory WHERE changeSetId=(SELECT MAX(changeSetId) FROM Applications WHERE applicationName=$2::text)-2 "
			") as tbl3 "
			"ON tbl2.filename=tbl3.filename;"
			);

		outTemp[0]=applicationName.C_String();
		outLengths[0]=(int) applicationName.GetLength();
		formats[0]=PQEXECPARAM_FORMAT_BINARY;
		outTemp[1]=applicationName.C_String();
		outLengths[1]=(int) applicationName.GetLength();
		formats[1]=PQEXECPARAM_FORMAT_BINARY;
		result = PQexecParams(pgConn, query,2,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);

		if (IsResultSuccessful(result, true)==false)
		{
			PQclear(result);
			return false;
		}
	}

	int numRows;
	numRows = PQntuples(result);
	if (numRows==0)
	{
		// Nothing was ever patched. However, read mostRecentRowPatchTime if possible
		if (applicationName.GetLength()==0)
		{
			// Lookup application if unspecified
			PGresult *result3;

			strcpy(query, 		
				"SELECT applicationName FROM Applications as tbl1, "
				"(SELECT applicationId From FileVersionHistory WHERE modificationDate=(select MAX(modificationDate) from FileVersionHistory LIMIT 1) LIMIT 1) as tbl2 "
				"WHERE tbl1.applicationID=tbl2.applicationID;"
				);

			result3 = PQexecParams(pgConn, query,0,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);

			if (IsResultSuccessful(result3, true)==false)
			{
				PQclear(result);
				return false;
			}

			if (PQntuples(result3)==0)
			{
				// No applications at all
				PQclear(result);
				PQclear(result3);
				return false;
			}

			int applicationNameColumnIndex = PQfnumber(result, "applicationName");
			applicationName = PQgetvalue(result3, 0, applicationNameColumnIndex);
			PQclear(result3);
		}
	}
	else if (applicationName.GetLength()==0)
	{
		int applicationNameColumnIndex = PQfnumber(result, "applicationName");
		applicationName = PQgetvalue(result, 0, applicationNameColumnIndex);
	}

	RakNet::RakString escapedApplicationName2 = GetEscapedString(applicationName);
	PGresult *result2;
	char *ts;
	int numRows2;

	// For the given application, get the highest file date
	sprintf(query, 
		"SELECT modificationDate from FileVersionHistory WHERE changeSetId=(SELECT changeSetId FROM Applications WHERE applicationName='%s' AND changeSetId!=0 LIMIT 1)-1 LIMIT 1;"
		, escapedApplicationName2.C_String());
	if (ExecuteBlockingCommand(query, &result2, false)==false)
	{
		sprintf(lastError,"ERROR: Query is bad in file %s at line %i in function GetMostRecentChangelistWithPatches\n",_FILE_AND_LINE_);
		PQclear(result);
		PQclear(result2);
		return false;
	}
	numRows2 = PQntuples(result2);
	if (numRows2==0)
	{
		// No application
		PQclear(result);
		PQclear(result2);
		return false;
	}
	ts=PQgetvalue(result2, 0, 0);
	*mostRecentRowPatchTime=atof(ts);
	PQclear(result2);

	if (numRows==0)
	{
		// No patches to serve
		*priorRowPatchTime=0;

		PQclear(result);
		return true;
	}


	// In SQL, SELECT (EXTRACT(EPOCH FROM now())) is equivalent to time() function
	sprintf(query, 
		"SELECT modificationDate from FileVersionHistory WHERE changeSetId=(SELECT changeSetId FROM Applications WHERE applicationName='%s' AND changeSetId!=0 LIMIT 1)-2 LIMIT 1;"
		, escapedApplicationName2.C_String());
	if (ExecuteBlockingCommand(query, &result2, false)==false)
	{
		sprintf(lastError,"ERROR: Query is bad in file %s at line %i in function GetMostRecentChangelistWithPatches\n",_FILE_AND_LINE_);
		PQclear(result);
		PQclear(result2);
		return false;
	}
	numRows2 = PQntuples(result2);
	if (numRows2==0)
	{
		sprintf(lastError,"ERROR: Query is bad in file %s at line %i in function GetMostRecentChangelistWithPatches\n",_FILE_AND_LINE_);
		PQclear(result);
		PQclear(result2);
		return false;
	}
	ts=PQgetvalue(result2, 0, 0);
	*priorRowPatchTime=atof(ts);
	PQclear(result2);
	

	int fileIdColumnIndex = PQfnumber(result, "fileId");
	int filenameColumnIndex = PQfnumber(result, "filename");
	int fileLengthColumnIndex = PQfnumber(result, "fileLength");
	int contentColumnIndex = PQfnumber(result, "content");
	int contentHashColumnIndex = PQfnumber(result, "contentHash");
	int patchColumnIndex = PQfnumber(result, "patch");
	int patchAlgorithmColumnIndex = PQfnumber(result, "patchAlgorithm");
	int priorHashColumnIndex = PQfnumber(result, "priorHash");
	int createFileColumnIndex = PQfnumber(result, "createFile");

	char *createFileResult;
	char *hardDriveFilename;
	char *hardDriveHash;
	char *fileData;
	char *patch;
	int rowIndex;
	int patchLength;
	char *contentHash;

	// AutopatcherPostgreRepository::GetPatches fills patchList with 
	// either PC_WRITE_FILE and filename (but no data) when the user has no hash
	// PC_WRITE_FILE with filename (but no data) when the user has a hash that does not match any patch
	// PC_HASH_1_WITH_PATCH with the filename, (HASH,patch) if the patch was found

	for (rowIndex=0; rowIndex < numRows; rowIndex++)
	{
		createFileResult = PQgetvalue(result, rowIndex, createFileColumnIndex);
		hardDriveFilename = PQgetvalue(result, rowIndex, filenameColumnIndex);
		if (createFileResult[0]==1)
		{
			hardDriveHash = PQgetvalue(result, rowIndex, contentHashColumnIndex);

			int fileId = ntohl(*((int*)PQgetvalue(result, rowIndex, fileIdColumnIndex)));
			int fileLength = ntohl(*((int*)PQgetvalue(result, rowIndex, fileLengthColumnIndex)));
			contentHash = PQgetvalue(result, rowIndex, contentHashColumnIndex);

			patchLength=PQgetlength(result, rowIndex, patchColumnIndex);
			if (patchLength==0)
			{
				// New file that never before existed

				// OK
				addedOrModifiedFileHashes->AddFile(hardDriveFilename,hardDriveFilename, contentHash, HASH_LENGTH, fileLength, FileListNodeContext(PC_WRITE_FILE,fileId,0,0),false,false);

				fileData = PQgetvalue(result, rowIndex, contentColumnIndex);
				addedFiles->AddFile(hardDriveFilename,hardDriveFilename, fileData, fileLength, fileLength, FileListNodeContext(PC_WRITE_FILE,fileId,0,0),false,false);
			}
			else
			{
				// Patch to next version
				patch = PQgetvalue(result, rowIndex, patchColumnIndex);
				// int patchAlgorithm = ntohl(*((int*)PQgetvalue(result, rowIndex, patchAlgorithmColumnIndex)));

				const char *tv = PQgetvalue(result, rowIndex, patchAlgorithmColumnIndex);
				int patchAlgorithm;
				if (tv)
					patchAlgorithm = ntohl(*((int*)tv));
				else
					patchAlgorithm = 0;

				char *temp = (char *) rakMalloc_Ex(patchLength + HASH_LENGTH*2, _FILE_AND_LINE_ );
				RakAssert(temp);
				char *priorHash = PQgetvalue(result, rowIndex, priorHashColumnIndex);
				memcpy(temp, priorHash, HASH_LENGTH);
				memcpy(temp+HASH_LENGTH, contentHash, HASH_LENGTH);
				memcpy(temp+HASH_LENGTH*2, patch, patchLength);

				// OK
				addedOrModifiedFileHashes->AddFile(hardDriveFilename,hardDriveFilename, contentHash, HASH_LENGTH, fileLength, FileListNodeContext(PC_WRITE_FILE,fileId,0,0),false,false);
				patchedFiles->AddFile(hardDriveFilename,hardDriveFilename, temp, HASH_LENGTH*2+patchLength, patchLength, FileListNodeContext(PC_HASH_2_WITH_PATCH,fileId,patchAlgorithm,0), false, true );

				// fileData = PQgetvalue(result, rowIndex, contentColumnIndex);
				// updatedFiles->AddFile(hardDriveFilename,hardDriveFilename, fileData, fileLength, fileLength, FileListNodeContext(PC_WRITE_FILE,fileId),false,false);
			}
		}
		else
		{
			// Deleted file
			deletedFiles->AddFile(hardDriveFilename,hardDriveFilename,0,0,0,FileListNodeContext(0,0,0,0), false);
		}
	}

	PQclear(result);
	return true;
}
bool AutopatcherPostgreRepository2::GetMostRecentChangelistWithPatches(RakNet::RakString &applicationName, FileList *patchedFiles, FileList *addedFiles, FileList *addedOrModifiedFileHashes, FileList *deletedFiles, double *priorRowPatchTime, double *mostRecentRowPatchTime)
{
	PGresult *result;
	char query[1024];
	if (applicationName.GetLength()>100)
		return false;

	(*priorRowPatchTime)=0;
	(*mostRecentRowPatchTime)=0;

	const char *outTemp[2];
	int outLengths[2];
	int formats[2];

	if (applicationName.GetLength()==0)
	{
		strcpy(query, 		
			"SELECT tbl1.applicationName, tbl4.* FROM  "
			"(SELECT applicationID, applicationName FROM Applications) as tbl1, "
			"(SELECT tbl2.applicationId, tbl2.fileId, tbl2.fileName, tbl2.fileLength, tbl2.contentHash, tbl2.createFile, tbl2.changeSetId, tbl2.pathToContent, tbl3.patch, tbl3.patchAlgorithm, tbl3.contentHash as priorHash FROM  "
			"(SELECT * From FileVersionHistory WHERE modificationDate=(select MAX(modificationDate) from FileVersionHistory) AND changeSetId!=0 ) as tbl2 "
			"LEFT OUTER JOIN "
			"(SELECT patch, patchAlgorithm, fileName, contentHash FROM FileVersionHistory WHERE  "
			"(changeSetId = (SELECT changeSetId FROM FileVersionHistory WHERE modificationDate=(select MAX(modificationDate) from FileVersionHistory) LIMIT 1) - 1 )) as tbl3 "
			"ON tbl2.filename=tbl3.filename) as tbl4; "
			);

		result = PQexecParams(pgConn, query,0,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);

		if (IsResultSuccessful(result, true)==false)
		{
			PQclear(result);
			return false;
		}
	}
	else
	{
		strcpy(query, 
			"SELECT tbl2.fileId, tbl2.fileName, tbl2.fileLength, tbl2.contentHash, tbl2.createFile, tbl2.changeSetId, tbl2.pathToContent, tbl3.patch, tbl3.patchAlgorithm, tbl3.contentHash as priorHash FROM  "
			"(SELECT * From FileVersionHistory WHERE changeSetId=(SELECT MAX(changeSetId) FROM Applications WHERE applicationName=$1::text)-1 AND changeSetId!=0 "
			") as tbl2 "
			"LEFT OUTER JOIN  "
			"(SELECT fileName, patch, patchAlgorithm, contentHash From FileVersionHistory WHERE changeSetId=(SELECT MAX(changeSetId) FROM Applications WHERE applicationName=$2::text)-2 "
			") as tbl3 "
			"ON tbl2.filename=tbl3.filename;"
			);

		outTemp[0]=applicationName.C_String();
		outLengths[0]=(int) applicationName.GetLength();
		formats[0]=PQEXECPARAM_FORMAT_BINARY;
		outTemp[1]=applicationName.C_String();
		outLengths[1]=(int) applicationName.GetLength();
		formats[1]=PQEXECPARAM_FORMAT_BINARY;
		result = PQexecParams(pgConn, query,2,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);

		if (IsResultSuccessful(result, true)==false)
		{
			PQclear(result);
			return false;
		}
	}

	int numRows;
	numRows = PQntuples(result);
	if (numRows==0)
	{
		// Nothing was ever patched. However, read mostRecentRowPatchTime if possible
		if (applicationName.GetLength()==0)
		{
			// Lookup application if unspecified
			PGresult *result3;

			strcpy(query, 		
				"SELECT applicationName FROM Applications as tbl1, "
				"(SELECT applicationId From FileVersionHistory WHERE modificationDate=(select MAX(modificationDate) from FileVersionHistory LIMIT 1) LIMIT 1) as tbl2 "
				"WHERE tbl1.applicationID=tbl2.applicationID;"
				);

			result3 = PQexecParams(pgConn, query,0,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);

			if (IsResultSuccessful(result3, true)==false)
			{
				PQclear(result);
				return false;
			}

			if (PQntuples(result3)==0)
			{
				// No applications at all
				PQclear(result);
				PQclear(result3);
				return false;
			}

			int applicationNameColumnIndex = PQfnumber(result, "applicationName");
			applicationName = PQgetvalue(result3, 0, applicationNameColumnIndex);
			PQclear(result3);
		}
	}
	else if (applicationName.GetLength()==0)
	{
		int applicationNameColumnIndex = PQfnumber(result, "applicationName");
		applicationName = PQgetvalue(result, 0, applicationNameColumnIndex);
	}

	RakNet::RakString escapedApplicationName2 = GetEscapedString(applicationName);
	PGresult *result2;
	char *ts;
	int numRows2;

	// For the given application, get the highest file date
	sprintf(query, 
		"SELECT modificationDate from FileVersionHistory WHERE changeSetId=(SELECT changeSetId FROM Applications WHERE applicationName='%s' AND changeSetId!=0 LIMIT 1)-1 LIMIT 1;"
		, escapedApplicationName2.C_String());
	if (ExecuteBlockingCommand(query, &result2, false)==false)
	{
		sprintf(lastError,"ERROR: Query is bad in file %s at line %i in function GetMostRecentChangelistWithPatches\n",_FILE_AND_LINE_);
		PQclear(result);
		PQclear(result2);
		return false;
	}
	numRows2 = PQntuples(result2);
	if (numRows2==0)
	{
		// No application
		PQclear(result);
		PQclear(result2);
		return false;
	}
	ts=PQgetvalue(result2, 0, 0);
	*mostRecentRowPatchTime=atof(ts);
	PQclear(result2);

	if (numRows==0)
	{
		// No patches to serve
		*priorRowPatchTime=0;

		PQclear(result);
		return true;
	}


	// In SQL, SELECT (EXTRACT(EPOCH FROM now())) is equivalent to time() function
	sprintf(query, 
		"SELECT modificationDate from FileVersionHistory WHERE changeSetId=(SELECT changeSetId FROM Applications WHERE applicationName='%s' AND changeSetId!=0 LIMIT 1)-2 LIMIT 1;"
		, escapedApplicationName2.C_String());
	if (ExecuteBlockingCommand(query, &result2, false)==false)
	{
		sprintf(lastError,"ERROR: Query is bad in file %s at line %i in function GetMostRecentChangelistWithPatches\n",_FILE_AND_LINE_);
		PQclear(result);
		PQclear(result2);
		return false;
	}
	numRows2 = PQntuples(result2);
	if (numRows2==0)
	{
		sprintf(lastError,"ERROR: Query is bad in file %s at line %i in function GetMostRecentChangelistWithPatches\n",_FILE_AND_LINE_);
		PQclear(result);
		PQclear(result2);
		return false;
	}
	ts=PQgetvalue(result2, 0, 0);
	*priorRowPatchTime=atof(ts);
	PQclear(result2);


	int fileIdColumnIndex = PQfnumber(result, "fileId");
	int filenameColumnIndex = PQfnumber(result, "filename");
	int fileLengthColumnIndex = PQfnumber(result, "fileLength");
	int pathToContentColumnIndex = PQfnumber(result, "pathToContent");
	int contentHashColumnIndex = PQfnumber(result, "contentHash");
	int patchColumnIndex = PQfnumber(result, "patch");
	int patchAlgorithmColumnIndex = PQfnumber(result, "patchAlgorithm");
	int priorHashColumnIndex = PQfnumber(result, "priorHash");
	int createFileColumnIndex = PQfnumber(result, "createFile");

	char *createFileResult;
	char *hardDriveFilename;
	char *hardDriveHash;
	//char *fileData;
	char *patch;
	int rowIndex;
	int patchLength;
	char *contentHash;

	// AutopatcherPostgreRepository::GetPatches fills patchList with 
	// either PC_WRITE_FILE and filename (but no data) when the user has no hash
	// PC_WRITE_FILE with filename (but no data) when the user has a hash that does not match any patch
	// PC_HASH_1_WITH_PATCH with the filename, (HASH,patch) if the patch was found

	for (rowIndex=0; rowIndex < numRows; rowIndex++)
	{
		createFileResult = PQgetvalue(result, rowIndex, createFileColumnIndex);
		hardDriveFilename = PQgetvalue(result, rowIndex, filenameColumnIndex);
		if (createFileResult[0]==1)
		{
			hardDriveHash = PQgetvalue(result, rowIndex, contentHashColumnIndex);

			int fileId = ntohl(*((int*)PQgetvalue(result, rowIndex, fileIdColumnIndex)));
			int fileLength = ntohl(*((int*)PQgetvalue(result, rowIndex, fileLengthColumnIndex)));
			contentHash = PQgetvalue(result, rowIndex, contentHashColumnIndex);

			patchLength=PQgetlength(result, rowIndex, patchColumnIndex);
			if (patchLength==0)
			{
				// New file that never before existed
				
				// fileData is given to AddFile
				char *pathToContent = PQgetvalue(result, rowIndex, pathToContentColumnIndex);
				FILE *fp = fopen(pathToContent, "rb");
				if (fp==0)
				{
					sprintf(lastError,"ERROR: Cannot open file %s in file %s at line %i in function GetMostRecentChangelistWithPatches\n",pathToContent, _FILE_AND_LINE_);
					PQclear(result);
					return false;
				}


				char *fileData = (char*) rakMalloc_Ex(fileLength, _FILE_AND_LINE_);
				RakAssert(fileData);
				fread(fileData, fileLength, 1, fp);
				fclose(fp);

				// OK
				addedOrModifiedFileHashes->AddFile(hardDriveFilename,hardDriveFilename, contentHash, HASH_LENGTH, fileLength, FileListNodeContext(PC_WRITE_FILE,fileId,0,0),false,false);

				// fileData = PQgetvalue(result, rowIndex, contentColumnIndex);
				// const char *pathToContent = PQgetvalue(result, rowIndex, pathToContentColumnIndex);

				// Last parameter is take the fileData pointer
				addedFiles->AddFile(hardDriveFilename,hardDriveFilename, fileData, fileLength, fileLength, FileListNodeContext(PC_WRITE_FILE,fileId,0,0),false,true);
			}
			else
			{
				// Patch to next version
				patch = PQgetvalue(result, rowIndex, patchColumnIndex);
				const char *tv = PQgetvalue(result, rowIndex, patchAlgorithmColumnIndex);
				int patchAlgorithm;
				if (tv)
					patchAlgorithm = ntohl(*((int*)tv));
				else
					patchAlgorithm = 0;

				char *temp = (char *) rakMalloc_Ex(patchLength + HASH_LENGTH*2, _FILE_AND_LINE_ );
				RakAssert(temp);
				char *priorHash = PQgetvalue(result, rowIndex, priorHashColumnIndex);
				memcpy(temp, priorHash, HASH_LENGTH);
				memcpy(temp+HASH_LENGTH, contentHash, HASH_LENGTH);
				memcpy(temp+HASH_LENGTH*2, patch, patchLength);

				// OK
				addedOrModifiedFileHashes->AddFile(hardDriveFilename,hardDriveFilename, contentHash, HASH_LENGTH, fileLength, FileListNodeContext(PC_WRITE_FILE,fileId,0,0),false,false);
				patchedFiles->AddFile(hardDriveFilename,hardDriveFilename, temp, HASH_LENGTH*2+patchLength, patchLength, FileListNodeContext(PC_HASH_2_WITH_PATCH,fileId,patchAlgorithm,0), false, true );

				// fileData = PQgetvalue(result, rowIndex, contentColumnIndex);
				// updatedFiles->AddFile(hardDriveFilename,hardDriveFilename, fileData, fileLength, fileLength, FileListNodeContext(PC_WRITE_FILE,fileId),false,false);
			}
		}
		else
		{
			// Deleted file
			deletedFiles->AddFile(hardDriveFilename,hardDriveFilename,0,0,0,FileListNodeContext(0,0,0,0), false);
		}
	}

	PQclear(result);
	return true;
}
bool AutopatcherPostgreRepository::UpdateApplicationFiles(const char *applicationName, const char *applicationDirectory, const char *userName, FileListProgress *cb)
{
	FileList filesOnHarddrive;
	filesOnHarddrive.AddCallback(cb);
	filesOnHarddrive.AddFilesFromDirectory(applicationDirectory,"", true, false, true, FileListNodeContext(0,0,0,0));
	if (filesOnHarddrive.fileList.Size()==0)
	{
		sprintf(lastError,"ERROR: Can't find files at %s in UpdateApplicationFiles\n",applicationDirectory);
		return false;
	}

	int numRows;
	PGresult *result;
	char query[512];
	if (strlen(applicationName)>100)
		return false;
	if (strlen(userName)>100)
		return false;

	RakNet::RakString escapedApplicationName = GetEscapedString(applicationName);
	sprintf(query, "SELECT applicationID FROM applications WHERE applicationName='%s';", escapedApplicationName.C_String());
	//sqlCommandMutex.Lock();
	if (ExecuteBlockingCommand(query, &result, false)==false)
	{
		//sqlCommandMutex.Unlock();
		PQclear(result);
		return false;
	}
	//sqlCommandMutex.Unlock();
	numRows = PQntuples(result);
	if (numRows==0)
	{
		sprintf(lastError,"ERROR: %s not found in UpdateApplicationFiles\n",applicationName);
		return false;
	}
	char *res;
	res = PQgetvalue(result,0,0);
	int applicationID;
	applicationID=atoi(res);
	PQclear(result);

	// If ExecuteBlockingCommand fails then it does a rollback
	//sqlCommandMutex.Lock();
	if (ExecuteBlockingCommand("BEGIN;", &result, false)==false)
	{
		//sqlCommandMutex.Unlock();
		PQclear(result);
		return false;
	}
	//sqlCommandMutex.Unlock();
	PQclear(result);

	sprintf(query, "UPDATE applications SET changeSetId = changeSetId + 1 where applicationID=%i; SELECT changeSetId FROM applications WHERE applicationID=%i;", applicationID, applicationID);
	//sqlCommandMutex.Lock();
	if (ExecuteBlockingCommand(query, &result, true)==false)
	{
		//sqlCommandMutex.Unlock();
		PQclear(result);
		return false;
	}
	//sqlCommandMutex.Unlock();
	numRows = PQntuples(result);
	if (numRows==0)
	{
		sprintf(lastError,"ERROR: applicationID %i not found in UpdateApplicationFiles\n", applicationID);
		Rollback();
		return false;
	}
	res = PQgetvalue(result,0,0);
	int changeSetId;
	changeSetId=atoi(res);
	PQclear(result);

	// +1 was added in the update
	changeSetId--;

	// Gets all newest files
	// TODO - This can be non-blocking
	sprintf(query, "SELECT DISTINCT ON (filename) filename, contentHash, createFile FROM FileVersionHistory WHERE applicationID=%i ORDER BY filename, fileId DESC;", applicationID);
	//sqlCommandMutex.Lock();
	result = PQexecParams(pgConn, query,0,0,0,0,0,PQEXECPARAM_FORMAT_BINARY);
	//sqlCommandMutex.Unlock();
	if (IsResultSuccessful(result, true)==false)
	{
		PQclear(result);
		return false;
	}

	int filenameColumnIndex = PQfnumber(result, "filename");
	int contentHashColumnIndex = PQfnumber(result, "contentHash");
	int createFileColumnIndex = PQfnumber(result, "createFile");

	//char *PQgetvalue(result,int row_number,int column_number);
	//int PQgetlength(result, int row_number, int column_number);

	unsigned fileListIndex;
	int rowIndex;
	RakNet::RakString hardDriveFilename;
	char *hardDriveHash;
	RakNet::RakString queryFilename;
	char *createFileResult;
	char *hash;
	bool addFile;
	FileList newFiles;
	numRows = PQntuples(result);
    
	// Loop through files om filesOnHarddrive
	// If the file in filesOnHarddrive does not exist in the query result, or if it does but the hash is different or non-existent, add this file to the create list
	for (fileListIndex=0; fileListIndex < filesOnHarddrive.fileList.Size(); fileListIndex++)
	{
		if (fileListIndex%10==0)
			printf("Hashing files %i/%i\n", fileListIndex+1, filesOnHarddrive.fileList.Size());
		addFile=true;
		hardDriveFilename=filesOnHarddrive.fileList[fileListIndex].filename;
		hardDriveHash=filesOnHarddrive.fileList[fileListIndex].data;
		for (rowIndex=0; rowIndex < numRows; rowIndex++ )
		{
			queryFilename = PQgetvalue(result, rowIndex, filenameColumnIndex);

			if (_stricmp(hardDriveFilename, queryFilename)==0)
			{
				createFileResult = PQgetvalue(result, rowIndex, createFileColumnIndex);
				hash = PQgetvalue(result, rowIndex, contentHashColumnIndex);
				if (createFileResult[0]==1 && memcmp(hash, hardDriveHash, HASH_LENGTH)==0)
				{
					// File exists in database and is the same
					addFile=false;
				}

				break;
			}
		}

		// Unless set to false, file does not exist in query result or is different.
		if (addFile==true)
		{
			newFiles.AddFile(hardDriveFilename,hardDriveFilename, filesOnHarddrive.fileList[fileListIndex].data, filesOnHarddrive.fileList[fileListIndex].dataLengthBytes, filesOnHarddrive.fileList[fileListIndex].fileLengthBytes, FileListNodeContext(0,0,0,0), false);
		}
	}
	
	// Go through query results that are marked as create
	// If a file that is currently on the database is not on the harddrive, add it to the delete list
	FileList deletedFiles;
	bool fileOnHarddrive;
	for (rowIndex=0; rowIndex < numRows; rowIndex++ )
	{
		queryFilename = PQgetvalue(result, rowIndex, filenameColumnIndex);
		createFileResult = PQgetvalue(result, rowIndex, createFileColumnIndex);
		if (createFileResult[0]!=1)
			continue; // If already false don't mark false again.

		fileOnHarddrive=false;
		for (fileListIndex=0; fileListIndex < filesOnHarddrive.fileList.Size(); fileListIndex++)
		{
			hardDriveFilename=filesOnHarddrive.fileList[fileListIndex].filename.C_String();
			hardDriveHash=filesOnHarddrive.fileList[fileListIndex].data;

			if (_stricmp(hardDriveFilename, queryFilename)==0)
			{
				fileOnHarddrive=true;
				break;
			}
		}

		if (fileOnHarddrive==false)
			deletedFiles.AddFile(queryFilename,queryFilename,0,0,0,FileListNodeContext(0,0,0,0), false);
	}

	// Query memory and files on harddrive no longer needed.  Free this memory since generating all the patches is memory intensive.
	PQclear(result);
	filesOnHarddrive.Clear();

	const char *outTemp[3];
	int outLengths[3];
	int formats[3];
	formats[0]=PQEXECPARAM_FORMAT_TEXT;
	formats[1]=PQEXECPARAM_FORMAT_BINARY; // Always happens to be binary
	formats[2]=PQEXECPARAM_FORMAT_BINARY; // Always happens to be binary

	// For each file in the delete list add a row indicating file deletion
	for (fileListIndex=0; fileListIndex < deletedFiles.fileList.Size(); fileListIndex++)
	{
		if (fileListIndex%10==0)
			printf("Tagging deleted files %i/%i\n", fileListIndex+1, deletedFiles.fileList.Size());

		// BUGGED
		sprintf(query, "INSERT INTO FileVersionHistory(applicationID, filename, createFile, changeSetID, userName) VALUES (%i, $1::text,FALSE,%i,'%s');", applicationID, changeSetId, GetEscapedString(userName).C_String());
		outTemp[0]=deletedFiles.fileList[fileListIndex].filename.C_String();
		outLengths[0]=(int)deletedFiles.fileList[fileListIndex].filename.GetLength();
		formats[0]=PQEXECPARAM_FORMAT_TEXT;
		//sqlCommandMutex.Lock();
		result = PQexecParams(pgConn, query,1,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);
		//sqlCommandMutex.Unlock();
		if (IsResultSuccessful(result, true)==false)
		{
			deletedFiles.Clear();
			newFiles.Clear();
			PQclear(result);
			return false;
		}
		PQclear(result);
	}
	
	// Clear the delete list as it is no longer needed.
	deletedFiles.Clear();

	int contentColumnIndex;		

	PGresult *uploadResult;
	PGresult *fileRows;
	char *content;
	char *fileID;
	int contentLength;
	int fileIDLength;
	int fileIDColumnIndex;
	char *hardDriveData;
	unsigned hardDriveDataLength;
	char *patch;
	unsigned patchLength;	

	// For each file in the create list
	for (fileListIndex=0; fileListIndex < newFiles.fileList.Size(); fileListIndex++)
	{
		if (fileListIndex%10==0)
			printf("Adding file %i/%i\n", fileListIndex+1, newFiles.fileList.Size());
		hardDriveFilename=newFiles.fileList[fileListIndex].filename;
//		hardDriveData=newFiles.fileList[fileListIndex].data+HASH_LENGTH;
		hardDriveHash=newFiles.fileList[fileListIndex].data;
		hardDriveDataLength=newFiles.fileList[fileListIndex].fileLengthBytes;

		char path[MAX_PATH];
		strcpy(path, applicationDirectory);
		strcat(path, "/");
		strcat(path, newFiles.fileList[fileListIndex].fullPathToFile);

		FILE *fp = fopen(path, "rb");
		if (fp==0)
		{
			newFiles.Clear();
			PQclear(fileRows);
			RakAssert(0);
			return false;
		}
		hardDriveData =(char*) rakMalloc_Ex( hardDriveDataLength, _FILE_AND_LINE_ );
		RakAssert(hardDriveData);
		fread(hardDriveData,1,hardDriveDataLength,fp);
		fclose(fp);

		sprintf( query, "SELECT fileID from FileVersionHistory WHERE applicationID=%i AND filename=$1::text AND createFile=TRUE;", applicationID );
		outTemp[0]=hardDriveFilename;
		outLengths[0]=(int)strlen(hardDriveFilename);
		formats[0]=PQEXECPARAM_FORMAT_TEXT;
		//sqlCommandMutex.Lock();
		fileRows = PQexecParams(pgConn, query,1,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_TEXT);
		//sqlCommandMutex.Unlock();
		if (IsResultSuccessful(fileRows, true)==false)
		{
			rakFree_Ex(hardDriveData, _FILE_AND_LINE_);
			newFiles.Clear();
			PQclear(fileRows);
			RakAssert(0);
			return false;
		}
		
		fileIDColumnIndex = PQfnumber( fileRows, "fileID" );
		numRows = PQntuples(fileRows);
		outTemp[0]=hardDriveFilename;
		outLengths[0]=(int)strlen(hardDriveFilename);

		// Create new patches for every create version
		for (rowIndex=0; rowIndex < numRows; rowIndex++ )
		{
			fileIDLength=PQgetlength(fileRows, rowIndex, fileIDColumnIndex);			
			fileID=PQgetvalue(fileRows, rowIndex, fileIDColumnIndex);
			
			formats[0]=PQEXECPARAM_FORMAT_TEXT;
			outTemp[0]=fileID;
			outLengths[0]=fileIDLength;
			
			// The last query handled all the relevant comparisons
			sprintf(query, "SELECT content from FileVersionHistory WHERE fileID=$1::int;" );
			//sqlCommandMutex.Lock();
			result = PQexecParams(pgConn, query,1,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);
			//sqlCommandMutex.Unlock();
			if (IsResultSuccessful(result, true)==false)
			{
				rakFree_Ex(hardDriveData, _FILE_AND_LINE_);
				Rollback();
				newFiles.Clear();
				PQclear(result);
				return false;
			}
			int numContent = PQntuples(result);
			if( numContent > 1 || numContent == 0 )
			{
				rakFree_Ex(hardDriveData, _FILE_AND_LINE_);
				Rollback();
				newFiles.Clear();
				PQclear(result);
				PQclear(fileRows);
				RakAssert(0);
				return false;
			}
			formats[0] = PQEXECPARAM_FORMAT_TEXT;

			contentColumnIndex = PQfnumber(result, "content");
			contentLength=PQgetlength(result, 0, contentColumnIndex);
			content=PQgetvalue(result, 0, contentColumnIndex);


			if (CreatePatch(content, contentLength, hardDriveData, hardDriveDataLength, &patch, &patchLength)==false)
			{
				rakFree_Ex(hardDriveData, _FILE_AND_LINE_);

				strcpy(lastError,"CreatePatch failed.\n");
				Rollback();

				newFiles.Clear();
				PQclear(result);
				PQclear(fileRows);
				return false;
			}
			
// 			outTemp[0]=fileID;
// 			outLengths[0]=fileIDLength;
// 			outTemp[1]=patch;
// 			outLengths[1]=patchLength;

			outTemp[0]=patch;
			outLengths[0]=patchLength;
			formats[0]=PQEXECPARAM_FORMAT_BINARY;
			
			//sqlCommandMutex.Lock();
			char buff[256];
			sprintf(buff, "UPDATE FileVersionHistory SET patch=$1::bytea, patchAlgorithm=0 where fileID=%s;", fileID);
			uploadResult = PQexecParams(pgConn, buff, 1,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);
			
			// Done with this patch data
			delete [] patch;

			//sqlCommandMutex.Unlock();
			if (IsResultSuccessful(uploadResult, true)==false)
			{
				rakFree_Ex(hardDriveData, _FILE_AND_LINE_);
				Rollback();
				newFiles.Clear();
				PQclear(result);
				return false;
			}

			

			PQclear(result);
			PQclear(uploadResult);
		}
		PQclear(fileRows);

		// Add totally new files
		sprintf(query, "INSERT INTO FileVersionHistory (applicationID, filename, fileLength, content, contentHash, createFile, changeSetID, userName) "
			"VALUES ("
			"%i,"
			"$1::text,"
			"%i,"
			"$2::bytea,"
			"$3::bytea,"
			"TRUE,"
			"%i,"
			"'%s'"
			");", applicationID, hardDriveDataLength, changeSetId, GetEscapedString(userName).C_String());

		outTemp[0]=hardDriveFilename;
		outTemp[1]=hardDriveData;
		outTemp[2]=hardDriveHash;
		outLengths[0]=(int)strlen(hardDriveFilename);
		outLengths[1]=hardDriveDataLength;
		outLengths[2]=HASH_LENGTH;
		formats[0]=PQEXECPARAM_FORMAT_TEXT;
		RakAssert(formats[1]==PQEXECPARAM_FORMAT_BINARY);
		RakAssert(formats[2]==PQEXECPARAM_FORMAT_BINARY);
		
		// Upload the new file
		//sqlCommandMutex.Lock();
		uploadResult = PQexecParams(pgConn, query,3,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);

		rakFree_Ex(hardDriveData, _FILE_AND_LINE_);

		//sqlCommandMutex.Unlock();
		if( !uploadResult )
		{
			// Libpq had a problem inserting the file to the table. Most likely due to it running out of
			// memory or a buffer being too small.
			RakAssert( 0 );
			newFiles.Clear();
			PQclear(result);
			return false;
		}
		
		if (IsResultSuccessful(uploadResult, true)==false)
		{
			newFiles.Clear();
			PQclear(uploadResult);
			PQclear(result);
			return false;
		}

		// Clear the upload result
		PQclear(uploadResult);
	}
	
	printf("DB COMMIT\n");

	// If ExecuteBlockingCommand fails then it does a rollback
	//sqlCommandMutex.Lock();
	if (ExecuteBlockingCommand("COMMIT;", &result, true)==false)
	{
		//sqlCommandMutex.Unlock();
		printf("COMMIT Failed!\n");
		PQclear(result);
		return false;
	}
	//sqlCommandMutex.Unlock();
	PQclear(result);

	printf("COMMIT Success\n");

	return true;
}
bool AutopatcherPostgreRepository2::UpdateApplicationFiles(const char *applicationName, const char *applicationDirectory, const char *userName, FileListProgress *cb)
{
	FileList filesOnHarddrive;
	filesOnHarddrive.AddCallback(cb);
	filesOnHarddrive.AddFilesFromDirectory(applicationDirectory,"", true, false, true, FileListNodeContext(0,0,0,0));
	if (filesOnHarddrive.fileList.Size()==0)
	{
		sprintf(lastError,"ERROR: Can't find files at %s in UpdateApplicationFiles\n",applicationDirectory);
		return false;
	}

	int numRows;
	PGresult *result;
	char query[512];
	if (strlen(applicationName)>100)
		return false;
	if (strlen(userName)>100)
		return false;

	RakNet::RakString escapedApplicationName = GetEscapedString(applicationName);
	sprintf(query, "SELECT applicationID FROM applications WHERE applicationName='%s';", escapedApplicationName.C_String());
	//sqlCommandMutex.Lock();
	if (ExecuteBlockingCommand(query, &result, false)==false)
	{
		//sqlCommandMutex.Unlock();
		PQclear(result);
		return false;
	}
	//sqlCommandMutex.Unlock();
	numRows = PQntuples(result);
	if (numRows==0)
	{
		sprintf(lastError,"ERROR: %s not found in UpdateApplicationFiles\n",applicationName);
		return false;
	}
	char *res;
	res = PQgetvalue(result,0,0);
	int applicationID;
	applicationID=atoi(res);
	PQclear(result);

	// If ExecuteBlockingCommand fails then it does a rollback
	//sqlCommandMutex.Lock();
	if (ExecuteBlockingCommand("BEGIN;", &result, false)==false)
	{
		//sqlCommandMutex.Unlock();
		PQclear(result);
		return false;
	}
	//sqlCommandMutex.Unlock();
	PQclear(result);

	sprintf(query, "UPDATE applications SET changeSetId = changeSetId + 1 where applicationID=%i; SELECT changeSetId FROM applications WHERE applicationID=%i;", applicationID, applicationID);
	//sqlCommandMutex.Lock();
	if (ExecuteBlockingCommand(query, &result, true)==false)
	{
		//sqlCommandMutex.Unlock();
		PQclear(result);
		return false;
	}
	//sqlCommandMutex.Unlock();
	numRows = PQntuples(result);
	if (numRows==0)
	{
		sprintf(lastError,"ERROR: applicationID %i not found in UpdateApplicationFiles\n", applicationID);
		Rollback();
		return false;
	}
	res = PQgetvalue(result,0,0);
	int changeSetId;
	changeSetId=atoi(res);
	PQclear(result);

	// +1 was added in the update
	changeSetId--;

	// Gets all newest files
	// TODO - This can be non-blocking
	sprintf(query, "SELECT DISTINCT ON (filename) filename, contentHash, createFile FROM FileVersionHistory WHERE applicationID=%i ORDER BY filename, fileId DESC;", applicationID);
	//sqlCommandMutex.Lock();
	result = PQexecParams(pgConn, query,0,0,0,0,0,PQEXECPARAM_FORMAT_BINARY);
	//sqlCommandMutex.Unlock();
	if (IsResultSuccessful(result, true)==false)
	{
		PQclear(result);
		return false;
	}

	int filenameColumnIndex = PQfnumber(result, "filename");
	int contentHashColumnIndex = PQfnumber(result, "contentHash");
	int createFileColumnIndex = PQfnumber(result, "createFile");

	//char *PQgetvalue(result,int row_number,int column_number);
	//int PQgetlength(result, int row_number, int column_number);

	unsigned fileListIndex;
	int rowIndex;
	RakNet::RakString hardDriveFilename;
	char *hardDriveHash;
	RakNet::RakString queryFilename;
	char *createFileResult;
	char *hash;
	bool addFile;
	FileList newFiles;
	numRows = PQntuples(result);

	// Loop through files om filesOnHarddrive
	// If the file in filesOnHarddrive does not exist in the query result, or if it does but the hash is different or non-existent, add this file to the create list
	for (fileListIndex=0; fileListIndex < filesOnHarddrive.fileList.Size(); fileListIndex++)
	{
		if (fileListIndex%10==0)
			printf("Hashing files %i/%i\n", fileListIndex+1, filesOnHarddrive.fileList.Size());
		addFile=true;
		hardDriveFilename=filesOnHarddrive.fileList[fileListIndex].filename;
		hardDriveHash=filesOnHarddrive.fileList[fileListIndex].data;
		for (rowIndex=0; rowIndex < numRows; rowIndex++ )
		{
			queryFilename = PQgetvalue(result, rowIndex, filenameColumnIndex);

			if (_stricmp(hardDriveFilename, queryFilename)==0)
			{
				createFileResult = PQgetvalue(result, rowIndex, createFileColumnIndex);
				hash = PQgetvalue(result, rowIndex, contentHashColumnIndex);
				if (createFileResult[0]==1 && memcmp(hash, hardDriveHash, HASH_LENGTH)==0)
				{
					// File exists in database and is the same
					addFile=false;
				}

				break;
			}
		}

		// Unless set to false, file does not exist in query result or is different.
		if (addFile==true)
		{
			newFiles.AddFile(hardDriveFilename,hardDriveFilename, filesOnHarddrive.fileList[fileListIndex].data, filesOnHarddrive.fileList[fileListIndex].dataLengthBytes, filesOnHarddrive.fileList[fileListIndex].fileLengthBytes, FileListNodeContext(0,0,0,0), false);
		}
	}

	// Go through query results that are marked as create
	// If a file that is currently on the database is not on the harddrive, add it to the delete list
	FileList deletedFiles;
	bool fileOnHarddrive;
	for (rowIndex=0; rowIndex < numRows; rowIndex++ )
	{
		queryFilename = PQgetvalue(result, rowIndex, filenameColumnIndex);
		createFileResult = PQgetvalue(result, rowIndex, createFileColumnIndex);
		if (createFileResult[0]!=1)
			continue; // If already false don't mark false again.

		fileOnHarddrive=false;
		for (fileListIndex=0; fileListIndex < filesOnHarddrive.fileList.Size(); fileListIndex++)
		{
			hardDriveFilename=filesOnHarddrive.fileList[fileListIndex].filename.C_String();
			hardDriveHash=filesOnHarddrive.fileList[fileListIndex].data;

			if (_stricmp(hardDriveFilename, queryFilename)==0)
			{
				fileOnHarddrive=true;
				break;
			}
		}

		if (fileOnHarddrive==false)
			deletedFiles.AddFile(queryFilename,queryFilename,0,0,0,FileListNodeContext(0,0,0,0), false);
	}

	// Query memory and files on harddrive no longer needed.  Free this memory since generating all the patches is memory intensive.
	PQclear(result);
	filesOnHarddrive.Clear();

	const char *outTemp[3];
	int outLengths[3];
	int formats[3];
	formats[0]=PQEXECPARAM_FORMAT_TEXT;
	formats[1]=PQEXECPARAM_FORMAT_BINARY; // Always happens to be binary
	formats[2]=PQEXECPARAM_FORMAT_BINARY; // Always happens to be binary

	// For each file in the delete list add a row indicating file deletion
	for (fileListIndex=0; fileListIndex < deletedFiles.fileList.Size(); fileListIndex++)
	{
		if (fileListIndex%10==0)
			printf("Tagging deleted files %i/%i\n", fileListIndex+1, deletedFiles.fileList.Size());

		// BUGGED
		sprintf(query, "INSERT INTO FileVersionHistory(applicationID, filename, createFile, changeSetID, userName) VALUES (%i, $1::text,FALSE,%i,'%s');", applicationID, changeSetId, GetEscapedString(userName).C_String());
		outTemp[0]=deletedFiles.fileList[fileListIndex].filename.C_String();
		outLengths[0]=(int)deletedFiles.fileList[fileListIndex].filename.GetLength();
		formats[0]=PQEXECPARAM_FORMAT_TEXT;
		//sqlCommandMutex.Lock();
		result = PQexecParams(pgConn, query,1,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);
		//sqlCommandMutex.Unlock();
		if (IsResultSuccessful(result, true)==false)
		{
			deletedFiles.Clear();
			newFiles.Clear();
			PQclear(result);
			return false;
		}
		PQclear(result);
	}

	// Clear the delete list as it is no longer needed.
	deletedFiles.Clear();

	int contentColumnIndex;		

	PGresult *uploadResult;
	PGresult *fileRows;
	char *fileID;
	//int contentLength;
	int fileIDLength;
	int fileIDColumnIndex;
//	char *newContent;
	unsigned hardDriveDataLength;
	char *patch;
	unsigned patchLength;	

	// For each file in the create list
	for (fileListIndex=0; fileListIndex < newFiles.fileList.Size(); fileListIndex++)
	{
		if (fileListIndex%10==0)
			printf("Adding file %i/%i\n", fileListIndex+1, newFiles.fileList.Size());
		hardDriveFilename=newFiles.fileList[fileListIndex].filename;
		//		hardDriveData=newFiles.fileList[fileListIndex].data+HASH_LENGTH;
		hardDriveHash=newFiles.fileList[fileListIndex].data;
		hardDriveDataLength=newFiles.fileList[fileListIndex].fileLengthBytes;

		char pathToNewContent[MAX_PATH];
		strcpy(pathToNewContent, applicationDirectory);
		strcat(pathToNewContent, "/");
		strcat(pathToNewContent, newFiles.fileList[fileListIndex].fullPathToFile);


		sprintf( query, "SELECT fileID from FileVersionHistory WHERE applicationID=%i AND filename=$1::text AND createFile=TRUE;", applicationID );
		outTemp[0]=hardDriveFilename;
		outLengths[0]=(int)strlen(hardDriveFilename);
		formats[0]=PQEXECPARAM_FORMAT_TEXT;
		//sqlCommandMutex.Lock();
		fileRows = PQexecParams(pgConn, query,1,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_TEXT);
		//sqlCommandMutex.Unlock();
		if (IsResultSuccessful(fileRows, true)==false)
		{
			newFiles.Clear();
			PQclear(fileRows);
			RakAssert(0);
			return false;
		}

		fileIDColumnIndex = PQfnumber( fileRows, "fileID" );
		numRows = PQntuples(fileRows);
		outTemp[0]=hardDriveFilename;
		outLengths[0]=(int)strlen(hardDriveFilename);

		/*
		FILE *fp = fopen(pathToNewContent, "rb");
		if (fp==0)
		{
			newFiles.Clear();
			PQclear(fileRows);
			RakAssert(0);
			return false;
		}
		newContent =(char*) rakMalloc_Ex( hardDriveDataLength, _FILE_AND_LINE_ );
		fread(newContent,1,hardDriveDataLength,fp);
		fclose(fp);
		*/

		// Create new patches for every create version
		for (rowIndex=0; rowIndex < numRows; rowIndex++ )
		{
			fileIDLength=PQgetlength(fileRows, rowIndex, fileIDColumnIndex);			
			fileID=PQgetvalue(fileRows, rowIndex, fileIDColumnIndex);

			formats[0]=PQEXECPARAM_FORMAT_TEXT;
			outTemp[0]=fileID;
			outLengths[0]=fileIDLength;

			// The last query handled all the relevant comparisons
			sprintf(query, "SELECT pathToContent from FileVersionHistory WHERE fileID=$1::int;" );
			//sqlCommandMutex.Lock();
			result = PQexecParams(pgConn, query,1,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);
			//sqlCommandMutex.Unlock();
			if (IsResultSuccessful(result, true)==false)
			{
//				rakFree_Ex(newContent, _FILE_AND_LINE_);
				Rollback();
				newFiles.Clear();
				PQclear(result);
				return false;
			}
			int numContent = PQntuples(result);
			if( numContent > 1 || numContent == 0 )
			{
			//	rakFree_Ex(newContent, _FILE_AND_LINE_);
				Rollback();
				newFiles.Clear();
				PQclear(result);
				PQclear(fileRows);
				RakAssert(0);
				return false;
			}
			formats[0] = PQEXECPARAM_FORMAT_TEXT;

			contentColumnIndex = PQfnumber(result, "pathToContent");

			// contentLength=PQgetlength(result, 0, contentColumnIndex);
			char *pathToOldContent;
			pathToOldContent=PQgetvalue(result, 0, contentColumnIndex);

			if (strcmp(pathToNewContent, pathToOldContent)==0)
			{
//				rakFree_Ex(newContent, _FILE_AND_LINE_);

				sprintf(lastError,"New file version cannot have the same path as the old file version. Path=%s.\n", pathToNewContent);
				Rollback();

				newFiles.Clear();
				PQclear(result);
				PQclear(fileRows);
				return false;
			}

			printf("%i/%i.%i/%i DIFF from %s to %s ...", fileListIndex+1, newFiles.fileList.Size(), rowIndex+1, numRows, pathToOldContent, pathToNewContent);
			int patchAlgorithm;
			int makePatchResult=MakePatch(pathToOldContent, pathToNewContent, &patch, &patchLength, &patchAlgorithm);
			if (makePatchResult < 0 || makePatchResult == 2)
			{
				strcpy(lastError,"MakePatch failed.\n");
				Rollback();

				newFiles.Clear();
				PQclear(result);
				PQclear(fileRows);
				return false;
			}
			if (makePatchResult ==1)
			{
				PQclear(result);
				strcpy(lastError,"MakePatch skipped - No old version on disk.\n");
			}
			else
			{
				
				/*
				if (CreatePatch(oldContent, contentLength, newContent, hardDriveDataLength, &patch, &patchLength)==false)
				{
					rakFree_Ex(oldContent, _FILE_AND_LINE_);
					rakFree_Ex(newContent, _FILE_AND_LINE_);

					strcpy(lastError,"CreatePatch failed.\n");
					Rollback();

					newFiles.Clear();
					PQclear(result);
					PQclear(fileRows);
					return false;
				}
				*/

	//			rakFree_Ex(oldContent, _FILE_AND_LINE_);
		//		oldContent=0;

	// 			outTemp[0]=fileID;
	// 			outLengths[0]=fileIDLength;
	// 			outTemp[1]=patch;
	// 			outLengths[1]=patchLength;

				outTemp[0]=patch;
				outLengths[0]=patchLength;

				//sqlCommandMutex.Lock();
				formats[0]=PQEXECPARAM_FORMAT_BINARY;
				char buff[256];
				sprintf(buff, "UPDATE FileVersionHistory SET patch=$1::bytea, patchAlgorithm=%i where fileID=%s;", patchAlgorithm, fileID);
				uploadResult = PQexecParams(pgConn, buff, 1,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);

				// Done with this patch data
				delete [] patch;

				//sqlCommandMutex.Unlock();
				if (IsResultSuccessful(uploadResult, true)==false)
				{
	//				rakFree_Ex(newContent, _FILE_AND_LINE_);
					Rollback();
					newFiles.Clear();
					PQclear(result);
					return false;
				}

				PQclear(result);
				PQclear(uploadResult);

				printf(" Done.\n");
			}

		}
		PQclear(fileRows);

//		rakFree_Ex(newContent, _FILE_AND_LINE_);
	//	newContent=0;

		// Add totally new files
		sprintf(query, "INSERT INTO FileVersionHistory (applicationID, filename, fileLength, pathToContent, contentHash, createFile, changeSetID, userName) "
			"VALUES ("
			"%i,"
			"$1::text,"
			"%i,"
			"$2::text,"
			"$3::bytea,"
			"TRUE,"
			"%i,"
			"'%s'"
			");", applicationID, hardDriveDataLength, changeSetId, GetEscapedString(userName).C_String());

		outTemp[0]=hardDriveFilename;
		outTemp[1]=pathToNewContent;
		outTemp[2]=hardDriveHash;
		outLengths[0]=(int)strlen(hardDriveFilename);
		outLengths[1]=(int)strlen(pathToNewContent);
		outLengths[2]=HASH_LENGTH;
		formats[0]=PQEXECPARAM_FORMAT_TEXT;
		formats[1]=PQEXECPARAM_FORMAT_TEXT;
		RakAssert(formats[2]==PQEXECPARAM_FORMAT_BINARY);

		// Upload the new file
		//sqlCommandMutex.Lock();
		uploadResult = PQexecParams(pgConn, query,3,0,outTemp,outLengths,formats,PQEXECPARAM_FORMAT_BINARY);

		//sqlCommandMutex.Unlock();
		if( !uploadResult )
		{
			// Libpq had a problem inserting the file to the table. Most likely due to it running out of
			// memory or a buffer being too small.
			RakAssert( 0 );
			newFiles.Clear();
			PQclear(result);
			return false;
		}

		if (IsResultSuccessful(uploadResult, true)==false)
		{
			newFiles.Clear();
			PQclear(uploadResult);
			PQclear(result);
			return false;
		}

		// Clear the upload result
		PQclear(uploadResult);
	}

	printf("DB COMMIT\n");

	// If ExecuteBlockingCommand fails then it does a rollback
	//sqlCommandMutex.Lock();
	if (ExecuteBlockingCommand("COMMIT;", &result, true)==false)
	{
		//sqlCommandMutex.Unlock();
		printf("COMMIT Failed!\n");
		PQclear(result);
		return false;
	}
	//sqlCommandMutex.Unlock();
	PQclear(result);

	printf("COMMIT Success\n");

	return true;
}
int AutopatcherPostgreRepository2::MakePatch(const char *oldFile, const char *newFile, char **patch, unsigned int *patchLength, int *patchAlgorithm)
{
	*patchAlgorithm=0;

	FILE *fpOld = fopen(oldFile, "rb");
	if (fpOld==0)
		return 1;
	fseek(fpOld, 0, SEEK_END);
	int contentLengthOld = ftell(fpOld);
	FILE *fpNew = fopen(newFile, "rb");
	if (fpNew==0)
	{
		fclose(fpOld);
		return 2;
	}
	fseek(fpNew, 0, SEEK_END);
	int contentLengthNew = ftell(fpNew);

	bool b = MakePatchBSDiff(fpOld, contentLengthOld, fpNew, contentLengthNew, patch, patchLength);
	fclose(fpOld);
	fclose(fpNew);
	if (b==false)
		return -1;
	return 0;
}
bool AutopatcherPostgreRepository2::MakePatchBSDiff(FILE *fpOld, int contentLengthOld, FILE *fpNew, int contentLengthNew, char **patch, unsigned int *patchLength)
{
	char *newContent, *oldContent;

	oldContent = (char*) rakMalloc_Ex(contentLengthOld, _FILE_AND_LINE_);
	RakAssert(oldContent);
	fseek(fpOld, 0, SEEK_SET);
	fread(oldContent, contentLengthOld, 1, fpOld);

	newContent = (char*) rakMalloc_Ex(contentLengthNew, _FILE_AND_LINE_);
	RakAssert(newContent);
	fseek(fpNew, 0, SEEK_SET);
	fread(newContent, contentLengthNew, 1, fpNew);

	bool b = CreatePatch(oldContent, contentLengthOld, newContent, contentLengthNew, patch, patchLength);

	if (b==false)
	{
		printf(
			"AutopatcherPostgreRepository2::MakePatchBSDiff failed.\n"
			"contentLengthOld=%i\n"
			"contentLengthNew=%i\n",
			contentLengthOld, contentLengthNew
			);
	}

	rakFree_Ex(oldContent, _FILE_AND_LINE_);
	rakFree_Ex(newContent, _FILE_AND_LINE_);
	return b;
}
const char *AutopatcherPostgreRepository::GetLastError(void) const
{
	return PostgreSQLInterface::GetLastError();
}
unsigned int AutopatcherPostgreRepository::GetPatchPart( const char *filename, unsigned int startReadBytes, unsigned int numBytesToRead, void *preallocatedDestination, FileListNodeContext context)
{
	PGresult *result;
	char query[512];
	char *patch, *contentHash;
	int patchLength;

	bool firstBlock=startReadBytes==0;
	if (firstBlock)
		sprintf(query, "SELECT substring(patch from %i for %i) FROM FileVersionHistory WHERE fileId=%i;", startReadBytes+1,numBytesToRead-HASH_LENGTH,context.flnc_extraData3);
	else
		sprintf(query, "SELECT substring(patch from %i for %i) FROM FileVersionHistory WHERE fileId=%i;", startReadBytes+1-HASH_LENGTH,numBytesToRead,context.flnc_extraData3);


	// CREATE NEW CONNECTION JUST FOR THIS QUERY
	// This is because the autopatcher is sharing this class, but this is called from multiple threads and mysql is not threadsafe
	filePartConnectionMutex.Lock();
	if (filePartConnection==0)
		filePartConnection=PQconnectdb(_conninfo);

	result = PQexecParams(filePartConnection, query,0,0,0,0,0,PQEXECPARAM_FORMAT_BINARY);

	if (IsResultSuccessful(result, true)==false)
	{
		PQclear(result);
		return -1;
	}

	int patchColumnIndex = PQfnumber(result, "substring");

	patch = PQgetvalue(result, 0, patchColumnIndex);
	patchLength = PQgetlength(result, 0, patchColumnIndex);

	if (firstBlock)
	{
		PGresult *result2;
		char query2[512];
		sprintf(query2, "SELECT contentHash FROM FileVersionHistory WHERE fileId=%i;", context.flnc_extraData1);

		result2 = PQexecParams(filePartConnection, query2,0,0,0,0,0,PQEXECPARAM_FORMAT_BINARY);

		int contentHashColumnIndex = PQfnumber(result2, "contentHash");
		contentHash = PQgetvalue(result2, 0, contentHashColumnIndex);

		memcpy(preallocatedDestination, contentHash, HASH_LENGTH);
		memcpy((char*) preallocatedDestination+HASH_LENGTH, patch, patchLength);
		PQclear(result);
		PQclear(result2);
		filePartConnectionMutex.Unlock();
		return patchLength+HASH_LENGTH;
	}
	else
	{
		memcpy(preallocatedDestination,patch,patchLength);
		PQclear(result);
		filePartConnectionMutex.Unlock();
		return patchLength;
	}
}
unsigned int AutopatcherPostgreRepository::GetFilePart( const char *filename, unsigned int startReadBytes, unsigned int numBytesToRead, void *preallocatedDestination, FileListNodeContext context)
{
	if (context.op==PC_HASH_1_WITH_PATCH)
	{
		return GetPatchPart(filename, startReadBytes, numBytesToRead, preallocatedDestination, context);
	}
	else
	{
		PGresult *result;
		char query[512];
		char *content;
		int contentLength;

		// Seems that substring is 1 based for its index, so add 1 to startReadBytes
		sprintf(query, "SELECT substring(content from %i for %i) FROM FileVersionHistory WHERE fileId=%i;", startReadBytes+1,numBytesToRead,context.flnc_extraData1);

		// CREATE NEW CONNECTION JUST FOR THIS QUERY
		// This is because the autopatcher is sharing this class, but this is called from multiple threads and mysql is not threadsafe
		filePartConnectionMutex.Lock();
		if (filePartConnection==0)
			filePartConnection=PQconnectdb(_conninfo);

		result = PQexecParams(filePartConnection, query,0,0,0,0,0,PQEXECPARAM_FORMAT_BINARY);

		content = PQgetvalue(result, 0, 0);
		contentLength=PQgetlength(result, 0, 0);
		memcpy(preallocatedDestination,content,contentLength);
		PQclear(result);

		filePartConnectionMutex.Unlock();


		return contentLength;

	}
}
unsigned int AutopatcherPostgreRepository2::GetFilePart( const char *filename, unsigned int startReadBytes, unsigned int numBytesToRead, void *preallocatedDestination, FileListNodeContext context)
{
	if (context.op==PC_HASH_1_WITH_PATCH)
	{
		return GetPatchPart(filename, startReadBytes, numBytesToRead, preallocatedDestination, context);
	}
	else
	{
		PGresult *result;
		char query[512];
		unsigned int bytesRead;

		RakAssert(context.op==PC_WRITE_FILE)

		sprintf(query, "SELECT pathToContent FROM FileVersionHistory WHERE fileId=%i;", context.flnc_extraData1);
		bool res = ExecuteBlockingCommand(query, &result, true);
		char *path;
		path = PQgetvalue(result,0,0);

		FILE *fp = fopen(path, "rb");
		fseek(fp, 0, SEEK_END);
		unsigned int fileLength = ftell(fp);
		fseek(fp, startReadBytes, SEEK_SET);
		if (startReadBytes+numBytesToRead>fileLength)
			bytesRead=fileLength-startReadBytes;
		else
			bytesRead=numBytesToRead;

		bytesRead=(unsigned int) fread(preallocatedDestination,1,bytesRead,fp);
		fclose(fp);
		PQclear(result);


		return bytesRead;
	}
}
const int AutopatcherPostgreRepository::GetIncrementalReadChunkSize(void) const
{
	return 262144*4*16;
}
