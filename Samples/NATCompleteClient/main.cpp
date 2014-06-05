/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "RakPeerInterface.h"
#include "RakSleep.h"
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include "Kbhit.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "NatPunchthroughClient.h"
#include "NatTypeDetectionClient.h"
#include "Getche.h"
#include "GetTime.h"
#include "Router2.h"
#include "UDPProxyClient.h"
#include "Gets.h"
#include "Itoa.h"

// To include miniupnp, see Samples\NATCompleteClient\readme.txt
#include "miniupnpc.h"
#include "upnpcommands.h"
#include "upnperrors.h"

using namespace RakNet;

#define DEFAULT_RAKPEER_PORT 50000
#define RAKPEER_PORT_STR "0"
#define DEFAULT_SERVER_PORT "61111"
#define DEFAULT_SERVER_ADDRESS "natpunch.jenkinssoftware.com"

enum SampleResult
{
	PENDING,
	FAILED,
	SUCCEEDED
};

#define SUPPORT_UPNP FAILED
#define SUPPORT_NAT_TYPE_DETECTION FAILED
#define SUPPORT_NAT_PUNCHTHROUGH PENDING
#define SUPPORT_ROUTER2 FAILED
#define SUPPORT_UDP_PROXY FAILED

struct SampleFramework
{
	virtual const char * QueryName(void)=0;
	virtual bool QueryRequiresServer(void)=0;
	virtual const char * QueryFunction(void)=0;
	virtual const char * QuerySuccess(void)=0;
	virtual bool QueryQuitOnSuccess(void)=0;
	virtual void Init(RakNet::RakPeerInterface *rakPeer)=0;
	virtual void ProcessPacket(Packet *packet)=0;
	virtual void Update(RakNet::RakPeerInterface *rakPeer)=0;
	virtual void Shutdown(RakNet::RakPeerInterface *rakPeer)=0;

	SampleResult sampleResult;
};

