/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

/// \file DS_HuffmanEncodingTreeFactory.h
/// \internal
/// \brief Creates instances of the class HuffmanEncodingTree
///

 
#ifndef __HUFFMAN_ENCODING_TREE_FACTORY
#define __HUFFMAN_ENCODING_TREE_FACTORY

#include "RakMemoryOverride.h"

namespace RakNet {
/// Forward declarations
class HuffmanEncodingTree;

/// \brief Creates instances of the class HuffmanEncodingTree
/// \details This class takes a frequency table and given that frequence table, will generate an instance of HuffmanEncodingTree
class HuffmanEncodingTreeFactory
{
public:
	/// Default constructor
	HuffmanEncodingTreeFactory();
	
	/// \brief Reset the frequency table. 
	/// \details You don't need to call this unless you want to reuse the class for a new tree
	void Reset( void );
	
	/// \brief Pass an array of bytes to this to add those elements to the frequency table.
	/// \param[in] array the data to insert into the frequency table 
	/// \param[in] size the size of the data to insert 
	void AddToFrequencyTable( unsigned char *array, int size );
	
	/// \brief Copies the frequency table to the array passed. Retrieve the frequency table.
	/// \param[in] _frequency The frequency table used currently
	void GetFrequencyTable( unsigned int _frequency[ 256 ] );
	
	/// \brief Returns the frequency table as a pointer.
	/// \return the address of the frenquency table 
	unsigned int * GetFrequencyTable( void );
	
	/// \brief Generate a HuffmanEncodingTree.
	/// \details You can also use GetFrequencyTable and GenerateFromFrequencyTable in the tree itself
	/// \return The generated instance of HuffmanEncodingTree
	HuffmanEncodingTree * GenerateTree( void );
	
private:

	/// Frequency table
	unsigned int frequency[ 256 ];
};

} // namespace RakNet

#endif
