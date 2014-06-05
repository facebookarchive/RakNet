/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "RakAssert.h"
#include "RakMemoryOverride.h"
#include "TransformationHistory.h"

TransformationHistoryCell::TransformationHistoryCell()
{

}
TransformationHistoryCell::TransformationHistoryCell(RakNet::TimeMS t, const Ogre::Vector3& pos, const Ogre::Vector3& vel, const Ogre::Quaternion& quat  ) :
time(t),
velocity(vel),
position(pos),
orientation(quat)
{
}

void TransformationHistory::Init(RakNet::TimeMS maxWriteInterval, RakNet::TimeMS maxHistoryTime)
{
	writeInterval=maxWriteInterval;
	maxHistoryLength = maxHistoryTime/maxWriteInterval+1;
	history.ClearAndForceAllocation(maxHistoryLength+1, _FILE_AND_LINE_ );
	RakAssert(writeInterval>0);
}
void TransformationHistory::Write(const Ogre::Vector3 &position, const Ogre::Vector3 &velocity, const Ogre::Quaternion &orientation, RakNet::TimeMS curTimeMS)
{
	if (history.Size()==0)
	{
		history.Push(TransformationHistoryCell(curTimeMS,position,velocity,orientation), _FILE_AND_LINE_ );
	}
	else
	{
		const TransformationHistoryCell &lastCell = history.PeekTail();
		if (curTimeMS-lastCell.time>=writeInterval)
		{
			history.Push(TransformationHistoryCell(curTimeMS,position,velocity,orientation), _FILE_AND_LINE_ );
			if (history.Size()>maxHistoryLength)
				history.Pop();
		}
	}	
}
void TransformationHistory::Overwrite(const Ogre::Vector3 &position, const Ogre::Vector3 &velocity, const Ogre::Quaternion &orientation, RakNet::TimeMS when)
{
	int historySize = history.Size();
	if (historySize==0)
	{
		history.Push(TransformationHistoryCell(when,position,velocity,orientation), _FILE_AND_LINE_ );
	}
	else
	{
		// Find the history matching this time, and change the values.
		int i;
		for (i=historySize-1; i>=0; i--)
		{
			TransformationHistoryCell &cell = history[i];
			if (when >= cell.time)
			{
				if (i==historySize-1 && when-cell.time>=writeInterval)
				{
					// Not an overwrite at all, but a new cell
					history.Push(TransformationHistoryCell(when,position,velocity,orientation), _FILE_AND_LINE_ );
					if (history.Size()>maxHistoryLength)
						history.Pop();
					return;
				}

				cell.time=when;
				cell.position=position;
				cell.velocity=velocity;
				cell.orientation=orientation;
				return;
			}
		}
	}	
}
TransformationHistory::ReadResult TransformationHistory::Read(Ogre::Vector3 *position, Ogre::Vector3 *velocity, Ogre::Quaternion *orientation,
								 RakNet::TimeMS when, RakNet::TimeMS curTime)
{
	int historySize = history.Size();
	if (historySize==0)
	{
		return VALUES_UNCHANGED;
	}

	int i;
	for (i=historySize-1; i>=0; i--)
	{
		const TransformationHistoryCell &cell = history[i];
		if (when >= cell.time)
		{
			if (i==historySize-1)
			{
				if (curTime<=cell.time)
					return VALUES_UNCHANGED;

				float divisor = (float)(curTime-cell.time);
				RakAssert(divisor!=0.0f);
				float lerp = (float)(when - cell.time) / divisor;
				if (position)
					*position=cell.position + (*position-cell.position) * lerp;
				if (velocity)
					*velocity=cell.velocity + (*velocity-cell.velocity) * lerp;
				if (orientation)
					*orientation = Ogre::Quaternion::Slerp(lerp, cell.orientation, *orientation,true);
			}
			else
			{
				const TransformationHistoryCell &nextCell = history[i+1];
				float divisor = (float)(nextCell.time-cell.time);
				RakAssert(divisor!=0.0f);
				float lerp = (float)(when - cell.time) / divisor;
				if (position)
					*position=cell.position + (nextCell.position-cell.position) * lerp;
				if (velocity)
					*velocity=cell.velocity + (nextCell.velocity-cell.velocity) * lerp;
				if (orientation)
					*orientation = Ogre::Quaternion::Slerp(lerp, cell.orientation, nextCell.orientation,true);
			}
			return INTERPOLATED;
		}
	}

	// Return the oldest one
	const TransformationHistoryCell &cell = history.Peek();
	if (position)
		*position=cell.position;
	if (orientation)
		*orientation=cell.orientation;
	if (velocity)
		*velocity=cell.velocity;
	return READ_OLDEST;
}
void TransformationHistory::Clear(void)
{
	history.Clear(_FILE_AND_LINE_);
}