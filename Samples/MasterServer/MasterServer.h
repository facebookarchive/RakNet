/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

// TOPOLOGY
// Always running
// Active game servers == active clients

#ifndef __MASTER_SERVER_H
#define __MASTER_SERVER_H

#include "MasterCommon.h"
#include "NetworkTypes.h"

/// \ingroup MASTER_SERVER_GROUP
/// \brief implements the master server
class MasterServer : public MasterCommon
{
public:
	MasterServer();
	~MasterServer();
protected:
	virtual void Update(RakPeerInterface *peer);
	virtual bool OnReceive(RakPeerInterface *peer, Packet *packet);
	// Event when a packet was tampered with mid-stream.  Override.
	void OnModifiedPacket(void);
	bool PropagateToGame(Packet *packet) const;
	void HandleQuery(Packet *packet);
	void HandleDelistServer(Packet *packet);
	void HandleUpdateServer(Packet *packet);
	void HandleRelayedConnectionNotification(Packet *packet);
};

#endif
