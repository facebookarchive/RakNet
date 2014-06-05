/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "Lobby2Presence.h"
#include "BitStream.h"

using namespace RakNet;


Lobby2Presence::Lobby2Presence() {
	status=UNDEFINED;
	isVisible=true;
}
Lobby2Presence::Lobby2Presence(const Lobby2Presence& input) {
	status=input.status;
	isVisible=input.isVisible;
	titleNameOrID=input.titleNameOrID;



	statusString=input.statusString;
}
Lobby2Presence& Lobby2Presence::operator = ( const Lobby2Presence& input )
{
	status=input.status;
	isVisible=input.isVisible;
	titleNameOrID=input.titleNameOrID;



	statusString=input.statusString;
	return *this;
}

Lobby2Presence::~Lobby2Presence()
{

}
void Lobby2Presence::Serialize(RakNet::BitStream *bitStream, bool writeToBitstream)
{
	unsigned char gs = (unsigned char) status;
	bitStream->Serialize(writeToBitstream,gs);
	status=(Status) gs;
	bitStream->Serialize(writeToBitstream,isVisible);
	bitStream->Serialize(writeToBitstream,titleNameOrID);



	bitStream->Serialize(writeToBitstream,statusString);
}
