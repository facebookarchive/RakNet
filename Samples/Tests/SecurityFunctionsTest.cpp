/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "SecurityFunctionsTest.h"

/*
Description:

Tests:
virtual void RakPeerInterface::AddToSecurityExceptionList  	(  	const char *   	 ip  	 )   	
virtual void RakPeerInterface::AddToBanList  	(  	const char *   	 IP, 		TimeMS  	milliseconds = 0	  	) 	
virtual void RakPeerInterface::GetIncomingPassword  	(  	char *   	 passwordData, 		int *  	passwordDataLength	  	) 
virtual void RakPeerInterface::InitializeSecurity  	(  	const char *   	 pubKeyE, 		const char *  	pubKeyN, 		const char *  	privKeyP, 		const char *  	privKeyQ	  	) 			
virtual bool RakPeerInterface::IsBanned  	(  	const char *   	 IP  	 )   	 
virtual bool RakPeerInterface::IsInSecurityExceptionList  	(  	const char *   	 ip  	 )  
virtual void RakPeerInterface::RemoveFromSecurityExceptionList  	(  	const char *   	 ip  	 )  
virtual void RakPeerInterface::RemoveFromBanList  	(  	const char *   	 IP  	 )   
virtual void RakPeerInterface::SetIncomingPassword  	(  	const char *   	 passwordData, 		int  	passwordDataLength	  	) 	
virtual void 	ClearBanList (void)=0

Success conditions:
All functions pass tests.

Failure conditions:
Any function fails test.

Client connects with no password
Client connects with wrong password
Client failed to connect with correct password
Client was banned but connected anyways
GetIncomingPassword returned wrong password
IsBanned does not show localhost as banned
Localhost was not unbanned
Client failed to connect after banlist removal
Client failed to connect after banlist removal with clear function
Client did not connect encrypted
Client connected encrypted but shouldn't have
IsInSecurityExceptionList does not register localhost addition

RakPeerInterface Functions used, tested indirectly by its use:
Startup
SetMaximumIncomingConnections
Receive
DeallocatePacket
Send
IsConnected
GetStatistics

RakPeerInterface Functions Explicitly Tested:
SetIncomingPassword  
GetIncomingPassword  
AddToBanList
IsBanned
RemoveFromBanList  
ClearBanList 
InitializeSecurity  //Disabled because of RakNetStatistics changes 		 		
AddToSecurityExceptionList  //Disabled because of RakNetStatistics changes 		   	 
IsInSecurityExceptionList //Disabled because of RakNetStatistics changes 	  	  
RemoveFromSecurityExceptionList //Disabled because of RakNetStatistics changes 	  

*/
int SecurityFunctionsTest::RunTest(DataStructures::List<RakString> params,bool isVerbose,bool noPauses)
{

	char thePassword[]="password";
	server=RakPeerInterface::GetInstance();

	client=RakPeerInterface::GetInstance();

	client->Startup(1,&SocketDescriptor(),1);
	server->Startup(1,&SocketDescriptor(60000,0),1);
	server->SetMaximumIncomingConnections(1);
	server->SetIncomingPassword(thePassword,(int)strlen(thePassword));

	char returnedPass[22];
	int returnedLen=22;
	server->GetIncomingPassword(returnedPass,&returnedLen);
	returnedPass[returnedLen]=0;//Password is a data block convert to null terminated string to make the test easier

	if (strcmp(returnedPass,thePassword)!=0)
	{
		if (isVerbose)
		{

			printf("%s was returned but %s is the password\n",returnedPass,thePassword);
			DebugTools::ShowError("GetIncomingPassword returned wrong password\n",!noPauses && isVerbose,__LINE__,__FILE__);
		}
		return 5;
	}

	SystemAddress serverAddress;

	serverAddress.SetBinaryAddress("127.0.0.1");
	serverAddress.port=60000;
	TimeMS entryTime=GetTimeMS();

	if (isVerbose)
		printf("Testing if  no password is rejected\n");

	while(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true)&&GetTimeMS()-entryTime<5000)
	{

		if(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))
		{
			client->Connect("127.0.0.1",serverAddress.port,0,0);
		}

		RakSleep(100);

	}

	if (CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true))
	{
		if (isVerbose)
			DebugTools::ShowError("Client connected with no password\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 1;
	}

	if (isVerbose)
		printf("Testing if incorrect password is rejected\n");

	char badPass[]="badpass";
	entryTime=GetTimeMS();
	while(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true)&&GetTimeMS()-entryTime<5000)
	{

		if(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))
		{
			client->Connect("127.0.0.1",serverAddress.port,badPass,(int)strlen(badPass));
		}

		RakSleep(100);

	}

	if (CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true))
	{
		if (isVerbose)
			DebugTools::ShowError("Client connected with wrong password\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 2;
	}

	if (isVerbose)
		printf("Testing if correct password is accepted\n");

	entryTime=GetTimeMS();
	while(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true)&&GetTimeMS()-entryTime<5000)
	{

		if(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))
		{
			client->Connect("127.0.0.1",serverAddress.port,thePassword,(int)strlen(thePassword));
		}

		RakSleep(100);

	}

	if (!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true))
	{
		if (isVerbose)
			DebugTools::ShowError("Client failed to connect with correct password\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 3;
	}

	while(CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))//disconnect client
	{

		client->CloseConnection (serverAddress,true,0,LOW_PRIORITY); 
	}

	if (isVerbose)
		printf("Testing if connection is rejected after adding to ban list\n");

	server->AddToBanList("127.0.0.1",0);

	entryTime=GetTimeMS();
	while(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true)&&GetTimeMS()-entryTime<5000)
	{

		if(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))
		{
			client->Connect("127.0.0.1",serverAddress.port,thePassword,(int)strlen(thePassword));
		}

		RakSleep(100);

	}

	if(!server->IsBanned("127.0.0.1"))
	{

		if (isVerbose)
			DebugTools::ShowError("IsBanned does not show localhost as banned\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 6;

	}

	if (CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true))
	{
		if (isVerbose)
			DebugTools::ShowError("Client was banned but connected anyways\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 4;
	}

	if (isVerbose)
		printf("Testing if connection is accepted after ban removal by RemoveFromBanList\n");

	server->RemoveFromBanList("127.0.0.1");
	if(server->IsBanned("127.0.0.1"))
	{

		if (isVerbose)
			DebugTools::ShowError("Localhost was not unbanned\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 7;

	}

	entryTime=GetTimeMS();
	while(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true)&&GetTimeMS()-entryTime<5000)
	{

		if(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))
		{
			client->Connect("127.0.0.1",serverAddress.port,thePassword,(int)strlen(thePassword));
		}

		RakSleep(100);

	}

	if (!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true))
	{
		if (isVerbose)
			DebugTools::ShowError("Client failed to connect after banlist removal\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 8;
	}

	while(CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))//disconnect client
	{

		client->CloseConnection (serverAddress,true,0,LOW_PRIORITY); 
	}

	if (isVerbose)
		printf("Testing if connection is rejected after adding to ban list\n");

	server->AddToBanList("127.0.0.1",0);

	entryTime=GetTimeMS();
	while(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true)&&GetTimeMS()-entryTime<5000)
	{

		if(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))
		{
			client->Connect("127.0.0.1",serverAddress.port,thePassword,(int)strlen(thePassword));
		}

		RakSleep(100);

	}

	if(!server->IsBanned("127.0.0.1"))
	{

		if (isVerbose)
			DebugTools::ShowError("IsBanned does not show localhost as banned\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 6;

	}

	if (CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true))
	{
		if (isVerbose)
			DebugTools::ShowError("Client was banned but connected anyways\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 4;
	}

	if (isVerbose)
		printf("Testing if connection is accepted after ban removal by ClearBanList\n");

	server->ClearBanList();
	if(server->IsBanned("127.0.0.1"))
	{

		if (isVerbose)
			DebugTools::ShowError("Localhost was not unbanned\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 7;

	}

	entryTime=GetTimeMS();
	while(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true)&&GetTimeMS()-entryTime<5000)
	{

		if(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))
		{
			client->Connect("127.0.0.1",serverAddress.port,thePassword,(int)strlen(thePassword));
		}

		RakSleep(100);

	}

	if (!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true))
	{
		if (isVerbose)
			DebugTools::ShowError("Client failed to connect after banlist removal with clear function\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 9;
	}

	while(CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))//disconnect client
	{

		client->CloseConnection (serverAddress,true,0,LOW_PRIORITY); 
	}

