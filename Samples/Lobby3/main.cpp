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
#include "Gets.h"

#include "TCPInterface.h"
#include "RakString.h"
#include "RakSleep.h"
#include "DR_SHA1.h"
#include "BitStream.h"

#include "Base64Encoder.h"

// See http://www.digip.org/jansson/doc/2.4/
// This is used to make it easier to parse the JSON returned from the master server
#include "jansson.h"


using namespace RakNet;

void main_RakNet(void)
{
	const char *serverURL = "localhost";
	//const char *serverURL = "lobby3.raknet.com";
	//const unsigned int serverPort=80;
	//const unsigned int serverPort=8888;
	//const bool useSSL=true;
	const bool useSSL=false;
	//const unsigned int serverPort=443;
	const unsigned int serverPort=8080;



	json_t *jsonObject = json_object();
	json_object_set(jsonObject, "__devId", json_string("defaultDevId1"));
	json_object_set(jsonObject, "__userId", json_string("defaultUserId1"));
	json_object_set(jsonObject, "__userPw", json_string("defaultPw"));
	json_object_set(jsonObject, "__appId", json_string("defaultAppId1"));
	json_object_set(jsonObject, "__customTableId", json_string("defaultCustomTableId"));
	json_object_set(jsonObject, "__timeToLiveSec", json_integer(0));
	json_object_set(jsonObject, "__timeToIdleSec", json_integer(6000));
	json_object_set(jsonObject, "__key", json_integer(0));
	json_object_set(jsonObject, "__mergeMode", json_string("OVERWRITE_EXISTING"));
	//json_object_set(jsonObject, "__autoFields", json_string("svrTimestamp,svrIP,svrSerial,svrGeoIP"));
	json_object_set(jsonObject, "__fieldMetadata", json_string("sampleField1Key(_ownerRW,_putMin),sampleField2Key(_userRW,_putSum)"));
	json_object_set(jsonObject, "__protocol", json_integer(0));
	json_object_set(jsonObject, "sampleField1Key", json_integer(1));
	json_object_set(jsonObject, "sampleField2Key", json_integer(2));

	// JSON_COMPACT is required or it won't match json-lib
	char *jsonStr = json_dumps(jsonObject, JSON_COMPACT | JSON_PRESERVE_ORDER);
	printf(jsonStr);


	// For testing, see http://hash.online-convert.com/sha1-generator

	const char *__sharedKey="defaultSharedKey";
	unsigned char output[SHA1_LENGTH];
	CSHA1::HMAC((unsigned char*) __sharedKey, strlen(__sharedKey), (unsigned char*) jsonStr, strlen(jsonStr), output);
	char outputBase64[SHA1_LENGTH*2+6];
	int bytesWritten = Base64Encoding(output, sizeof(output), outputBase64);
	//outputBase64[bytesWritten]=0;
	json_object_set(jsonObject, "__hash", json_string(outputBase64));
	jsonStr = json_dumps(jsonObject, JSON_COMPACT | JSON_PRESERVE_ORDER);

	// GAE SSL https://developers.google.com/appengine/docs/ssl
	char URI[128];
	sprintf(URI, "%s/customTable/update", serverURL);
	TCPInterface *tcp = RakNet::OP_NEW<TCPInterface>(__FILE__,__LINE__); // Requires build with OPEN_SSL_CLIENT_SUPPORT
	tcp->Start(0, 64);
	tcp->Connect(serverURL, serverPort, true);
	RakString rspost = RakString::FormatForPOST(
		URI,
		RakString("text/plain; charset=UTF-8"),
		jsonStr
		);

	RakSleep(100);
	SystemAddress serverAddr = tcp->HasCompletedConnectionAttempt();
	RakAssert(serverAddr!=UNASSIGNED_SYSTEM_ADDRESS);
	if (useSSL)
		tcp->StartSSLClient(serverAddr);
	tcp->Send(rspost.C_String(), rspost.GetLength(), serverAddr, false);
	RakSleep(1000);
	Packet *p;
	
	while (1)
	{
		p = tcp->Receive();
		if (p)
		{
			printf((const char*) p->data);
			break;
		}
	}
}

void main(void)
{
//	main_sockets();
	main_RakNet();
}
