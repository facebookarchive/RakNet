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
/// \brief The server plugin for the autopatcher.  Must be running for the client to get patches.

// TODO - bsdiff doesn't work for files above 100 megabytes.
// See http://xdelta.org/
// XDelta is GPL 2, however I could run that as a separate EXE and invoke to only transmit the delta file. 

// See http://pocketsoft.com/rtpatch.html
// See use rdiff instead of bsdiff, or perhaps librsync

// See https://code.google.com/p/open-vcdiff/
// https://code.google.com/p/open-vcdiff/wiki/HowToUseOpenVcdiff
// https://github.com/gtoubassi/femtozip/wiki/Sdch

#ifndef __AUTOPATCHER_SERVER_H
#define __AUTOPATCHER_SERVER_H

#include "RakNetTypes.h"
#include "Export.h"
#include "PluginInterface2.h"
#include "PacketPriority.h"
#include "ThreadPool.h"
#include "BitStream.h"
#include "RakString.h"
#include "FileList.h"
#include "IncrementalReadInterface.h"

namespace RakNet
{
/// Forward declarations
class RakPeerInterface;
struct Packet;
class AutopatcherRepositoryInterface;
class FileListTransfer;

class RAK_DLL_EXPORT AutopatcherServerLoadNotifier
{
public:
	/// Current queue state of the autopatcher
	struct AutopatcherState
	{
		/// How many requests have been queued to be processed later
		unsigned int requestsQueued;

		/// How many requests are currently working (including downloading files).
		/// This will not normally exceed AutopatcherServer::SetMaxConurrentUsers()
		unsigned int requestsWorking;
	};

	/// The server only handles two types of requests - to get a change list since a certain date, or to get a patch
	enum RequestType
	{
		ASUMC_GET_CHANGELIST,
		ASUMC_GET_PATCH,
	};

	enum QueueOperation
	{
		QO_WAS_ADDED,
		QO_WAS_ABORTED,
		QO_POPPED_ONTO_TO_PROCESSING_THREAD
	};

	enum GetChangelistResult
	{
		GCR_DELETE_FILES,
		GCR_ADD_FILES,
		GCR_ADD_AND_DELETE_FILES,
		GCR_NOTHING_TO_DO,
		GCR_REPOSITORY_ERROR,
	};

	enum PatchResult
	{
		PR_NO_FILES_NEEDED_PATCHING,
		PR_REPOSITORY_ERROR,
		PR_DISALLOWED_DOWNLOADING_ORIGINAL_FILES,
		PR_PATCHES_WERE_SENT,
		PR_ABORTED_FROM_INPUT_THREAD,
		PR_ABORTED_FROM_DOWNLOAD_THREAD,
	};

	/// The server queues have been updated
	/// \param[out] remoteSystem Which system this refers to
	/// \param[out] requestType Either added to / removed a changelist request, or a get patch request
	/// \param[out] queueOperation The operation was added to the queue, removed from the queue to be processed, or removed because it was aborted
	/// \param[out] autopatcherState Current size of the request queue, and how many requests are working
	virtual void OnQueueUpdate(SystemAddress remoteSystem,
		AutopatcherServerLoadNotifier::RequestType requestType,
		AutopatcherServerLoadNotifier::QueueOperation queueOperation,
		AutopatcherServerLoadNotifier::AutopatcherState *autopatcherState)
	{(void) remoteSystem; (void) requestType; (void) queueOperation; (void) autopatcherState;}

	virtual void OnGetChangelistCompleted(
		SystemAddress remoteSystem,
		AutopatcherServerLoadNotifier::GetChangelistResult getChangelistResult,
		AutopatcherServerLoadNotifier::AutopatcherState *autopatcherState)
	{(void) remoteSystem; (void) autopatcherState;}

	/// A file transfer has completed, or was not necessary
	/// \param[out] remoteSystem Which system this refers to
	/// \param[out] autopatcherState Current size of the request queue, and how many requests are working
	virtual void OnGetPatchCompleted(SystemAddress remoteSystem,
		AutopatcherServerLoadNotifier::PatchResult patchResult,
		AutopatcherServerLoadNotifier::AutopatcherState *autopatcherState)
	{(void) remoteSystem; (void) patchResult; (void) autopatcherState;};
};

/// \brief Sample implementation of AutopatcherServerLoadNotifier using printf
class RAK_DLL_EXPORT AutopatcherServerLoadNotifier_Printf : public AutopatcherServerLoadNotifier
{
public:
	virtual void OnQueueUpdate(SystemAddress remoteSystem,
		AutopatcherServerLoadNotifier::RequestType requestType,
		AutopatcherServerLoadNotifier::QueueOperation queueOperation,
		AutopatcherServerLoadNotifier::AutopatcherState *autopatcherState);

