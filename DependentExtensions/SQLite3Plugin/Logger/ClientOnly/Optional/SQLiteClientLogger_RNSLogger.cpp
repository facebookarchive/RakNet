#include "SQLiteClientLogger_RNSLogger.h"
#include "RakNetTime.h"
#include "GetTime.h"
#include "RakNetStatistics.h"
#include "RakPeerInterface.h"
#include "SQLiteClientLoggerPlugin.h"

using namespace RakNet;

static const char *DEFAULT_RAKNET_STATISTICS_TABLE="RakNetStatistics";

SQLiteClientLogger_RakNetStatistics::SQLiteClientLogger_RakNetStatistics()
{
	lastUpdate=0;
}
SQLiteClientLogger_RakNetStatistics::~SQLiteClientLogger_RakNetStatistics()
{
}
void SQLiteClientLogger_RakNetStatistics::Update(void)
{
	RakNet::TimeUS time = RakNet::GetTimeUS();
	if (time-lastUpdate>1000000)
	{
		lastUpdate=time;
		unsigned int i;
		RakNetStatistics rns;
		for (i=0; i < rakPeerInterface->GetMaximumNumberOfPeers(); i++)
		{
			if (rakPeerInterface->GetStatistics( i, &rns ))
			{
				/*
				rns.valueOverLastSecond[USER_MESSAGE_BYTES_PUSHED],
					rns.valueOverLastSecond[USER_MESSAGE_BYTES_SENT],
					rns.valueOverLastSecond[USER_MESSAGE_BYTES_RESENT],
					rns.valueOverLastSecond[USER_MESSAGE_BYTES_RECEIVED_PROCESSED],
					rns.valueOverLastSecond[USER_MESSAGE_BYTES_RECEIVED_IGNORED],
					rns.valueOverLastSecond[ACTUAL_BYTES_SENT],
					rns.valueOverLastSecond[ACTUAL_BYTES_RECEIVED],
					rns.runningTotal[USER_MESSAGE_BYTES_PUSHED],
					rns.runningTotal[USER_MESSAGE_BYTES_SENT],
					rns.runningTotal[USER_MESSAGE_BYTES_RESENT],
					rns.runningTotal[USER_MESSAGE_BYTES_RECEIVED_PROCESSED],
					rns.runningTotal[USER_MESSAGE_BYTES_RECEIVED_IGNORED],
					rns.runningTotal[ACTUAL_BYTES_SENT],
					rns.runningTotal[ACTUAL_BYTES_RECEIVED],
					rns.connectionStartTime,
					rns.BPSLimitByCongestionControl,
					rns.isLimitedByCongestionControl,
					rns.BPSLimitByOutgoingBandwidthLimit,
					rns.isLimitedByOutgoingBandwidthLimit,
					rns.messageInSendBuffer[IMMEDIATE_PRIORITY],
					rns.messageInSendBuffer[HIGH_PRIORITY],
					rns.messageInSendBuffer[MEDIUM_PRIORITY],
					rns.messageInSendBuffer[LOW_PRIORITY],
					rns.bytesInSendBuffer[IMMEDIATE_PRIORITY],
					rns.bytesInSendBuffer[HIGH_PRIORITY],
					rns.bytesInSendBuffer[MEDIUM_PRIORITY],
					rns.bytesInSendBuffer[LOW_PRIORITY],
					rns.messagesInResendBuffer,
					rns.bytesInResendBuffer,
					rns.packetlossLastSecond,
					rns.packetlossTotal,
					*/

			

				rakSqlLog(
					DEFAULT_RAKNET_STATISTICS_TABLE,
					"valueOverLastSecond[USER_MESSAGE_BYTES_PUSHED],"
					"valueOverLastSecond[USER_MESSAGE_BYTES_SENT],"
					"valueOverLastSecond[USER_MESSAGE_BYTES_RESENT],"
					"valueOverLastSecond[USER_MESSAGE_BYTES_RECEIVED_PROCESSED],"
					"valueOverLastSecond[USER_MESSAGE_BYTES_RECEIVED_IGNORED],"
					"valueOverLastSecond[ACTUAL_BYTES_SENT],"
					"valueOverLastSecond[ACTUAL_BYTES_RECEIVED],"
					"BPSLimitByCongestionControl,"
					"BPSLimitByOutgoingBandwidthLimit,"
					"bytesInSendBuffer,"
					"messagesInResendBuffer,"
					"bytesInResendBuffer,"
					"packetlossLastSecond,"
					"packetlossTotal",
					( \
					rns.valueOverLastSecond[USER_MESSAGE_BYTES_PUSHED], \
					rns.valueOverLastSecond[USER_MESSAGE_BYTES_SENT], \
					rns.valueOverLastSecond[USER_MESSAGE_BYTES_RESENT], \
					rns.valueOverLastSecond[USER_MESSAGE_BYTES_RECEIVED_PROCESSED], \
					rns.valueOverLastSecond[USER_MESSAGE_BYTES_RECEIVED_IGNORED], \
					rns.valueOverLastSecond[ACTUAL_BYTES_SENT], \
					rns.valueOverLastSecond[ACTUAL_BYTES_RECEIVED], \
					rns.BPSLimitByCongestionControl, \
					rns.BPSLimitByOutgoingBandwidthLimit, \
					rns.bytesInSendBuffer[IMMEDIATE_PRIORITY]+rns.bytesInSendBuffer[HIGH_PRIORITY]+rns.bytesInSendBuffer[MEDIUM_PRIORITY]+rns.bytesInSendBuffer[LOW_PRIORITY], \
					rns.messagesInResendBuffer, \
					rns.bytesInResendBuffer, \
					rns.packetlossLastSecond, \
					rns.packetlossTotal \
					));
			}
		}
	}
}
