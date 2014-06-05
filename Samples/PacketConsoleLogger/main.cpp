/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "TelnetTransport.h"
#include "ConsoleServer.h"
#include "LogCommandParser.h"
#include "PacketConsoleLogger.h"
#include "RakPeerInterface.h"
#include "RakSleep.h"
#include <stdio.h>
#include "Getche.h"
#include "MessageIdentifiers.h"
#include "Kbhit.h"

using namespace RakNet;

void main(void)
{
	printf("Shows how to connect telnet to read PacketLogger output from RakPeer.\n");

	RakPeerInterface *rakPeer = RakNet::RakPeerInterface::GetInstance();
	TelnetTransport tt;
	ConsoleServer consoleServer;
	LogCommandParser lcp;
	PacketConsoleLogger pcl;
	pcl.SetLogCommandParser(&lcp);
	consoleServer.AddCommandParser(&lcp);
	consoleServer.SetTransportProvider(&tt, 23);
	rakPeer->AttachPlugin(&pcl);

	RakNet::SocketDescriptor sd(0,0);
	RakNet::StartupResult sr = rakPeer->Startup(32, &sd, 1);
	(void) sr;
	RakAssert(sr==RAKNET_STARTED);

	printf("Use telnet 127.0.0.1 23 to connect from the command window\n");
	printf("Use 'Turn Windows features on and off' with 'Telnet Client' if needed.\n");
	printf("Once telnet has connected, type 'Logger subscribe'\n");
	printf("Press any key in this window once you have done all this.\n");
	RakNet::Packet *packet;
	while (!kbhit())
	{
		consoleServer.Update();
		RakSleep(30);
	}

	RakNet::ConnectionAttemptResult car = rakPeer->Connect("natpunch.jenkinssoftware.com", 61111, 0, 0);
	(void) car;
	while (1)
	{
		for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet=rakPeer->Receive())
		{
		}

		consoleServer.Update();
		RakSleep(30);
	}	
}