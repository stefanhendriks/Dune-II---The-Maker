/// \file
/// \brief Fully connected mesh plugin.  This will connect RakPeer to all connecting peers, and all peers the connecting peer knows about.
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

#ifndef __FULLY_CONNECTED_MESH_H
#define __FULLY_CONNECTED_MESH_H

class RakPeerInterface;
#include "NetworkTypes.h"
#include "ArrayList.h"
#include "PluginInterface.h"
#include "BitStream.h"

/// Stores data for each peer in the mesh
struct FCM_RemotePeer
{
	PlayerID playerId;
	enum FCM_RemotePeerState
	{
		NEWLY_LISTED,
		CONNECTING,
		CONNECTED,
	} connectionState;

	bool remotelyKnown; // When a remote peer sends us a list of peers this is used to track which peers we know about and which they know about

	unsigned timer; // Used for timed operations, such as removing dead entries from a list when connections do not succeed
};

/// Fully connected mesh plugin.  This will connect RakPeer to all connecting peers, and all peers the connecting peer knows about.
class FullyConnectedMesh : public PluginInterface
{
public:
	FullyConnectedMesh();
	~FullyConnectedMesh();

	// --------------------------------------------------------------------------------------------
	// User functions
	// --------------------------------------------------------------------------------------------
	/// Plaintext encoding of the password.  If you use a password, use secure connections
	void Initialize(const char *password);

	// --------------------------------------------------------------------------------------------
	// Packet handling functions
	// --------------------------------------------------------------------------------------------
	virtual void OnDisconnect(RakPeerInterface *peer);
	virtual void OnUpdate(RakPeerInterface *peer);
	virtual int OnReceive(RakPeerInterface *peer, Packet *packet);
	virtual bool PropagateToGame(Packet *packet) const;
	
	// --------------------------------------------------------------------------------------------
	// Overridable event callbacks
	// --------------------------------------------------------------------------------------------
	/// Too much data was in a packet.  User should override in a derived class to handle this
	virtual void OnDataOverflow(RakPeerInterface *peer, Packet *packet);
	/// Not enough data was in a packet.  User should override in a derived class to handle this
	virtual void OnDataUnderflow(RakPeerInterface *peer, Packet *packet);
	/// Packet data that just doesn't make sense
	virtual void OnInvalidPacket(RakPeerInterface *peer, Packet *packet);
	/// Wrong password
	virtual void OnJoinMeshAuthorizationFailed(RakPeerInterface *peer, Packet *packet);

	unsigned GetMeshPeerListSize(void) const;
	PlayerID GetPeerIDAtIndex(unsigned index);
protected:
	// ID_CONNECTION_REQUEST_ACCEPTED
	void HandleConnectionRequestAccepted(RakPeerInterface *peer, Packet *packet);
	// ID_CONNECTION_LOST or ID_DISCONNECTION_NOTIFICATION.  Host may be lost
	void HandleDroppedConnection(RakPeerInterface *peer, Packet *packet);
	void HandleMeshJoinRequest(RakPeerInterface *peer, Packet *packet);
	void HandleMeshJoinResponse(RakPeerInterface *peer, Packet *packet);

	void AddUniquePeer(RakPeerInterface *peer, FCM_RemotePeer::FCM_RemotePeerState state, PlayerID playerId, bool remotelyKnown);
	
	virtual void AppendMeshJoinAuthorization(RakNet::BitStream *bitStream);
	virtual bool ValidateMeshJoinAuthorization(RakNet::BitStream *bitStream);
	void SerializeRemotelyUnknownPeers(RakNet::BitStream *bitStream);
	bool DeserializeMeshPeerList(RakPeerInterface *peer, RakNet::BitStream *bitStream);
	void DeleteFromPeerList(PlayerID playerId);
	void MarkAllRemotelyKnown(bool value);
	int MeshPeerIndexFromPlayerID(PlayerID playerId);
	void ConnectToNewlyListedServers(RakPeerInterface *peer);

	char *pw;
	BasicDataStructures::List<FCM_RemotePeer*> meshPeerList;

#ifdef _DEBUG
	bool initialized;
#endif
};

#endif



