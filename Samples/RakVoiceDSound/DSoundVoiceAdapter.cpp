/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "DSoundVoiceAdapter.h"
#include "RakAssert.h"
#include "RakPeerInterface.h"

/// To test sending to myself
//#define _TEST_LOOPBACK

using namespace RakNet;

DSoundVoiceAdapter DSoundVoiceAdapter::instance;

DSoundVoiceAdapter::DSoundVoiceAdapter()
{
	rakVoice = 0;
	ds = 0;
	dsC = 0;
	dsbIncoming = 0;
	dsbOutgoing = 0;
	mute = false;
	memset(incomingBufferNotifications,0,sizeof(incomingBufferNotifications));
	memset(outgoingBufferNotifications,0,sizeof(outgoingBufferNotifications));
}

DSoundVoiceAdapter* DSoundVoiceAdapter::Instance()
{
	return &instance;
}

bool DSoundVoiceAdapter::SetupAdapter(RakVoice *rakVoice, HWND hwnd, DWORD dwCoopLevel)
{
	// Check if it was already initialized
	RakAssert(ds == 0);
	if (ds != 0)
		return false;

	HRESULT hr;

	if (FAILED(hr = DirectSoundCreate8(NULL, &ds, NULL)))
	{
		DXTRACE_ERR_MSGBOX(L"DirectSoundCreate8, when initiliazing DirectSound", hr);
		return false;
	}

	if (FAILED(hr = ds->SetCooperativeLevel(hwnd, dwCoopLevel)))
	{
		DXTRACE_ERR_MSGBOX(L"IDirectSound8::SetCooperativeLevel", hr);
		Release();
		return false;
	}

	// Check if the rest of the initialization fails, and if so, release any allocated resources
	if (SetupAdapter(rakVoice))
	{
		return true;
	}
	else
	{
		Release();
		return false;
	}
}

bool DSoundVoiceAdapter::SetupAdapter(RakVoice *rakVoice, IDirectSound8 *pDS)
{
	// Check if it was already initialized
	RakAssert(ds == 0);
	if (ds != 0)
		return false;

	// User provided his own device object, so use it and add another reference
	pDS->AddRef();
	ds = pDS;
	
	// Check if the rest of the initialization fails, and if so, release any allocated resources
	if (SetupAdapter(rakVoice))
	{
		return true;
	}
	else
	{
		Release();
		return false;
	}
}

bool DSoundVoiceAdapter::SetupAdapter(RakVoice *rakVoice)
{
	RakAssert(rakVoice);
	// Make sure rakVoice was initialized
	RakAssert((rakVoice->IsInitialized())&&(rakVoice->GetRakPeerInterface()!=NULL));

	this->rakVoice = rakVoice;

	if (!SetupIncomingBuffer())
		return false;
	
	return SetupOutgoingBuffer();
}

bool DSoundVoiceAdapter::SetupIncomingBuffer()
{

	//
	//
	// Create the buffer for incoming sound
	//
	//

	WAVEFORMATEX wfx; 
	DSBUFFERDESC dsbdesc; 
	LPDIRECTSOUNDBUFFER pDsb = NULL;
	HRESULT hr; 
	// Set up WAV format structure. 
	memset(&wfx, 0, sizeof(WAVEFORMATEX)); 
	wfx.wFormatTag = WAVE_FORMAT_PCM; 
	wfx.nChannels = 1; 
	wfx.nSamplesPerSec = rakVoice->GetSampleRate(); 
	wfx.nBlockAlign = 2; 
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign; 
	wfx.wBitsPerSample = 16; 
	// Set up DSBUFFERDESC structure. 
	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); 
	dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
	dsbdesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY
		| DSBCAPS_LOCSOFTWARE; // Create in software, because DX documentation says its the best option when using notifications
	dsbdesc.dwBufferBytes = rakVoice->GetBufferSizeBytes()*FRAMES_IN_SOUND; 
	dsbdesc.lpwfxFormat = &wfx; 
	// Create buffer. 
	if (FAILED(hr = ds->CreateSoundBuffer(&dsbdesc, &pDsb, NULL)))
	{
		DXTRACE_ERR_MSGBOX(L"IDirectSound8::CreateSoundBuffer, when creating buffer for incoming sound )", hr);
		return false;
	}
	hr = pDsb->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*) &dsbIncoming);
	pDsb->Release();
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(L"IDirectSoundBuffer::QueryInterface, when getting IDirectSoundBuffer8 interface for incoming sound", hr);
		return false;
	}
	//
	// Setup the notification events
	//
	for (int i=0; i<FRAMES_IN_SOUND; i++)
	{
		incomingBufferNotifications[i].dwOffset = i*rakVoice->GetBufferSizeBytes();
#if defined(WINDOWS_PHONE_8)
		if ((incomingBufferNotifications[i].hEventNotify = CreateEventEx(0, 0, CREATE_EVENT_MANUAL_RESET, 0))==NULL)
#else
		if ((incomingBufferNotifications[i].hEventNotify = CreateEvent(NULL, TRUE, FALSE, NULL))==NULL)
#endif
		{
			DXTRACE_ERR_MSGBOX(L"CreateEvent", GetLastError());
			return false;
		}
	}
	IDirectSoundNotify8 *dsbNotify=0;
	if (FAILED(hr=dsbIncoming->QueryInterface(IID_IDirectSoundNotify8, (LPVOID*) &dsbNotify)))
	{
		DXTRACE_ERR_MSGBOX(L"IDirectSoundBuffer8::QueryInterface, when getting IDirectSoundNotify8 interface for incoming sound", hr);
		return false;
	}
	hr = dsbNotify->SetNotificationPositions(FRAMES_IN_SOUND, incomingBufferNotifications);
	dsbNotify->Release();
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(L"IDirectSoundNotify8::SetNotificationPositions, when setting notifications for incoming sound", hr);
		return false;
	}

	if (FAILED(hr = dsbIncoming->Play(0,0,DSBPLAY_LOOPING)))
	{
		DXTRACE_ERR_MSGBOX(L"IDirectSoundBuffer8::Play, when starting incoming sound buffer", hr);
		return false;
	}

	return true;
}

