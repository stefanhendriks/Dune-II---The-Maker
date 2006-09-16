/// \file
/// \brief Provides the interface for RakNet's plugin system.
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

#ifndef __MESSAGE_HANDLER_INTERFACE_H
#define __MESSAGE_HANDLER_INTERFACE_H

class RakPeerInterface;
struct Packet;
struct InternalPacket;

#include "NetworkTypes.h"

// MessageHandlers should derive from MessageHandlerInterface and be attached to RakPeer using the function AttachMessageHandler
// On a user call to Receive, OnReceive is called for every MessageHandlerInterface, which can then take action based on the packet
// passed to it.  This is used to transparently add game-independent functional modules, similar to browser plugins
class MessageHandlerInterface
{
public:
	///OnUpdate is called everytime a packet is checked for .
	/// \param[in] peer - the instance of RakPeer that is calling Receive
	virtual void OnAttach(RakPeerInterface *peer);

	///OnUpdate is called everytime a packet is checked for .
	/// \param[in] peer - the instance of RakPeer that is calling Receive
	
	virtual void OnUpdate(RakPeerInterface *peer);

	///OnReceive is called for every packet.
	/// \param[in] peer - the instance of RakPeer that is calling Receive
	/// \param[in] packet - the packet that is being returned to the user
	/// \return true to absorb the packet, false to allow the packet to propagate to another handler, or to the game
	virtual bool OnReceive(RakPeerInterface *peer, Packet *packet);

	///Called when RakPeer is shutdown
	/// \param[in] playerId - the instance of RakPeer that is calling Receive
	virtual void OnDisconnect(RakPeerInterface *peer);

	///PropagateToGame tells RakPeer if a particular packet should be sent to the game or notIf you create a custom packet ID just for this handler you would not want to propagate it to the game, for example
	/// \param[in] id - The first byte of the packet in question
	/// \return true (default) to allow a packet to propagate to the game.  False to only let the packet be sent to message handlers
	virtual bool PropagateToGame(Packet *packet) const;

	// Events on low level sends and receives.  These functions may be called from different threads at the same time.
	virtual void OnDirectSocketSend(const char *data, const unsigned bitsUsed, PlayerID remoteSystemID);
	virtual void OnDirectSocketReceive(const char *data, const unsigned bitsUsed, PlayerID remoteSystemID);
	virtual void OnInternalPacket(InternalPacket *internalPacket, unsigned frameNumber, PlayerID remoteSystemID, unsigned time, bool isSend);
};

#endif

