/// \file
/// \brief The main class used for data transmission and most of RakNet's functionality.
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

#ifndef __RAK_PEER_H
#define __RAK_PEER_H

#include "ReliabilityLayer.h"
#include "RakPeerInterface.h"
#include "RPCNode.h"
#include "RSACrypt.h"
#include "BitStream.h"
#include "SingleProducerConsumer.h"
#include "RPCMap.h"
#include "SimpleMutex.h"

class HuffmanEncodingTree;
class PluginInterface;

/// The primary interface for RakNet, RakPeer contains all major functions for the library.  RakServer and RakClient are simply specialized
/// Implementations of RakPeer.
/// See the individual functions for what the class can do.
/// \brief Defines the functions used by a game server
class RakPeer : public RakPeerInterface
{
public:
	///Constructor
	RakPeer();

	///Destructor
	virtual ~RakPeer();
	// --------------------------------------------------------------------------------------------Major Low Level Functions - Functions needed by most users--------------------------------------------------------------------------------------------
	/// Starts the network threads, opens the listen port.
	/// You must call this before calling SetMaximumIncomingConnections or ConnectMultiple calls while already active are ignored.  To call this function again with different settings, you must first call Disconnect()To accept incoming connections, use SetMaximumIncomingConnectionsParameters:
	/// \param[in] MaximumNumberOfPeers Required so the network can preallocate and for thread safety.- A pure client would set this to 1.  A pure server would set it to the number of allowed clients.- A hybrid would set it to the sum of both types of connections
	/// \param[in] localPort The port to listen for connections on.
	/// \param[in] _threadSleepTimer >=0 for how many ms to Sleep each internal update cycle (recommended 30 for low performance, 0 for regular)
	/// \param[in] forceHostAddress Can force RakNet to use a particular IP to host on.  Pass 0 to automatically pick an IP
	/// \return False on failure (can't create socket or thread), true on success.
	bool Initialize( unsigned short MaximumNumberOfPeers, unsigned short localPort, int _threadSleepTimer, const char *forceHostAddress=0 );

	/// Secures connections though a combination of SHA1, AES128, SYN Cookies, and RSA to preventconnection spoofing, replay attacks, data eavesdropping, packet tampering, and MitM attacks.
	/// There is a significant amount of processing and a slight amount of bandwidthoverhead for this feature.
	/// If you accept connections, you must call this or else secure connections will not be enabledfor incoming connections.
	/// If you are connecting to another system, you can call this with values for the(e and p,q) public keys before connecting to prevent MitM
	/// \pre Must be called while offline
	/// \param[in] pubKeyE A pointer to the public keys from the RSACrypt class.  
	/// \param[in] pubKeyN A pointer to the public keys from the RSACrypt class. 
	/// \param[in] privKeyP Public key generated from the RSACrypt class.  
	/// \param[in] privKeyQ Public key generated from the RSACrypt class.  If the private keys are 0, then a new key will be generated when this function is called@see the Encryption sample
	void InitializeSecurity(const char *pubKeyE, const char *pubKeyN, const char *privKeyP, const char *privKeyQ );

	///Disables all security.
	/// \note Must be called while offline
	void DisableSecurity( void );

	/// Sets how many incoming connections are allowed. If this is less than the number of players currently connected,
	/// no more players will be allowed to connect.  If this is greater than the maximum number of peers allowed,
	/// it will be reducedto the maximum number of peers allowed.  Defaults to 0.
	/// \param[in] numberAllowed Maximum number of incoming connections allowed.
	void SetMaximumIncomingConnections( unsigned short numberAllowed );

	/// Returns the number of maximum incoming connection. 
	/// \return the maximum number of incoming connections, which is always <= MaximumNumberOfPeers
	unsigned short GetMaximumIncomingConnections( void ) const;

	/// Sets the password incoming connections must match in the call to Connect (defaults to none). Pass 0 to passwordData to specify no password
	/// \param[in] passwordData A data block that incoming connections must match.  This can be just a password, or can be a stream of data.Specify 0 for no password data
	/// \param[in] passwordDataLength The length in bytes of passwordData
	void SetIncomingPassword( const char* passwordData, int passwordDataLength );

	///Get the password set by SetIncomingPassword in a BitStream
	/// \return The password in a BitStream. 
	RakNet::BitStream *GetIncomingPassword( void );

	///Call this to connect to the specified host (ip or domain name) and server port.
	/// Calling Connect and not calling SetMaximumIncomingConnections acts as a dedicated client.
	/// Calling both acts as a true peer. This is a non-blocking connection.
	/// You know the connection is successful when IsConnected() returns trueor receive gets a packet with the type identifier ID_CONNECTION_ACCEPTED.
	/// If the connection is notsuccessful, such as rejected connection or no response then neither of these things will happen.
	/// \pre Requires that you first call Initialize
	/// \param[in] host Either a dotted IP address or a domain name
	/// \param[in] remotePort Which port to connect to on the remote machine.
	/// \param[in] passwordData A data block that must match the data block on the server.  This can be just a password, or can be a stream of data
	/// \param[in] passwordDataLength The length in bytes of passwordData
	/// \return True on successful initiation. False on incorrect parameters, internal error, or too many existing peers
	bool Connect( const char* host, unsigned short remotePort, char* passwordData, int passwordDataLength );

