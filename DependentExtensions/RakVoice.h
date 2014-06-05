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
/// \brief Voice compression and transmission interface


#ifndef __RAK_VOICE_H
#define __RAK_VOICE_H

#include "RakNetTypes.h"
#include "PluginInterface2.h"
#include "DS_OrderedList.h"
#include "NativeTypes.h"

namespace RakNet {

class RakPeerInterface;

// How many frames large to make the circular buffers in the VoiceChannel structure
#define FRAME_OUTGOING_BUFFER_COUNT 100
#define FRAME_INCOMING_BUFFER_COUNT 100

/// \internal
struct VoiceChannel
{
	RakNetGUID guid;
	void *enc_state;
	void *dec_state;
	void *pre_state;
	unsigned int remoteSampleRate;
	
	// Circular buffer of unencoded sound data read from the user.
	char *outgoingBuffer;
	// Each frame sent to speex requires this many samples, of whatever size you are using.
	int speexOutgoingFrameSampleCount;
	// Index in is bytes.
	// Write index points to the next byte to write to, which must be free.
	unsigned outgoingReadIndex, outgoingWriteIndex;
	bool isSendingVoiceData;
	bool bufferOutput;
	bool copiedOutgoingBufferToBufferedOutput;
	unsigned short outgoingMessageNumber;

	// Circular buffer of unencoded sound data to be passed to the user.  Each element in the buffer is of size bufferSizeBytes bytes.
	char *incomingBuffer;
	int speexIncomingFrameSampleCount;
	unsigned incomingReadIndex, incomingWriteIndex;	// Index in bytes
	unsigned short incomingMessageNumber;  // The ID_VOICE message number we expect to get.  Used to drop out of order and detect how many missing packets in a sequence

	RakNet::TimeMS lastSend;
};
int VoiceChannelComp( const RakNetGUID &key, VoiceChannel * const &data );

/// Voice compression and transmission interface
class RAK_DLL_EXPORT RakVoice : public PluginInterface2
{
public:
	RakVoice();
	virtual ~RakVoice();

	// --------------------------------------------------------------------------------------------
	// User functions
	// --------------------------------------------------------------------------------------------

	/// \brief Starts RakVoice
	/// \param[in] speexSampleRate 8000, 16000, or 32000
	/// \param[in] bufferSizeBytes How many bytes long inputBuffer and outputBuffer are in SendFrame and ReceiveFrame are.  Should be your sample size * the number of samples to encode at once.
	void Init(unsigned short speexSampleRate, unsigned bufferSizeBytes);

	/// \brief Changes encoder complexity
	/// Specifying higher values might help when encoding non-speech sounds.
	/// \param[in] complexity 0 to 10. The higher the value, the more CPU it needs. Recommended values are from 2 to 4.
	void SetEncoderComplexity(int complexity);

	/// \brief Enables or disables VAD (Voice Activity Detection)
	/// Enabling VAD can help reduce the amount of data transmitted, by automatically disabling outgoing data, when no voice is detected.
	/// Don't turn this off or the receive buffer fills up and you eventually get very long delays!!
	/// \pre Only applies to encoder.
	/// \param[in] enable true to enable, false to disable. True by default
	void SetVAD(bool enable);

	/// \brief Enables or disables the noise filter
	/// \pre Only applies to encoder.
	/// \param[in] enable true to enable, false to disable.
	void SetNoiseFilter(bool enable);

	/// \brief Enables or disables VBR
	/// VBR is variable bitrate. Uses less bandwidth but more CPU if on.
	/// \pre Only applies to encoder.
	/// \param[in] enable true to enable VBR, false to disable
	void SetVBR(bool enable);

	/// \brief Returns the complexity of the encoder
	/// \pre Only applies to encoder.
	/// \return a value from 0 to 10.
	int GetEncoderComplexity(void);

	/// \brief Returns current state of VAD.
	/// \pre Only applies to encoder.
	/// \return true if VAD is enable, false otherwise
	bool IsVADActive(void);

	/// \brief Returns the current state of the noise filter
	/// \pre Only applies to encoder.
	/// \return true if the noise filter is active, false otherwise.
	bool IsNoiseFilterActive();

	/// \brief Returns the current state of VBR
	/// \pre Only applies to encoder.
	/// \return true if VBR is active, false otherwise.
	bool IsVBRActive();

	/// Shuts down RakVoice
	void Deinit(void);
	
	/// \brief Opens a channel to another connected system
	/// You will get ID_RAKVOICE_OPEN_CHANNEL_REPLY on success
	/// \param[in] recipient Which system to open a channel to
	void RequestVoiceChannel(RakNetGUID recipient);

