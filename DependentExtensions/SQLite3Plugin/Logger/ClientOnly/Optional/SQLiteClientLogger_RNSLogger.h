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
/// \brief Writes RakNetStatistics for all connected systems once per second to SQLiteClientLogger
///



#ifndef __SQL_LITE_CLIENT_LOGGER_RAKNET_STATISTICS_H_
#define __SQL_LITE_CLIENT_LOGGER_RAKNET_STATISTICS_H_

#include "PluginInterface2.h"

namespace RakNet
{
	/// \ingroup PACKETLOGGER_GROUP
	/// \brief Packetlogger that outputs to a file
	class RAK_DLL_EXPORT SQLiteClientLogger_RakNetStatistics : public PluginInterface2
	{
	public:
		SQLiteClientLogger_RakNetStatistics();
		virtual ~SQLiteClientLogger_RakNetStatistics();
		virtual void Update(void);
	protected:
		RakNet::TimeUS lastUpdate;
	};
}

#endif