	/// Stops the network threads and close all connections.  Multiple calls are ok.
	/// \param[in] blockDuration How long you should wait for all remaining packets to go out.
	/// If you set it to 0 then the disconnection notification probably won't arrive
	virtual void Disconnect( unsigned int blockDuration );

	/// Returns if the network thread is running
	/// \return true if the network thread is running, false otherwise
	bool IsActive( void ) const;

	/// Fills the array remoteSystems with the playerID of all the systems we are connected to
	/// \param[out] remoteSystems An array of PlayerID structures to be filled with the PlayerIDs of the systems we are connected to -
	/// pass 0 to remoteSystems to only get the number of systems we are connected to
	/// \param[in] numberOfSystems As input, the size of remoteSystems array.  As output, the number of elements put into the array 
	bool GetConnectionList( PlayerID *remoteSystems, unsigned short *numberOfSystems ) const;

	/// Sends a block of data to the specified system that you are connected to.
	/// This function only works while the connected (Use the Connect function).
	/// \param[in] data The block of data to send
	/// \param[in] length The size in bytes of the data to send
	/// \param[in] priority What priority level to send on.
	/// \param[in] reliability How reliability to send this data
	/// \param[in] orderingChannel When using ordered or sequenced packets, what channel to order these on.- Packets are only ordered relative to other packets on the same stream
	/// \param[in] playerId Who to send this packet to, or in the case of broadcasting who not to send it to.  Use UNASSIGNED_PLAYER_ID to specify none
	/// \param[in] broadcast True to send this packet to all connected systems. If true, then playerId specifies who not to send the packet to.
	/// \return False if we are not connected to the specified recipient.  True otherwise
	bool Send( const char *data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel, PlayerID playerId, bool broadcast );

	///Sends a block of data to the specified system that you are connected to.This function only works while the client is connected (Use the Connect function).
	/// \param[in] bitStream The bitstream to send
	/// \param[in] priority What priority level to send on.
	/// \param[in] reliability How reliability to send this data
	/// \param[in] orderingChannel When using ordered or sequenced packets, what channel to order these on.- Packets are only ordered relative to other packets on the same stream
	/// \param[in] playerId Who to send this packet to, or in the case of broadcasting who not to send it to.  Use UNASSIGNED_PLAYER_ID to specify none
	/// \param[in] broadcast True to send this packet to all connected systems. If true, then playerId specifies who not to send the packet to.
	/// \return False if we are not connected to the specified recipient.  True otherwise
	bool Send( RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, PlayerID playerId, bool broadcast );

	/// Gets a packet from the incoming packet queue.
	/// Use DeallocatePacket() to deallocate the packet after you are done with it.
	/// If the server is not active this will also return 0, as all waiting packets are flushed when the server is Disconnected
	/// User-thread functions, such as RPC calls and the plugin function PluginInterface::OnUpdate occur here.
	/// \attention The packet returned from Receive must be returned to DeallocatePacket() in the same order that you got it.
	/// \return 0 if no packets are waiting to be handled, otherwise a pointer to a packet.
	// sa CoreNetworkStructures.h contains struct Packet
	Packet* Receive( void );

	/// Call this to deallocate a packet returned by Receive when you are done handling it.
	/// \attention The packet returned from Receive must be returned to DeallocatePacket() in the same order that you got it.
	/// \param[in] packet The packet to deallocate.	
	void DeallocatePacket( Packet *packet );

	/// Return the total number of connections we are allowed
	unsigned short GetMaximumNumberOfPeers( void ) const;

	// --------------------------------------------------------------------------------------------Remote Procedure Call Functions - Functions to initialize and perform RPC--------------------------------------------------------------------------------------------
	/// \ingroup RAKNET_RPC
	/// Register a C or static member function as available for calling as a remote procedure call
	/// \param[in] uniqueID: A null-terminated unique string to identify this procedure.  Recommended you use the macro CLASS_MEMBER_ID for class member functions
	/// \param[in] functionPointer(...): The name of the function to be used as a function pointer. This can be called whether active or not, and registered functions stay registered unless unregistered
	void RegisterAsRemoteProcedureCall( char* uniqueID, void ( *functionPointer ) ( RPCParameters *rpcParms ) );

	/// \ingroup RAKNET_RPC
	/// Register a C++ member function as available for calling as a remote procedure call.
	/// \param[in] uniqueID: A null terminated string to identify this procedure.Recommended you use the macro REGISTER_CLASS_MEMBER_RPC
	/// \param[in] functionPointer: The name of the function to be used as a function pointer. This can be called whether active or not, and registered functions stay registered unless unregistered with UnregisterAsRemoteProcedureCall
	/// \sa ObjectMemberRPC.cpp
	void RegisterClassMemberRPC( char* uniqueID, void *functionPointer );

	/// \ingroup RAKNET_RPC
	/// Unregisters a C function as available for calling as a remote procedure call that was formerly registeredwith RegisterAsRemoteProcedureCallOnly call offline
	/// \param[in] uniqueID A string of only letters to identify this procedure.  Recommended you use the macro CLASS_MEMBER_ID for class member functions.  Must match the parameterpassed to RegisterAsRemoteProcedureCall
	void UnregisterAsRemoteProcedureCall( char* uniqueID );