/*//Disabled because of statistics changes

	if (isVerbose)
		printf("Testing InitializeSecurity on server\n");

	//-----------------------------

	// RSACrypt is a using namespace RakNet;
class that handles RSA encryption/decryption internally

	RSACrypt rsacrypt;

	uint32_t e;
	uint32_t modulus[RAKNET_RSA_FACTOR_LIMBS];

	uint32_t p[RAKNET_RSA_FACTOR_LIMBS/2],q[RAKNET_RSA_FACTOR_LIMBS/2];

	printf("Generating %i bit key. This will take a while...\n", RAKNET_RSA_FACTOR_LIMBS*32);
	rsacrypt.generatePrivateKey(RAKNET_RSA_FACTOR_LIMBS);
	e=rsacrypt.getPublicExponent();
	rsacrypt.getPublicModulus(modulus);
	rsacrypt.getPrivateP(p);
	rsacrypt.getPrivateQ(q);

	RakPeerInterface::DestroyInstance(server);
	server=RakPeerInterface::GetInstance();

	server->InitializeSecurity(0,0,(char*)p, (char*)q);
	server->Startup(1,30,&SocketDescriptor(60000,0),1);
	server->SetMaximumIncomingConnections(1);
	server->SetIncomingPassword(thePassword,strlen(thePassword));

	if (isVerbose)
		printf("Testing if client connects encrypted\n");

	entryTime=GetTimeMS();
	while(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true)&&GetTimeMS()-entryTime<5000)
	{

		if(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))
		{
			client->Connect("127.0.0.1",serverAddress.port,thePassword,strlen(thePassword));
		}

		RakSleep(100);

	}

	char str2[]="AAAAAAAAAA";
	str2[0]=(char)(ID_USER_PACKET_ENUM+1);
	client->Send(str2,(int) strlen(str2)+1, HIGH_PRIORITY, RELIABLE_ORDERED ,0, UNASSIGNED_SYSTEM_ADDRESS, true);
	client->Send(str2,(int) strlen(str2)+1, HIGH_PRIORITY, RELIABLE_ORDERED ,0, UNASSIGNED_SYSTEM_ADDRESS, true);

	Packet *packet;
	entryTime=GetTimeMS();
	while(GetTimeMS()-entryTime<1000)
	{
		for (packet=server->Receive(); packet;server->DeallocatePacket(packet), packet=server->Receive())
		{

		}
	}

	RakNetStatistics *rss;

	rss=client->GetStatistics(serverAddress);

	if (rss->encryptionBitsSent<=0)//If we did connect encrypted we should see encryptionBitsSent
	{
		if (isVerbose)
			DebugTools::ShowError("Client did not connect encrypted\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 10;
	}

	while(CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))//disconnect client
	{

		client->CloseConnection (serverAddress,true,0,LOW_PRIORITY); 
	}

	//Destroy to clear statistics
	RakPeerInterface::DestroyInstance(client);

	client=RakPeerInterface::GetInstance();

	client->Startup(1,30,&SocketDescriptor(),1);

	if (isVerbose)
		printf("Testing AddToSecurityExceptionList client should connect without encryption\n");

	server->AddToSecurityExceptionList("127.0.0.1");

	if (!server->IsInSecurityExceptionList("127.0.0.1"))
	{
		if (isVerbose)
			DebugTools::ShowError("IsInSecurityExceptionList does not register localhost addition\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 12;
	}

	entryTime=GetTimeMS();
	while(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true)&&GetTimeMS()-entryTime<5000)
	{

		if(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))
		{
			client->Connect("127.0.0.1",serverAddress.port,thePassword,strlen(thePassword));
		}

		RakSleep(100);

	}

	str2[0]=(char)(ID_USER_PACKET_ENUM+1);
	client->Send(str2,(int) strlen(str2)+1, HIGH_PRIORITY, RELIABLE_ORDERED ,0, UNASSIGNED_SYSTEM_ADDRESS, true);
	client->Send(str2,(int) strlen(str2)+1, HIGH_PRIORITY, RELIABLE_ORDERED ,0, UNASSIGNED_SYSTEM_ADDRESS, true);

	//	Packet *packet;

	entryTime=GetTimeMS();
	while(GetTimeMS()-entryTime<1000)
	{
		for (packet=server->Receive(); packet;server->DeallocatePacket(packet), packet=server->Receive())
		{

		}
	}

	rss=client->GetStatistics(serverAddress);

	if (rss->encryptionBitsSent>0)//If we did connect encrypted we should see encryptionBitsSent
	{
		if (isVerbose)
			DebugTools::ShowError("Client connected encrypted but shouldn't have\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 11;
	}

	if (isVerbose)
		printf("Testing RemoveFromSecurityExceptionList\n");

	while(CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))//disconnect client
	{

		client->CloseConnection (serverAddress,true,0,LOW_PRIORITY); 
	}

	server->RemoveFromSecurityExceptionList("127.0.0.1");

	if (isVerbose)
		printf("Testing if client connects encrypted\n");

	entryTime=GetTimeMS();
	while(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true)&&GetTimeMS()-entryTime<5000)
	{

		if(!CommonFunctions::ConnectionStateMatchesOptions (client,serverAddress,true,true,true,true))
		{
			client->Connect("127.0.0.1",serverAddress.port,thePassword,strlen(thePassword));
		}

		RakSleep(100);

	}

	str2[0]=(char)(ID_USER_PACKET_ENUM+1);
	client->Send(str2,(int) strlen(str2)+1, HIGH_PRIORITY, RELIABLE_ORDERED ,0, UNASSIGNED_SYSTEM_ADDRESS, true);
	client->Send(str2,(int) strlen(str2)+1, HIGH_PRIORITY, RELIABLE_ORDERED ,0, UNASSIGNED_SYSTEM_ADDRESS, true);

	entryTime=GetTimeMS();
	while(GetTimeMS()-entryTime<1000)
	{
		for (packet=server->Receive(); packet;server->DeallocatePacket(packet), packet=server->Receive())
		{

		}
	}

	rss=client->GetStatistics(serverAddress);

	if (rss->encryptionBitsSent<=0)//If we did connect encrypted we should see encryptionBitsSent
	{
		if (isVerbose)
			DebugTools::ShowError("Client did not connect encrypted\n",!noPauses && isVerbose,__LINE__,__FILE__);
		return 10;
	}

*/

	return 0;

}

