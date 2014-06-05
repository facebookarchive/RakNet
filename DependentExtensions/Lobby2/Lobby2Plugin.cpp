/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "Lobby2Plugin.h"

using namespace RakNet;

Lobby2Plugin::Lobby2Plugin()
{
	orderingChannel=0;
	packetPriority=HIGH_PRIORITY;
}
Lobby2Plugin::~Lobby2Plugin()
{

}
void Lobby2Plugin::SetOrderingChannel(char oc)
{
	orderingChannel=oc;
}
void Lobby2Plugin::SetSendPriority(PacketPriority pp)
{
	packetPriority=pp;
}
void Lobby2Plugin::SetMessageFactory(Lobby2MessageFactory *f)
{
	msgFactory=f;
}
Lobby2MessageFactory* Lobby2Plugin::GetMessageFactory(void) const
{
	return msgFactory;
}
void Lobby2Plugin::SetCallbackInterface(Lobby2Callbacks *cb)
{
	ClearCallbackInterfaces();
	callbacks.Insert(cb, _FILE_AND_LINE_ );
}
void Lobby2Plugin::AddCallbackInterface(Lobby2Callbacks *cb)
{
	RemoveCallbackInterface(cb);
	callbacks.Insert(cb, _FILE_AND_LINE_ );
}
void Lobby2Plugin::RemoveCallbackInterface(Lobby2Callbacks *cb)
{
	unsigned long index = callbacks.GetIndexOf(cb);
	if (index!=MAX_UNSIGNED_LONG)
		callbacks.RemoveAtIndex(index);
}
void Lobby2Plugin::ClearCallbackInterfaces()
{
	callbacks.Clear(false, _FILE_AND_LINE_);
}