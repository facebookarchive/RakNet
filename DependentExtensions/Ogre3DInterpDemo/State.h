/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __STATE_H
#define __STATE_H

class FSM;

// States are stored in the FSM class (Finite state machine)
// The FSM only has one active state at a time and stores the state history stack
// State data can be held in this class - however data which is used between states is best stored elsewhere.
class State
{
public:
	State();
	~State();
	// OnEnter is called when this state will not be the current state
	// loadResources true means this state is now in the history stack and was not there before
	virtual void OnEnter(const FSM *caller, bool loadResources);
	// OnLeave is called when this state is currently the current state and will no longer be the current state
	// unloadResources true means this state is no longer in the history stack and we will probably not be entering it again via the back button
	virtual void OnLeave(const FSM *caller, bool unloadResources);
	// Called once for every time this state is added to the FSM history stack
	virtual void FSMAddRef(const FSM *caller);
	// Called once for every time this state is removed from the FSM history stack.
	virtual void FSMRemoveRef(const FSM *caller);
	// The number of times this state is in the FSM history stack.
	unsigned FSMRefCount(void) const;
protected:
	unsigned fsmRefCount;
};

// Same as State, but self-deletes when fsmRefCount==0
class ManagedState : public State
{
public:
	virtual void FSMRemoveRef(const FSM *caller);
};

#endif
