/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

//
// Main view controller, that presents the chat
//

#import "ViewController.h"
#import "ServerDetailsViewController.h"

@implementation ViewController

@synthesize mSendText;
@synthesize mTextBox;


#pragma mark - Raknet specific code

//
// The implemention of protocol ChatServerDetailsProtocol , which is called when we enter the server details
- (void) connectToChatServer:(NSString *)serverIP serverPort:(NSString *)serverPort
{
	mRakPeer = RakNet::RakPeerInterface::GetInstance();
    
    // We're not specifying a port for the client, so that the OS picks one
    RakNet::SocketDescriptor socketDescriptor(0,0);
    socketDescriptor.socketFamily = AF_INET;
    mRakPeer->Startup(8, &socketDescriptor, 1);
    mRakPeer->SetOccasionalPing(true);

    // Connect to the chat server
	RakNet::SocketDescriptor serverAddress(serverPort.intValue, serverIP.UTF8String);
    RakNet::ConnectionAttemptResult car = mRakPeer->Connect(serverAddress.hostAddress, serverAddress.port, "Rumpelstiltskin", (int) strlen("Rumpelstiltskin"));
	assert(car==RakNet::CONNECTION_ATTEMPT_STARTED);
	
	[self appendMessage:[NSString stringWithFormat:@"Connecting to server at %s:%d ...", serverAddress.hostAddress, static_cast<int>(serverAddress.port)]];
}

//
// Copied from Multiplayer.cpp
// If the first byte is ID_TIMESTAMP, then we want the 5th byte
// Otherwise we want the 1st byte
static unsigned char GetPacketIdentifier(RakNet::Packet *p)
{
	if (p==0)
		return 255;
    
	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		RakAssert(p->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char) p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	else
		return (unsigned char) p->data[0];
}


-(void) shutdownChatClient
{
    // Be nice and let the server know we quit.
    mRakPeer->Shutdown(300);
    // We're done with the network
	RakNet::RakPeerInterface::DestroyInstance(mRakPeer);
}

//Sends a message to the server and appends it to the text view
- (IBAction)sendMessage
{
	NSString* message = [NSString stringWithFormat:@"Me: %@",mSendText.text];
	const char* cMessage = [message cStringUsingEncoding:NSASCIIStringEncoding];
	mRakPeer->Send(cMessage, (int) strlen(cMessage)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
    
	[mSendText setText:@""];
	[self appendMessage:message];
}

//Appends a string to the text view
-(void)appendMessage:(NSString*)message
{
	mTextBox.text = [NSString stringWithFormat:@"%@%@\n", mTextBox.text,message];
	[mTextBox flashScrollIndicators];
	[mTextBox scrollRangeToVisible:NSMakeRange(mTextBox.text.length,0)];
}

//Update the network and checks for incoming packets
-(void)tickClient
{
	// If we haven't initialized yet, just return
    if (mRakPeer==0)
		return;
	
    RakNet::Packet* p;
    
    // Get a packet from either the server or the client
    for (p=mRakPeer->Receive(); p; mRakPeer->DeallocatePacket(p), p=mRakPeer->Receive())
    {
        // We got a packet, get the identifier with our handy function
        unsigned char packetIdentifier = GetPacketIdentifier(p);
        
        // Check if this is a network message packet
        switch (packetIdentifier)
        {
			case ID_DISCONNECTION_NOTIFICATION:
				// Connection lost normally
				[self appendMessage:@"ID_DISCONNECTION_NOTIFICATION"];
				break;
			case ID_ALREADY_CONNECTED:
				// Connection lost normally
				[self appendMessage:@"ID_ALREADY_CONNECTED"];
				break;
			case ID_INCOMPATIBLE_PROTOCOL_VERSION:
				[self appendMessage:@"ID_INCOMPATIBLE_PROTOCOL_VERSION"];
				break;
			case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				[self appendMessage:@"ID_REMOTE_DISCONNECTION_NOTIFICATION"]; 
				break;
			case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				[self appendMessage:@"ID_REMOTE_CONNECTION_LOST"];
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
				[self appendMessage:@"ID_REMOTE_NEW_INCOMING_CONNECTION"];
				break;
			case ID_CONNECTION_BANNED: // Banned from this server
				[self appendMessage:@"We are banned from this server."];
				break;			
			case ID_CONNECTION_ATTEMPT_FAILED:
				[self appendMessage:@"Connection attempt failed"];
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				// Sorry, the server is full.  I don't do anything here but
				// A real app should tell the user
				[self appendMessage:@"ID_NO_FREE_INCOMING_CONNECTIONS"];
				break;
                
			case ID_INVALID_PASSWORD:
				[self appendMessage:@"ID_INVALID_PASSWORD"];
				break;
                
			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				[self appendMessage:@"ID_CONNECTION_LOST"];
				break;
                
			case ID_CONNECTION_REQUEST_ACCEPTED:
				// This tells the client they have connected
				[self appendMessage:[NSString stringWithFormat:@"ID_CONNECTION_REQUEST_ACCEPTED to %s with GUID %s", p->systemAddress.ToString(true,':'), p->guid.ToString()]];
				[self appendMessage:[NSString stringWithFormat:@"My external address is %s", mRakPeer->GetExternalID(p->systemAddress).ToString(true, ':')]];
				break;
				
			default:
				// It's a client, so just show the message
				[self appendMessage:[NSString stringWithCString:(const char*)p->data encoding:NSASCIIStringEncoding]];
				break;
        }
    }
    
}


#pragma mark - View lifecycle

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Release any cached data, images, etc that aren't in use.
}

- (void) dealloc
{
	[mSendText release];
	[mTextBox release];
	
	[super dealloc];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
    
	[self shutdownChatClient];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
	[super viewWillDisappear:animated];
}

- (void)viewDidDisappear:(BOOL)animated
{
	[super viewDidDisappear:animated];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
    } else {
        return YES;
    }
}


@end
