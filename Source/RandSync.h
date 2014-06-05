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
/// \brief \b [Internal] Random number generator
///



#ifndef __RAND_SYNC_H
#define __RAND_SYNC_H 

#include "Export.h"
#include "Rand.h"
#include "DS_Queue.h"
#include "NativeTypes.h"

namespace RakNet {

class BitStream;

class RAK_DLL_EXPORT RakNetRandomSync
{
public:
	RakNetRandomSync();
	virtual ~RakNetRandomSync();
	void SeedMT( uint32_t _seed );
	void SeedMT( uint32_t _seed, uint32_t skipValues );
	float FrandomMT( void );
	unsigned int RandomMT( void );
	uint32_t GetSeed( void ) const;
	uint32_t GetCallCount( void ) const;
	void SetCallCount( uint32_t i );

	virtual void SerializeConstruction(RakNet::BitStream *constructionBitstream);
	virtual bool DeserializeConstruction(RakNet::BitStream *constructionBitstream);
	virtual void Serialize(RakNet::BitStream *outputBitstream);
	virtual void Deserialize(RakNet::BitStream *outputBitstream);

protected:
	void Skip( uint32_t count );
	DataStructures::Queue<unsigned int> usedValues;
	uint32_t seed;
	uint32_t callCount;
	uint32_t usedValueBufferCount;
	RakNetRandom rnr;
};
} // namespace RakNet


#endif