	/// \ingroup RAKNET_RPC
	/// Calls a C function on the remote system that was already registered using RegisterAsRemoteProcedureCall.
	/// If you want that function to return data you should call RPC from that system in the same way
	/// send (this does not indicate the recipient performed the call), false on failure
	/// \param[in] uniqueID A NULL terimianted string to this procedure.  Recommended you use the macro CLASS_MEMBER_ID for class member functions.  Must match the parameter
	/// \param[in] data The data to send
	/// \param[in] bitLength The number of bits of \a data
	/// \param[in] priority What priority level to send on.
	/// \param[in] reliability How reliability to send this data
	/// \param[in] orderingChannel When using ordered or sequenced packets, what channel to order these on.
	/// \param[in] playerId Who to send this packet to, or in the case of broadcasting who not to send it to.  Use UNASSIGNED_PLAYER_ID to specify none
	/// \param[in] broadcast True to send this packet to all connected systems. If true, then playerId specifies who not to send the packet to.
	/// \param[in] shiftTimestamp True to treat the first 4 bytes as a timestamp and make it system relative on arrival (Same as ID_TIMESTAMP for a packet enumeration type)
	/// \param[in] objectID For static functions, pass UNASSIGNED_OBJECT_ID.  For member functions, you must derive from NetworkIDGenerator and pass the value returned by NetworkIDGenerator::GetNetworkID for that object.
	/// \param[in] replyFromTarget If 0, this function is non-blocking.  Otherwise it will block while waiting for a reply from the target procedure, which is remtely written to RPCParameters::replyToSender and copied to replyFromTarget.  The block will return early on disconnect or if the sent packet is unreliable and more than 3X the ping has elapsed.
	/// \return True on a successful packet send (this does not indicate the recipient performed the call), false on failure
	bool RPC( char* uniqueID, const char *data, unsigned int bitLength, PacketPriority priority, PacketReliability reliability, char orderingChannel, PlayerID playerId, bool broadcast, bool shiftTimestamp, ObjectID objectID, RakNet::BitStream *replyFromTarget );

	/// \ingroup RAKNET_RPC
	/// Calls a C function on the remote system that was already registered using RegisterAsRemoteProcedureCall.
	/// If you want that function to return data you should call RPC from that system in the same wayReturns true on a successful packet
	/// send (this does not indicate the recipient performed the call), false on failure
	/// \param[in] uniqueID A NULL terimianted string to this procedure.  Recommended you use the macro CLASS_MEMBER_ID for class member functions.  Must match the parameter
	/// \param[in] bitStream The bitstream to send
	/// \param[in] priority What priority level to send on.
	/// \param[in] reliability How reliability to send this data
	/// \param[in] orderingChannel When using ordered or sequenced packets, what channel to order these on.
	/// \param[in] playerId Who to send this packet to, or in the case of broadcasting who not to send it to.  Use UNASSIGNED_PLAYER_ID to specify none
	/// \param[in] broadcast True to send this packet to all connected systems. If true, then playerId specifies who not to send the packet to.
	/// \param[in] shiftTimestamp True to treat the first 4 bytes as a timestamp and make it system relative on arrival (Same as ID_TIMESTAMP for a packet enumeration type)
	/// \param[in] objectID For static functions, pass UNASSIGNED_OBJECT_ID.  For member functions, you must derive from NetworkIDGenerator and pass the value returned by NetworkIDGenerator::GetNetworkID for that object.
	/// \param[in] replyFromTarget If 0, this function is non-blocking.  Otherwise it will block while waiting for a reply from the target procedure, which is remtely written to RPCParameters::replyToSender and copied to replyFromTarget.  The block will return early on disconnect or if the sent packet is unreliable and more than 3X the ping has elapsed.
	/// \return True on a successful packet send (this does not indicate the recipient performed the call), false on failure
	bool RPC( char* uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, PlayerID playerId, bool broadcast, bool shiftTimestamp, ObjectID objectID, RakNet::BitStream *replyFromTarget );
	
	// --------------------------------------------------------------------------------------------Player Management Functions--------------------------------------------------------------------------------------------
	/// Close the connection to another host (if we initiated the connection it will disconnect, if they did it will kick them out).
	/// \param[in] target Which connection to close
	/// \param[in] sendDisconnectionNotification True to send ID_DISCONNECTION_NOTIFICATION to the recipient.  False to close it silently.
	/// \param[in] depreciated Ignore this
	void CloseConnection( const PlayerID target, bool sendDisconnectionNotification, int depreciated=0 );

	/// Given a playerID, returns an index from 0 to the maximum number of players allowed - 1.
	/// \param[in] playerId The PlayerID we are referring to
	/// \return The index of this PlayerID
	int GetIndexFromPlayerID( const PlayerID playerId );

	/// This function is only useful for looping through all players.
	/// Given an index, will return a PlayerID.
	/// \param[in] index Index should range between 0 and the maximum number of players allowed - 1.
	/// \return The PlayerID
	PlayerID GetPlayerIDFromIndex( int index );

	/// Bans an IP from connecting.  Banned IPs persist between connections.
	/// param[in] IP Dotted IP address. Can use * as a wildcard, such as 128.0.0.* will banAll IP addresses starting with 128.0.0
	/// \param[in] milliseconds how many ms for a temporary ban.  Use 0 for a permanent ban
	void AddToBanList( const char *IP, unsigned int milliseconds=0 );

