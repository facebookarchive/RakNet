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
/// \brief Extends SQLite3ServerPlugin to support logging functions, including compressing images.
///



#ifndef ___SQLITE_SERVER_LOGGER_PLUGIN_H
#define ___SQLITE_SERVER_LOGGER_PLUGIN_H

#include "SQLite3ServerPlugin.h"
#include "SQLiteLoggerCommon.h"

class RakPeerInterface;

#define MAX_PACKETS_PER_CPU_INPUT_THREAD 16

namespace RakNet
{

	/// \brief Extends SQLite3ServerPlugin to support logging functions, including compressing images.
	/// \details SQLiteClientLoggerPlugin has the ability to send logs. Logs contain a description of the name of the table, the name of the columns, the types, and the data.<BR>
	/// This class will create tables as necessary to support the client inputs.<BR>
	/// Also, if images are sent, the server will format them from uncompressed to compressed JPG using jpeg-7 in a thread.<BR>
	/// Compatible as a plugin with both RakPeerInterface and PacketizedTCP
	/// \ingroup SQL_LITE_3_PLUGIN
	class RAK_DLL_EXPORT SQLiteServerLoggerPlugin : public SQLite3ServerPlugin
	{
	public:
		SQLiteServerLoggerPlugin();
		virtual ~SQLiteServerLoggerPlugin();

		/// Used with SetSessionManagementMode()
		enum SessionManagementMode
		{
			/// \brief USE_ANY_DB_HANDLE is for games where the clients are not allowed to create new databases, and the clients don't care which database is written to. Typically only used if there is only one database ever.
			/// \details Ignore SQLiteClientLoggerPlugin::StartSession(), and rely on a prior call to SQLite3ServerPlugin::AddDBHandle().
			/// If no handles exist, logging calls silently fail.
			USE_ANY_DB_HANDLE,

			/// \brief USE_NAMED_DB_HANDLE is for games where the clients are not allowed to create new databases. Instead, they use named databases precreated.
			/// \details Interpret the sessionId parameter passed to SQLiteClientLoggerPlugin::StartSession() as the dbIdentifier parameter passed to SQLite3ServerPlugin::AddDBHandle().
			/// Use this database if it exists. If not, logging calls silently fail.
			USE_NAMED_DB_HANDLE,

			/// \brief CREATE_EACH_NAMED_DB_HANDLE is for single player games or multiplayer games where every game has a unique sesionId.
			/// \details Use the sessionId parameter passed to SQLiteClientLoggerPlugin::StartSession() as a dbIdentifier.
			/// A new database is created for each user
			CREATE_EACH_NAMED_DB_HANDLE,

			/// \brief CREATE_SHARED_NAMED_DB_HANDLE is for multiplayer games where you don't want to have to transmit and synchronize a unique sessionId. Everyone is in the same sessionId.
			/// \details Use the sessionId parameter passed to SQLiteClientLoggerPlugin::StartSession() as a dbIdentifier.
			/// A new database is created only if the sessionId is different. Two users using the same sessionId will write to the same database
			CREATE_SHARED_NAMED_DB_HANDLE,
		};
		
		/// \brief Determine if and how to automatically create databases, rather than call SQLite3ServerPlugin::AddDBHandle()
		/// \details Typically you want one database to hold the events of a single application session, rather than one database for all sessions over all time.
		/// A user of SQLiteClientLoggerPlugin can optionally call SQLiteClientLoggerPlugin::StartSession() in order to join a session to enable this.
		/// Call this before calling AddDBHandle(), and before any clients connect
		/// \param[in] _sessionManagementMode See SessionManagementMode. Default is CREATE_EACH_NAMED_DB_HANDLE.
		/// \param[in] _createDirectoryForFile If true, uses the current server date as a directory to the filename. This ensures filenames do not overwrite each other, even if the sessionId is reused. Defaults to true.
		/// \param[in] _newDatabaseFilePath For CREATE_EACH_NAMED_DB_HANDLE and CREATE_SHARED_NAMED_DB_HANDLE, will create the databases here. Not used for the other modes. Defaults to whatever the operating system picks (usually the application directory).
		void SetSessionManagementMode(SessionManagementMode _sessionManagementMode, bool _createDirectoryForFile, const char *_newDatabaseFilePath);

		/// Close all connected sessions, writing all databases immediately
		void CloseAllSessions(void);

		/// \brief Enable using realtime shader based DXT compression on an Nvidia based video card. This will activate OpenGL
		/// \details Call this before anyone connects.
		/// If not enabled, or initialization fails, then jpg compression is used instead on the CPU.
		/// Defaults to false, in case the target system does not support OpenGL
		/// \param[in] enable True to enable, false to disable.
		void SetEnableDXTCompression(bool enable);