bool DSoundVoiceAdapter::SetupOutgoingBuffer()
{
	HRESULT hr;

	//
	//
	// Create the buffer for outgoing sound
	//
	//
	if (FAILED(hr=DirectSoundCaptureCreate8(NULL, &dsC, NULL)))
	{
		DXTRACE_ERR_MSGBOX(L"DirectSoundCaptureCreate8", hr);
		return false;
	}

	// Set up WAV format structure. 
	WAVEFORMATEX wfx; 
	memset(&wfx, 0, sizeof(WAVEFORMATEX)); 
	wfx.wFormatTag = WAVE_FORMAT_PCM; 
	wfx.nChannels = 1; 
	wfx.nSamplesPerSec = rakVoice->GetSampleRate(); 
	wfx.nBlockAlign = 2; 
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign; 
	wfx.wBitsPerSample = 16; 
	// Set up DSCBUFFERDESC structure. 
	DSCBUFFERDESC dscbd;
	memset(&dscbd, 0, sizeof(DSCBUFFERDESC));
	dscbd.dwSize = sizeof(DSCBUFFERDESC);
	dscbd.dwFlags = 0;
	dscbd.dwBufferBytes = rakVoice->GetBufferSizeBytes()*FRAMES_IN_SOUND; 
	dscbd.dwReserved = 0;
	dscbd.lpwfxFormat = &wfx;
	dscbd.dwFXCount = 0;
	dscbd.lpDSCFXDesc = NULL;
	// Create capture buffer.
	LPDIRECTSOUNDCAPTUREBUFFER pDscb = NULL;
	if (FAILED(hr = dsC->CreateCaptureBuffer(&dscbd, &pDscb, NULL)))
	{
		DXTRACE_ERR_MSGBOX(L"IDirectSoundCapture8::CreateCaptureBuffer, when creating buffer for outgoing sound )", hr);
		return false;
	}
	hr = pDscb->QueryInterface(IID_IDirectSoundCaptureBuffer8, (LPVOID*) &dsbOutgoing);
	pDscb->Release();
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(L"IDirectSoundBuffer::QueryInterface, when getting IDirectSoundCaptureBuffer8 interface for outgoing sound", hr);
		return false;
	}
	//
	// Setup the notification events
	//
	for (int i=0; i<FRAMES_IN_SOUND; i++)
	{
		outgoingBufferNotifications[i].dwOffset = i*rakVoice->GetBufferSizeBytes();
		if ((outgoingBufferNotifications[i].hEventNotify = CreateEventEx(0, 0, CREATE_EVENT_MANUAL_RESET, 0))==NULL)
		{
			DXTRACE_ERR_MSGBOX(L"CreateEvent", GetLastError());
			return false;
		}
	}
	IDirectSoundNotify8 *dsbNotify=0;
	if (FAILED(hr=dsbOutgoing->QueryInterface(IID_IDirectSoundNotify8, (LPVOID*) &dsbNotify)))
	{
		DXTRACE_ERR_MSGBOX(L"IDirectSoundCaptureBuffer8::QueryInterface, when getting IDirectSoundNotify8 interface for outgoing sound", hr);
		return false;
	}
	hr = dsbNotify->SetNotificationPositions(FRAMES_IN_SOUND, outgoingBufferNotifications);
	dsbNotify->Release();
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(L"IDirectSoundNotify8::SetNotificationPositions, when setting notifications for outgoing sound", hr);
		return false;
	}

	if (FAILED(hr = dsbOutgoing->Start(DSCBSTART_LOOPING)))
	{
		DXTRACE_ERR_MSGBOX(L"IDirectSoundCaptureBuffer8::Start, when starting outgoing sound buffer", hr);
		return false;
	}

	return true;
}

