/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "CommonFunctions.h"

CommonFunctions::CommonFunctions(void)
{
}

CommonFunctions::~CommonFunctions(void)
{
}

bool CommonFunctions::ConnectionStateMatchesOptions(RakPeerInterface *peer,SystemAddress currentSystem,bool isConnected,bool isConnecting,bool isPending,bool isDisconnecting,bool isNotConnected,bool isLoopBack , bool isSilentlyDisconnecting)
{
	ConnectionState connectionState=peer->GetConnectionState(currentSystem);
	switch(connectionState)
	{
	case IS_CONNECTED:
		return isConnected;
		break;

	case IS_CONNECTING:
		return isConnecting;
		break;

	case IS_PENDING:
		return isPending;
		break;

	case IS_DISCONNECTING:
		return isDisconnecting;
		break;

	case IS_LOOPBACK:
		return isLoopBack;
		break;

	case IS_NOT_CONNECTED:
		return isNotConnected;
		break;

	case IS_SILENTLY_DISCONNECTING:
		return isSilentlyDisconnecting;
		break;

	default:
		return false;
		break;
	}
}

bool CommonFunctions::WaitAndConnect(RakPeerInterface *peer,char* ip,unsigned short int port,int millisecondsToWait)
{

	SystemAddress connectToAddress;

	connectToAddress.SetBinaryAddress(ip);
	connectToAddress.port=port;
	TimeMS entryTime=GetTimeMS();

	while(!CommonFunctions::ConnectionStateMatchesOptions (peer,connectToAddress,true)&&GetTimeMS()-entryTime<millisecondsToWait)
	{

		if(!CommonFunctions::ConnectionStateMatchesOptions (peer,connectToAddress,true,true,true,true))
		{
			peer->Connect(ip,port,0,0);
		}

		RakSleep(100);

	}

	if (ConnectionStateMatchesOptions (peer,connectToAddress,true))
	{
		return 1;
	}

	return 0;
}

void CommonFunctions::DisconnectAndWait(RakPeerInterface *peer,char* ip,unsigned short int port)
{
	SystemAddress targetAddress;

	targetAddress.SetBinaryAddress(ip);
	targetAddress.port=port;

	while(CommonFunctions::ConnectionStateMatchesOptions (peer,targetAddress,true,true,true,true))//disconnect client
	{

		peer->CloseConnection (targetAddress,true,0,LOW_PRIORITY); 
	}

}

bool CommonFunctions::WaitForMessageWithID(RakPeerInterface *reciever,int id,int millisecondsToWait)
{

	RakTimer timer(millisecondsToWait);

	Packet *packet;
	while(!timer.IsExpired())
	{
		for (packet=reciever->Receive(); packet;reciever->DeallocatePacket(packet), packet=reciever->Receive())
		{

			//printf("Packet %i\n",packet->data[0]);
			if (packet->data[0]==id)
			{
				reciever->DeallocatePacket(packet);
				return true;
			}

		}

	}

	return false;
}

Packet *CommonFunctions::WaitAndReturnMessageWithID(RakPeerInterface *reciever,int id,int millisecondsToWait)
{

	RakTimer timer(millisecondsToWait);

	Packet *packet;
	while(!timer.IsExpired())
	{
		for (packet=reciever->Receive(); packet;reciever->DeallocatePacket(packet), packet=reciever->Receive())
		{

		//	printf("Packet %i\n",packet->data[0]);
			if (packet->data[0]==id)
			{
				return packet;
			}

		}

	}

	return 0;
}