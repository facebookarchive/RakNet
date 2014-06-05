/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

/// \file
/// \brief Connection between DirectSound and RakVoice
///

/*
This sample uses RakVoice along with DirectSound 8.

Instead of using IDirectSoundBuffer::GetCurrentPosition / IDirectSoundCaptureBuffer::GetCurrentPosition
to keep track of the play/write cursors, notifications are used instead.
Speex encodes/decodes sound in blocks (frames), so the size of the used DirectSound buffers is
multiple of the Speex's frame size. This makes it easier to implement DirectSound notifications at the end
of each frame.

*/

#ifndef __DSOUNDVOICEADAPTER_H
#define __DSOUNDVOICEADAPTER_H

#include "RakVoice.h"

// If you get:
// Error	1	fatal error C1083: Cannot open include file: 'dsound.h': No such file or directory
// It is because this project depends on Directx SDK. You must have the DirectX SDK installed.
//  Also, check if you have the DXSDK_DIR environment variable point to the right DX SDK path,
// or change the project settings to point to the right include and lib directories
#include "dsound.h"
#include "dxerr.h"

namespace RakNet {

class DSoundVoiceAdapter
{

public:

	// --------------------------------------------------------------------------------------------
	// User functions
	// --------------------------------------------------------------------------------------------

	/// Returns the singleton
	static DSoundVoiceAdapter* Instance();

	/// \brief Setups the connection between RakVoice and DirectSound
	///   This function initializes all the required DirectSound objects for you. If you already have
	///  initialized DirectSound on your own, use the other supplied SetupAdater function, where you can
	///  pass your own DirectSound device object
	/// \param[in] rakVoice RakVoice object to use, fully Initialized AND attached to a RakPeerInterface.
	/// \param[in] hwnd Your Window Handle. Required for DirectSound initialization
	/// \param[in] dwCoopLevel DirectSound cooperative level. Required for DirectSound initialization
	bool SetupAdapter(RakVoice *rakVoice, HWND hwnd, DWORD dwCoopLevel=DSSCL_EXCLUSIVE);

	/// \brief Setups the connection between RakVoice and DirectSound
	/// \param[in] rakVoice RakVoice object to use, fully Initialized AND attached to a RakPeerInterface.
	/// \param[in] pDS DirectSound Device Object to use.
	/// \pre IMPORTANT : Don't forget to initialized and attach rakVoice, before calling this method.
	/// \return true on success, false if an error occurred.
	bool SetupAdapter(RakVoice *rakVoice, IDirectSound8 *pDS);


	/// \brief Releases any resources used
	void Release();

	/// \brief This needs to be called once per game frame
	void Update();

	/// \brief Turns on/off outgoing traffic
	/// \param[in] true to mute, false to allow outgoing traffic.
	void SetMute(bool mute);

	/// \brief Returns the used DirectSound Device object
	IDirectSound8* GetDSDeviceObject();

private:

	enum
	{
		FRAMES_IN_SOUND = 2 // Number of voice frames the DirectSound buffers will contain
	};

	/// Internal setup function called after proper directsound initialization
	bool SetupAdapter(RakVoice *rakVoice);
	bool SetupIncomingBuffer();	
	bool SetupOutgoingBuffer();
	void BroadcastFrame(void *ptr);


	DSoundVoiceAdapter();
	DSoundVoiceAdapter(DSoundVoiceAdapter &) {}

	static DSoundVoiceAdapter instance;
	RakVoice *rakVoice;
	IDirectSound8 *ds; // Pointer to the DirectSound Device Object
	IDirectSoundCapture8 *dsC; // Pointer to the DirectSound Capture Device Object
	IDirectSoundBuffer8 *dsbIncoming; // DirectSound buffer for incoming sound (what  you'll hear)
	IDirectSoundCaptureBuffer8 *dsbOutgoing; // DirectSound buffer used for outgoing sound (capture from your microphone and send to the other players)
	bool mute;

	// DirectSound notification positions with the required Win32 Event objects
	DSBPOSITIONNOTIFY incomingBufferNotifications[FRAMES_IN_SOUND];
	DSBPOSITIONNOTIFY outgoingBufferNotifications[FRAMES_IN_SOUND];

};

} // namespace RakNet

#endif
