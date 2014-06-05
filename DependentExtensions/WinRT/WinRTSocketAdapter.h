/*
#if defined(WINDOWS_STORE_RT)

#pragma once

#include <windows.h>
#include "WinRTSockAddr.h"
#include "SocketIncludes.h"

namespace RakNet
{                    
	int WinRTClose( Windows::Networking::Sockets::DatagramSocket ^s);

	int WinRTClose( Windows::Networking::Sockets::StreamSocket ^s);
	
	Windows::Networking::Sockets::DatagramSocket^ WinRTCreateDatagramSocket(int af,
                   int type,
                   int protocol);

	Windows::Networking::Sockets::StreamSocket^ WinRTCreateStreamSocket(int af,
                   int type,
                   int protocol);
                   
	int WinRTBind(Windows::Networking::Sockets::DatagramSocket ^s,
                 const struct sockaddr *addr,
                 socklen_t namelen);
    
	int WinRTBind(Windows::Networking::Sockets::StreamSocket ^s,
                 const struct sockaddr *addr,
                 socklen_t namelen);

	int WinRTGetSockName(Windows::Networking::Sockets::DatagramSocket ^s,
                        struct sockaddr *name,
                        socklen_t* namelen);
                        
	int WinRTGetSockOpt (Windows::Networking::Sockets::DatagramSocket ^s,
                        int level,
                        int optname,
                        char * optval,
                        socklen_t *optlen);
                        
	int WinRTInet_Addr(const char * cp);
	
	int WinRTIOCTLSocket(Windows::Networking::Sockets::DatagramSocket ^s,
                        long cmd,
                        unsigned long *argp);
                        
	int WinRTListen (Windows::Networking::Sockets::DatagramSocket ^s,
                    int backlog);

	int WinRTListen (Windows::Networking::Sockets::StreamSocket ^s,
                    int backlog);
                    
                   
	int WinRTSetSockOpt(Windows::Networking::Sockets::DatagramSocket ^s,
                       int level,
                       int optname,
                       const char * optval,
                       socklen_t optlen);

	int WinRTSetSockOpt(Windows::Networking::Sockets::StreamSocket ^s,
                       int level,
                       int optname,
                       const char * optval,
                       socklen_t optlen);
         
}

#endif // defined(WINDOWS_STORE_RT)
*/