SystemAddress SelectAmongConnectedSystems(RakNet::RakPeerInterface *rakPeer, const char *hostName)
{
	DataStructures::List<SystemAddress> addresses;
	DataStructures::List<RakNetGUID> guids;
	rakPeer->GetSystemList(addresses, guids);
	if (addresses.Size()==0)
	{
		return RakNet::UNASSIGNED_SYSTEM_ADDRESS;
	}
	if (addresses.Size()>1)
	{
		printf("Select IP address for %s.\n", hostName);
		char buff[64];
		for (unsigned int i=0; i < addresses.Size(); i++)
		{
			addresses[i].ToString(true, buff);
			printf("%i. %s\n", i+1, buff);
		}
		Gets(buff,sizeof(buff));
		if (buff[0]==0)
		{
			return RakNet::UNASSIGNED_SYSTEM_ADDRESS;
		}
		unsigned int idx = atoi(buff);
		if (idx<=0 || idx > addresses.Size())
		{
			return RakNet::UNASSIGNED_SYSTEM_ADDRESS;
		}
		return addresses[idx-1];
	}
	else
		return addresses[0];
};
SystemAddress ConnectBlocking(RakNet::RakPeerInterface *rakPeer, const char *hostName, const char *defaultAddress, const char *defaultPort)
{
	char ipAddr[64];
	if (defaultAddress==0 || defaultAddress[0]==0)
		printf("Enter IP of system %s is running on: ", hostName);
	else
		printf("Enter IP of system %s, or press enter for default: ", hostName);
	Gets(ipAddr,sizeof(ipAddr));
	if (ipAddr[0]==0)
	{
		if (defaultAddress==0 || defaultAddress[0]==0)
		{
			printf("Failed. No address entered for %s.\n", hostName);
			return RakNet::UNASSIGNED_SYSTEM_ADDRESS;
		}
		else
		{
			strcpy(ipAddr, defaultAddress);
		}
	}
	char port[64];
	if (defaultAddress==0 || defaultAddress[0]==0)
		printf("Enter port of system %s is running on: ", hostName);
	else
		printf("Enter port of system %s, or press enter for default: ", hostName);
	Gets(port, sizeof(port));
	if (port[0]==0)
	{
		if (defaultPort==0 || defaultPort[0]==0)
		{
			printf("Failed. No port entered for %s.\n", hostName);
			return RakNet::UNASSIGNED_SYSTEM_ADDRESS;
		}
		else
		{
			strcpy(port, defaultPort);
		}
	}
	if (rakPeer->Connect(ipAddr, atoi(port), 0, 0)!=RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		printf("Failed connect call for %s.\n", hostName);
		return RakNet::UNASSIGNED_SYSTEM_ADDRESS;
	}
	printf("Connecting...\n");
	RakNet::Packet *packet;
	while (1)
	{
		for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet=rakPeer->Receive())
		{
			if (packet->data[0]==ID_CONNECTION_REQUEST_ACCEPTED)
			{
				return packet->systemAddress;
			}
			else if (packet->data[0]==ID_NO_FREE_INCOMING_CONNECTIONS)
			{
				printf("ID_NO_FREE_INCOMING_CONNECTIONS");
				return RakNet::UNASSIGNED_SYSTEM_ADDRESS;
			}
			else
			{
				return RakNet::UNASSIGNED_SYSTEM_ADDRESS;
			}
			RakSleep(100);
		}
	}
}
struct UPNPFramework : public SampleFramework
{
	UPNPFramework() { sampleResult=SUPPORT_UPNP;} 
	virtual const char * QueryName(void) {return "UPNPFramework";}
	virtual bool QueryRequiresServer(void) {return false;}
	virtual const char * QueryFunction(void) {return "Use UPNP to open the router";}
	virtual const char * QuerySuccess(void) {return "Other systems can now connect to you on the opened port.";}
	virtual bool QueryQuitOnSuccess(void) {return true;}
	virtual void Init(RakNet::RakPeerInterface *rakPeer)
	{
		if (sampleResult==FAILED) return;

		struct UPNPDev * devlist = 0;
		devlist = upnpDiscover(2000, 0, 0, 0, 0, 0);
		if (devlist)
		{
			printf("List of UPNP devices found on the network :\n");
			struct UPNPDev * device;
			for(device = devlist; device; device = device->pNext)
			{
				printf(" desc: %s\n st: %s\n\n",
					device->descURL, device->st);
			}

			char lanaddr[64];	/* my ip address on the LAN */
			struct UPNPUrls urls;
			struct IGDdatas data;
			if (UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr))==1)
			{
				// 4/16/2012 Why was I doing this? Just to read my external port? That shouldn't be necessary
				/*
				SystemAddress serverAddress=SelectAmongConnectedSystems(rakPeer, "NatTypeDetectionServer");
				if (serverAddress==RakNet::UNASSIGNED_SYSTEM_ADDRESS)
				{
					serverAddress=ConnectBlocking(rakPeer, "NatTypeDetectionServer", DEFAULT_SERVER_ADDRESS, DEFAULT_SERVER_PORT);
					if (serverAddress==RakNet::UNASSIGNED_SYSTEM_ADDRESS)
					{
						printf("Failed to connect to a server.\n");
						sampleResult=FAILED;
						return;
					}
				}


				char iport[32];
				Itoa(sockets[0]->boundAddress.GetPort(),iport,10);
				char eport[32];
				Itoa(rakPeer->GetExternalID(serverAddress).GetPort(),eport,10);
				*/

				// Use same external and internal ports
				DataStructures::List<RakNetSocket2* > sockets;
				rakPeer->GetSockets(sockets);
				char iport[32];
				Itoa(sockets[0]->GetBoundAddress().GetPort(),iport,10);
				char eport[32];
				strcpy(eport, iport);


				// Version 1.5
// 				int r = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype,
// 					 eport, iport, lanaddr, 0, "UDP", 0);

				// Version miniupnpc-1.6.20120410
				int r = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype,
					 					    eport, iport, lanaddr, 0, "UDP", 0, "0");

				if(r!=UPNPCOMMAND_SUCCESS)
					printf("AddPortMapping(%s, %s, %s) failed with code %d (%s)\n",
					eport, iport, lanaddr, r, strupnperror(r));

				char intPort[6];
				char intClient[16];

				// Version 1.5
// 				r = UPNP_GetSpecificPortMappingEntry(urls.controlURL,
// 					data.first.servicetype,
// 					eport, "UDP",
// 					intClient, intPort);

				// Version miniupnpc-1.6.20120410
				char desc[128];
				char enabled[128];
				char leaseDuration[128];
				r = UPNP_GetSpecificPortMappingEntry(urls.controlURL,
					data.first.servicetype,
					eport, "UDP",
					intClient, intPort,
					desc, enabled, leaseDuration);