void DSoundVoiceAdapter::Release()
{
	// Release DirectSound buffer used for incoming voice
	if (dsbIncoming)
	{
		dsbIncoming->Stop();
		dsbIncoming->Release();
		dsbIncoming = 0;
	}

	// Release DirectSound buffer used for outgoing voice
	if (dsbOutgoing)
	{
		dsbOutgoing->Stop();
		dsbOutgoing->Release();
		dsbOutgoing = 0;
	}

	// Release DirectSound device object
	if (ds)
	{
		ds->Release();
		ds = 0;
	}

	if (dsC)
	{
		dsC->Release();
		dsC = 0;
	}

	// Release  the notification events
	for (int i=0; i<FRAMES_IN_SOUND;i++)
	{
		if (incomingBufferNotifications[i].hEventNotify!=0 && CloseHandle(incomingBufferNotifications[i].hEventNotify)==0)
		{
			DXTRACE_ERR_MSGBOX(L"CloseHandle", GetLastError());
		}
		if (outgoingBufferNotifications[i].hEventNotify!=0 && CloseHandle(outgoingBufferNotifications[i].hEventNotify)==0)
		{
			DXTRACE_ERR_MSGBOX(L"CloseHandle", GetLastError());
		}
	}
	memset(incomingBufferNotifications,0, sizeof(incomingBufferNotifications));
	memset(outgoingBufferNotifications,0, sizeof(outgoingBufferNotifications));
}

IDirectSound8* DSoundVoiceAdapter::GetDSDeviceObject()
{
	return ds;
}

void DSoundVoiceAdapter::Update()
{
	HRESULT hr;
	void *audioPtr;
	DWORD audioPtrbytes;

	for (int i=0; i<FRAMES_IN_SOUND; i++)
	{
		//
		// Update incoming sound
		//
		if (WaitForSingleObject(incomingBufferNotifications[i].hEventNotify, 0)==WAIT_OBJECT_0)
		{
			// The lock offset is the buffer right before the one the event refers to
			DWORD dwOffset = (i==0) ? incomingBufferNotifications[FRAMES_IN_SOUND-1].dwOffset : incomingBufferNotifications[i-1].dwOffset;
			hr = dsbIncoming->Lock(dwOffset, rakVoice->GetBufferSizeBytes(), &audioPtr, &audioPtrbytes, NULL, NULL, 0);
			RakAssert(audioPtrbytes==rakVoice->GetBufferSizeBytes());
			if (SUCCEEDED(hr))
			{
				rakVoice->ReceiveFrame(audioPtr);
				dsbIncoming->Unlock(audioPtr, audioPtrbytes, NULL, 0);
			}
			ResetEvent(incomingBufferNotifications[i].hEventNotify);
		}

		//
		// Update outgoing sound
		//
		if (WaitForSingleObject(outgoingBufferNotifications[i].hEventNotify, 0)==WAIT_OBJECT_0)
		{

			/* If we're set to mute, we don't send anything, and just reset the event */
			if (!mute)
			{		
				// The lock offset is the buffer right before the one the event refers to
				DWORD dwOffset = (i==0) ? outgoingBufferNotifications[FRAMES_IN_SOUND-1].dwOffset : outgoingBufferNotifications[i-1].dwOffset;
				hr = dsbOutgoing->Lock(dwOffset, rakVoice->GetBufferSizeBytes(), &audioPtr, &audioPtrbytes, NULL, NULL, 0);
				RakAssert(audioPtrbytes==rakVoice->GetBufferSizeBytes());
				if (SUCCEEDED(hr))
				{
					BroadcastFrame(audioPtr);
					dsbOutgoing->Unlock(audioPtr, audioPtrbytes, NULL, 0);
				}
			}
			ResetEvent(outgoingBufferNotifications[i].hEventNotify);
		}

	}
}

void DSoundVoiceAdapter::BroadcastFrame(void *ptr)
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

void DSoundVoiceAdapter::SetMute(bool mute)
{
	this->mute = mute;
}
