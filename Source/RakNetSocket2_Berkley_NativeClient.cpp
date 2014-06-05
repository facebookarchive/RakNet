/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "EmptyHeader.h"

#ifdef RAKNET_SOCKET_2_INLINE_FUNCTIONS

#ifndef RAKNETSOCKET2_BERKLEY_NATIVE_CLIENT_CPP
#define RAKNETSOCKET2_BERKLEY_NATIVE_CLIENT_CPP

// Every platform except windows store 8 and native client supports Berkley sockets
#if !defined(WINDOWS_STORE_RT)

#include "Itoa.h"

// Shared on most platforms, but excluded from the listed


void DomainNameToIP_Berkley_IPV4And6( const char *domainName, char ip[65] )
{
#if RAKNET_SUPPORT_IPV6==1
	struct addrinfo hints, *res, *p;
	int status;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
	hints.ai_socktype = SOCK_DGRAM;

	if ((status = getaddrinfo(domainName, NULL, &hints, &res)) != 0) {
		memset(ip, 0, sizeof(ip));
		return;
	}

	p=res;
// 	for(p = res;p != NULL; p = p->ai_next) {
		void *addr;
//		char *ipver;

		// get the pointer to the address itself,
		// different fields in IPv4 and IPv6:
		if (p->ai_family == AF_INET)
		{
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
			strcpy(ip, inet_ntoa( ipv4->sin_addr ));
		} 
		else
		{
			// TODO - test
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			// inet_ntop function does not exist on windows
			// http://www.mail-archive.com/users@ipv6.org/msg02107.html
			getnameinfo((struct sockaddr *)ipv6, sizeof(struct sockaddr_in6), ip, 1, NULL, 0, NI_NUMERICHOST);
		}
		freeaddrinfo(res); // free the linked list
//	}
#else
	(void) domainName;
	(void) ip;
#endif // #if RAKNET_SUPPORT_IPV6==1
}


void DomainNameToIP_Berkley_IPV4( const char *domainName, char ip[65] )
{
	static struct in_addr addr;
	memset(&addr,0,sizeof(in_addr));
	
	// Use inet_addr instead? What is the difference?
	struct hostent * phe = gethostbyname( domainName );

	if ( phe == 0 || phe->h_addr_list[ 0 ] == 0 )
	{
		//cerr << "Yow! Bad host lookup." << endl;
		memset(ip,0,65*sizeof(char));
		return;
	}

	if (phe->h_addr_list[ 0 ]==0)
	{
		memset(ip,0,65*sizeof(char));
		return;
	}

	memcpy( &addr, phe->h_addr_list[ 0 ], sizeof( struct in_addr ) );
	strcpy(ip, inet_ntoa( addr ));
}



void DomainNameToIP_Berkley( const char *domainName, char ip[65] )
{
#if RAKNET_SUPPORT_IPV6==1
	return DomainNameToIP_Berkley_IPV4And6(domainName, ip);
#else
	return DomainNameToIP_Berkley_IPV4(domainName, ip);
#endif
}




#endif // !defined(WINDOWS_STORE_RT) && !defined(__native_client__)

#endif // file header

#endif // #ifdef RAKNET_SOCKET_2_INLINE_FUNCTIONS
