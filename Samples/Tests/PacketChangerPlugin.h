/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#pragma once

#include "RakNetTypes.h"
#include "PluginInterface2.h"
#include "PacketPriority.h"
#include "MessageIdentifiers.h"
#include "InternalPacket.h"

using namespace RakNet;
class PacketChangerPlugin: public PluginInterface2
{
public:
	PacketChangerPlugin(void);
	~PacketChangerPlugin(void);

	/// \param[in] peer the instance of RakPeer that is calling Receive
	void OnAttach(void) {}

	/// Called when the interface is detached
	/// \param[in] peer the instance of RakPeer that is calling Receive
	void OnDetach(void) {}

	/// Update is called every time a packet is checked for .
	void Update(void) {}

	/// OnReceive is called for every packet.
	/// \param[in] packet the packet that is being returned to the user
	/// \return True to allow the game and other plugins to get this message, false to absorb it
	PluginReceiveResult OnReceive(Packet *packet) {(void) packet; return RR_CONTINUE_PROCESSING;}

	/// Called when RakPeer is initialized
	void OnStartup(void) {}

	/// Called when RakPeer is shutdown
	void OnShutdown(void) {}

	/// Called when a connection is dropped because the user called RakPeer::CloseConnection() for a particular system
	/// \param[in] systemAddress The system whose connection was closed
	/// \param[in] rakNetGuid The guid of the specified system
	/// \param[in] lostConnectionReason How the connection was closed: manually, connection lost, or notification of disconnection
	void OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason ){(void) systemAddress; (void) rakNetGUID; (void) lostConnectionReason;}

	/// Called when we got a new connection
	/// \param[in] systemAddress Address of the new connection
	/// \param[in] rakNetGuid The guid of the specified system
	/// \param[in] isIncoming If true, this is ID_NEW_INCOMING_CONNECTION, or the equivalent
	void OnNewConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, bool isIncoming) {(void) systemAddress; (void) rakNetGUID; (void) isIncoming;}

	/// Called when a connection attempt fails
	/// \param[in] systemAddress Address of the connection
	/// \param[in] failedConnectionReason Why the connection failed
	void OnFailedConnectionAttempt(Packet *packet, PI2_FailedConnectionAttemptReason failedConnectionAttemptReason) {(void) failedConnectionAttemptReason;}

	/// Called on a send to the socket, per datagram, that does not go through the reliability layer
	/// \param[in] data The data being sent
	/// \param[in] bitsUsed How many bits long \a data is
	/// \param[in] remoteSystemAddress Which system this message is being sent to
	void OnDirectSocketSend(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress) {(void) data; (void) bitsUsed; (void) remoteSystemAddress;}

	/// Called on a receive from the socket, per datagram, that does not go through the reliability layer
	/// \param[in] data The data being sent
	/// \param[in] bitsUsed How many bits long \a data is
	/// \param[in] remoteSystemAddress Which system this message is being sent to
	void OnDirectSocketReceive(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress) {(void) data; (void) bitsUsed; (void) remoteSystemAddress;}

	/// Called on a send or receive of a message within the reliability layer
	/// \param[in] internalPacket The user message, along with all send data.
	/// \param[in] frameNumber The number of frames sent or received so far for this player depending on \a isSend .  Indicates the frame of this user message.
	/// \param[in] remoteSystemAddress The player we sent or got this packet from
	/// \param[in] time The current time as returned by GetTimeMS()
	/// \param[in] isSend Is this callback representing a send event or receive event?
	void OnInternalPacket(InternalPacket *internalPacket, unsigned frameNumber, SystemAddress remoteSystemAddress, TimeMS time, int isSend);// {(void) internalPacket; (void) frameNumber; (void) remoteSystemAddress; (void) time; (void) isSend;}

	/// Called when we get an ack for a message we reliabily sent
	/// \param[in] messageNumber The numerical identifier for which message this is
	/// \param[in] remoteSystemAddress The player we sent or got this packet from
	/// \param[in] time The current time as returned by GetTimeMS()
	void OnAck(unsigned int messageNumber, SystemAddress remoteSystemAddress, TimeMS time) {(void) messageNumber; (void) remoteSystemAddress; (void) time;}

	/// System called RakPeerInterface::PushBackPacket
	/// \param[in] data The data being sent
	/// \param[in] bitsUsed How many bits long \a data is
	/// \param[in] remoteSystemAddress The player we sent or got this packet from
	void OnPushBackPacket(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress) {(void) data; (void) bitsUsed; (void) remoteSystemAddress;}

};
