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
#include "State.h"



State::State()
{
	fsmRefCount=0;
}
State::~State()
{

}
void State::OnEnter(const FSM *caller, bool loadResources)
{

}
void State::OnLeave(const FSM *caller, bool unloadResources)
{
	
}
void State::FSMAddRef(const FSM *caller)
{
	++fsmRefCount;
}
void State::FSMRemoveRef(const FSM *caller)
{
	RakAssert(fsmRefCount!=0);
	--fsmRefCount;
}
unsigned State::FSMRefCount(void) const
{
	return fsmRefCount;
}
void ManagedState::FSMRemoveRef(const FSM *caller)
{
	RakAssert(fsmRefCount!=0);
	if (--fsmRefCount)
		delete this;
}