	/// Allows a previously banned IP to connect. 
	/// param[in] Dotted IP address. Can use * as a wildcard, such as 128.0.0.* will banAll IP addresses starting with 128.0.0
	void RemoveFromBanList( const char *IP );

	/// Allows all previously banned IPs to connect.
	void ClearBanList( void );

	/// Returns true or false indicating if a particular IP is banned.
	/// \param[in] IP - Dotted IP address.
	/// \return true if IP matches any IPs in the ban list, accounting for any wildcards. false otherwise.
	bool IsBanned( const char *IP );
	
	// --------------------------------------------------------------------------------------------Pinging Functions - Functions dealing with the automatic ping mechanism--------------------------------------------------------------------------------------------
	/// Send a ping to the specified connected system.
	/// \pre The sender and recipient must already be started via a successful call to Initialize
	/// \param[in] target  who to ping
	void Ping( const PlayerID target );

	/// Send a ping to the specified unconnected system.The remote system, if it is Initialized, will respond with ID_PONG.The final ping time will be encoded in the following 4 bytes (2-5) as an unsigned intRequires:The sender and recipient must already be started via a successful call to Initialize
	/// \param[in] host Either a dotted IP address or a domain name.  Can be 255.255.255.255 for LAN broadcast.
	/// \param[in] remotePort Which port to connect to on the remote machine.
	/// \param[in] onlyReplyOnAcceptingConnections Only request a reply if the remote system is accepting connections
	void Ping( const char* host, unsigned short remotePort, bool onlyReplyOnAcceptingConnections );

	/// Returns the average of all ping times read for the specific player or -1 if none read yet
	/// \param[in] playerId Which player we are referring to
	/// \return The ping time for this player, or -1
	int GetAveragePing( const PlayerID playerId );

	/// Returns the last ping time read for the specific player or -1 if none read yet
	/// \param[in] playerId Which player we are referring to
	/// \return The last ping time for this player, or -1
	int GetLastPing( const PlayerID playerId ) const;

	/// Returns the lowest ping time read or -1 if none read yet
	/// \param[in] playerId Which player we are referring to
	/// \return The lowest ping time for this player, or -1
	int GetLowestPing( const PlayerID playerId ) const;

	/// Ping the remote systems every so often, or not. This is off by default.  Can be called anytime.
	/// \param[in] doPing True to start occasional pings.  False to stop them.
	void SetOccasionalPing( bool doPing );
	
	// --------------------------------------------------------------------------------------------Static Data Functions - Functions dealing with API defined synchronized memory--------------------------------------------------------------------------------------------
	/// All systems have a block of data associated with them, for user use.  This block of data can be used to easily 
	/// specify typical system data that you want to know on connection, such as the player's name.
	/// \param[in] playerId Which system you are referring to.  Pass the value returned by GetInternalID to refer to yourself
	/// \return The data passed to SetRemoteStaticData stored as a bitstream
	RakNet::BitStream * GetRemoteStaticData( const PlayerID playerId );

	/// All systems have a block of data associated with them, for user use.  This block of data can be used to easily
	/// specify typical system data that you want to know on connection, such as the player's name.
	/// \param[in] playerId Whose static data to change.  Use your own playerId to change your own static data
	/// \param[in] data a block of data to store
	/// \param[in] length The length of data in bytes 
	void SetRemoteStaticData( const PlayerID playerId, const char *data, const int length );

	/// Sends your static data to the specified system. This is automatically done on connection.
	/// You should call this when you change your static data.To send the static data of another system (such as relaying their data) you should do this
	/// normally with Send)
	/// \param[in] target Who to send your static data to.  Specify UNASSIGNED_PLAYER_ID to broadcast to all
	void SendStaticData( const PlayerID target );

	/// Sets the data to send along with a LAN server discovery or offline ping reply.
	/// \a length should be under 400 bytes, as a security measure against flood attacks
	/// \param[in] data a block of data to store, or 0 for none
	/// \param[in] length The length of data in bytes, or 0 for none
	/// \sa Ping.cpp
	void SetOfflinePingResponse( const char *data, const unsigned int length );
	
	//--------------------------------------------------------------------------------------------Network Functions - Functions dealing with the network in general--------------------------------------------------------------------------------------------
	/// Return the unique address identifier that represents you on the the network and is based on your local IP / port.
	/// Note that unlike in previous versions, this is a struct and is not sequential
	PlayerID GetInternalID( void ) const;

	/// Return the unique address identifier that represents you on the the network and is based on your externalIP / port
	/// (the IP / port the specified player uses to communicate with you)
	/// \note that unlike in previous versions, this is a struct and is not sequential
	/// \param[in] target Which remote system you are referring to for your external ID
	PlayerID GetExternalID( const PlayerID target ) const;

	/// Set the MTU per datagram.  It's important to set this correctly - otherwise packets will be needlessly split, decreasing performance and throughput.
	/// Maximum allowed size is MAXIMUM_MTU_SIZE.
	/// Too high of a value will cause packets not to arrive at worst and be fragmented at best.
	/// Too low of a value will split packets unnecessarily.
	/// sa MTUSize.h
	/// \pre Can only be called when not connected.
	/// \return false on failure (we are connected), else true
	bool SetMTUSize( int size );

	/// Returns the current MTU size
	/// \return The current MTU size
	int GetMTUSize( void ) const;

	///Returns the number of IP addresses we have
	unsigned GetNumberOfAddresses( void );