	virtual void OnGetChangelistCompleted(
		SystemAddress remoteSystem,
		AutopatcherServerLoadNotifier::GetChangelistResult getChangelistResult,
		AutopatcherServerLoadNotifier::AutopatcherState *autopatcherState);

	virtual void OnGetPatchCompleted(SystemAddress remoteSystem,
		AutopatcherServerLoadNotifier::PatchResult patchResult,
		AutopatcherServerLoadNotifier::AutopatcherState *autopatcherState);
};

/// \brief The server plugin for the autopatcher.  Must be running for the client to get patches.
class RAK_DLL_EXPORT AutopatcherServer : public PluginInterface2 , public ThreadDataInterface, FileListProgress, IncrementalReadInterface
{
public:
	// Constructor
	AutopatcherServer();

	// Destructor
	~AutopatcherServer();

	/// DO THIS FIRST
	/// Implement to start the worker threads.
	/// Before this is called, no queries will be performed
	/// When this is called, AllocAutopatcherRepositoryInterface will be called with \a repositoryAllocationParameters
	/// The system works in three phases.
	/// 1. Get change list since a given date. This uses one of the worker threads.
	/// 2. If either running a full scan, or files have changed since a given date, get the list of patches. This uses one of the worker threads and does an intensive comparison of the hashes the client has vs. the files in the database
	/// 3. If the total amount of data to be sent exceeds DATABASE_READ_CHUNK_SIZE, defined in the cpp file, then the database will be read from incrementally during this download phase. This uses an sql connection object, which may or may not be also in use by one of the threads.
	/// If you have more sql connections than threads, this tends to prevent the same connection from being used to incrementally read files for a downloader, and to be used in a worker thread.
	/// \param[in] numThreads Number of processing threads, which handles the CPU intensive tasks of generating a patch list and comparing files
	/// \param[in] numSQLConnections Number of SQL connection objects passed to \a sqlConnectionPtrArray. Must be greater than or equal to numThreads
	/// \param[in] sqlConnectionPtrArray List of pointers to AutopatcherRepositoryInterface. C++ note: Don't just cast a derived class array, you need to take the pointer address of each item
	void StartThreads(int numThreads, int numSQLConnections, AutopatcherRepositoryInterface **sqlConnectionPtrArray);

	/// Load the most recent patch in memory and keep it there
	/// This can take a lot of memory, but greatly speeds up serving patches, since disk access is not incurred
	/// \param[in] applicationName 0 means all, otherwise the name of the application to cache
	void CacheMostRecentPatch(const char *applicationName);

	/// What parameters to use for the RakPeerInterface::Send() call when uploading files.
	/// \param[in] _priority See RakPeerInterface::Send()
	/// \param[in] _orderingChannel See RakPeerInterface::Send()
	void SetUploadSendParameters(PacketPriority _priority, char _orderingChannel);

	/// This plugin has a dependency on the FileListTransfer plugin, which it uses to actually send the files.
	/// So you need an instance of that plugin registered with RakPeerInterface, and a pointer to that interface should be passed here.
	/// \param[in] flt A pointer to a registered instance of FileListTransfer
	void SetFileListTransferPlugin(FileListTransfer *flt);

	/// This is the maximum number of users the patcher will service at one time (generally about equal to the number of downloads at once)
	/// If this limit is exceeded, the request packet will be put into a queue and serviced when slots are available
	/// Defaults to 0 (unlimited)
	/// \param[in] maxConcurrentUsers Pass 0 for unlimited, otherwise the max users to serve at once
	void SetMaxConurrentUsers(unsigned int _maxConcurrentUsers);

	/// \return Returns what was passed to SetMaxConurrentUsers();
	unsigned int GetMaxConurrentUsers(void) const;

	/// Set a callback to get notifications of when user requests are queued and processed
	/// This is primarily of use to load balance the server
	/// \param[in] asumc An externally allocated instance of AutopatcherServerLoadNotifier. Pass 0 to disable.
	void SetLoadManagementCallback(AutopatcherServerLoadNotifier *asumc);

	/// Set whether or not the client can download files that were never modified, that they do not have
	/// Defaults to true
	/// Set to false to disallow downloading the entire game through the autopatcher. In this case, the user must have a copy of the game through other means (such as a CD install)
	/// \param[in] allow True to allow downloading original game files, false to disallow
	void SetAllowDownloadOfOriginalUnmodifiedFiles(bool allow);

	/// Clear buffered input and output
	void Clear(void);

