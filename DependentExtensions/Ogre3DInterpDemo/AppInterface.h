/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __APP_INTERFACE_H
#define __APP_INTERFACE_H

#include "AppTypes.h"


class FSM;
class RunnableState;
class AppInterface
{
public:
	AppInterface();
	virtual ~AppInterface();
	// Called first to initialize memory
	virtual void PreConfigure(void);
	// Called after LoadDefaultResources.  Return false to quit.
	virtual bool Configure(void)=0;
	// Called to do startup required after Configuration
	virtual void PostConfigure(const char *defaultResourceConfigurationPath);
	virtual void Update(AppTime curTimeMS,AppTime elapsedTimeMS);
	virtual void OnAppShutdown(void);

	// Logging, lifetime may be ignored
	virtual void DebugOut(unsigned int lifetimeMS, const char *format, ...);

	// Like focus in windows - start and stop rendering but don't necessarily stop running.
	bool HasFocus(void) const;
	virtual void SetFocus(bool hasFocus);

	// Built-in state machine
	void AllocateStates(int numStates);
	virtual void SetState(int stateType, RunnableState* state);
	RunnableState* GetState(int stateType) const;
	RunnableState* GetCurrentState(void) const;
	void PushState(RunnableState* state);
	void PushState(int stateType);
	void PopState(int popCount=1);
	int GetStateHistorySize(void) const;

	// Just setting a flag
	virtual bool ShouldQuit(void) const;
	void Quit(void);
	AppTime GetLastCurrentTime(void) const;
	AppTime GetLastElapsedTime(void) const;

protected:
	// Allocated in PostConfigure
	FSM *primaryFSM;
	// Up to the derived class to allocate this using allocateStates
	RunnableState **primaryStates;
	AppTime lastElapsedTimeMS, lastCurTimeMS;
	int primaryStatesLength;
	bool hasFocus;
	bool quit;
};

#endif
