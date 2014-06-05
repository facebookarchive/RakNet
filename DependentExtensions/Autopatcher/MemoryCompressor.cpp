/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

// See http://www.bzip.org/1.0.3/bzip2-manual-1.0.3.html#low-level for docs on bzip
#include "MemoryCompressor.h"
#include <assert.h>
#include <stdlib.h>

CompressorBase::CompressorBase()
{
	output=0;
	allocatedSize=0;
	streamInited=false;
	stream.bzalloc=0;
	stream.bzfree=0;
	stream.opaque=0;
	totalRead=totalWritten=0;
}
CompressorBase::~CompressorBase()
{
}
MemoryCompressor::MemoryCompressor()
{
	compressedInputLength=0;
}
MemoryCompressor::~MemoryCompressor()
{
	Clear();
}
MemoryDecompressor::~MemoryDecompressor()
{
	Clear();
}
bool MemoryCompressor::Compress(char *input, const unsigned inputLength, bool finish)
{
	int res;
	unsigned inBefore,outBefore, read;
	unsigned written;

	if (output==0)
	{
		allocatedSize=inputLength;
		if (allocatedSize < BZ_MAX_UNUSED)
			allocatedSize=BZ_MAX_UNUSED;
		output=(char*)malloc(allocatedSize);
	}

	if (streamInited==false)
	{
		res = BZ2_bzCompressInit ( &stream, 
			9, // x 100K Block size.  Memory to use = 400k + ( 8 x block size ).  So this is 400K + 9 x 900K = 7.6 megabytes.  Larger sizes give better compression.
			0, // Verbosity.
			0 ); // Default work factor

		streamInited=true;
		if (res!=BZ_OK)
			return false;
	}

	read=written=0;
	unsigned readThisSession;
	readThisSession=0;
	compressedInputLength+=inputLength;

	if (totalWritten==allocatedSize)
	{
		allocatedSize+=inputLength;
		output=(char*)realloc(output, allocatedSize);
	}

	while(1)
	{
		stream.next_out=output+totalWritten;
		stream.avail_in=inputLength-readThisSession;
		stream.avail_out=allocatedSize-totalWritten;
		stream.next_in=input+readThisSession;
		inBefore=stream.total_in_lo32;
		outBefore=stream.total_out_lo32;
		//printf("%i\n", stream.avail_in);
		res = BZ2_bzCompress( &stream, finish ? BZ_FINISH : BZ_RUN );
		read=stream.total_in_lo32-inBefore;
		written=stream.total_out_lo32-outBefore;
		totalRead+=read;
		totalWritten+=written;
		readThisSession+=read;

		if (stream.avail_out>0)
		// if ((stream.avail_in==0 && stream.avail_out>0) || (read==0 && written==0))
		{
			if (finish)
			{
				allocatedSize=GetTotalOutputSize();
				output=(char*)realloc(output,allocatedSize);
				BZ2_bzCompressEnd( &stream );
				streamInited=false;
			}

			return true;
		}

		if (totalWritten==allocatedSize || read==0)
		{
			allocatedSize*=2;
			output=(char*)realloc(output, allocatedSize);
		}
	}
}
bool MemoryDecompressor::Decompress(char *input, const unsigned inputLength, bool ignoreStreamEnd)
{
	unsigned inBefore,outBefore, read;
	unsigned written;

	int res;
	if (output==0)
	{
		allocatedSize=inputLength*2;
		if (allocatedSize < BZ_MAX_UNUSED)
			allocatedSize=BZ_MAX_UNUSED;
		output=(char*)malloc(allocatedSize);
	}

	if (streamInited==false)
	{
		res = BZ2_bzDecompressInit( &stream, 
			0, // Verbosity.
			0 ); // Disable small memory usage

		streamInited=true;

		if (res!=BZ_OK)
			return false;
	}

	unsigned readThisSession;
	readThisSession=0;
	read=written=0;

	if (totalWritten==allocatedSize)
	{
		allocatedSize+=inputLength*4;
		output=(char*)realloc(output, allocatedSize);
	}

	while(1)
	{
		stream.next_out=output+totalWritten;
		stream.avail_in=inputLength-readThisSession;
		stream.avail_out=allocatedSize-totalWritten;
		stream.next_in=input+readThisSession;
		inBefore=stream.total_in_lo32;
		outBefore=stream.total_out_lo32;
		res = BZ2_bzDecompress( &stream );
		read=stream.total_in_lo32-inBefore;
		written=stream.total_out_lo32-outBefore;
		readThisSession+=read;
		totalRead+=read;
		totalWritten+=written;

		if (res==BZ_STREAM_END)
		{
			BZ2_bzDecompressEnd( &stream );
			
			if (ignoreStreamEnd==true)
			{
				// Stream end marker but there is more data so just keep reading
				res = BZ2_bzDecompressInit( &stream, 
					0, // Verbosity.
					0 ); // Disable small memory usage
			}
			else
			{
				streamInited=false;
				return true;
			}
		}
		else if ((stream.avail_in==0 && stream.avail_out>0) || (read==0 && written==0))
		{
			allocatedSize=GetTotalOutputSize();
			output=(char*)realloc(output,allocatedSize);
			return true;
		}
		else if (res!=BZ_OK)
		{
			Clear();
			return false;
		}

		if (totalWritten==allocatedSize || read==0)
		{
			allocatedSize+=inputLength*4;
			output=(char*)realloc(output, allocatedSize);
		}
	}
}
char *CompressorBase::GetOutput(void) const
{
	return output;
}
unsigned MemoryCompressor::GetCompressedInputLength(void) const
{
	return compressedInputLength;
}
unsigned CompressorBase::GetTotalOutputSize(void) const
{
	return totalWritten;
}
unsigned CompressorBase::GetTotalInputSize(void) const
{
	return totalRead;
}
void MemoryCompressor::Clear(void)
{
	if (output)
	{
		free(output);
		output=0;
	}

	if (streamInited)
		BZ2_bzCompressEnd( &stream );

	totalRead=totalWritten=compressedInputLength=0;
}
void MemoryDecompressor::Clear(void)
{
	if (output)
	{
		free(output);
		output=0;
	}

	if (streamInited)
		BZ2_bzDecompressEnd( &stream );
}

