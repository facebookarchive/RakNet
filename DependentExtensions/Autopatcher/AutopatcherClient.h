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
/// \brief Client plugin for the autopatcher


#ifndef __AUTOPATCHER_CLIENT_H
#define __AUTOPATCHER_CLIENT_H

#include "RakNetTypes.h"
#include "Export.h"
#include "PluginInterface2.h"
#include "PacketPriority.h"
#include "FileList.h"
#include "SimpleMutex.h"
#include "FileListTransferCBInterface.h"
#include "AutopatcherPatchContext.h"

namespace RakNet
{

class RakPeerInterface;
struct Packet;
class FileListTransfer;

class RAK_DLL_EXPORT AutopatcherClientCBInterface : public FileListTransferCBInterface
{
public:
	virtual PatchContext ApplyPatchBSDiff(const char *oldFilePath, char **newFileContents, unsigned int *newFileSize, char *patchContents, unsigned int patchSize);
	virtual PatchContext ApplyPatchBase(const char *oldFilePath, char **newFileContents, unsigned int *newFileSize, char *patchContents, unsigned int patchSize, uint32_t patchAlgorithm);
};

/// \ingroup Autopatcher
class RAK_DLL_EXPORT AutopatcherClient : public PluginInterface2
{
public:
	// Constructor
	AutopatcherClient();

	// Destructor
	~AutopatcherClient();

	/// Are we in the middle of patching?
	/// \return True if yes, false otherwise.
	bool IsPatching(void) const;

	/// What parameters to use for the RakPeerInterface::Send() call when uploading files.
	/// \param[in] _priority See RakPeerInterface::Send()
	/// \param[in] _orderingChannel See RakPeerInterface::Send()
	void SetUploadSendParameters(PacketPriority _priority, char _orderingChannel);

	/// This plugin has a dependency on the FileListTransfer plugin, which it uses to actually send the files.
	/// So you need an instance of that plugin registered with RakPeerInterface, and a pointer to that interface should be passed here.
	/// \param[in] flt A pointer to a registered instance of FileListTransfer
	void SetFileListTransferPlugin(FileListTransfer *flt);

	/// Patches a certain directory associated with a named application to match the same named application on the patch server
	/// \param[in] _applicationName The name of the application
	/// \param[in] _applicationDirectory The directory to write the output to.
	/// \param[in] lastUpdateDate Returned by time() (seconds since EPOCH) on the server, as well as returned in GetServerDate() after you call PatchApplication successfully. When distributing your application, set to the server time() when the distribution was created. You can pass 0 as well, however this will do a full scan of the entire game so is very slow. See 'Optimizing for large games' in the manual under Help/autopatcher.html
	/// \param[in] host The address of the remote system to send the message to.
	/// \param[in] onFileCallback Callback to call per-file (optional).  When fileIndex+1==setCount in the callback then the download is done
	/// \param[in] _restartOutputFilename If it is necessary to restart this application, where to write the restart data to.  You can include a path in this filename.
	/// \param[in] pathToRestartExe What exe to launch from the AutopatcherClientRestarter .  argv[0] will work to relaunch this application.
	/// \return true on success, false on failure.  On failure, ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR is returned, with the error message written using the stringCompressor class starting after the message id.
	bool PatchApplication(const char *_applicationName, const char *_applicationDirectory, double lastUpdateDate, SystemAddress host, AutopatcherClientCBInterface *onFileCallback, const char *restartOutputFilename, const char *pathToRestartExe);

	/// After getting ID_AUTOPATCHER_FINISHED or ID_AUTOPATCHER_RESTART_APPLICATION, the server date will be internally recorded.  You can send this to PatchApplication::lastUpdateDate to only check for files newer than that date.
	double GetServerDate(void) const;

	/// Stop processing the current download
	/// \note The files in progress will continue to transfer. The only way to stop it is to call CloseConnection(true), then reconnect to the server once you get ID_DISCONNECTION_NOTIFICATION
	void CancelDownload(void);

	/// Free internal memory.
	void Clear(void);

	/// \internal For plugin handling
	virtual void Update(void);
	/// \internal For plugin handling
	virtual PluginReceiveResult OnReceive(Packet *packet);
	void OnClosedConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );
	/// \internal For plugin handling
	virtual void OnShutdown(void);
	void OnThreadCompletion(void);

protected:
	PluginReceiveResult OnCreationList(Packet *packet);
	void OnDeletionList(Packet *packet);
	PluginReceiveResult OnDownloadFinishedInternal(Packet *packet);
	PluginReceiveResult OnDownloadFinished(Packet *packet);
	
	friend class AutopatcherClientCallback;
	void CopyAndRestart(const char *filePath);
	void Redownload(const char *filePath);

	char applicationDirectory[512];
	char applicationName[512];
	char copyOnRestartOut[512];
	char restartExe[512];
	double serverDate;
	FileListTransfer *fileListTransfer;
	PacketPriority priority;
	SystemAddress serverId;
	SystemIndex serverIdIndex;
	char orderingChannel;
	unsigned short setId;
	AutopatcherClientCBInterface *userCB;
	bool patchComplete;
	FileList redownloadList, copyAndRestartList;

	bool processThreadCompletion;
	SimpleMutex processThreadCompletionMutex;
};

} // namespace RakNet

#endif
