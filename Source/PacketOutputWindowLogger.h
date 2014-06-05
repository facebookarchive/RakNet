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
/// \brief This will write all incoming and outgoing network messages to a file
///


#include "NativeFeatureIncludes.h"
#if _RAKNET_SUPPORT_PacketLogger==1

#ifndef __PACKET_OUTPUT_WINDOW_LOGGER_H_
#define __PACKET_OUTPUT_WINDOW_LOGGER_H_

#include "PacketLogger.h"

namespace RakNet
{

/// \ingroup PACKETLOGGER_GROUP
/// \brief Packetlogger that outputs to the output window in the debugger. Windows only.
class RAK_DLL_EXPORT  PacketOutputWindowLogger : public PacketLogger
{
public:
	PacketOutputWindowLogger();
	virtual ~PacketOutputWindowLogger();
	virtual void WriteLog(const char *str);
protected:
};

} // namespace RakNet

#endif

#endif // _RAKNET_SUPPORT_*
