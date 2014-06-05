/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "VariableListDeltaTracker.h"

using namespace RakNet;

VariableListDeltaTracker::VariableListDeltaTracker() {nextWriteIndex=0;}
VariableListDeltaTracker::~VariableListDeltaTracker()
{
	unsigned int i;
	for (i=0; i < variableList.Size(); i++)
		rakFree_Ex(variableList[i].lastData,_FILE_AND_LINE_);
}

// Call before using a series of WriteVar
void VariableListDeltaTracker::StartWrite(void) {nextWriteIndex=0;}

void VariableListDeltaTracker::FlagDirtyFromBitArray(unsigned char *bArray)
{
	unsigned short readOffset=0;
	for (readOffset=0; readOffset < variableList.Size(); readOffset++)
	{
		bool result = ( bArray[ readOffset >> 3 ] & ( 0x80 >> ( readOffset & 7 ) ) ) !=0;

		if (result==true)
			variableList[readOffset].isDirty=true;
	}
}
VariableListDeltaTracker::VariableLastValueNode::VariableLastValueNode()
{
	lastData=0;
}
VariableListDeltaTracker::VariableLastValueNode::VariableLastValueNode(const unsigned char *data, int _byteLength)
{
	lastData=(char*) rakMalloc_Ex(_byteLength,_FILE_AND_LINE_);
	memcpy(lastData,data,_byteLength);
	byteLength=_byteLength;
	isDirty=false;
}
VariableListDeltaTracker::VariableLastValueNode::~VariableLastValueNode()
{
}
