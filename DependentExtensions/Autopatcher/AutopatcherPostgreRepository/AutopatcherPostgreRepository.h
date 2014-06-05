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


#define _USE_POSTGRE_REPOSITORY
#ifdef _USE_POSTGRE_REPOSITORY

#include "AutopatcherRepositoryInterface.h"
#include "PostgreSQLInterface.h"
#include "Export.h"

struct pg_conn;
typedef struct pg_conn PGconn;

struct pg_result;
typedef struct pg_result PGresult;


#ifndef __POSTGRE_REPOSITORY_H
#define __POSTGRE_REPOSITORY_H

namespace RakNet
{
class FileListProgress;

/// \ingroup Autopatcher
///  An implementation of the AutopatcherRepositoryInterface to use PostgreSQL to store the relevant data
class RAK_DLL_EXPORT AutopatcherPostgreRepository : public AutopatcherRepositoryInterface, public PostgreSQLInterface
{
public:
	AutopatcherPostgreRepository();
	virtual ~AutopatcherPostgreRepository();


	/// Create the tables used by the autopatcher, for all applications.  Call this first.
	/// \return True on success, false on failure.
	virtual bool CreateAutopatcherTables(void);

	/// Destroy the tables used by the autopatcher.  Don't call this unless you don't want to use the autopatcher anymore, or are testing.
	/// \return True on success, false on failure.
	bool DestroyAutopatcherTables(void);

	/// Add an application for use by files.  Call this second.
	/// \param[in] applicationName A null terminated string.
	/// \param[in] userName Stored in the database, but otherwise unused.  Useful to track who added this application.
	/// \return True on success, false on failure.
	bool AddApplication(const char *applicationName, const char *userName);

	/// Remove an application and files used by that application.
	/// \param[in] applicationName A null terminated string previously passed to AddApplication
	/// \return True on success, false on failure.
	bool RemoveApplication(const char *applicationName);

	/// Update all the files for an application to match what is at the specified directory.  Call this third.
	/// Be careful not to call this with the wrong directory.
	/// This is implemented in a Begin and Rollback block so you won't a messed up database from get partial updates.
	/// \note It takes 10 bytes of memory to create a patch per byte on disk for a file. So you should not have any files larger than 1/10th your server memory.
	/// \param[in] applicationName A null terminated string previously passed to AddApplication
	/// \param[in] applicationDirectory The base directory of your application.  All files in this directory and subdirectories are added.
	/// \param[in] userName Stored in the database, but otherwise unused.  Useful to track who added this revision
	/// \param[in] cb Callback to get progress updates. Pass 0 to not use.
	/// \return True on success, false on failure.
	virtual bool UpdateApplicationFiles(const char *applicationName, const char *applicationDirectory, const char *userName, FileListProgress *cb);

	/// Get list of files added and deleted since a certain date.  This is used by AutopatcherServer and not usually explicitly called.
	/// \param[in] applicationName A null terminated string previously passed to AddApplication
	/// \param[out] addedFiles A list of the current versions of filenames with SHA1_LENGTH byte hashes as their data that were created after \a sinceData
	/// \param[out] deletedFiles A list of the current versions of filenames that were deleted after \a sinceData
	/// \param[in] sinceDate
	/// \return True on success, false on failure.
	virtual bool GetChangelistSinceDate(const char *applicationName, FileList *addedOrModifiedFilesWithHashData, FileList *deletedFiles, double sinceDate);

	/// Get patches (or files) for every file in input, assuming that input has a hash for each of those files.  This is used by AutopatcherServer and not usually explicitly called.
	/// \param[in] applicationName A null terminated string previously passed to AddApplication
	/// \param[in] input A list of files with hashes to get from the database.  If this hash exists, a patch to the current version is returned if this file is not the current version.  Otherwise the current version is returned.
	/// \param[in] allowDownloadOfOriginalUnmodifiedFiles If false, then if a file has never been modified and there is no hash for it in the input list, return false. This is to prevent clients from just downloading the game from the autopatcher.
	/// \param[out] patchList A list of files with either the filedata or the patch.  This is a subset of \a input.  The context data for each file will be either PC_WRITE_FILE (to just write the file) or PC_HASH_WITH_PATCH (to patch).  If PC_HASH_WITH_PATCH, then the file contains a SHA1_LENGTH byte patch followed by the hash.  The datalength is patchlength + SHA1_LENGTH
	/// \return 1 on success, 0 on database failure, -1 on tried to download original unmodified file
	virtual int GetPatches(const char *applicationName, FileList *input, bool allowDownloadOfOriginalUnmodifiedFiles, FileList *patchList);

