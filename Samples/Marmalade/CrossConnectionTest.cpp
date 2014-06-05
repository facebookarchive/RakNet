/*
 * This file is part of the Airplay SDK Code Samples.
 *
 * Copyright (C) 2001-2011 Ideaworks3D Ltd.
 * All Rights Reserved.
 *
 * This source code is intended only as a supplement to Ideaworks Labs
 * Development Tools and/or on-line documentation.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

/**
 * @page ExampleS3EHelloWorld S3E Hello World Example
 *
 * The following example, in typical Hello World style, displays the phrase
 * "Hello, World!" on screen.
 *
 * The functions required to achieve this are:
 * <ul>
 *   <li>s3eDebugPrint()
 * </ul>
 *
 * All examples will follow this basic pattern; a brief description of what
 * the example does will be given followed by a list of all the important
 * functions and, perhaps, classes.
 *
 * Should the example be more complex, a more detailed explanation of what the
 * example does and how it does it will be added. Note that most examples
 * use an example framework to remove boilerplate code and allow the projects
 * to be made up of a single source file for easy viewing. This framework can
 * be found in the examples/s3e/ExamplesMain directory.
 *
 * @include s3eHelloWorld.cpp
 */

#include "s3e.h"

// ------- RAKNET INCLUDES ------------
#include "RakPeerInterface.h"
#include "PacketLogger.h"
#include "Rand.h"
#include "Kbhit.h"
#include <stdio.h>
#include "RakSleep.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "GetTime.h"

using namespace RakNet;

