#include "SQLiteClientLogger_PacketLogger.h"
#include "SQLiteClientLoggerPlugin.h"
#include "RakPeerInterface.h"
#include "InternalPacket.h"
#include "MessageIdentifiers.h"

using namespace RakNet;

static const char *DEFAULT_PACKET_LOGGER_TABLE="PacketLogger";

SQLiteClientLogger_PacketLogger::SQLiteClientLogger_PacketLogger()
{
}
SQLiteClientLogger_PacketLogger::~SQLiteClientLogger_PacketLogger()
{
}
void SQLiteClientLogger_PacketLogger::OnDirectSocketSend(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress)
{
	char str1[64], str2[62], str3[64], str4[64];
	SystemAddress localSystemAddress = rakPeerInterface->GetExternalID(remoteSystemAddress);
	localSystemAddress.ToString(true, str1);
	rakPeerInterface->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString(str2);
	remoteSystemAddress.ToString(true, str3);
	rakPeerInterface->GetGuidFromSystemAddress(remoteSystemAddress).ToString(str4);

	
	rakSqlLog(DEFAULT_PACKET_LOGGER_TABLE, "SndRcv,Type,PacketNumber,FrameNumber,PacketID,BitLength,LocalIP,LocalGuid,RemoteIP,RemoteGuid,splitPacketId,SplitPacketIndex,splitPacketCount,orderingIndex,misc", \
	          ("Snd", "Raw",0,          0,          IDTOString(data[0]), bitsUsed, str1, str2, str3, str4, "","","","","") );
}
void SQLiteClientLogger_PacketLogger::OnDirectSocketReceive(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress)
{
	char str1[64], str2[62], str3[64], str4[64];
	SystemAddress localSystemAddress = rakPeerInterface->GetExternalID(remoteSystemAddress);
	localSystemAddress.ToString(true, str1);
	rakPeerInterface->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString(str2);
	remoteSystemAddress.ToString(true, str3);
	rakPeerInterface->GetGuidFromSystemAddress(remoteSystemAddress).ToString(str4);
	
	rakSqlLog(DEFAULT_PACKET_LOGGER_TABLE, "SndRcv,Type,PacketNumber,FrameNumber,PacketID,BitLength,LocalIP,LocalGuid,RemoteIP,RemoteGuid,splitPacketId,SplitPacketIndex,splitPacketCount,orderingIndex,misc", \
		     ("Rcv", "Raw", "",         "",         IDTOString(data[0]),bitsUsed, str1, str2, str3, str4, "","","","","") );
}
void SQLiteClientLogger_PacketLogger::OnInternalPacket(InternalPacket *internalPacket, unsigned frameNumber, SystemAddress remoteSystemAddress, RakNet::TimeMS time, bool isSend)
{
	char str1[64], str2[62], str3[64], str4[64];
	SystemAddress localSystemAddress = rakPeerInterface->GetExternalID(remoteSystemAddress);
	localSystemAddress.ToString(true, str1);
	rakPeerInterface->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString(str2);
	remoteSystemAddress.ToString(true, str3);
	rakPeerInterface->GetGuidFromSystemAddress(remoteSystemAddress).ToString(str4);
	
	unsigned char typeByte;
	char *typeStr;
	if (internalPacket->data[0]==ID_TIMESTAMP && BITS_TO_BYTES(internalPacket->dataBitLength)>sizeof(RakNet::TimeMS)+1)
	{
		typeByte=internalPacket->data[1+sizeof(RakNet::TimeMS)];
		typeStr="Timestamp";
	}
	else
	{
		typeByte=internalPacket->data[0];
		typeStr="Normal";
	}
	
	const char* sendType = (isSend) ? "Snd" : "Rcv";
	
	rakSqlLog(DEFAULT_PACKET_LOGGER_TABLE, "SndRcv,Type,PacketNumber,FrameNumber,PacketID,BitLength,LocalIP,LocalGuid,RemoteIP,RemoteGuid,splitPacketId,SplitPacketIndex,splitPacketCount,orderingIndex,misc", \
		     (sendType, typeStr, internalPacket->reliableMessageNumber, frameNumber, IDTOString(typeByte), internalPacket->dataBitLength, str1, str2, str3, str4, internalPacket->splitPacketId, internalPacket->splitPacketIndex, internalPacket->splitPacketCount, internalPacket->orderingIndex,"") );
}
void SQLiteClientLogger_PacketLogger::OnAck(unsigned int messageNumber, SystemAddress remoteSystemAddress, RakNet::TimeMS time)
{
	char str1[64], str2[62], str3[64], str4[64];
	SystemAddress localSystemAddress = rakPeerInterface->GetExternalID(remoteSystemAddress);
	localSystemAddress.ToString(true, str1);
	rakPeerInterface->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString(str2);
	remoteSystemAddress.ToString(true, str3);
	rakPeerInterface->GetGuidFromSystemAddress(remoteSystemAddress).ToString(str4);
	
	rakSqlLog(DEFAULT_PACKET_LOGGER_TABLE, "SndRcv,Type,PacketNumber,FrameNumber,PacketID,BitLength,LocalIP,LocalGuid,RemoteIP,RemoteGuid,splitPacketId,SplitPacketIndex,splitPacketCount,orderingIndex,misc", \
		          ("Rcv", "Ack",messageNumber, "", "", "", str1, str2, str3, str4, "","","","","") );
}
void SQLiteClientLogger_PacketLogger::OnPushBackPacket(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress)
{
	char str1[64], str2[62], str3[64], str4[64];
	SystemAddress localSystemAddress = rakPeerInterface->GetExternalID(remoteSystemAddress);
	localSystemAddress.ToString(true, str1);
	rakPeerInterface->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString(str2);
	remoteSystemAddress.ToString(true, str3);
	rakPeerInterface->GetGuidFromSystemAddress(remoteSystemAddress).ToString(str4);
	
	rakSqlLog(DEFAULT_PACKET_LOGGER_TABLE, "SndRcv,Type,PacketNumber,FrameNumber,PacketID,BitLength,LocalIP,LocalGuid,RemoteIP,RemoteGuid,splitPacketId,SplitPacketIndex,splitPacketCount,orderingIndex,misc", \
		          ("Local", "PushBackPacket","",  "", IDTOString(data[0]), bitsUsed, str1, str2, str3, str4, "","","","","") );
}
void SQLiteClientLogger_PacketLogger::WriteMiscellaneous(const char *type, const char *msg)
{
	rakSqlLog(DEFAULT_PACKET_LOGGER_TABLE, "SndRcv,Type,PacketNumber,FrameNumber,PacketID,BitLength,LocalIP,LocalGuid,RemoteIP,RemoteGuid,splitPacketId,SplitPacketIndex,splitPacketCount,orderingIndex,misc", \
		          ("Local", type,"",  "", "", "", "", "", "","","","","","",msg) );
}