		struct ProcessingStatus
		{
			int packetsBuffered;
			int cpuPendingProcessing;
			int cpuProcessedAwaitingDeallocation;
			int cpuNumThreadsWorking;
			int sqlPendingProcessing;
			int sqlProcessedAwaitingDeallocation;
			int sqlNumThreadsWorking;
		};

		/// Return the thread and command processing statuses
		void GetProcessingStatus(ProcessingStatus *processingStatus);
		/// \internal
		void Update(void);

		/// \internal For plugin handling
		virtual PluginReceiveResult OnReceive(Packet *packet);
		/// \internal For plugin handling
		virtual void OnClosedConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );
		/// \internal For plugin handling
		virtual void OnShutdown(void);
		virtual void OnAttach(void);
		virtual void OnDetach(void);

		struct SessionNameAndSystemAddress
		{
			RakNet::RakString sessionName;
			SystemAddress systemAddress;
			sqlite3 *referencedPointer;
			RakNet::TimeMS timestampDelta;
//			RakNet::TimeMS dbAgeWhenCreated;
		};
		DataStructures::List<SessionNameAndSystemAddress> loggedInSessions;

		// An incoming data packet, and when it arrived
		struct CPUThreadInputNode
		{
			RakNet::Packet *packet;
		//	RakNet::TimeMS whenMessageArrived;
			// Time difference from their time to server time, plus the age of the database at the time the session was created
			// Applied to CPUThreadOutputNode::clientSendingTime before being passed to SQL
			RakNet::TimeMS timestampDelta;
			RakNet::RakString dbIdentifier;
		};
		// As packets arrive, they are added to a CPUThreadInput structure.
		// When the structure is full, or when a maximum amount of time has elapsed, whichever is first, then it is pushed to a thread for processing
		// Deallocated in the thread
		struct CPUThreadInput
		{
			// One or more incoming data packets, and when those packets arrived
			// Processed on the CPU, possibly with batch processing for image compression
			CPUThreadInputNode cpuInputArray[MAX_PACKETS_PER_CPU_INPUT_THREAD];
			int arraySize;
		};
		// Each CPUThreadInputNode is unpacked to a CPUThreadOutputNode
		// Images are now in compressed format, should the parameter list indeed have a query
		struct CPUThreadOutputNode
		{
			RakNet::Packet *packet; // Passthrough
//			RakNet::TimeMS whenMessageArrived; // Passthrough
			RakNet::RakString dbIdentifier; // Passthrough
			// SystemAddress systemAddress;
			char ipAddressString[32];
			RakNet::RakString tableName;
			RakNet::RakString file;
			RakNet::TimeMS clientSendingTime;
			unsigned char parameterCount;
			bool isFunctionCall;
			DataStructures::List<RakNet::RakString> insertingColumnNames;
			LogParameter parameterList[MAX_SQLLITE_LOGGER_PARAMETERS];
			uint32_t tickCount;
			int line;
		};
		// List of CPUThreadOutputNode
		struct CPUThreadOutput
		{
			// cpuOutputNodeArray pointers are not deallocated, just handed over to SQLThreadInput
			// Each element in the array is pushed to one SQLThreadInput
			CPUThreadOutputNode *cpuOutputNodeArray[MAX_PACKETS_PER_CPU_INPUT_THREAD];
			int arraySize;
		};
		struct SQLThreadInput
		{
			sqlite3 *dbHandle;
			CPUThreadOutputNode *cpuOutputNode;
		};
		struct SQLThreadOutput
		{
			// cpuOutputNode gets deallocated here
			CPUThreadOutputNode *cpuOutputNode;
		};

	protected:
		unsigned int CreateDBHandle(RakNet::RakString dbIdentifier);
		void CloseUnreferencedSessions(void);

		SessionManagementMode sessionManagementMode;
		bool createDirectoryForFile;
		RakNet::RakString newDatabaseFilePath;

		ThreadPool<CPUThreadInput*, CPUThreadOutput*> cpuLoggerThreadPool;
		ThreadPool<SQLThreadInput, SQLThreadOutput> sqlLoggerThreadPool;

		CPUThreadInput *LockCpuThreadInput(void);
		void ClearCpuThreadInput(void);
		void UnlockCpuThreadInput(void);
		void CancelLockCpuThreadInput(void);
		void PushCpuThreadInputIfNecessary(void);
		void PushCpuThreadInput(void);
		void StopCPUSQLThreads(void);

		CPUThreadInput *cpuThreadInput;
		RakNet::TimeMS whenCpuThreadInputAllocated;
		bool dxtCompressionEnabled;

	};
};

#endif
