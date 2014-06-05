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
/// \brief Test the command console implementations


#include "TCPInterface.h"
#include "ConsoleServer.h"
#include "RakNetCommandParser.h"
#include "TelnetTransport.h"

#include "RakPeerInterface.h"
#include "LogCommandParser.h"
#include "GetTime.h"
#include "RakNetTransport2.h"
#include "LinuxStrings.h"
#include <stdio.h>


void TestTCPInterface(void);
void TestCommandServer(RakNet::TransportInterface *ti, unsigned short port, RakNet::RakPeerInterface *rakPeer);

int main(void)
{
 	RakNet::RakPeerInterface *rakPeer = RakNet::RakPeerInterface::GetInstance();
 	RakNet::SocketDescriptor sd(60000,0);
 	rakPeer->Startup(128,&sd,1);
 	rakPeer->SetMaximumIncomingConnections(128);

	RakNet::TelnetTransport tt;
	TestCommandServer(&tt, 23, rakPeer); // Uncomment to use Telnet as a client.  Telnet uses port 23 by default.

// 	RakNet::RakNetTransport2 rt2;
// 	rakPeer->AttachPlugin(&rt2);
// 	TestCommandServer(&rt2, 60000,rakPeer); // Uncomment to use RakNet as a client

	return 1;
}

void TestCommandServer(RakNet::TransportInterface *ti, unsigned short port, RakNet::RakPeerInterface *rakPeer)
{
	RakNet::ConsoleServer consoleServer;
	RakNet::RakNetCommandParser rcp;
	RakNet::LogCommandParser lcp;
	RakNet::TimeMS lastLog=0;

	printf("This sample demonstrates the command console server, which can be.\n");
	printf("a standalone application or part of your game server.  It allows you to\n");
	printf("easily parse text strings sent from a client using either secure RakNet\n");
	printf("or Telnet.\n");
	printf("See the 'CommandConsoleClient' project for the RakNet client.\n");
	printf("Difficulty: Intermediate\n\n");

	printf("Command server started on port %i.\n", port);
	consoleServer.AddCommandParser(&rcp);
	consoleServer.AddCommandParser(&lcp);
	consoleServer.SetTransportProvider(ti, port);
	consoleServer.SetPrompt("> "); // Show this character when waiting for user input
	rcp.SetRakPeerInterface(rakPeer);
	lcp.AddChannel("TestChannel");
	while (1)
	{
		consoleServer.Update();
		// Ignore raknet packets for this sample.
		rakPeer->DeallocatePacket(rakPeer->Receive());

		if (RakNet::GetTimeMS() > lastLog + 4000)
		{
			lcp.WriteLog("TestChannel", "Test of logger");
			lastLog=RakNet::GetTimeMS();
		}

#ifdef _WIN32
		Sleep( 30 );
#else
		usleep( 30 * 1000 );
#endif
	}	
}