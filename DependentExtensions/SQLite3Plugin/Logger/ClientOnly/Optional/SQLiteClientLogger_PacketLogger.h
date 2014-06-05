/// \file
/// \brief This will write all incoming and outgoing network messages the SQLiteClientLoggerPlugin
///
/// This file is part of RakNet Copyright (c) 2014, Oculus VR, Inc.
///
/// Usage of RakNet is subject to the appropriate license agreement.


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
