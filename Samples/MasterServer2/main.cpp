/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include <WinSock2.h>
#include <windows.h>
#include <Ws2tcpip.h>
#include <stdio.h>

void main_sockets(void)
{
	WSADATA winsockInfo;
	WSAStartup( MAKEWORD( 2, 2 ), &winsockInfo );
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in serverAddr;
	memset(&serverAddr,0,sizeof(sockaddr_in));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = 0;
	int j = bind(sock,(struct sockaddr *) &serverAddr,sizeof(serverAddr));
	struct hostent * phe = gethostbyname( "masterserver2.raknet.com" );
	memcpy( &serverAddr.sin_addr.s_addr, phe->h_addr_list[ 0 ], sizeof( struct in_addr ) );
	serverAddr.sin_port        = htons(80);
	connect(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
	const char *postRequest =
		"POST /testServer HTTP/1.1\r\n"
		"Content-Length: 83\r\n"
		"Content-Type: text/plain; charset=UTF-8\r\n"
		"Host: masterserver2.raknet.com\r\n"
		"Connection: Keep-Alive\r\n"
		"\r\n"
		"{'__gameId': 'myGame','__clientReqId': '0','__timeoutSec': '60','mapname': 'myMap'}\r\n";
	send(sock, postRequest, strlen(postRequest), 0);
	char outputBuffer[512];
	memset(outputBuffer,0,512);
	recv(sock, outputBuffer, 512, 0);
	printf(outputBuffer);
}

#include "TCPInterface.h"
#include "RakString.h"
#include "RakSleep.h"
#include "jansson.h"
#include "GetTime.h"

#define MASTER_SERVER_ADDRESS "masterserver2.raknet.com"
#define MASTER_SERVER_PORT 80
//#define MASTER_SERVER_ADDRESS "localhost"
//#define MASTER_SERVER_PORT 8080
using namespace RakNet;
void main_RakNet_Post(void)
{
	TCPInterface *tcp = RakNet::OP_NEW<TCPInterface>(__FILE__,__LINE__);
	tcp->Start(0, 64);
	tcp->Connect(MASTER_SERVER_ADDRESS, MASTER_SERVER_PORT, true);

	json_t *jsonObject = json_object();
	json_object_set(jsonObject, "__gameId", json_string("MotoGP_13") );
	json_object_set(jsonObject, "__clientReqId", json_integer(0) );
	json_object_set(jsonObject, "__timeoutSec", json_integer(60) );
	//json_object_set(jsonObject, "mapname", json_string(RakString::NonVariadic("Joué-lés-tours").URLEncode().C_String()) );

	char *ds = json_dumps(jsonObject,0);
	RakString rspost = RakString::FormatForPOST(
		RakString(MASTER_SERVER_ADDRESS "/testServer"),
		RakString("application/json; charset=UTF-8"),
		ds);
	json_decref(jsonObject);

	RakSleep(100);
	SystemAddress serverAddr = tcp->HasCompletedConnectionAttempt();
	tcp->Send(rspost.C_String(), rspost.GetLength(), serverAddr, false);

	RakNet::Time timeout = RakNet::GetTime()+2000;
	while (RakNet::GetTime() < timeout) 
	{
		Packet *p = tcp->Receive();
		if (p)
		{
			printf((const char*) p->data);
			break;
		}

		RakSleep(30);
	}
	
	//if (p) printf((const char*) p->data);
	tcp->Stop();
}

#include "HTTPConnection2.h"
void main_RakNet_Get(void)
{
	HTTPConnection2 *httpConnection2;
	httpConnection2 = HTTPConnection2::GetInstance();

	TCPInterface *tcp = RakNet::OP_NEW<TCPInterface>(__FILE__,__LINE__);
	tcp->Start(0, 64);
	tcp->AttachPlugin(httpConnection2);

	// tcp->Connect("masterserver2.raknet.com", MASTER_SERVER_PORT, true);
	RakString rsRequest = RakString::FormatForGET(
		RakString(MASTER_SERVER_ADDRESS "/testServer?__gameId=MotoGP_13"));
	httpConnection2->TransmitRequest(rsRequest, MASTER_SERVER_ADDRESS, MASTER_SERVER_PORT);

	while (1)
	{
		// The following code is TCP operations for talking to the master server, and parsing the reply
		SystemAddress sa;
		Packet *packet;
		// This is kind of crappy, but for TCP plugins, always do HasCompletedConnectionAttempt, then Receive(), then HasFailedConnectionAttempt(),HasLostConnection()
		sa = tcp->HasCompletedConnectionAttempt();
		if (sa != UNASSIGNED_SYSTEM_ADDRESS)
		{
			printf("Connected to %s\n", sa.ToString());
		}
		for (packet = tcp->Receive(); packet; tcp->DeallocatePacket(packet), packet = tcp->Receive())
			;
		sa = tcp->HasFailedConnectionAttempt();
		if (sa != UNASSIGNED_SYSTEM_ADDRESS)
		{
			printf("Failed to connect to %s\n", sa.ToString());
		}
		sa = tcp->HasLostConnection();
		if (sa != UNASSIGNED_SYSTEM_ADDRESS)
		{
			printf("Lost connection to %s\n", sa.ToString());
		}

		RakString stringTransmitted;
		RakString hostTransmitted;
		RakString responseReceived;
		SystemAddress hostReceived;
		int contentOffset;
		if (httpConnection2->GetResponse(stringTransmitted, hostTransmitted, responseReceived, hostReceived, contentOffset))
		{
			if (responseReceived.IsEmpty()==false)
			{
				if (contentOffset==-1)
				{
					// No content
					printf(responseReceived.C_String());
				}
				else
				{
//					printf("\n--------------\n\n");
//					printf(responseReceived.C_String());
					json_error_t error;
					json_t *root = json_loads(responseReceived.C_String() + contentOffset, JSON_REJECT_DUPLICATES, &error);
					if (!root)
					{
						printf("Error parsing JSON\n", __LINE__);
						printf(responseReceived.C_String());
					}
					else
					{
						void *iter = json_object_iter(root);
						while (iter)
						{
							const char *firstKey = json_object_iter_key(iter);
							if (stricmp(firstKey, "GET")==0)
							{
								json_t* jsonArray = json_object_iter_value(iter);
								size_t arraySize = json_array_size(jsonArray);
								for (unsigned int i=0; i < arraySize; i++)
								{
									json_t* object = json_array_get(jsonArray, i);
									json_t* gameIdVal = json_object_get(object, "__gameId");
									//RakAssert(cityNameVal->type==JSON_STRING);
									//RakString val = RakString::NonVariadic(json_string_value(cityNameVal)).URLDecode();

									printf("gameId: %i.\n", json_integer_value(gameIdVal));
								}

								if (arraySize==0)
									printf("No results.\n");

								break;
							}
							else if (stricmp(firstKey, "POST")==0)
							{
								
								break;
							}
							else
							{
								iter = json_object_iter_next(root, iter);
								RakAssert(iter != 0);
							}
						}

						json_decref(root);
					}
				}
			}
		}

		RakSleep(30);
	}
}


void main(void)
{
//	main_sockets();
	main_RakNet_Post();
//	main_RakNet_Get();
}
