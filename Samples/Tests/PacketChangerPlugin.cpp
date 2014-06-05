/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "PacketChangerPlugin.h"

PacketChangerPlugin::PacketChangerPlugin(void)
{
}

PacketChangerPlugin::~PacketChangerPlugin(void)
{
}

void PacketChangerPlugin::OnInternalPacket(InternalPacket *internalPacket, unsigned frameNumber, SystemAddress remoteSystemAddress, TimeMS time, int isSend)
{

	internalPacket->data[0]=ID_USER_PACKET_ENUM+2;

	//(void) frameNumber; (void) remoteSystemAddress; (void) time; (void) isSend;

}