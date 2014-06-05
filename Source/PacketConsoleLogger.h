/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

/// \file
/// \brief This will write all incoming and outgoing network messages to the log command parser, which can be accessed through Telnet
///

#include "NativeFeatureIncludes.h"
#if _RAKNET_SUPPORT_LogCommandParser==1 && _RAKNET_SUPPORT_PacketLogger==1

#ifndef __PACKET_CONSOLE_LOGGER_H_
#define __PACKET_CONSOLE_LOGGER_H_

#include "PacketLogger.h"

namespace RakNet
{
/// Forward declarations
class LogCommandParser;

/// \ingroup PACKETLOGGER_GROUP
/// \brief Packetlogger that logs to a remote command console
class RAK_DLL_EXPORT  PacketConsoleLogger : public PacketLogger
{
public:
	PacketConsoleLogger();
	// Writes to the command parser used for logging, which is accessed through a secondary communication layer (such as Telnet or RakNet) - See ConsoleServer.h
	virtual void SetLogCommandParser(LogCommandParser *lcp);
	virtual void WriteLog(const char *str);
protected:
	LogCommandParser *logCommandParser;
};

} // namespace RakNet

#endif

#endif // _RAKNET_SUPPORT_*