				if(r!=UPNPCOMMAND_SUCCESS)
				{
					printf("GetSpecificPortMappingEntry() failed with code %d (%s)\n",
					r, strupnperror(r));
					sampleResult=FAILED;
				}
				else
					sampleResult=SUCCEEDED;
			}
			else
				sampleResult=FAILED;
		}
		else
			sampleResult=FAILED;
	}

	virtual void ProcessPacket(Packet *packet)
	{
	}
	virtual void Update(RakNet::RakPeerInterface *rakPeer)
	{
		if (sampleResult==FAILED) return;
	}
	virtual void Shutdown(RakNet::RakPeerInterface *rakPeer)
	{
	}

};
struct NatTypeDetectionFramework : public SampleFramework
{
	// Set to FAILED to skip this test
	NatTypeDetectionFramework() { sampleResult=SUPPORT_NAT_TYPE_DETECTION; ntdc=0;}
	virtual const char * QueryName(void) {return "NatTypeDetectionFramework";}
	virtual bool QueryRequiresServer(void) {return true;}
	virtual const char * QueryFunction(void) {return "Determines router type to avoid NAT punch attempts that cannot\nsucceed.";}
	virtual const char * QuerySuccess(void) {return "If our NAT type is Symmetric, we can skip NAT punch to other symmetric NATs.";}
	virtual bool QueryQuitOnSuccess(void) {return false;}
	virtual void Init(RakNet::RakPeerInterface *rakPeer)
	{
		if (sampleResult==FAILED) return;

		SystemAddress serverAddress=SelectAmongConnectedSystems(rakPeer, "NatTypeDetectionServer");
		if (serverAddress==RakNet::UNASSIGNED_SYSTEM_ADDRESS)
		{
			serverAddress=ConnectBlocking(rakPeer, "NatTypeDetectionServer", DEFAULT_SERVER_ADDRESS, DEFAULT_SERVER_PORT);
			if (serverAddress==RakNet::UNASSIGNED_SYSTEM_ADDRESS)
			{
				printf("Failed to connect to a server.\n");
				sampleResult=FAILED;
				return;
			}
		}
		ntdc = new NatTypeDetectionClient;
		rakPeer->AttachPlugin(ntdc);
		ntdc->DetectNATType(serverAddress);
		timeout=RakNet::GetTimeMS() + 5000;
	}

	virtual void ProcessPacket(Packet *packet)
	{
		if (packet->data[0]==ID_NAT_TYPE_DETECTION_RESULT)
		{
			RakNet::NATTypeDetectionResult r = (RakNet::NATTypeDetectionResult) packet->data[1];
			printf("NAT Type is %s (%s)\n", NATTypeDetectionResultToString(r), NATTypeDetectionResultToStringFriendly(r));
			printf("Using NATPunchthrough can connect to systems using:\n");
			for (int i=0; i < (int) RakNet::NAT_TYPE_COUNT; i++)
			{
				if (CanConnect(r,(RakNet::NATTypeDetectionResult)i))
				{
					if (i!=0)
						printf(", ");
					printf("%s", NATTypeDetectionResultToString((RakNet::NATTypeDetectionResult)i));
				}
			}
			printf("\n");
			if (r==RakNet::NAT_TYPE_PORT_RESTRICTED || r==RakNet::NAT_TYPE_SYMMETRIC)
			{
				// For UPNP, see Samples\UDPProxy
				printf("Note: Your router must support UPNP or have the user manually forward ports.\n");
				printf("Otherwise NATPunchthrough may not always succeed.\n");
			}

			sampleResult=SUCCEEDED;
		}
	}
	virtual void Update(RakNet::RakPeerInterface *rakPeer)
	{
		if (sampleResult==FAILED) return;

		if (sampleResult==PENDING && RakNet::GetTimeMS()>timeout)
		{
			printf("No response from the server, probably not running NatTypeDetectionServer plugin.\n");
			sampleResult=FAILED;
		}
	}
	virtual void Shutdown(RakNet::RakPeerInterface *rakPeer)
	{
		delete ntdc;
		ntdc=0;
	}

	NatTypeDetectionClient *ntdc;
	RakNet::TimeMS timeout;
};

