/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __INTERVAL_TIMER_H
#define __INTERVAL_TIMER_H

#include "RakNetTypes.h"

struct IntervalTimer
{
	void SetPeriod(RakNet::TimeMS period);
	bool UpdateInterval(RakNet::TimeMS elapsed);

	RakNet::TimeMS basePeriod, remaining;	
};

#endif