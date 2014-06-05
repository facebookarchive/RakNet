/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

/// \file DynDNS.h
/// \brief Helper to class to update DynDNS
/// This can be used to determine what permissions are should be allowed to the other system
///


#include "NativeFeatureIncludes.h"
#if _RAKNET_SUPPORT_DynDNS==1 && _RAKNET_SUPPORT_TCPInterface==1

#ifndef __DYN_DNS_H
#define __DYN_DNS_H

#include "RakString.h"

namespace RakNet
{

class TCPInterface;

enum DynDnsResultCode
{
	// ----- Success -----
	RC_SUCCESS,
	RC_DNS_ALREADY_SET, // RakNet detects no action is needed

	// ----- Ignorable failure (treat same as success) -----
	RC_NO_CHANGE, // DynDNS detects no action is needed (treated as abuse though)

	// ----- User error -----
	RC_NOT_DONATOR, // You have to pay to do this
	RC_NO_HOST, // This host does not exist at all
	RC_BAD_AUTH, // You set the wrong password
	RC_NOT_YOURS, // This is not your host

	// ----- Permanent failure -----
	RC_ABUSE, // Your host has been blocked, too many failures disable your account
	RC_TCP_FAILED_TO_START, // TCP port already in use
	RC_TCP_DID_NOT_CONNECT, // DynDNS down?
	RC_UNKNOWN_RESULT, // DynDNS returned a result code that was not documented as of 12/4/2010 on http://www.dyndns.com/developers/specs/flow.pdf
	RC_PARSING_FAILURE, // Can't read the result returned, format change?
	RC_CONNECTION_LOST_WITHOUT_RESPONSE, // Lost the connection to DynDNS while communicating
	RC_BAD_AGENT, // ???
	RC_BAD_SYS, // ???
	RC_DNS_ERROR, // ???
	RC_NOT_FQDN, // ???
	RC_NUM_HOST, // ???
	RC_911, // ???
	RC_DYNDNS_TIMEOUT // DynDNS did not respond
};

// Can only process one at a time with the current implementation
class RAK_DLL_EXPORT DynDNS
{
public:
	DynDNS();
	~DynDNS();

	// Pass 0 for newIPAddress to autodetect whatever you are uploading from
	// usernameAndPassword should be in the format username:password
	void UpdateHostIPAsynch(const char *dnsHost, const char *newIPAddress, const char *usernameAndPassword );
	void Update(void);

	// Output
	bool IsRunning(void) const {return connectPhase!=CP_IDLE;}
	bool IsCompleted(void) const {return connectPhase==CP_IDLE;}
	RakNet::DynDnsResultCode GetCompletedResultCode(void) {return result;}
	const char *GetCompletedDescription(void) const {return resultDescription;}
	bool WasResultSuccessful(void) const {return result==RC_SUCCESS || result==RC_DNS_ALREADY_SET || result==RC_NO_CHANGE;}
	char *GetMyPublicIP(void) const {return (char*) myIPStr;} // We get our public IP as part of the process. This is valid once completed

protected:
	void Stop(void);
	void SetCompleted(RakNet::DynDnsResultCode _result, const char *_resultDescription) {Stop(); result=_result; resultDescription=_resultDescription;}

	enum ConnectPhase
	{
		CP_CONNECTING_TO_CHECKIP,
		CP_WAITING_FOR_CHECKIP_RESPONSE,
		CP_CONNECTING_TO_DYNDNS,
		CP_WAITING_FOR_DYNDNS_RESPONSE,
		CP_IDLE
	};

	TCPInterface *tcp;
	RakNet::RakString getString;
	SystemAddress serverAddress;
	ConnectPhase connectPhase;
	RakNet::RakString host;
	RakNet::Time phaseTimeout;
	SystemAddress checkIpAddress;
	const char *resultDescription;
	RakNet::DynDnsResultCode result;
	char myIPStr[32];
};

} // namespace RakNet

#endif // __DYN_DNS_H

#endif // _RAKNET_SUPPORT_DynDNS