	///Returns the dotted IP address for the specified playerId
	/// \param[in] playerId Any player ID other than UNASSIGNED_PLAYER_ID, even if that player is not currently connected
	const char* PlayerIDToDottedIP( const PlayerID playerId ) const;

	///Converts a dotted IP to a playerId
	/// \param[in]  host Either a dotted IP address or a domain name
	/// \param[in]  remotePort Which port to connect to on the remote machine.
	/// \param[out] playerId  The result of this operation
	void IPToPlayerID( const char* host, unsigned short remotePort, PlayerID *playerId );

	///Returns an IP address at index 0 to GetNumberOfAddresses-1
	const char* GetLocalIP( unsigned int index );

	///Allow or disallow connection responses from any IP. Normally this should be false, but may be necessary
	/// when connection to servers with multiple IP addresses.
	/// \param[in] allow - True to allow this behavior, false to not allow. Defaults to false. Value persists between connections
	void AllowConnectionResponseIPMigration( bool allow );

	/// Sends a one byte message ID_ADVERTISE_SYSTEM to the remote unconnected system.
	/// This will tell the remote system our external IP outside the LAN, and can be used for NAT punch through
	/// \pre The sender and recipient must already be started via a successful call to Initialize
	/// \param[in] host Either a dotted IP address or a domain name
	/// \param[in] remotePort Which port to connect to on the remote machine.
	/// \param[in] data Optional data to append to the packet.
	/// \param[in] dataLength length of data in bytes.  Use 0 if no data.
	void AdvertiseSystem( char *host, unsigned short remotePort, const char *data, int dataLength );

	// --------------------------------------------------------------------------------------------Compression Functions - Functions related to the compression layer--------------------------------------------------------------------------------------------
	/// Enables or disables frequency table tracking.  This is required to get a frequency table, which is used in GenerateCompressionLayer()
	/// This value persists between connect calls and defaults to false (no frequency tracking)
	/// \pre You can call this at any time - however you SHOULD only call it when disconnected.  Otherwise you will only trackpart of the values sent over the network.
	/// \param[in] doCompile True to enable tracking 
	void SetCompileFrequencyTable( bool doCompile );

	/// Returns the frequency of outgoing bytes into output frequency table
	/// The purpose is to save to file as either a master frequency table from a sample game session for passing to
	/// GenerateCompressionLayer() 
	/// \pre You should only call this when disconnected. Requires that you first enable data frequency tracking by calling SetCompileFrequencyTable(true)
	/// \param[out] outputFrequencyTable  The frequency of each corresponding byte
	/// \return False (failure) if connected or if frequency table tracking is not enabled. Otherwise true (success)
	bool GetOutgoingFrequencyTable( unsigned int outputFrequencyTable[ 256 ] );

	/// This is an optional function to generate the compression layer based on the input frequency table.
	/// If you want to use it you should call this twice - once with inputLayer as true and once as false.
	/// The frequency table passed here with inputLayer=true should match the frequency table on the recipient with inputLayer=false.
	/// Likewise, the frequency table passed here with inputLayer=false should match the frequency table on the recipient with inputLayer=true.
	/// Calling this function when there is an existing layer will overwrite the old layer.
	/// \pre You should only call this when disconnected
	/// \param[in] inputFrequencyTable A frequency table for your data
	/// \param[in] inputLayer Is this the input layer?
	/// \return false (failure) if connected.  Otherwise true (success)
	/// \sa Compression.cpp
	bool GenerateCompressionLayer( unsigned int inputFrequencyTable[ 256 ], bool inputLayer );

	/// Delete the output or input layer as specified.  This is not necessary to call and is only valuable for freeing memory.
	/// \pre You should only call this when disconnected
	/// \param[in] inputLayer True to mean the inputLayer, false to mean the output layer
	/// \return false (failure) if connected.  Otherwise true (success)
	bool DeleteCompressionLayer( bool inputLayer );

	/// Returns the compression ratio. A low compression ratio is good.  Compression is for outgoing data
	/// \return The compression ratio
	float GetCompressionRatio( void ) const;

	///Returns the decompression ratio.  A high decompression ratio is good.  Decompression is for incoming data
	/// \return The decompression ratio
	float GetDecompressionRatio( void ) const;

	// --------------------------------------------------------------------------------------------Message Handler Functions--------------------------------------------------------------------------------------------
	/// Attatches a Plugin interface to run code automatically on message receipt in the Receive call
	/// \param[in] messageHandler Pointer to a message handler to attach
	void AttachPlugin( PluginInterface *messageHandler );

	///Detatches a Plugin interface to run code automatically on message receipt
	/// \param[in] messageHandler Pointer to a message handler to detatch
	void DetachPlugin( PluginInterface *messageHandler );

	// --------------------------------------------------------------------------------------------Miscellaneous Functions--------------------------------------------------------------------------------------------
	/// Retrieves the data you passed to the passwordData parameter in Connect
	/// \param[out] passwordData  Should point to a block large enough to hold the password data you passed to Connect
	/// \param[in] passwordDataLength Maximum size of the array passwordData.  Modified to hold the number of bytes actually written
	void GetPasswordData( char *passwordData, int *passwordDataLength );