struct NatPunchthoughClientFramework : public SampleFramework, public NatPunchthroughDebugInterface_Printf
{
	SystemAddress serverAddress;

	// Set to FAILED to skip this test
	NatPunchthoughClientFramework() { sampleResult=SUPPORT_NAT_PUNCHTHROUGH; npClient=0;}
	virtual const char * QueryName(void) {return "NatPunchthoughClientFramework";}
	virtual bool QueryRequiresServer(void) {return true;}
	virtual const char * QueryFunction(void) {return "Causes two systems to try to connect to each other at the same\ntime, to get through routers.";}
	virtual const char * QuerySuccess(void) {return "We can now communicate with the other system, including connecting.";}
	virtual bool QueryQuitOnSuccess(void) {return true;}
	virtual void Init(RakNet::RakPeerInterface *rakPeer)
	{
		if (sampleResult==FAILED) return;

		serverAddress=SelectAmongConnectedSystems(rakPeer, "NatPunchthroughServer");
		if (serverAddress==RakNet::UNASSIGNED_SYSTEM_ADDRESS)
		{
			serverAddress=ConnectBlocking(rakPeer, "NatPunchthroughServer", DEFAULT_SERVER_ADDRESS, DEFAULT_SERVER_PORT);
			if (serverAddress==RakNet::UNASSIGNED_SYSTEM_ADDRESS)
			{
				printf("Failed to connect to a server.\n");
				sampleResult=FAILED;
				return;
			}
		}

		npClient = new NatPunchthroughClient;
		npClient->SetDebugInterface(this);
		rakPeer->AttachPlugin(npClient);


		char guid[128];
		printf("Enter RakNetGuid of the remote system, which should have already connected\nto the server.\nOr press enter to just listen.\n");
		Gets(guid,sizeof(guid));
		if (guid[0])
		{
			RakNetGUID remoteSystemGuid;
			remoteSystemGuid.FromString(guid);
			npClient->OpenNAT(remoteSystemGuid, serverAddress);
			isListening=false;

			timeout=RakNet::GetTimeMS() + 10000;
		}
		else
		{
			printf("Listening\n");
			printf("My GUID is %s\n", rakPeer->GetMyGUID().ToString());
			isListening=true;

			// Find the stride of our router in advance
			npClient->FindRouterPortStride(serverAddress);

		}
	}

	virtual void ProcessPacket(Packet *packet)
	{
		if (
			packet->data[0]==ID_NAT_TARGET_NOT_CONNECTED ||
			packet->data[0]==ID_NAT_TARGET_UNRESPONSIVE ||
			packet->data[0]==ID_NAT_CONNECTION_TO_TARGET_LOST ||
			packet->data[0]==ID_NAT_PUNCHTHROUGH_FAILED
			)
		{
			RakNetGUID guid;
			if (packet->data[0]==ID_NAT_PUNCHTHROUGH_FAILED)
			{
				guid=packet->guid;
			}
			else
			{
				RakNet::BitStream bs(packet->data,packet->length,false);
				bs.IgnoreBytes(1);
				bool b = bs.Read(guid);
				RakAssert(b);
			}

			switch (packet->data[0])
			{
			case ID_NAT_TARGET_NOT_CONNECTED:
				printf("Failed: ID_NAT_TARGET_NOT_CONNECTED\n");
				break;
			case ID_NAT_TARGET_UNRESPONSIVE:
				printf("Failed: ID_NAT_TARGET_UNRESPONSIVE\n");
				break;
			case ID_NAT_CONNECTION_TO_TARGET_LOST:
				printf("Failed: ID_NAT_CONNECTION_TO_TARGET_LOST\n");
				break;
			case ID_NAT_PUNCHTHROUGH_FAILED:
				printf("Failed: ID_NAT_PUNCHTHROUGH_FAILED\n");
				break;
			}

			sampleResult=FAILED;
			return;
		}
		else if (packet->data[0]==ID_NAT_PUNCHTHROUGH_SUCCEEDED)
		{
			unsigned char weAreTheSender = packet->data[1];
			if (weAreTheSender)
				printf("NAT punch success to remote system %s.\n", packet->systemAddress.ToString(true));
			else
				printf("NAT punch success from remote system %s.\n", packet->systemAddress.ToString(true));

			char guid[128];
			printf("Enter RakNetGuid of the remote system, which should have already connected.\nOr press enter to quit.\n");
			Gets(guid,sizeof(guid));
			if (guid[0])
			{
				RakNetGUID remoteSystemGuid;
				remoteSystemGuid.FromString(guid);
				npClient->OpenNAT(remoteSystemGuid, serverAddress);
			
				timeout=RakNet::GetTimeMS() + 10000;
			}
			else
			{
				sampleResult=SUCCEEDED;
			}
		}
	}
	virtual void Update(RakNet::RakPeerInterface *rakPeer)
	{
		if (sampleResult==FAILED) return;

		if (sampleResult==PENDING && RakNet::GetTimeMS()>timeout && isListening==false)
		{
			printf("No response from the server, probably not running NatPunchthroughServer plugin.\n");
			sampleResult=FAILED;
		}
	}
	virtual void Shutdown(RakNet::RakPeerInterface *rakPeer)
	{
		delete npClient;
		npClient=0;
	}

