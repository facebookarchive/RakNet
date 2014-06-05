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

#import <UIKit/UIKit.h>

// RakNet headers
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "PacketLogger.h"
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include "RakNetTypes.h"

// For simplicity, this sample doesn't support secure connections
#if LIBCAT_SECURITY==1
#error RakNet secure connections not supported for this sample. If you wish to add support, check the "Chat Example Client" sample
#endif

#import "ChatServerDetailsProtocol.h"

@interface ViewController : UIViewController<ChatServerDetailsProtocol>
{
    RakNet::RakPeerInterface *mRakPeer;
    UITextField *mSendText;
    UITextView *mTextBox;
}

-(void)appendMessage:(NSString*)message;
-(IBAction)sendMessage;
-(void)tickClient; // Updates the client

@property (nonatomic, retain) IBOutlet UITextField *mSendText;
@property (nonatomic, retain) IBOutlet UITextView *mTextBox;

@end
