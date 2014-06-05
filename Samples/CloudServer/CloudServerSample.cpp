/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "CloudServerHelper.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "FullyConnectedMesh2.h"
#include "TwoWayAuthentication.h"
#include "CloudClient.h"
#include "DynDNS.h"
#include "RakPeerInterface.h"
#include "RakSleep.h"
#include "ConnectionGraph2.h"

int main(int argc, char **argv)
{
	// Used to update DNS
	RakNet::DynDNS dynDNS;
	RakNet::CloudServerHelper_DynDns cloudServerHelper(&dynDNS);
	if (!cloudServerHelper.ParseCommandLineParameters(argc, argv))
		return 1;

	// ---- RAKPEER -----
	RakNet::RakPeerInterface *rakPeer;
	rakPeer=RakNet::RakPeerInterface::GetInstance();

	// ---- PLUGINS -----
	// Used to load balance clients, allow for client to client discovery
	RakNet::CloudServer cloudServer;
	// Used to update the local cloudServer
	RakNet::CloudClient cloudClient;
	// Used to determine the host of the server fully connected mesh, as well as to connect servers automatically
	RakNet::FullyConnectedMesh2 fullyConnectedMesh2;
	// Used for servers to verify each other - otherwise any system could pose as a server
	// Could also be used to verify and restrict clients if paired with the MessageFilter plugin
	RakNet::TwoWayAuthentication twoWayAuthentication;
	// Used to tell servers about each other
	RakNet::ConnectionGraph2 connectionGraph2;

	rakPeer->AttachPlugin(&cloudServer);
	rakPeer->AttachPlugin(&cloudClient);
	rakPeer->AttachPlugin(&fullyConnectedMesh2);
	rakPeer->AttachPlugin(&twoWayAuthentication);
	rakPeer->AttachPlugin(&connectionGraph2);

	if (!cloudServerHelper.StartRakPeer(rakPeer))
		return 1;

	RakNet::CloudServerHelperFilter sampleFilter; // Keeps clients from updating stuff to the server they are not supposed to
	sampleFilter.serverGuid=rakPeer->GetMyGUID();
	cloudServerHelper.SetupPlugins(&cloudServer, &sampleFilter, &cloudClient, &fullyConnectedMesh2, &twoWayAuthentication,&connectionGraph2, cloudServerHelper.serverToServerPassword);

	int ret;
	do 
	{
		ret = cloudServerHelper.JoinCloud(rakPeer, &cloudServer, &cloudClient, &fullyConnectedMesh2, &twoWayAuthentication, &connectionGraph2, dynDNS.GetMyPublicIP());
	} while (ret==2);
	if (ret==1)
		return 1;

	// Should now be connect to the cloud, using authentication and FullyConnectedMesh2
	printf("Running.\n");
	RakNet::Packet *packet;
	while (1)
	{
		for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet=rakPeer->Receive())
		{
			cloudServerHelper.OnPacket(packet, rakPeer, &cloudClient, &cloudServer, &fullyConnectedMesh2, &twoWayAuthentication, &connectionGraph2);
		}

		// Update() returns false on DNS update failure
		if (!cloudServerHelper.Update())
			break;

		// Any additional server processing beyond hosting the CloudServer can go here
		RakSleep(30);
	}


	RakNet::RakPeerInterface::DestroyInstance(rakPeer);
	return 0;
}
