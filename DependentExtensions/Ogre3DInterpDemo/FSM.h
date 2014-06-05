/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __FINITE_STATE_MACHINE_H
#define __FINITE_STATE_MACHINE_H

#include "DS_List.h"

class State;

// Finite state machine
// Stores a stack of states
// The top of the stack is the current state.  There is only one current state.  The stack contains the state history.
// OnEnter, OnLeave, FSMAddRef, FSMRemoveRef of class State is called as appropriate.
class FSM
{
public:
	FSM();
	~FSM();
	void Clear(void);
	State *CurrentState(void) const;
	State *GetState(const int index) const;
	int GetStateIndex(State *state) const;
	int GetStateHistorySize(void) const;
	void RemoveState(const int index);
	void AddState(State *state);
	void ReplaceState(const int index, State *state);
	void SetPriorState(const int index);

protected:
	DataStructures::List<State*> stateHistory;
};


#endif