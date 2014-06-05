/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __PROFANITY_FILTER__H__
#define __PROFANITY_FILTER__H__

#include "DS_List.h"
#include "RakString.h"

namespace RakNet {

class ProfanityFilter
{
public:
	ProfanityFilter();
	~ProfanityFilter();

	// Returns true if the string has profanity, false if not.
	bool HasProfanity(const char *str);

	// Removes profanity. Returns number of occurrences of profanity matches (including 0)
	int FilterProfanity(const char *str, char *output, bool filter = true); 		
	
	// Number of profanity words loaded
	int Count();

	void AddWord(RakNet::RakString newWord);
private:	
	DataStructures::List<RakNet::RakString> words;

	char RandomBanChar();

	static char BANCHARS[];
	static char WORDCHARS[];
};

} // namespace RakNet

#endif // __PROFANITY__H__
