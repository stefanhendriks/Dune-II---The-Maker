/// \file
/// \brief \b RakNet's plugin functionality system.  You can derive from this to create your own plugins.
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of Raknet is subject to the appropriate licence agreement.
/// "Shareware" Licensees with Rakkarsoft LLC are subject to the
/// shareware license found at
/// http://www.rakkarsoft.com/shareWareLicense.html
/// "Commercial" Licensees are subject to the commercial license found at
/// http://www.rakkarsoft.com/sourceCodeLicense.html
/// Custom license users are subject to the terms therein.
/// All other users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __PLUGIN_INTERFACE_H
#define __PLUGIN_INTERFACE_H

class RakPeerInterface;
struct Packet;
struct InternalPacket;

#include "NetworkTypes.h"

/// \brief PluginInterface provides a mechanism to add functionality in a modular way.
///
/// MessageHandlers should derive from PluginInterface and be attached to RakPeer using the function AttachPlugin
/// On a user call to Receive, OnReceive is called for every PluginInterface, which can then take action based on the packet
/// passed to it.  This is used to transparently add game-independent functional modules, similar to browser plugins
///
/// \sa DataReplicator
/// \sa FullyConnectedMesh
/// \sa PacketLogger
class PluginInterface
{
public:
	/// Called when the interface is attached
	/// \param[in] peer the instance of RakPeer that is calling Receive
	virtual void OnAttach(RakPeerInterface *peer);

	/// Called when RakPeer is initialized
	/// \param[in] peer the instance of RakPeer that is calling Receive
	virtual void OnInitialize(RakPeerInterface *peer);

	/// OnUpdate is called every time a packet is checked for .
	/// \param[in] peer - the instance of RakPeer that is calling Receive
	virtual void OnUpdate(RakPeerInterface *peer);

	/// OnReceive is called for every packet.
	/// \param[in] peer the instance of RakPeer that is calling Receive
	/// \param[in] packet the packet that is being returned to the user
	/// \return 2 to absorb but do not deallocate the packet, 1 to absorb and automatically deallocate the packet, 0 to allow the packet to propagate to another handler, or to the game
	virtual int OnReceive(RakPeerInterface *peer, Packet *packet);

	/// Called when RakPeer is shutdown
	/// \param[in] peer the instance of RakPeer that is calling Receive
	virtual void OnDisconnect(RakPeerInterface *peer);

	/// PropagateToGame tells RakPeer if a particular packet should be sent to the game or notIf you create a custom packet ID just for this handler you would not want to propagate it to the game, for example
	/// \param[in] id The first byte of the packet in question
	/// \retval true (default) Allow a packet to propagate to the game
	/// \return false Only let the packet be sent to message handlers
	virtual bool PropagateToGame(Packet *packet) const;

	/// Called on a send to the socket, per datagram, that does not go through the reliability layer
	/// \param[in] data The data being sent
	/// \param[in] bitsUsed How many bits long \a data is
	/// \param[in] remoteSystemID Which system this message is being sent to
	virtual void OnDirectSocketSend(const char *data, const unsigned bitsUsed, PlayerID remoteSystemID);
	
	/// Called on a receive from the socket, per datagram, that does not go through the reliability layer
	/// \param[in] data The data being sent
	/// \param[in] bitsUsed How many bits long \a data is
	/// \param[in] remoteSystemID Which system this message is being sent to
	virtual void OnDirectSocketReceive(const char *data, const unsigned bitsUsed, PlayerID remoteSystemID);
	
	/// Called on a send or recieve within the reliability layer
	/// \param[in] internalPacket The user message, along with all send data.
	/// \param[in] frameNumber The number of frames sent or received so far for this player depending on \a isSend .  Indicates the frame of this user message.
	/// \param[in] remoteSystemID The player we sent or got this packet from
	/// \param[in] time The current time as returned by RakNet::GetTime()
	/// \param[in] isSend Is this callback representing a send event or receive event?
	virtual void OnInternalPacket(InternalPacket *internalPacket, unsigned frameNumber, PlayerID remoteSystemID, unsigned time, bool isSend);
};

#endif