	/// For the most recent update, return files that were patched, added, or deleted. For files that were patched, return both the patch in \a patchedFiles and the current version in \a updatedFiles
	/// \param[in,out] applicationName Name of the application to get patches for. If empty, uses the most recently updated application, and the string will be updated to reflect this name.
	/// \param[out] patchedFiles A list of patched files with op PC_HASH_2_WITH_PATCH. It has 2 hashes, the priorHash and the currentHash. The currentHash is checked on the client after patching for patch success. The priorHash is checked in AutopatcherServer::OnGetPatch() to see if the client is able to hash with the version they currently have
	/// \param[out] patchedFiles A list of new files. It contains the actual data in addition to the filename
	/// \param[out] addedOrModifiedFileHashes A list of file hashes that were either modified or new. This is returned to the client when replying to ID_AUTOPATCHER_CREATION_LIST, which tells the client what files have changed on the server since a certain date
	/// \param[out] deletedFiles A list of the current versions of filenames that were deleted in the most recent patch
	/// \param[out] whenPatched time in seconds since epoch when patched. Use time() function to get this in C
	/// \return true on success, false on failure
	virtual bool GetMostRecentChangelistWithPatches(RakNet::RakString &applicationName, FileList *patchedFiles, FileList *addedFiles, FileList *addedOrModifiedFileHashes, FileList *deletedFiles, double *priorRowPatchTime, double *mostRecentRowPatchTime);

	/// If any of the above functions fail, the error string is stored internally.  Call this to get it.
	virtual const char *GetLastError(void) const;

	/// Read part of a file into \a destination
	/// Return the number of bytes written. Return 0 when file is done.
	/// \param[in] filename Filename to read
	/// \param[in] startReadBytes What offset from the start of the file to read from
	/// \param[in] numBytesToRead How many bytes to read. This is also how many bytes have been allocated to preallocatedDestination
	/// \param[out] preallocatedDestination Write your data here
	/// \return The number of bytes read, or 0 if none
	virtual unsigned int GetFilePart( const char *filename, unsigned int startReadBytes, unsigned int numBytesToRead, void *preallocatedDestination, FileListNodeContext context);

	/// \return Passed to FileListTransfer::Send() as the _chunkSize parameter.
	virtual const int GetIncrementalReadChunkSize(void) const;
	
	// Use a separate connection for file parts, because PGConn is not threadsafe
	PGconn *filePartConnection;
	SimpleMutex filePartConnectionMutex;

protected:
	virtual unsigned int GetPatchPart( const char *filename, unsigned int startReadBytes, unsigned int numBytesToRead, void *preallocatedDestination, FileListNodeContext context);
};


// This version references the version on the harddrive, rather than store the patch in the database
// It is necessary if your game has files over about 400 megabytes.
class RAK_DLL_EXPORT AutopatcherPostgreRepository2 : public AutopatcherPostgreRepository
{
public:
	virtual bool CreateAutopatcherTables(void);
	virtual bool GetMostRecentChangelistWithPatches(RakNet::RakString &applicationName, FileList *patchedFiles, FileList *addedFiles, FileList *addedOrModifiedFileHashes, FileList *deletedFiles, double *priorRowPatchTime, double *mostRecentRowPatchTime);
	virtual bool UpdateApplicationFiles(const char *applicationName, const char *applicationDirectory, const char *userName, FileListProgress *cb);
	virtual unsigned int GetFilePart( const char *filename, unsigned int startReadBytes, unsigned int numBytesToRead, void *preallocatedDestination, FileListNodeContext context);
	
	/// Can override this to create patches using a different tool
	/// \param[in] oldFile Path to the old version of the file, on disk
	/// \param[in] newFile Path to the updated file, on disk
	/// \param[out] patch Pointer you should allocate, to hold the patch
	/// \param[out] patchLength Write the length of the resultant patch here
	/// \param[out] patchAlgorithm Stored in the database. Use if you want to represent what algorithm was used. Transmitted to the client for decompression
	virtual int MakePatch(const char *oldFile, const char *newFile, char **patch, unsigned int *patchLength, int *patchAlgorithm);
protected:
	// Implements MakePatch using bsDiff. Uses a lot of memory, should not use for files above about 100 megabytes.
	virtual bool MakePatchBSDiff(FILE *fpOld, int contentLengthOld, FILE *fpNew, int contentLengthNew, char **patch, unsigned int *patchLength);

};

} // namespace RakNet

#endif
#endif