	/// \brief Closes an existing voice channel.
	/// Other system will get ID_RAKVOICE_CLOSE_CHANNEL
	/// \param[in] recipient Which system to close a channel with
	void CloseVoiceChannel(RakNetGUID recipient);

	/// \brief Closes all existing voice channels
	/// Other systems will get ID_RAKVOICE_CLOSE_CHANNEL
	void CloseAllChannels(void);

	/// \brief Sends voice data to a system on an open channel
	/// \pre \a recipient must refer to a system with an open channel via RequestVoiceChannel
	/// \param[in] recipient The system to send voice data to
	/// \param[in] inputBuffer The voice data.  The size of inputBuffer should be what was specified as bufferSizeBytes in Init
	bool SendFrame(RakNetGUID recipient, void *inputBuffer);

	/// \brief Returns if we are currently sending voice data, accounting for voice activity detection
	/// \param[in] Which system to check
	/// \return If we are sending voice data for the specified system
	bool IsSendingVoiceDataTo(RakNetGUID recipient);

	/// \brief Gets decoded voice data, from one or more remote senders
	/// \param[out] outputBuffer The voice data.  The size of outputBuffer should be what was specified as bufferSizeBytes in Init
	void ReceiveFrame(void *outputBuffer);

	/// Returns the value sample rate, as passed to Init
	/// \return the sample rate
	int GetSampleRate(void) const;

	/// Returns the buffer size in bytes, as passed to Init
	/// \return buffer size in bytes
	int GetBufferSizeBytes(void) const;

	/// Returns true or false, indicating if the object has been initialized
	/// \return true if initialized, false otherwise.
	bool IsInitialized(void) const;

	/// Returns the RakPeerInterface that the object is attached to.
	/// \return the respective RakPeerInterface, or NULL not attached.
	RakPeerInterface* GetRakPeerInterface(void) const;

	/// How many bytes are on the write buffer, waiting to be passed to a call to RakPeer::Send (internally)
	/// This should remain at a fairly small near-constant size as outgoing data is sent to the Send function
	/// \param[in] guid The system to query, or RakNet::UNASSIGNED_SYSTEM_ADDRESS for the sum of all channels.
	/// \return Number of bytes on the write buffer
	unsigned GetBufferedBytesToSend(RakNetGUID guid) const;

	/// How many bytes are on the read buffer, waiting to be passed to a call to ReceiveFrame
	/// This should remain at a fairly small near-constant size as incoming data is read out at the same rate as outgoing data from the remote system
	/// \param[in] guid The system to query, or RakNet::UNASSIGNED_SYSTEM_ADDRESS for the sum of all channels.
	/// \return Number of bytes on the read buffer.
	unsigned GetBufferedBytesToReturn(RakNetGUID guid) const;

	/// Enables/disables loopback mode
	/// \param[in] true to enable, false to disable
	void SetLoopbackMode(bool enabled);

	/// Returns true or false, indicating if the loopback mode is enabled
	/// \return true if enabled, false otherwise.
	bool IsLoopbackMode(void) const;

	// --------------------------------------------------------------------------------------------
	// Message handling functions
	// --------------------------------------------------------------------------------------------
	virtual void OnShutdown(void);
	virtual void Update(void);
	virtual PluginReceiveResult OnReceive(Packet *packet);
	virtual void OnClosedConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );
protected:
	void OnOpenChannelRequest(Packet *packet);
	void OnOpenChannelReply(Packet *packet);
	virtual void OnVoiceData(Packet *packet);
	void OpenChannel(Packet *packet);
	void FreeChannelMemory(RakNetGUID recipient);
	void FreeChannelMemory(unsigned index, bool removeIndex);
	void WriteOutputToChannel(VoiceChannel *channel, char *dataToWrite);
	void SetEncoderParameter(void* enc_state, int vartype, int val);
	void SetPreprocessorParameter(void* pre_state, int vartype, int val);
	
	DataStructures::OrderedList<RakNetGUID, VoiceChannel*, VoiceChannelComp> voiceChannels;
	int32_t sampleRate;
	unsigned bufferSizeBytes;
	float *bufferedOutput;
	unsigned bufferedOutputCount;
	bool zeroBufferedOutput;
	int defaultEncoderComplexity;
	bool defaultVADState;
	bool defaultDENOISEState;
	bool defaultVBRState;
	bool loopbackMode;

};

} // namespace RakNet

#endif