	/// \internal For plugin handling
	virtual void OnAttach(void);
	/// \internal For plugin handling
	virtual void OnDetach(void);;
	/// \internal For plugin handling
	virtual void Update(void);
	/// \internal For plugin handling
	virtual PluginReceiveResult OnReceive(Packet *packet);
	/// \internal For plugin handling
	virtual void OnShutdown(void);
	/// \internal For plugin handling
	virtual void OnStartup(RakPeerInterface *peer);
	/// \internal For plugin handling
	virtual void OnClosedConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );

	struct ThreadData
	{
		AutopatcherServer *server;
		RakNet::RakString applicationName;
		double lastUpdateDate;
		SystemAddress systemAddress;
		FileList *clientList;
		unsigned short setId;
	};

	/// \deprecated
	struct ResultTypeAndBitstream
	{
		ResultTypeAndBitstream() {patchList=0; deletedFiles=0; addedOrModifiedFilesWithHashData=0;}
		int resultType;
		SystemAddress systemAddress;
		RakNet::BitStream bitStream1;
		RakNet::BitStream bitStream2;
		FileList *patchList;
		FileList *deletedFiles, *addedOrModifiedFilesWithHashData;
		// bool fatalError;
		int resultCode; // 1 = success, 0 = unknown error, -1 = failed allowDownloadOfOriginalUnmodifiedFiles check
		unsigned short setId;
		double currentDate;
		enum
		{
			GET_CHANGELIST_SINCE_DATE,
			GET_PATCH,
		} operation;
	};

protected:
	friend AutopatcherServer::ResultTypeAndBitstream* GetChangelistSinceDateCB(AutopatcherServer::ThreadData pap, bool *returnOutput, void* perThreadData);
	friend AutopatcherServer::ResultTypeAndBitstream* GetPatchCB(AutopatcherServer::ThreadData pap, bool *returnOutput, void* perThreadData);
	PluginReceiveResult OnGetChangelistSinceDate(Packet *packet);
	PluginReceiveResult OnGetPatch(Packet *packet);
	void OnGetChangelistSinceDateInt(Packet *packet);
	void OnGetPatchInt(Packet *packet);
	void* PerThreadFactory(void *context);
	void PerThreadDestructor(void* factoryResult, void *context);
	void RemoveFromThreadPool(SystemAddress systemAddress);
	virtual unsigned int GetFilePart( const char *filename, unsigned int startReadBytes, unsigned int numBytesToRead, void *preallocatedDestination, FileListNodeContext context);

	//AutopatcherRepositoryInterface *repository;
	FileListTransfer *fileListTransfer;
	PacketPriority priority;
	char orderingChannel;

	// The point of the threadPool is so that SQL queries, which are blocking, happen in the thread and don't slow down the rest of the application
	// The threadPool has a queue for incoming processing requests.  As systems disconnect their pending requests are removed from the list.
	ThreadPool<ThreadData, ResultTypeAndBitstream*> threadPool;

	SimpleMutex connectionPoolMutex;
	DataStructures::Queue<AutopatcherRepositoryInterface *> connectionPool;

	// How many users are currently patching
	// unsigned int patchingUserCount;

	SimpleMutex patchingUsersMutex;
	DataStructures::List<SystemAddress> patchingUsers;
	bool IncrementPatchingUserCount(SystemAddress sa);
	bool DecrementPatchingUserCount(SystemAddress sa);
	bool PatchingUserLimitReached(void) const;
    virtual void OnFilePushesComplete( SystemAddress systemAddress, unsigned short setID );
	virtual void OnSendAborted( SystemAddress systemAddress );

	unsigned int maxConcurrentUsers;
	// If maxConcurrentUsers is exceeded, then incoming requests are put into this queue
	DataStructures::Queue<Packet *> userRequestWaitingQueue;
	void AddToWaitingQueue(Packet *packet);
	Packet *AbortOffWaitingQueue(void);
	Packet *PopOffWaitingQueue(void);

	AutopatcherServerLoadNotifier *loadNotifier;
	void CallPacketCallback(Packet *packet, AutopatcherServerLoadNotifier::QueueOperation queueOperation);
	void CallPatchCompleteCallback(const SystemAddress &systemAddress, AutopatcherServerLoadNotifier::PatchResult patchResult);

	RakNet::RakString cache_appName;
	FileList cache_patchedFiles;
	FileList cache_addedFiles;
	FileList cache_addedOrModifiedFileHashes;
	FileList cache_deletedFiles;
	double cache_minTime, cache_maxTime;
	bool cacheLoaded;
	bool allowDownloadOfOriginalUnmodifiedFiles;
};

} // namespace RakNet

#endif
