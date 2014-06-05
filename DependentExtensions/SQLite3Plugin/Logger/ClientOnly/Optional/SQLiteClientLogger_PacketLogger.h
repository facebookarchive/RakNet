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
/// \brief This will write all incoming and outgoing network messages the SQLiteClientLoggerPlugin
///



#ifndef __SQL_LITE_CLIENT_LOGGER_PACKET_LOGGER_H_
#define __SQL_LITE_CLIENT_LOGGER_PACKET_LOGGER_H_

#include "PacketLogger.h"

namespace RakNet
{

/// \ingroup PACKETLOGGER_GROUP
/// \brief Packetlogger that outputs to a file
class RAK_DLL_EXPORT  SQLiteClientLogger_PacketLogger : public PacketLogger
{
public:
	SQLiteClientLogger_PacketLogger();
	virtual ~SQLiteClientLogger_PacketLogger();
	
	virtual void OnDirectSocketSend(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress);
	virtual void OnDirectSocketReceive(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress);
	virtual void OnInternalPacket(InternalPacket *internalPacket, unsigned frameNumber, SystemAddress remoteSystemAddress, RakNet::TimeMS time, bool isSend);
	virtual void OnAck(unsigned int messageNumber, SystemAddress remoteSystemAddress, RakNet::TimeMS time);
	virtual void OnPushBackPacket(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress);
	virtual void WriteMiscellaneous(const char *type, const char *msg);
protected:

	virtual void WriteLog(const char *str) {}
};

}

#endif
