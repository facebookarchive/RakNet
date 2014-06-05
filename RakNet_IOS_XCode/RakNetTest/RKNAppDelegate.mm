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
//  RKNAppDelegate.m
//  RakNetTest
//

#import "RKNAppDelegate.h"
#include "RakPeerInterface.h"
#include "RakAssert.h"
#include "RakSleep.h"
#include "MessageIdentifiers.h"

@implementation RKNAppDelegate

- (void)dealloc
{
    [_window release];
    [super dealloc];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    RakNet::RakPeerInterface *rakPeer = RakNet::RakPeerInterface::GetInstance();
    RakNet::SocketDescriptor sd(0,0);
    RakNet::StartupResult sr = rakPeer->Startup(32, &sd, 1);
    (void) sr;
    RakAssert(sr==SR_SUCCESS);
    RakNet::ConnectionAttemptResult car = rakPeer->Connect("natpunch.jenkinssoftware.com", 61111, 0, 0);
    (void) car;
    RakSleep(1000);
    RakNet::Packet *packet;
    for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet=rakPeer->Receive())
    {
        if (packet->data[0]==ID_CONNECTION_REQUEST_ACCEPTED)
            printf("ID_CONNECTION_REQUEST_ACCEPTED");
    }
    
    // Override point for customization after application launch.
    return YES;
}
							
- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
