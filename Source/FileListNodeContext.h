/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

/// \file FileListNodeContext.h
///


#ifndef __FILE_LIST_NODE_CONTEXT_H
#define __FILE_LIST_NODE_CONTEXT_H

#include "BitStream.h"

struct FileListNodeContext
{
	FileListNodeContext() {dataPtr=0; dataLength=0;}
	FileListNodeContext(unsigned char o, uint32_t f1, uint32_t f2, uint32_t f3) : op(o), flnc_extraData1(f1), flnc_extraData2(f2), flnc_extraData3(f3) {dataPtr=0; dataLength=0;}
	~FileListNodeContext() {}

	unsigned char op;
	uint32_t flnc_extraData1;
	uint32_t flnc_extraData2;
	uint32_t flnc_extraData3;
	void *dataPtr;
	unsigned int dataLength;
};

inline RakNet::BitStream& operator<<(RakNet::BitStream& out, FileListNodeContext& in)
{
	out.Write(in.op);
	out.Write(in.flnc_extraData1);
	out.Write(in.flnc_extraData2);
	out.Write(in.flnc_extraData3);
	return out;
}
inline RakNet::BitStream& operator>>(RakNet::BitStream& in, FileListNodeContext& out)
{
	in.Read(out.op);
	bool success = in.Read(out.flnc_extraData1);
	(void) success;
	assert(success);
	success = in.Read(out.flnc_extraData2);
	(void) success;
	assert(success);
	success = in.Read(out.flnc_extraData3);
	(void) success;
	assert(success);
	return in;
}

#endif