// main entry point for the application
int main()
{
	// ------- RAKNET CODE ------------
	RakPeerInterface *rakPeer1, *rakPeer2;
	rakPeer1=RakPeerInterface::GetInstance();
	rakPeer2=RakPeerInterface::GetInstance();
	rakPeer1->SetMaximumIncomingConnections(8);
	rakPeer2->SetMaximumIncomingConnections(8);

	bool gotConnectionRequestAccepted[2];
	bool gotNewIncomingConnection[2];
	Packet *packet;
	SocketDescriptor sd1(60000,0);
	SocketDescriptor sd2(2000,0);
	unsigned short numSystems[2];

    // Wait for a quit request from the host OS
    while (!s3eDeviceCheckQuitRequest())
    {
        if (s3eTimerGetMs() % 3000 < 1000)
            s3eSurfaceClear(255, 0, 0);
        else if (s3eTimerGetMs() % 3000 < 2000)
            s3eSurfaceClear(0, 255, 0);
        else
            s3eSurfaceClear(0, 0, 255);

		// ------- RAKNET CODE ------------
		gotConnectionRequestAccepted[0]=false;
		gotConnectionRequestAccepted[1]=false;
		gotNewIncomingConnection[0]=false;
		gotNewIncomingConnection[1]=false;
		numSystems[0]=0;
		numSystems[1]=0;

		rakPeer1->Startup(1,&sd1, 1);
		rakPeer2->Startup(1,&sd2, 1);
		RakSleep(100);
		rakPeer1->Connect("127.0.0.1", 2000, 0, 0);
		rakPeer2->Connect("127.0.0.1", 60000, 0, 0);
		RakSleep(100);
		for (packet=rakPeer1->Receive(); packet; rakPeer1->DeallocatePacket(packet), packet=rakPeer1->Receive())
		{
			if (packet->data[0]==ID_NEW_INCOMING_CONNECTION)
				gotNewIncomingConnection[0]=true;
			else if (packet->data[0]==ID_CONNECTION_REQUEST_ACCEPTED)
				gotConnectionRequestAccepted[0]=true;
			else if (packet->data[0]==ID_CONNECTION_ATTEMPT_FAILED)
				printf("Error on rakPeer1, got ID_CONNECTION_ATTEMPT_FAILED\n");
		}
		for (packet=rakPeer2->Receive(); packet; rakPeer2->DeallocatePacket(packet), packet=rakPeer2->Receive())
		{
			if (packet->data[0]==ID_NEW_INCOMING_CONNECTION)
				gotNewIncomingConnection[1]=true;
			else if (packet->data[0]==ID_CONNECTION_REQUEST_ACCEPTED)
				gotConnectionRequestAccepted[1]=true;
			else if (packet->data[0]==ID_CONNECTION_ATTEMPT_FAILED)
				printf("Error on rakPeer2, got ID_CONNECTION_ATTEMPT_FAILED\n");
		}
		rakPeer1->GetConnectionList(0,&numSystems[0]);
		rakPeer2->GetConnectionList(0,&numSystems[1]);

		if (gotConnectionRequestAccepted[0]==true && gotConnectionRequestAccepted[1]==true)
		{
			printf("Test passed\n");
		}
		else if (numSystems[0]!=1 || numSystems[1]!=1)
		{
			printf("Test failed, system 1 has %i connections and system 2 has %i connections.\n", numSystems[0], numSystems[1]);
		}
		else if (gotConnectionRequestAccepted[0]==false && gotConnectionRequestAccepted[1]==false)
		{
			printf("Test failed, ID_CONNECTION_REQUEST_ACCEPTED is false for both instances\n");
		}
		else if (gotNewIncomingConnection[0]==true && gotNewIncomingConnection[1]==true)
		{
			printf("Test failed, ID_NEW_INCOMING_CONNECTION is true for both instances\n");
		}
		else if (gotConnectionRequestAccepted[0]==false && gotConnectionRequestAccepted[1]==false)
		{
			printf("Test failed, ID_NEW_INCOMING_CONNECTION is false for both instances\n");
		}
		else if (gotConnectionRequestAccepted[0]==true && gotNewIncomingConnection[1]==false)
		{
			printf("Test failed, ID_CONNECTION_REQUEST_ACCEPTED for first instance, but not ID_NEW_INCOMING_CONNECTION for second\n");
		}
		else if (gotConnectionRequestAccepted[1]==true && gotNewIncomingConnection[0]==false)
		{
			printf("Test failed, ID_CONNECTION_REQUEST_ACCEPTED for second instance, but not ID_NEW_INCOMING_CONNECTION for first\n");
		}
		else if ((int)gotConnectionRequestAccepted[0]+
			(int)gotConnectionRequestAccepted[1]!=1)
		{
			printf("Test failed, does not have exactly one instance of ID_CONNECTION_REQUEST_ACCEPTED\n");
		}
		else if ((int)gotNewIncomingConnection[0]+
			(int)gotNewIncomingConnection[1]!=1)
		{
			printf("Test failed, does not have exactly one instance of ID_NEW_INCOMING_CONNECTION\n");
		}
		else if ((int)gotConnectionRequestAccepted[0]+
			(int)gotConnectionRequestAccepted[1]+
			(int)gotNewIncomingConnection[0]+
			(int)gotNewIncomingConnection[1]!=2)
		{
			printf("Test failed, does not have exactly one instance of ID_CONNECTION_REQUEST_ACCEPTED and one instance of ID_NEW_INCOMING_CONNECTION\n");
		}
		else
			printf("Test passed\n");

		rakPeer1->Shutdown(0);
		rakPeer2->Shutdown(0);

        // flip the surface buffer
        s3eSurfaceShow();
        // sleep for 0ms to allow the OS to process events etc
        s3eDeviceYield(0);

        //Update keyboard and touchscreen event queues
        s3eKeyboardUpdate();
        s3ePointerUpdate();

        //Quit if there was any activity since the last time the s3e*Update() functions
        //were called
        if (s3eKeyboardAnyKey())
            break;
        if (s3ePointerGetState(S3E_POINTER_BUTTON_SELECT) & S3E_POINTER_STATE_PRESSED)
            break;
    }
    return 0;
}