	///Put a packet back at the end of the receive queue in case you don't want to deal with it immediately
	/// \param[in] packet The packet you want to push back.
	/// \param[in] pushAtHead True to push the packet so that the next receive call returns it.  False to push it at the end of the queue (obviously pushing it at the end makes the packets out of order)
	void PushBackPacket( Packet *packet, bool pushAtHead );

	// --------------------------------------------------------------------------------------------Statistical Functions - Functions dealing with API performance--------------------------------------------------------------------------------------------

	/// Returns a structure containing a large set of network statistics for the specified system.
	/// You can map this data to a string using the C style StatisticsToString() function
	/// \param[in] playerId: Which connected system to get statistics for
	/// \return 0 on can't find the specified system.  A pointer to a set of data otherwise.
	/// \sa RakNetStatistics.h
	RakNetStatisticsStruct * const GetStatistics( const PlayerID playerId );

	// --------------------------------------------------------------------------------------------EVERYTHING AFTER THIS COMMENT IS FOR INTERNAL USE ONLY--------------------------------------------------------------------------------------------

	/// \internal
	RPCMap *GetRPCMap( const PlayerID playerId);


	/// \internal
	/// \brief Holds the clock differences between systems, along with the ping
	struct PingAndClockDifferential
	{
		short pingTime;
		unsigned int clockDifferential;
	};

	/// \internal
	/// \brief All the information representing a connected system system
	struct RemoteSystemStruct
	{
		PlayerID playerId;  /// The remote system associated with this reliability layer
		PlayerID myExternalPlayerId;  /// Your own IP, as reported by the remote system
		ReliabilityLayer reliabilityLayer;  /// The reliability layer associated with this player
		bool weInitiatedTheConnection; /// True if we started this connection via Connect.  False if someone else connected to us.
		PingAndClockDifferential pingAndClockDifferential[ PING_TIMES_ARRAY_SIZE ];  /// last x ping times and calculated clock differentials with it
		int pingAndClockDifferentialWriteIndex;  /// The index we are writing into the pingAndClockDifferential circular buffer
		int lowestPing; ///The lowest ping value encounter
		unsigned int nextPingTime;  /// When to next ping this player
		unsigned int lastReliableSend; /// When did the last reliable send occur.  Reliable sends must occur at least once every TIMEOUT_TIME/2 units to notice disconnects
		RakNet::BitStream staticData; /// static data
		unsigned int connectionTime; /// connection time, if active.
		unsigned char AESKey[ 16 ]; /// Security key.
		bool setAESKey; /// true if security is enabled.
		RPCMap rpcMap; /// Mapping of RPC calls to single byte integers to save transmission bandwidth.
		enum ConnectMode {NO_ACTION, DISCONNECT_ASAP, REQUESTED_CONNECTION, HANDLING_CONNECTION_REQUEST, UNVERIFIED_SENDER, SET_ENCRYPTION_ON_MULTIPLE_16_BYTE_PACKET, CONNECTED} connectMode;
	};

protected:

#ifdef _WIN32
	// friend unsigned __stdcall RecvFromNetworkLoop(LPVOID arguments);
	friend void __stdcall ProcessPortUnreachable( unsigned int binaryAddress, unsigned short port, RakPeer *rakPeer );
	friend void __stdcall ProcessNetworkPacket( unsigned int binaryAddress, unsigned short port, const char *data, int length, RakPeer *rakPeer );
	friend unsigned __stdcall UpdateNetworkLoop( LPVOID arguments );
#else
	// friend void*  RecvFromNetworkLoop( void*  arguments );
	friend void ProcessPortUnreachable( unsigned int binaryAddress, unsigned short port, RakPeer *rakPeer );
	friend void ProcessNetworkPacket( unsigned int binaryAddress, unsigned short port, const char *data, int length, RakPeer *rakPeer );
	friend void* UpdateNetworkLoop( void* arguments );
#endif

	//void RemoveFromRequestedConnectionsList( const PlayerID playerId );
	bool SendConnectionRequest( const char* host, unsigned short remotePort );
	///Get the reliability layer associated with a playerID.  
	/// \param[in] playerID The player identifier 
	/// \return 0 if none
	RemoteSystemStruct *GetRemoteSystemFromPlayerID( const PlayerID playerID ) const;
	///Parse out a connection request packet
	void ParseConnectionRequestPacket( RakPeer::RemoteSystemStruct *remoteSystem, PlayerID playerId, const char *data, int byteSize);
	///When we get a connection request from an ip / port, accept it unless full
	void OnConnectionRequest( RakPeer::RemoteSystemStruct *remoteSystem, unsigned char *AESKey, bool setAESKey );
	///Send a reliable disconnect packet to this player and disconnect them when it is delivered
	void NotifyAndFlagForDisconnect( const PlayerID playerId, bool performImmediate );
	///Returns how many remote systems initiated a connection to us
	unsigned short GetNumberOfRemoteInitiatedConnections( void ) const;
	///Get a free remote system from the list and assign our playerID to it.  Should only be called from the update thread - not the user thread
	RemoteSystemStruct * AssignPlayerIDToRemoteSystemList( const PlayerID playerId, RemoteSystemStruct::ConnectMode connectionMode );
	///An incoming packet has a timestamp, so adjust it to be relative to this system
	void ShiftIncomingTimestamp( char *data, PlayerID playerId ) const;
	///Get the most probably accurate clock differential for a certain player
	unsigned int GetBestClockDifferential( const PlayerID playerId ) const;