	NatPunchthroughClient *npClient;
	RakNet::TimeMS timeout;
	bool isListening;
};

struct Router2Framework : public SampleFramework
{
	// Set to FAILED to skip this test
	Router2Framework() { sampleResult=SUPPORT_ROUTER2; router2=0;}
	virtual const char * QueryName(void) {return "Router2Framework";}
	virtual bool QueryRequiresServer(void) {return false;}
	virtual const char * QueryFunction(void) {return "Connect to a peer we cannot directly connect to using the\nbandwidth of a shared peer.";}
	virtual const char * QuerySuccess(void) {return "Router2 assumes we will now connect to the other system.";}
	virtual bool QueryQuitOnSuccess(void) {return true;}
	virtual void Init(RakNet::RakPeerInterface *rakPeer)
	{
		if (sampleResult==FAILED) return;

		printf("Given your application's bandwidth, how much traffic can be forwarded through a single peer?\nIf you use more than half the available bandwidth, then this plugin won't work for you.\n");;
		char supportedStr[64];
		do 
		{
			printf("Enter a number greater than or equal to 0: ");
			Gets(supportedStr,sizeof(supportedStr));
		} while (supportedStr[0]==0);
		int supported=atoi(supportedStr);
		if (supported<=0)
		{
			printf("Aborting Router2\n");
			sampleResult=FAILED;
			return;
		}

		SystemAddress peerAddress = SelectAmongConnectedSystems(rakPeer, "shared peer");
		if (peerAddress==RakNet::UNASSIGNED_SYSTEM_ADDRESS)
		{
			peerAddress=ConnectBlocking(rakPeer, "shared peer", "", RAKPEER_PORT_STR);
			if (peerAddress==RakNet::UNASSIGNED_SYSTEM_ADDRESS)
			{
				printf("Failed to connect to a shared peer.\n");
				sampleResult=FAILED;
				return;
			}
		}

		char guid[64];
		printf("Destination system must be connected to the shared peer.\n");
		do 
		{
			printf("Enter RakNetGUID of destination system: ");
			Gets(guid,sizeof(guid));
		} while (guid[0]==0);
		RakNetGUID endpointGuid;
		endpointGuid.FromString(guid);
		router2 = new Router2;
		rakPeer->AttachPlugin(router2);
		router2->EstablishRouting(endpointGuid);

		timeout=RakNet::GetTimeMS() + 5000;
	}
	virtual void ProcessPacket(Packet *packet)
	{
	}
	virtual void Update(RakNet::RakPeerInterface *rakPeer)
	{
		if (sampleResult==FAILED) return;

		if (sampleResult==PENDING && RakNet::GetTimeMS()>timeout)
		{
			printf("No response from any system, probably not running Router2 plugin.\n");
			sampleResult=FAILED;
		}
	}
	virtual void Shutdown(RakNet::RakPeerInterface *rakPeer)
	{
		delete router2;
		router2=0;
	}
	Router2 *router2;
	RakNet::TimeMS timeout;
};
struct UDPProxyClientFramework : public SampleFramework, public UDPProxyClientResultHandler
{
	// Set to FAILED to skip this test
	UDPProxyClientFramework() { sampleResult=SUPPORT_UDP_PROXY; udpProxy=0;}
	virtual const char * QueryName(void) {return "UDPProxyClientFramework";}
	virtual bool QueryRequiresServer(void) {return true;}
	virtual const char * QueryFunction(void) {return "Connect to a peer using a shared server connection.";}
	virtual const char * QuerySuccess(void) {return "We can now communicate with the other system, including connecting, within 5 seconds.";}
	virtual bool QueryQuitOnSuccess(void) {return false;}
	virtual void Init(RakNet::RakPeerInterface *rakPeer)
	{
		if (sampleResult==FAILED) return;

		SystemAddress serverAddress=SelectAmongConnectedSystems(rakPeer, "UDPProxyCoordinator");
		if (serverAddress==RakNet::UNASSIGNED_SYSTEM_ADDRESS)
		{
			serverAddress=ConnectBlocking(rakPeer, "UDPProxyCoordinator", DEFAULT_SERVER_ADDRESS, DEFAULT_SERVER_PORT);
			if (serverAddress==RakNet::UNASSIGNED_SYSTEM_ADDRESS)
			{
				printf("Failed to connect to a server.\n");
				sampleResult=FAILED;
				return;
			}
		}
		udpProxy = new UDPProxyClient;
		rakPeer->AttachPlugin(udpProxy);
		udpProxy->SetResultHandler(this);

		char guid[128];
		printf("Enter RakNetGuid of the remote system, which should have already connected\nto the server.\nOr press enter to just listen.\n");
		Gets(guid,sizeof(guid));
		RakNetGUID targetGuid;
		targetGuid.FromString(guid);

		if (guid[0])
		{
			RakNetGUID remoteSystemGuid;
			remoteSystemGuid.FromString(guid);
			udpProxy->RequestForwarding(serverAddress, UNASSIGNED_SYSTEM_ADDRESS, targetGuid, UDP_FORWARDER_MAXIMUM_TIMEOUT, 0);
			isListening=false;
		}
		else
		{
			printf("Listening\n");
			printf("My GUID is %s\n", rakPeer->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS).ToString());
			isListening=true;
		}

