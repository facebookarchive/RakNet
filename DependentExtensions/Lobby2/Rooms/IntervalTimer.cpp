/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "IntervalTimer.h"

void IntervalTimer::SetPeriod(RakNet::TimeMS period) {basePeriod=period; remaining=0;}
bool IntervalTimer::UpdateInterval(RakNet::TimeMS elapsed)
{
	if (elapsed >= remaining)
	{
		RakNet::TimeMS difference = elapsed-remaining;
		if (difference >= basePeriod)
		{
			remaining=basePeriod;
		}
		else
		{
			remaining=basePeriod-difference;
		}

		return true;
	}

	remaining-=elapsed;
	return false;
}