/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

/// \file DS_BytePool.h
///


#ifndef __BYTE_POOL_H
#define __BYTE_POOL_H

#include "RakMemoryOverride.h"
#include "DS_MemoryPool.h"
#include "Export.h"
#include "SimpleMutex.h"
#include "RakAssert.h"

// #define _DISABLE_BYTE_POOL
// #define _THREADSAFE_BYTE_POOL

namespace DataStructures
{
	// Allocate some number of bytes from pools.  Uses the heap if necessary.
	class RAK_DLL_EXPORT BytePool
	{
	public:
		BytePool();
		~BytePool();
		// Should be at least 8 times bigger than 8192
		void SetPageSize(int size);
		unsigned char* Allocate(int bytesWanted, const char *file, unsigned int line);
		void Release(unsigned char *data, const char *file, unsigned int line);
		void Clear(const char *file, unsigned int line);
	protected:	
		MemoryPool<unsigned char[128]> pool128;
		MemoryPool<unsigned char[512]> pool512;
		MemoryPool<unsigned char[2048]> pool2048;
		MemoryPool<unsigned char[8192]> pool8192;
#ifdef _THREADSAFE_BYTE_POOL
		SimpleMutex mutex128;
		SimpleMutex mutex512;
		SimpleMutex mutex2048;
		SimpleMutex mutex8192;
#endif
	};
}

#endif