		timeout=RakNet::GetTimeMS() + 5000;
	}
	virtual void ProcessPacket(Packet *packet)
	{
	}
	virtual void Update(RakNet::RakPeerInterface *rakPeer)
	{
		if (sampleResult==FAILED) return;

		if (sampleResult==PENDING && RakNet::GetTimeMS()>timeout && isListening==false)
		{
			printf("No response from the server, probably not running UDPProxyCoordinator plugin.\n");
			sampleResult=FAILED;
		}
	}
	virtual void Shutdown(RakNet::RakPeerInterface *rakPeer)
	{
		delete udpProxy;
		udpProxy=0;
	}

	virtual void OnForwardingSuccess(const char *proxyIPAddress, unsigned short proxyPort,
		SystemAddress proxyCoordinator, SystemAddress sourceAddress, SystemAddress targetAddress, RakNetGUID targetGuid, RakNet::UDPProxyClient *proxyClientPlugin)
	{
		printf("Datagrams forwarded by proxy %s:%i to target %s.\n", proxyIPAddress, proxyPort, targetAddress.ToString(false));
		printf("Connecting to proxy, which will be received by target.\n");
		ConnectionAttemptResult car = proxyClientPlugin->GetRakPeerInterface()->Connect(proxyIPAddress, proxyPort, 0, 0);
		RakAssert(car==CONNECTION_ATTEMPT_STARTED);
		sampleResult=SUCCEEDED;
	}
	virtual void OnForwardingNotification(const char *proxyIPAddress, unsigned short proxyPort,
		SystemAddress proxyCoordinator, SystemAddress sourceAddress, SystemAddress targetAddress, RakNetGUID targetGuid, RakNet::UDPProxyClient *proxyClientPlugin)
	{
		printf("Source %s has setup forwarding to us through proxy %s:%i.\n", sourceAddress.ToString(false), proxyIPAddress, proxyPort);

		sampleResult=SUCCEEDED;
	}
	virtual void OnNoServersOnline(SystemAddress proxyCoordinator, SystemAddress sourceAddress, SystemAddress targetAddress, RakNetGUID targetGuid, RakNet::UDPProxyClient *proxyClientPlugin)
	{
		printf("Failure: No servers logged into coordinator.\n");
		sampleResult=FAILED;
	}
	virtual void OnRecipientNotConnected(SystemAddress proxyCoordinator, SystemAddress sourceAddress, SystemAddress targetAddress, RakNetGUID targetGuid, RakNet::UDPProxyClient *proxyClientPlugin)
	{
		printf("Failure: Recipient not connected to coordinator.\n");
		sampleResult=FAILED;
	}
	virtual void OnAllServersBusy(SystemAddress proxyCoordinator, SystemAddress sourceAddress, SystemAddress targetAddress, RakNetGUID targetGuid, RakNet::UDPProxyClient *proxyClientPlugin)
	{
		printf("Failure: No servers have available forwarding ports.\n");
		sampleResult=FAILED;
	}
	virtual void OnForwardingInProgress(const char *proxyIPAddress, unsigned short proxyPort, SystemAddress proxyCoordinator, SystemAddress sourceAddress, SystemAddress targetAddress, RakNetGUID targetGuid, RakNet::UDPProxyClient *proxyClientPlugin)
	{
		printf("Notification: Forwarding already in progress.\n");
	}

	UDPProxyClient *udpProxy;
	RakNet::TimeMS timeout;
	bool isListening;
};
void PrintPacketMessages(Packet *packet, RakPeerInterface *rakPeer)
{

	switch (packet->data[0])
	{
	case ID_DISCONNECTION_NOTIFICATION:
		// Connection lost normally
		printf("ID_DISCONNECTION_NOTIFICATION\n");
		break;
	case ID_NEW_INCOMING_CONNECTION:
		printf("ID_NEW_INCOMING_CONNECTION\n");
		break;
	case ID_ALREADY_CONNECTED:
		// Connection lost normally
		printf("ID_ALREADY_CONNECTED\n");
		break;
	case ID_INCOMPATIBLE_PROTOCOL_VERSION:
		printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
		break;
	case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
		printf("ID_REMOTE_DISCONNECTION_NOTIFICATION\n"); 
		break;
	case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
		printf("ID_REMOTE_CONNECTION_LOST\n");
		break;
	case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
		printf("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
		break;
	case ID_CONNECTION_BANNED: // Banned from this server
		printf("We are banned from this server.\n");
		break;			
	case ID_CONNECTION_ATTEMPT_FAILED:
		printf("Connection attempt failed\n");
		break;
	case ID_NO_FREE_INCOMING_CONNECTIONS:
		printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
		break;

	case ID_INVALID_PASSWORD:
		printf("ID_INVALID_PASSWORD\n");
		break;

	case ID_CONNECTION_LOST:
		printf("ID_CONNECTION_LOST from %s\n", packet->systemAddress.ToString(true));
		break;

	case ID_CONNECTION_REQUEST_ACCEPTED:
		// This tells the client they have connected
		printf("ID_CONNECTION_REQUEST_ACCEPTED to %s with GUID %s\n", packet->systemAddress.ToString(true), packet->guid.ToString());
		printf("My external address is %s\n", rakPeer->GetExternalID(packet->systemAddress).ToString(true));
		break;
	}
}

enum FeatureList
{
	_UPNPFramework,
	_NatTypeDetectionFramework,
	_NatPunchthoughFramework,
	_Router2Framework,
	_UDPProxyClientFramework,
	FEATURE_LIST_COUNT
};
int main(void)
{
	RakNet::RakPeerInterface *rakPeer=RakNet::RakPeerInterface::GetInstance();
	printf("Enter local port, or press enter for default: ");
	char buff[64];
	Gets(buff,sizeof(buff));
	unsigned short port = DEFAULT_RAKPEER_PORT;
	if (buff[0]!=0)
		port = atoi(buff);
	RakNet::SocketDescriptor sd(port,0);
	if (rakPeer->Startup(32,&sd,1)!=RakNet::RAKNET_STARTED)
	{
		printf("Failed to start rakPeer! Quitting\n");
		RakNet::RakPeerInterface::DestroyInstance(rakPeer);
		getch();
		return 1;
	}
	rakPeer->SetMaximumIncomingConnections(32);

	SampleFramework *samples[FEATURE_LIST_COUNT];
	unsigned int i=0;
	samples[i++] = new UPNPFramework;
	samples[i++] = new NatTypeDetectionFramework;
	samples[i++] = new NatPunchthoughClientFramework;
	samples[i++] = new Router2Framework;
	samples[i++] = new UDPProxyClientFramework;
	assert(i==FEATURE_LIST_COUNT);

	bool isFirstPrint=true;
	for (i=0; i < FEATURE_LIST_COUNT; i++)
	{
		if (isFirstPrint)
		{
			printf("NAT traversal client\nSupported operations:\n");
			isFirstPrint=false;
		}
		printf("\n%s\nRequires server: %s\nDescription: %s\n", samples[i]->QueryName(), samples[i]->QueryRequiresServer()==1 ? "Yes" : "No", samples[i]->QueryFunction());
	}

	printf("\nDo you have a server running the NATCompleteServer project? (y/n): ");

	char responseLetter=getche();
	bool hasServer=responseLetter=='y' || responseLetter=='Y' || responseLetter==' ';
	printf("\n");
	if (hasServer==false)
		printf("Note: Only UPNP and Router2 are supported without a server\nYou may want to consider using the Lobby2/Steam project. They host the\nservers for you.\n\n");

	FeatureList currentStage=_UPNPFramework;

	if (hasServer==false)
	{
		while (samples[(int) currentStage]->QueryRequiresServer()==true)
		{
			printf("No server: Skipping %s\n", samples[(int) currentStage]->QueryName());
			int stageInt = (int) currentStage;
			stageInt++;
			currentStage=(FeatureList)stageInt;
			if (currentStage==FEATURE_LIST_COUNT)
			{
				printf("Connectivity not possible. Exiting\n");
				getch();
				return 1;
			}
		}
	}

	while (1)
	{
		printf("Executing %s\n", samples[(int) currentStage]->QueryName());
		samples[(int) currentStage]->Init(rakPeer);

		bool thisSampleDone=false;
		while (1)
		{
			samples[(int) currentStage]->Update(rakPeer);
			RakNet::Packet *packet;
			for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet=rakPeer->Receive())
			{
				for (i=0; i < FEATURE_LIST_COUNT; i++)
				{
					samples[i]->ProcessPacket(packet);
				}

				PrintPacketMessages(packet, rakPeer);
			}

			if (samples[(int) currentStage]->sampleResult==FAILED ||
				samples[(int) currentStage]->sampleResult==SUCCEEDED)
			{
				printf("\n");
				thisSampleDone=true;
				if (samples[(int) currentStage]->sampleResult==FAILED)
				{
					printf("Failed %s\n", samples[(int) currentStage]->QueryName());

					int stageInt = (int) currentStage;
					stageInt++;
					currentStage=(FeatureList)stageInt;
					if (currentStage==FEATURE_LIST_COUNT)
					{
						printf("Connectivity not possible. Exiting\n");
						rakPeer->Shutdown(100);
						RakNet::RakPeerInterface::DestroyInstance(rakPeer);
						getch();
						return 1;
					}
					else
					{
						printf("Proceeding to next stage.\n");
						break;
					}
				}
				else
				{
					printf("Passed %s\n", samples[(int) currentStage]->QueryName());
					if (samples[(int) currentStage]->QueryQuitOnSuccess())
					{

						printf("Press any key to quit.\n");
						while (!kbhit())
						{
							for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet=rakPeer->Receive())
							{
								for (i=0; i < FEATURE_LIST_COUNT; i++)
								{
									samples[i]->ProcessPacket(packet);
								}

								PrintPacketMessages(packet, rakPeer);
							}
							RakSleep(30);
						}

						rakPeer->Shutdown(100);
						RakNet::RakPeerInterface::DestroyInstance(rakPeer);
						printf("Press enter to quit.\n");
						char temp[32];
						Gets(temp,sizeof(temp));
						getch();
						return 1;
					}

					printf("Proceeding to next stage.\n");
					int stageInt = (int) currentStage;
					stageInt++;
					if (stageInt<FEATURE_LIST_COUNT)
					{
						currentStage=(FeatureList)stageInt;
					}
					else
					{
						printf("Press any key to quit when done.\n");

						while (!kbhit())
						{
							for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet=rakPeer->Receive())
							{
								for (i=0; i < FEATURE_LIST_COUNT; i++)
								{
									samples[i]->ProcessPacket(packet);
								}

								PrintPacketMessages(packet, rakPeer);
							}
							RakSleep(30);
						}

						rakPeer->Shutdown(100);
						RakNet::RakPeerInterface::DestroyInstance(rakPeer);
						getch();
						return 1;
					}
					break;
				}
			}

			RakSleep(30);
		}
	}

	getch();
	return 0;
}
