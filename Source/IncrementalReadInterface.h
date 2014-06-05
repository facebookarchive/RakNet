/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __INCREMENTAL_READ_INTERFACE_H
#define __INCREMENTAL_READ_INTERFACE_H

#include "FileListNodeContext.h"
#include "Export.h"

namespace RakNet
{

class RAK_DLL_EXPORT IncrementalReadInterface
{
public:
	IncrementalReadInterface() {}
	virtual ~IncrementalReadInterface() {}

	/// Read part of a file into \a destination
	/// Return the number of bytes written. Return 0 when file is done.
	/// \param[in] filename Filename to read
	/// \param[in] startReadBytes What offset from the start of the file to read from
	/// \param[in] numBytesToRead How many bytes to read. This is also how many bytes have been allocated to preallocatedDestination
	/// \param[out] preallocatedDestination Write your data here
	/// \return The number of bytes read, or 0 if none
	virtual unsigned int GetFilePart( const char *filename, unsigned int startReadBytes, unsigned int numBytesToRead, void *preallocatedDestination, FileListNodeContext context);
};

} // namespace RakNet

#endif
