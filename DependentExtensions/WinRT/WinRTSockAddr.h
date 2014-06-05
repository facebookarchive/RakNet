/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __SOCK_ADDR_H
#define __SOCK_ADDR_H

#include "NativeTypes.h"

namespace RakNet
{

// All pointers to socket address structures are often cast to pointers
// to this type before use in various functions and system calls:

struct sockaddr {
    unsigned short    sa_family;    // address family, AF_xxx
    char              sa_data[14];  // 14 bytes of protocol address
};


struct in_addr {
    unsigned long s_addr;          // load with inet_pton()
};

// IPv4 AF_INET sockets:

struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET, AF_INET6
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
};


// IPv6 AF_INET6 sockets:

struct in6_addr {
    unsigned char   s6_addr[16];   // load with inet_pton()
};


struct sockaddr_in6 {
    uint16_t       sin6_family;   // address family, AF_INET6
    uint16_t       sin6_port;     // port number, Network Byte Order
    uint32_t       sin6_flowinfo; // IPv6 flow information
    struct in6_addr sin6_addr;     // IPv6 address
    uint32_t       sin6_scope_id; // Scope ID
};

// General socket address holding structure, big enough to hold either
// struct sockaddr_in or struct sockaddr_in6 data:


#define _SS_MAXSIZE 128
#define _SS_ALIGNSIZE (sizeof(int64_t))

typedef uint32_t sa_family_t;

#define _SS_PAD1SIZE (_SS_ALIGNSIZE - sizeof(sa_family_t))
#define _SS_PAD2SIZE (_SS_MAXSIZE - (sizeof(sa_family_t)+ \
                      _SS_PAD1SIZE + _SS_ALIGNSIZE))
struct sockaddr_storage {
    sa_family_t  ss_family;

    char _ss_pad1[_SS_PAD1SIZE];
    int64_t _ss_align;
    char _ss_pad2[_SS_PAD2SIZE];
};

#define INADDR_ANY              (unsigned long)0x00000000
#define INADDR_LOOPBACK         0x7f000001
#define INADDR_BROADCAST        (unsigned long)0xffffffff
#define INADDR_NONE             0xffffffff

/* Supported address families. */
#define AF_UNSPEC	0
#define AF_UNIX		1	/* Unix domain sockets 		*/
#define AF_INET		2	/* Internet IP Protocol 	*/
#define AF_AX25		3	/* Amateur Radio AX.25 		*/
#define AF_IPX		4	/* Novell IPX 			*/
#define AF_APPLETALK	5	/* Appletalk DDP 		*/
#define	AF_NETROM	6	/* Amateur radio NetROM 	*/
#define AF_BRIDGE	7	/* Multiprotocol bridge 	*/
#define AF_AAL5		8	/* Reserved for Werner's ATM 	*/
#define AF_X25		9	/* Reserved for X.25 project 	*/
#define AF_INET6	10	/* IP version 6			*/
#define AF_MAX		12	/* For now.. */


#ifndef IPPROTO_IP
#define	IPPROTO_IP		0		/* dummy for IP */
#endif
#ifndef IPPROTO_HOPOPTS
#define IPPROTO_HOPOPTS		0		/* IPv6 hop-by-hop options */
#endif
#ifndef IPPROTO_ICMP
#define	IPPROTO_ICMP		1		/* control message protocol */
#endif
#ifndef IPPROTO_IGMP
#define	IPPROTO_IGMP		2		/* group mgmt protocol */
#endif
#ifndef IPPROTO_IPV4
#define IPPROTO_IPV4		4
#endif
#ifndef IPPROTO_TCP
#define	IPPROTO_TCP		6		/* tcp */
#endif
#ifndef IPPROTO_EGP
#define	IPPROTO_EGP		8		/* exterior gateway protocol */
#endif
#ifndef IPPROTO_PIGP
#define IPPROTO_PIGP		9
#endif
#ifndef IPPROTO_UDP
#define	IPPROTO_UDP		17		/* user datagram protocol */
#endif
#ifndef IPPROTO_DCCP
#define	IPPROTO_DCCP		33		/* datagram congestion control protocol */
#endif
#ifndef IPPROTO_IPV6
#define IPPROTO_IPV6		41
#endif
#ifndef IPPROTO_ROUTING
#define IPPROTO_ROUTING		43		/* IPv6 routing header */
#endif
#ifndef IPPROTO_FRAGMENT
#define IPPROTO_FRAGMENT	44		/* IPv6 fragmentation header */
#endif
#ifndef IPPROTO_RSVP
#define IPPROTO_RSVP		46 		/* resource reservation */
#endif
#ifndef IPPROTO_GRE
#define	IPPROTO_GRE		47		/* General Routing Encap. */
#endif
#ifndef IPPROTO_ESP
#define	IPPROTO_ESP		50		/* SIPP Encap Sec. Payload */
#endif
#ifndef IPPROTO_AH
#define	IPPROTO_AH		51		/* SIPP Auth Header */
#endif
#ifndef IPPROTO_MOBILE
#define IPPROTO_MOBILE		55
#endif
#ifndef IPPROTO_ICMPV6
#define IPPROTO_ICMPV6		58		/* ICMPv6 */
#endif
#ifndef IPPROTO_NONE
#define IPPROTO_NONE		59		/* IPv6 no next header */
#endif
#ifndef IPPROTO_DSTOPTS
#define IPPROTO_DSTOPTS		60		/* IPv6 destination options */
#endif