	void PushPortRefused( const PlayerID target );
	///Handles an RPC packet.  This is sending an RPC request
	/// \param[in] data A packet returned from Receive with the ID ID_RPC
	/// \param[in] length The size of the packet data 
	/// \param[in] playerId The sender of the packet 
	/// \return true on success, false on a bad packet or an unregistered function
	bool HandleRPCPacket( const char *data, int length, PlayerID playerId );

	///Handles an RPC reply packet.  This is data returned from an RPC call
	/// \param[in] data A packet returned from Receive with the ID ID_RPC
	/// \param[in] length The size of the packet data 
	/// \param[in] playerId The sender of the packet 
	void HandleRPCReplyPacket( const char *data, int length, PlayerID playerId );

#ifdef __USE_IO_COMPLETION_PORTS

	bool SetupIOCompletionPortSocket( int index );
#endif
	///Set this to true to terminate the Peer thread execution 
	bool endThreads;
	///true if the peer thread is active. 
	bool isMainLoopThreadActive;
	bool occasionalPing;  /// Do we occasionally ping the other systems?*/
	///Store the maximum number of peers allowed to connect
	unsigned short maximumNumberOfPeers;
	///Store the maximum number of peers able to connect, including reserved connection slots for pings, etc.
	unsigned short remoteSystemListSize;
	///Store the maximum incoming connection allowed 
	unsigned short maximumIncomingConnections;
	///localStaticData necessary because we may not have a RemoteSystemStruct representing ourselves in the list
	RakNet::BitStream incomingPasswordBitStream, outgoingPasswordBitStream, localStaticData, offlinePingResponse;
	///Local Player ID 
	PlayerID myPlayerId;
	///This is an array of pointers to RemoteSystemStruct
	/// This allows us to preallocate the list when starting, so we don't have to allocate or delete at runtime.
	/// Another benefit is that is lets us add and remove active players simply by setting playerId
	/// and moving elements in the list by copying pointers variables without affecting running threads, even if they are in thereliability layer
	RemoteSystemStruct* remoteSystemList;

	enum
	{
//		requestedConnections_MUTEX,
		incomingPasswordBitStream_Mutex,
		outgoingPasswordBitStream_Mutex,
	//	remoteSystemList_Mutex,    /// This mutex is to lock remoteSystemList::PlayerID and only used for critical cases
		//  updateCycleIsRunning_Mutex,
		offlinePingResponse_Mutex,
		//bufferedCommandQueue_Mutex, /// This mutex is to buffer all send calls to run from the update thread.  This is to get around the problem of the update thread changing playerIDs in the remoteSystemList while in the send call and thus having the sends go to the wrong player
		//bufferedCommandPool_Mutex, /// This mutex is to buffer all send calls to run from the update thread.  This is to get around the problem of the update thread changing playerIDs in the remoteSystemList while in the send call and thus having the sends go to the wrong player
		NUMBER_OF_RAKPEER_MUTEXES
	};
	SimpleMutex rakPeerMutexes[ NUMBER_OF_RAKPEER_MUTEXES ];
	///RunUpdateCycle is not thread safe but we don't need to mutex calls. Just skip calls if it is running already

	bool updateCycleIsRunning;
	///The list of people we have tried to connect to recently

	//BasicDataStructures::Queue<RequestedConnectionStruct*> requestedConnectionsList;
	///Data that both the client and the server needs

	unsigned int bytesSentPerSecond, bytesReceivedPerSecond;
	// bool isSocketLayerBlocking;
	// bool continualPing,isRecvfromThreadActive,isMainLoopThreadActive, endThreads, isSocketLayerBlocking;
	unsigned int validationInteger;
#ifdef _WIN32

	HANDLE
#else
	pthread_t
#endif
		processPacketsThreadHandle, recvfromThreadHandle;
	SimpleMutex incomingQueueMutex, banListMutex; //,synchronizedMemoryQueueMutex, automaticVariableSynchronizationMutex;
	//BasicDataStructures::Queue<Packet *> incomingpacketProducerConsumer; //, synchronizedMemorypacketProducerConsumer;
	// BitStream enumerationData;

	/// @brief Automatic Variable Synchronization Mechanism
	/// automatic variable synchronization takes a primary and secondary identifier
	/// The unique primary identifier is the index into the automaticVariableSynchronizationList
	/// The unique secondary identifier (UNASSIGNED_OBJECT_ID for none) is in an unsorted list of memory blocks
	struct MemoryBlock
	{
		char *original, *copy;
		unsigned short size;
		ObjectID secondaryID;
		bool isAuthority;
		bool ( *synchronizationRules ) ( char*, char* );
	};

	struct BanStruct
	{
		char *IP;
		unsigned int timeout; // 0 for none
	};

	struct RequestedConnectionStruct
	{
		PlayerID playerId;
		unsigned int nextRequestTime;
		unsigned char requestsMade;
		char *data;
		unsigned short dataLength;
		enum {CONNECT=1, PING=2, PING_OPEN_CONNECTIONS=4, ADVERTISE_SYSTEM=8} actionToTake;
	};

	//BasicDataStructures::List<BasicDataStructures::List<MemoryBlock>* > automaticVariableSynchronizationList;
	BasicDataStructures::List<BanStruct*> banList;
	BasicDataStructures::List<PluginInterface*> messageHandlerList;
	BasicDataStructures::SingleProducerConsumer<RequestedConnectionStruct> requestedConnectionList;

