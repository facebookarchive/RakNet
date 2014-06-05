/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "FSM.h"
#include "State.h"
#include "RakAssert.h"



FSM::FSM()
{

}
FSM::~FSM()
{
	Clear();
}
void FSM::Clear(void)
{
	unsigned i;
	if (stateHistory.Size())
		stateHistory[stateHistory.Size()-1]->OnLeave(this, true);
	for (i=0; i < stateHistory.Size(); i++)
		stateHistory[i]->FSMRemoveRef(this);
	stateHistory.Clear(false, _FILE_AND_LINE_);
}
State *FSM::CurrentState(void) const
{
	if (stateHistory.Size()==0)
		return 0;
	return stateHistory[stateHistory.Size()-1];
}
State *FSM::GetState(int index) const
{
	RakAssert(index>=0 && index < (int) stateHistory.Size());
	return stateHistory[(unsigned) index];
}
int FSM::GetStateIndex(State *state) const
{
	return (int) stateHistory.GetIndexOf(state);
}
int FSM::GetStateHistorySize(void) const
{
	return stateHistory.Size();
}
void FSM::RemoveState(const int index)
{
	RakAssert(index>=0 && index < (int) stateHistory.Size());
	if (index==stateHistory.Size()-1)
		stateHistory[index]->OnLeave(this, true);
	stateHistory[index]->FSMRemoveRef(this);
	stateHistory.RemoveAtIndex((const unsigned int)index);
	if (index==stateHistory.Size())
		stateHistory[stateHistory.Size()-1]->OnEnter(this, false);
}
void FSM::AddState(State *state)
{
	if (stateHistory.Size())
		stateHistory[stateHistory.Size()-1]->OnLeave(this, false);
	state->FSMAddRef(this);
	state->OnEnter(this, true);
	stateHistory.Insert(state, _FILE_AND_LINE_ );
}
void FSM::ReplaceState(const int index, State *state)
{
	RakAssert(index>=0 && index < (int) stateHistory.Size());
	if (state!=stateHistory[index])
	{
		if (index==stateHistory.Size()-1)
			stateHistory[index]->OnLeave(this, true);
		stateHistory[index]->FSMRemoveRef(this);
		state->FSMAddRef(this);
		if (index==stateHistory.Size()-1)
			state->OnEnter(this, true);
		stateHistory[index]=state;
	}
}
void FSM::SetPriorState(const int index)
{
	RakAssert(index>=0 && index < (int) stateHistory.Size());
	stateHistory[stateHistory.Size()-1]->OnLeave(this, true);
	for (unsigned i=stateHistory.Size()-1; i > (unsigned) index; --i)
	{
		stateHistory[i]->FSMRemoveRef(this);
		stateHistory.RemoveFromEnd();
	}
	stateHistory[index]->OnEnter(this, false);
}