RakString SecurityFunctionsTest::GetTestName()
{

	return "SecurityFunctionsTest";

}

RakString SecurityFunctionsTest::ErrorCodeToString(int errorCode)
{

	switch (errorCode)
	{

	case 0:
		return "No error";
		break;

	case 1:
		return "Client connected with no password";
		break;

	case 2:
		return "Client connected with wrong password";
		break;

	case 3:
		return "Client failed to connect with correct password";
		break;

	case 4:
		return "Client was banned but connected anyways";
		break;

	case 5:
		return "GetIncomingPassword returned wrong password";
		break;

	case 6:
		return "IsBanned does not show localhost as banned";
		break;

	case 7:
		return "Localhost was not unbanned";
		break;

	case 8:
		return "Client failed to connect after banlist removal";
		break;

	case 9:
		return "Client failed to connect after banlist removal with clear function";
		break;

	case 10:
		return "Client did not connect encrypted";
		break;

	case 11:
		return "Client connected encrypted but shouldn't have";
		break;

	case 12:
		return "IsInSecurityExceptionList does not register localhost addition";
		break;

	default:
		return "Undefined Error";
	}

}

SecurityFunctionsTest::SecurityFunctionsTest(void)
{
}

SecurityFunctionsTest::~SecurityFunctionsTest(void)
{
}

void SecurityFunctionsTest::DestroyPeers()
{

RakPeerInterface::DestroyInstance(client);
RakPeerInterface::DestroyInstance(server);

}