	/// Compression stuff
	unsigned int frequencyTable[ 256 ];
	HuffmanEncodingTree *inputTree, *outputTree;
	unsigned int rawBytesSent, rawBytesReceived, compressedBytesSent, compressedBytesReceived;
	// void DecompressInput(RakNet::BitStream *bitStream);
	// void UpdateOutgoingFrequencyTable(RakNet::BitStream * bitStream);
	void GenerateSYNCookieRandomNumber( void );
	void SecuredConnectionResponse( const PlayerID playerId );
	void SecuredConnectionConfirmation( RakPeer::RemoteSystemStruct * remoteSystem, char* data );
	bool RunUpdateCycle( void );
	// void RunMutexedUpdateCycle(void);

	struct BufferedCommandStruct
	{
		char *data;
		int numberOfBitsToSend;
		PacketPriority priority;
		PacketReliability reliability;
		char orderingChannel;
		PlayerID playerId;
		bool broadcast;
		RemoteSystemStruct::ConnectMode connectionMode;
		ObjectID objectID;
		bool blockingCommand; // Only used for RPC
		enum {BCS_SEND, BCS_CLOSE_CONNECTION, BCS_RPC, BCS_RPC_SHIFT, BCS_DO_NOTHING} command;
	};

	// Single producer single consumer queue using a linked list
	//BufferedCommandStruct* bufferedCommandReadIndex, bufferedCommandWriteIndex;
	BasicDataStructures::SingleProducerConsumer<BufferedCommandStruct> bufferedCommands;

	//BasicDataStructures::Queue<BufferedCommandStruct *> bufferedCommandQueue;
	//BasicDataStructures::Queue<BufferedCommandStruct *> bufferedCommandPool;
	//RakPeer::BufferedCommandStruct *LockWriteBufferedCommandStruct(bool *outParam_wasInserted); // Get the next BCS to write
	//void UnlockWriteBufferedCommandStruct(RakPeer::BufferedCommandStruct *bcs, bool *outParam_wasInserted); // Done with the next BCS to write
	//RakPeer::BufferedCommandStruct *LockReadBufferedCommandStruct(void); // Get the next BCS to read
	//RakPeer::BufferedCommandStruct *UnlockReadBufferedCommandStruct(void); // Done with the next BCS to read
	
	//void FreeBufferedCommandStruct(RakPeer::BufferedCommandStruct *bcs);

	bool AllowIncomingConnections(void) const;

	// Sends static data using immediate send mode or not (if called from user thread, put false for performImmediate.  If called from update thread, put true).
	// This is done for efficiency, so we don't buffer send calls that are from the network thread anyway
	void SendStaticDataInternal( const PlayerID target, bool performImmediate );
	void PingInternal( const PlayerID target, bool performImmediate );
	bool ValidSendTarget(PlayerID playerId, bool broadcast);
	// This stores the user send calls to be handled by the update thread.  This way we don't have thread contention over playerIDs
	void CloseConnectionInternal( const PlayerID target, bool sendDisconnectionNotification, bool performImmediate );
	void SendBuffered( RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, PlayerID playerId, bool broadcast, RemoteSystemStruct::ConnectMode connectionMode );
	bool SendImmediate( char *data, int numberOfBitsToSend, PacketPriority priority, PacketReliability reliability, char orderingChannel, PlayerID playerId, bool broadcast, bool useCallerDataAllocation, unsigned int currentTime );
	bool HandleBufferedRPC(BufferedCommandStruct *bcs, unsigned time);
	void ClearBufferedCommands(void);
	void ClearRequestedConnectionList(void);

	//BasicDataStructures::AVLBalancedBinarySearchTree<RPCNode> rpcTree;
	RPCMap rpcMap; // Can't use StrPtrHash because runtime insertions will screw up the indices
	int MTUSize;
	bool trackFrequencyTable;
	int threadSleepTimer;

	SOCKET connectionSocket;

	// Used for RPC replies
	RakNet::BitStream *replyFromTargetBS;
	PlayerID replyFromTargetPlayer;
	bool replyFromTargetBroadcast;
	bool blockOnRPCReply;

	// Histogram statistics
	//unsigned int nextReadBytesTime;
	//int lastSentBytes,lastReceivedBytes;
	/// Encryption and security
	big::RSACrypt<RSA_BIT_SIZE> rsacrypt;
	big::u32 publicKeyE;
	RSA_BIT_SIZE publicKeyN;
	bool keysLocallyGenerated, usingSecurity;
	unsigned int randomNumberExpirationTime;
	unsigned char newRandomNumber[ 20 ], oldRandomNumber[ 20 ];
	///How long it has been since things were updated by a call to receiveUpdate thread uses this to determine how long to sleep for
	unsigned int lastUserUpdateCycle;
	/// True to allow connection accepted packets from anyone.  False to only allow these packets from serverswe requested a connection to.
	bool allowConnectionResponseIPMigration;

	// The packetProducerConsumer transfers the packets from the network thread to the user thread. The bufferedPacket holds packets that couldn't be processed
	// immediately while waiting on blocked RPCs
	BasicDataStructures::SingleProducerConsumer<Packet> packetProducerConsumer;
	BasicDataStructures::Queue<Packet*> bufferedPacket;
};

#endif