/*Standard socket types */
#define  SOCK_STREAM             1 /*virtual circuit*/
#define  SOCK_DGRAM              2 /*datagram*/
#define  SOCK_RAW                3 /*raw socket*/
#define  SOCK_RDM                4 /*reliably-delivered message*/
#define  SOCK_CONN_DGRAM         5 /*connection datagram*/

inline uint16_t ntohs(uint16_t s) {return (((s>> 8)) | (s << 8));};
inline uint16_t htons(uint16_t s) {return (((s>> 8)) | (s << 8));};
inline uint32_t ntohl(uint32_t s) {return (((s&0x000000FF)<<24)+((s&0x0000FF00)<<8)+((s&0x00FF0000)>>8)+((s&0xFF000000)>>24)); };
inline uint32_t htonl(uint32_t s) {return (((s&0x000000FF)<<24)+((s&0x0000FF00)<<8)+((s&0x00FF0000)>>8)+((s&0xFF000000)>>24)); };

/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define	SOL_SOCKET	0xffff		/* options for socket level */

/*
 * Additional options, not kept in so_options.
 */
#define SO_SNDBUF	0x1001		/* send buffer size */
#define SO_RCVBUF	0x1002		/* receive buffer size */
#define SO_SNDLOWAT	0x1003		/* send low-water mark */
#define SO_RCVLOWAT	0x1004		/* receive low-water mark */
#define SO_SNDTIMEO	0x1005		/* send timeout */
#define SO_RCVTIMEO	0x1006		/* receive timeout */
#define	SO_ERROR	0x1007		/* get error status and clear */
#define	SO_TYPE		0x1008		/* get socket type */

/*
 * Option flags per-socket.
 */
#define	SO_DEBUG	0x0001		/* turn on debugging info recording */
#define	SO_ACCEPTCONN	0x0002		/* socket has had listen() */
#define	SO_REUSEADDR	0x0004		/* allow local address reuse */
#define	SO_KEEPALIVE	0x0008		/* keep connections alive */
#define	SO_DONTROUTE	0x0010		/* just use interface addresses */
#define	SO_BROADCAST	0x0020		/* permit sending of broadcast msgs */
#define	SO_USELOOPBACK	0x0040		/* bypass hardware when possible */
#define	SO_LINGER	0x0080		/* linger on close if data present */
#define	SO_OOBINLINE	0x0100		/* leave received OOB data in line */

#define IP_TOS          1
#define IP_TTL          2
#define IP_HDRINCL      3
#define IP_OPTIONS      4
#define IP_ROUTER_ALERT 5
#define IP_RECVOPTS     6
#define IP_RETOPTS      7
#define IP_PKTINFO      8
#define IP_PKTOPTIONS   9
#define IP_MTU_DISCOVER 10
#define IP_RECVERR      11
#define IP_RECVTTL      12
#define IP_RECVTOS      13
#define IP_MTU          14
#define IP_FREEBIND     15

#ifndef AI_PASSIVE
# define AI_PASSIVE     1
#endif /* AI_PASSIVE */

#define SD_RECEIVE 0
#define SD_SEND 1
#define SD_BOTH 2

struct addrinfo {
    int              ai_flags;
    int              ai_family;
    int              ai_socktype;
    int              ai_protocol;
    size_t           ai_addrlen;
    struct sockaddr *ai_addr;
    char            *ai_canonname;
    struct addrinfo *ai_next;
};

inline char *inet_ntoa(in_addr in)
{
	static char staticBuff[32];
	char workingbuff[8];
	int part1 = ((in.s_addr & 0xFF000000) >> 24);
	int part2 = ((in.s_addr & 0x00FF0000) >> 16);
	int part3 = ((in.s_addr & 0x0000FF00) >> 8);
	int part4 = ((in.s_addr & 0x000000FF) >> 0);

	_itoa(part1,staticBuff,10);
	strcat(staticBuff,".");
	_itoa(part2,workingbuff,10);
	strcat(staticBuff,workingbuff);
	strcat(staticBuff,".");
	_itoa(part3,workingbuff,10);
	strcat(staticBuff,workingbuff);
	strcat(staticBuff,".");
	_itoa(part4,workingbuff,10);
	strcat(staticBuff,workingbuff);
	return (char*) staticBuff;
}


};

#endif
