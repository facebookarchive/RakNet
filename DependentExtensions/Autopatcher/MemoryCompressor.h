/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef _MEMORY_COMPRESSOR_H
#define _MEMORY_COMPRESSOR_H

#include "bzlib.h"

/// \internal
/// Do not use this class directly.  Use MemoryCompressor and MemoryDecompressor.
class CompressorBase
{
public:
	CompressorBase();
	~CompressorBase();
	
	/// Get the compressed data.  The length currently written is returned from GetTotalOutputSize().
	/// \return The compressed data
	char *GetOutput(void) const;
	
	/// \return The number of bytes outputted so far.
	unsigned GetTotalOutputSize(void) const;
	
	/// \return The number of bytes input by the user so far
	unsigned GetTotalInputSize(void) const;
protected:
	bz_stream stream;
	char *output;
	unsigned allocatedSize;
	unsigned totalRead, totalWritten;
	bool streamInited;
};

/// Compress one or more blocks of data
class MemoryCompressor : public CompressorBase
{
public:
	MemoryCompressor();
	~MemoryCompressor();
	
	/// Compress a block of data.  Pass true to finish if this is the last block in the series.  If you don't know if it's the last block, you can call it again with 0 for inputLength
	/// \note Data passed to input isn't necessarily immediately compressed to output.  You can force a write by passing true to finish.
	/// Multiple calls concatenate the written data.
	/// \param[in] input A pointer to a block of data
	/// \param[in] inputLength The length of input
	/// \param[in] finish Write the last of the data.
	bool Compress(char *input, const unsigned inputLength, bool finish);
	
	/// Resets the compressor and all data.
	void Clear(void);

	// Number of bytes total passed to /a inputLength in the Compress() function
	unsigned GetCompressedInputLength(void) const;

protected:

	unsigned compressedInputLength;
};

class MemoryDecompressor : public CompressorBase
{
public:
	~MemoryDecompressor();
	
	/// Read \a inputLength bytes of compressed data from \a input
	/// Writes the decompressed output to GetOutput().  Note that unlike the class MemoryCompressor, output data is updated immediately and not internally buffered
	/// \param[in] input A pointer to a block of data
	/// \param[in] inputLength The length of input
	/// \param[in] ignoreStreamEnd Normally when Compress is called with finish==true stream end markers are placed.  These are honored, such that the read will end early if a stream marker is hit.  Pass true to ignore this and just output all the data.
	bool Decompress(char *input, const unsigned inputLength, bool ignoreStreamEnd);
	
	/// Resets the compressor and all data.
	void Clear(void);
};

#endif
