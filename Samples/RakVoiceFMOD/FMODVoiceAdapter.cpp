/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"

#include "FMODVoiceAdapter.h"
#include "fmod_errors.h"


/// To test sending to myself
//#define _TEST_LOOPBACK


// Number of RakVoice frames in the fmod sound
#define FRAMES_IN_SOUND 4

using namespace RakNet;

FMODVoiceAdapter FMODVoiceAdapter::instance;

FMODVoiceAdapter::FMODVoiceAdapter(){
	rakVoice=0;
	fmodSystem = 0;
	recSound=0;
	sound=0;
	channel=0;
	mute=false;
}

FMODVoiceAdapter* FMODVoiceAdapter::Instance(){
	return &instance;
}

bool FMODVoiceAdapter::SetupAdapter(FMOD::System *fmodSystem, RakVoice *rakVoice)
{
	FMOD_RESULT fmodErr;

	RakAssert(fmodSystem);
	RakAssert(rakVoice);
	// Make sure rakVoice was initialized
	RakAssert((rakVoice->IsInitialized())&&(rakVoice->GetRakPeerInterface()!=NULL));

	this->fmodSystem = fmodSystem;
	this->rakVoice = rakVoice;
	lastPlayPos = 0;
	lastRecordingPos = 0;

	//
	// Create the FMOD sound used to record
	//
	FMOD_CREATESOUNDEXINFO exinfo;
	memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
	exinfo.cbsize           = sizeof(FMOD_CREATESOUNDEXINFO);
	exinfo.numchannels      = 1;
	exinfo.format           = FMOD_SOUND_FORMAT_PCM16;
	exinfo.defaultfrequency = rakVoice->GetSampleRate();
	exinfo.length			= rakVoice->GetBufferSizeBytes()*FRAMES_IN_SOUND;

	fmodErr = fmodSystem->createSound(0, FMOD_2D | FMOD_SOFTWARE | FMOD_OPENUSER, &exinfo, &recSound);
	if (fmodErr!=FMOD_OK)
		return false;

	// Create the FMOD sound used to play incoming sound data
	fmodErr = fmodSystem->createSound(0, FMOD_2D | FMOD_SOFTWARE | FMOD_OPENUSER, &exinfo, &sound);
	if (fmodErr!=FMOD_OK)
		return false;

	// Start playing the sound used for output
	sound->setMode(FMOD_LOOP_NORMAL);
	fmodErr= fmodSystem->playSound(FMOD_CHANNEL_REUSE, sound, false, &channel);
	if (fmodErr!=FMOD_OK)
		return false;

	// Start recording
	fmodErr=fmodSystem->recordStart(0,recSound, true);
	if (fmodErr!=FMOD_OK)
		return false;

	return true;
}


void FMODVoiceAdapter::Update(void)
{
	RakAssert(fmodSystem);
	UpdateSound(true);
	UpdateSound(false);
}

void FMODVoiceAdapter::Release(void)
{
	FMOD_RESULT err;

	if (fmodSystem==NULL) return;

	// Stop recording
	bool recording=false;
	err = fmodSystem->isRecording(0,&recording);
	RakAssert(err==FMOD_OK);
	if (recording){
		fmodSystem->recordStop(0);
	}

	// Stop what we hear
	bool playing;
	err = channel->isPlaying(&playing);
	RakAssert(err==FMOD_OK);
	if (playing){
		channel->stop();
	}

	if (recSound!=NULL)
	{
		recSound->release();
		recSound = NULL;
	}

	if (sound!=NULL)
	{
		sound->release();
		sound = NULL;
	}
	
}


void FMODVoiceAdapter::SetMute(bool mute)
{
	this->mute = mute;
}


void FMODVoiceAdapter::UpdateSound(bool isRec)
{
	FMOD_RESULT fmodErr;
	unsigned int soundLength;
	const int sampleSize = 2;

	FMOD::Sound *snd = (isRec) ? recSound : sound;
	unsigned int& lastPos = (isRec) ? lastRecordingPos : lastPlayPos;

	// get current Play or recording position
	unsigned int currPos;
	if (isRec){
		fmodErr=fmodSystem->getRecordPosition(0,&currPos);
		RakAssert(fmodErr==FMOD_OK);
	} else {
		fmodErr=channel->getPosition(&currPos, FMOD_TIMEUNIT_PCM);
		RakAssert(fmodErr==FMOD_OK);
	}

	// Get length of sound in samples
	fmodErr=snd->getLength(&soundLength, FMOD_TIMEUNIT_PCM);
	RakAssert(fmodErr==FMOD_OK);

	// calculate some variables we'll need ahead
	int bufferSizeBytes = rakVoice->GetBufferSizeBytes();

	// Round down the current position to a multiple of buffer size in samples
	currPos -= currPos % (bufferSizeBytes/sampleSize);

	if ( ((!isRec)||(isRec && !mute)) && (currPos != lastPos) ) 	
	{
		void *ptr1, *ptr2;
		unsigned int len1, len2;
		int blockLength;
	
		blockLength = (int)currPos - (int)lastPos;
		// Check for wrap around, and adjust
		if (blockLength < 0)
		{
			blockLength += soundLength;
		}

		// Lock to get access to the raw data
		snd->lock(lastPos * sampleSize, blockLength * sampleSize, &ptr1, &ptr2, &len1, &len2);

		//  Since the length and current position are both a multiple of bufferSizeBytes
		// just treat treat one full buffer at a time
		int numFrames = len1 / bufferSizeBytes;
		while(numFrames--){
			if (isRec) {
				BroadcastFrame(ptr1);
			} else {
				rakVoice->ReceiveFrame(ptr1);
			}
			ptr1 = (char*)ptr1 + bufferSizeBytes;
		}
		numFrames = len2 / bufferSizeBytes;
		while(numFrames--) {
			if (isRec){
				BroadcastFrame(ptr2);
			} else {
				rakVoice->ReceiveFrame(ptr2);
			}
			ptr2 = (char*)ptr2 + bufferSizeBytes;
		}

		snd->unlock(ptr1, ptr2, len1, len2);
	}

	lastPos = currPos;
}



void FMODVoiceAdapter::BroadcastFrame(void *ptr)
{
#ifndef _TEST_LOOPBACK
	unsigned i;

	unsigned int numPeers = rakVoice->GetRakPeerInterface()->GetMaximumNumberOfPeers();
	for (i=0; i < numPeers; i++)
	{
		rakVoice->SendFrame(rakVoice->GetRakPeerInterface()->GetGUIDFromIndex(i), ptr);
	}
#else
	rakVoice->SendFrame(RakNet::UNASSIGNED_SYSTEM_ADDRESS, ptr);
#endif

}
