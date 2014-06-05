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
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include "Kbhit.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "UDPForwarder.h"
#include "SocketLayer.h"

int main()
{
	RakNet::RakPeerInterface *rakPeer[2];
	rakPeer[0]=RakNet::RakPeerInterface::GetInstance();
	rakPeer[1]=RakNet::RakPeerInterface::GetInstance();
	RakNet::SocketDescriptor sd1(50000,0),sd2(50002,0);
	RakNet::StartupResult sr;
#if RAKNET_SUPPORT_IPV6==1
	sd1.socketFamily=AF_INET6; // Test out IPV6
#endif
	sr = rakPeer[0]->Startup(1,&sd1, 1);
	RakAssert(sr==RakNet::RAKNET_STARTED);
#if RAKNET_SUPPORT_IPV6==1
	sd2.socketFamily=AF_INET6; // Test out IPV6
#endif
	sr = rakPeer[1]->Startup(1,&sd2, 1);
	RakAssert(sr==RakNet::RAKNET_STARTED);
	rakPeer[1]->SetMaximumIncomingConnections(1);
	RakNet::UDPForwarder udpForwarder;
	
	printf("Demonstrates the UDP Forwarder class\n");
	printf("It routes datagrams from system to another, at the UDP level.\n");
	printf("You probably won't use UDPForwarder directly.\n");
	printf("See UDPProxyClient, UDPProxyServer, UDPProxyCoordinator.\n");
	
	// Start the forwarder
	udpForwarder.Startup();

	// RakNet will send a message at least every 5 seconds. Add another second to account for thread latency
	const RakNet::TimeMS timeoutOnNoDataMS=6000;

	// Address is probably 192.168.0.1. Fix it to be 127.0.0.1.
	// Only necessary to do this when connecting through the loopback on the local system. In a real system we'd stick with the external IP
	RakNet::SystemAddress peer0Addr = rakPeer[0]->GetMyBoundAddress();
	RakAssert(peer0Addr!=RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	RakNet::SystemAddress peer1Addr = rakPeer[1]->GetMyBoundAddress();
	RakAssert(peer1Addr!=RakNet::UNASSIGNED_SYSTEM_ADDRESS);
// 	peer0Addr.FromString("127.0.0.1");
// 	peer1Addr.FromString("127.0.0.1");

	unsigned short fowardPort;
	SOCKET forwardingSocket;
	if (!udpForwarder.StartForwarding(peer0Addr,peer1Addr, timeoutOnNoDataMS, "127.0.0.1", sd1.socketFamily, &fowardPort, &forwardingSocket))
	{
		printf("Socket error\n");
		return 1;
	}

	RakNet::SystemAddress boundAddress;
	RakNet::SocketLayer::GetSystemAddress( forwardingSocket, &boundAddress );
	printf("UDPForwarder bound on %s\n", boundAddress.ToString(false));

	// Send a connect message to the forwarder, on the port to forward to rakPeer[1]
	RakNet::ConnectionAttemptResult car = rakPeer[0]->Connect(boundAddress.ToString(false), fowardPort, 0, 0);
	RakAssert(car==RakNet::CONNECTION_ATTEMPT_STARTED);
	
	printf("'q'uit.\n");
	RakNet::Packet *p;
	while (1)
	{
		for (int i=0; i < 2 ; i++)
		{
			p=rakPeer[i]->Receive();
			while (p)
			{
				if (p->data[0]==ID_DISCONNECTION_NOTIFICATION)
					printf("%s disconnected\n", p->systemAddress.ToString(true));
				else if (p->data[0]==ID_CONNECTION_LOST)
					printf("Lost connection to %s (failure)\n", p->systemAddress.ToString(true));
				else if (p->data[0]==ID_NO_FREE_INCOMING_CONNECTIONS)
					printf("%s has no free incoming connections.\n", p->systemAddress.ToString(true));
				else if (p->data[0]==ID_NEW_INCOMING_CONNECTION)
					printf("%s connected to us (success)\n", p->systemAddress.ToString(true));
				else if (p->data[0]==ID_CONNECTION_REQUEST_ACCEPTED)
					printf("Connection request accepted from %s (success)\n", p->systemAddress.ToString(true));
				else if (p->data[0]==ID_CONNECTION_ATTEMPT_FAILED)
					printf("Failed to connect to %s (failure)\n", p->systemAddress.ToString(true));
	
				rakPeer[i]->DeallocatePacket(p);
				p=rakPeer[i]->Receive();
			}
		}

		if (kbhit())
		{
			char ch = getch();
			if (ch=='q' || ch=='Q')
				break;
		}

		RakSleep(30);
	}

	rakPeer[0]->Shutdown(100,0);
	rakPeer[1]->Shutdown(100,0);
	RakNet::RakPeerInterface::DestroyInstance(rakPeer[0]);
	RakNet::RakPeerInterface::DestroyInstance(rakPeer[1]);
	return 0;
}
