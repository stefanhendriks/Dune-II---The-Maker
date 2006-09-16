/// \file
/// \brief An interface for RakPeer.  Simply contains all user functions as pure virtuals.
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

#ifndef __RAK_PEER_INTERFACE_H
#define __RAK_PEER_INTERFACE_H

#include "PacketPriority.h"
#include "NetworkTypes.h"
#include "BitStream.h"
#include "RakNetStatistics.h"


class PluginInterface;
struct RPCMap;

/// The primary interface for RakNet, RakPeer contains all major functions for the library.  RakServer and RakClient are simply specialized
/// Implementations of RakPeer.
/// See the individual functions for what the class can do.
/// \brief Defines the functions used by a game server
class RakPeerInterface
{
public:
	///Destructor
	virtual ~RakPeerInterface()	{}

	// --------------------------------------------------------------------------------------------Major Low Level Functions - Functions needed by most users--------------------------------------------------------------------------------------------
	/// Starts the network threads, opens the listen port.
	/// You must call this before calling SetMaximumIncomingConnections or ConnectMultiple calls while already active are ignored.  To call this function again with different settings, you must first call Disconnect()To accept incoming connections, use SetMaximumIncomingConnectionsParameters:
	/// \param[in] MaximumNumberOfPeers Required so the network can preallocate and for thread safety.- A pure client would set this to 1.  A pure server would set it to the number of allowed clients.- A hybrid would set it to the sum of both types of connections
	/// \param[in] localPort The port to listen for connections on.
	/// \param[in] _threadSleepTimer >=0 for how many ms to Sleep each internal update cycle (recommended 30 for low performance, 0 for regular)
	/// \param[in] forceHostAddress Can force RakNet to use a particular IP to host on.  Pass 0 to automatically pick an IP
	/// \return False on failure (can't create socket or thread), true on success.
	virtual bool Initialize( unsigned short MaximumNumberOfPeers, unsigned short localPort, int _threadSleepTimer, const char *forceHostAddress=0 )=0;

	/// Secures connections though a combination of SHA1, AES128, SYN Cookies, and RSA to preventconnection spoofing, replay attacks, data eavesdropping, packet tampering, and MitM attacks.
	/// There is a significant amount of processing and a slight amount of bandwidthoverhead for this feature.
	/// If you accept connections, you must call this or else secure connections will not be enabledfor incoming connections.
	/// If you are connecting to another system, you can call this with values for the(e and p,q) public keys before connecting to prevent MitM
	/// \pre Must be called while offline
	/// \param[in] pubKeyE A pointer to the public keys from the RSACrypt class.  
	/// \param[in] pubKeyN A pointer to the public keys from the RSACrypt class. 
	/// \param[in] privKeyP Public key generated from the RSACrypt class.  
	/// \param[in] privKeyQ Public key generated from the RSACrypt class.  If the private keys are 0, then a new key will be generated when this function is called@see the Encryption sample
	virtual void InitializeSecurity(const char *pubKeyE, const char *pubKeyN, const char *privKeyP, const char *privKeyQ )=0;

	///Disables all security.
	/// \note Must be called while offline
	virtual void DisableSecurity( void )=0;

	/// Sets how many incoming connections are allowed. If this is less than the number of players currently connected,
	/// no more players will be allowed to connect.  If this is greater than the maximum number of peers allowed,
	/// it will be reducedto the maximum number of peers allowed.  Defaults to 0.
	/// \param[in] numberAllowed Maximum number of incoming connections allowed.
	virtual void SetMaximumIncomingConnections( unsigned short numberAllowed )=0;

	/// Returns the number of maximum incoming connection. 
	/// \return the maximum number of incoming connections, which is always <= MaximumNumberOfPeers
	virtual unsigned short GetMaximumIncomingConnections( void ) const=0;

	/// Sets the password incoming connections must match in the call to Connect (defaults to none). Pass 0 to passwordData to specify no password
	/// \param[in] passwordData A data block that incoming connections must match.  This can be just a password, or can be a stream of data.Specify 0 for no password data
	/// \param[in] passwordDataLength The length in bytes of passwordData
	virtual void SetIncomingPassword( const char* passwordData, int passwordDataLength )=0;

	///Get the password set by SetIncomingPassword in a BitStream
	/// \return The password in a BitStream. 
	virtual RakNet::BitStream *GetIncomingPassword( void )=0;

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
	virtual bool Connect( const char* host, unsigned short remotePort, char* passwordData, int passwordDataLength )=0;

	/// Stops the network threads and close all connections.  Multiple calls are ok.
	/// \param[in] blockDuration How long you should wait for all remaining packets to go out.
	/// If you set it to 0 then the disconnection notification probably won't arrive
	virtual void Disconnect( unsigned int blockDuration )=0;

	/// Returns if the network thread is running
	/// \return true if the network thread is running, false otherwise
	virtual bool IsActive( void ) const=0;

	/// Fills the array remoteSystems with the playerID of all the systems we are connected to
	/// \param[out] remoteSystems An array of PlayerID structures to be filled with the PlayerIDs of the systems we are connected to -
	/// pass 0 to remoteSystems to only get the number of systems we are connected to
	/// \param[in] numberOfSystems As input, the size of remoteSystems array.  As output, the number of elements put into the array 
	virtual bool GetConnectionList( PlayerID *remoteSystems, unsigned short *numberOfSystems ) const=0;

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
	virtual bool Send( const char *data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel, PlayerID playerId, bool broadcast )=0;

	///Sends a block of data to the specified system that you are connected to.This function only works while the client is connected (Use the Connect function).
	/// \param[in] bitStream The bitstream to send
	/// \param[in] priority What priority level to send on.
	/// \param[in] reliability How reliability to send this data
	/// \param[in] orderingChannel When using ordered or sequenced packets, what channel to order these on.- Packets are only ordered relative to other packets on the same stream
	/// \param[in] playerId Who to send this packet to, or in the case of broadcasting who not to send it to.  Use UNASSIGNED_PLAYER_ID to specify none
	/// \param[in] broadcast True to send this packet to all connected systems. If true, then playerId specifies who not to send the packet to.
	/// \return False if we are not connected to the specified recipient.  True otherwise
	virtual bool Send( RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, PlayerID playerId, bool broadcast )=0;

	/// Gets a packet from the incoming packet queue.
	/// Use DeallocatePacket() to deallocate the packet after you are done with it.
	/// If the server is not active this will also return 0, as all waiting packets are flushed when the server is Disconnected
	/// User-thread functions, such as RPC calls and the plugin function PluginInterface::OnUpdate occur here.
	/// \attention The packet returned from Receive must be returned to DeallocatePacket() in the same order that you got it.
	/// \return 0 if no packets are waiting to be handled, otherwise a pointer to a packet.
	// sa CoreNetworkStructures.h contains struct Packet
	virtual Packet* Receive( void )=0;

	/// Call this to deallocate a packet returned by Receive when you are done handling it.
	/// \attention The packet returned from Receive must be returned to DeallocatePacket() in the same order that you got it.
	/// \param[in] packet The packet to deallocate.	
	virtual void DeallocatePacket( Packet *packet )=0;

	/// Return the total number of connections we are allowed
	virtual unsigned short GetMaximumNumberOfPeers( void ) const=0;

	// --------------------------------------------------------------------------------------------Remote Procedure Call Functions - Functions to initialize and perform RPC--------------------------------------------------------------------------------------------
	/// \ingroup RAKNET_RPC
	/// Register a C or static member function as available for calling as a remote procedure call
	/// \param[in] uniqueID: A null-terminated unique string to identify this procedure.  Recommended you use the macro CLASS_MEMBER_ID for class member functions
	/// \param[in] functionPointer(...): The name of the function to be used as a function pointer. This can be called whether active or not, and registered functions stay registered unless unregistered
	virtual void RegisterAsRemoteProcedureCall( char* uniqueID, void ( *functionPointer ) ( RPCParameters *rpcParms ) )=0;

	/// \ingroup RAKNET_RPC
	/// Register a C++ member function as available for calling as a remote procedure call.
	/// \param[in] uniqueID: A null terminated string to identify this procedure.Recommended you use the macro REGISTER_CLASS_MEMBER_RPC
	/// \param[in] functionPointer: The name of the function to be used as a function pointer. This can be called whether active or not, and registered functions stay registered unless unregistered with UnregisterAsRemoteProcedureCall
	/// \sa ObjectMemberRPC.cpp
	virtual void RegisterClassMemberRPC( char* uniqueID, void *functionPointer )=0;

	///\ingroup RAKNET_RPC
	/// Unregisters a C function as available for calling as a remote procedure call that was formerly registeredwith RegisterAsRemoteProcedureCallOnly call offline
	/// \param[in] uniqueID A string of only letters to identify this procedure.  Recommended you use the macro CLASS_MEMBER_ID for class member functions.  Must match the parameterpassed to RegisterAsRemoteProcedureCall
	virtual void UnregisterAsRemoteProcedureCall( char* uniqueID )=0;

	/// \ingroup RAKNET_RPC
	/// Calls a C function on the remote system that was already registered using RegisterAsRemoteProcedureCall.
	/// If you want that function to return data you should call RPC from that system in the same wayReturns true on a successful packet
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
	virtual bool RPC( char* uniqueID, const char *data, unsigned int bitLength, PacketPriority priority, PacketReliability reliability, char orderingChannel, PlayerID playerId, bool broadcast, bool shiftTimestamp, ObjectID objectID, RakNet::BitStream *replyFromTarget )=0;

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
	virtual bool RPC( char* uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, PlayerID playerId, bool broadcast, bool shiftTimestamp, ObjectID objectID, RakNet::BitStream *replyFromTarget )=0;

	// --------------------------------------------------------------------------------------------Player Management Functions--------------------------------------------------------------------------------------------
	/// Close the connection to another host (if we initiated the connection it will disconnect, if they did it will kick them out).
	/// \param[in] target Which connection to close
	/// \param[in] sendDisconnectionNotification True to send ID_DISCONNECTION_NOTIFICATION to the recipient.  False to close it silently.
	/// \param[in] depreciated Ignore this
	virtual void CloseConnection( const PlayerID target, bool sendDisconnectionNotification, int depreciated=0 )=0;

	/// Given a playerID, returns an index from 0 to the maximum number of players allowed - 1.
	/// \param[in] playerId The PlayerID we are referring to
	/// \return The index of this PlayerID
	virtual int GetIndexFromPlayerID( const PlayerID playerId )=0;

	/// This function is only useful for looping through all players.
	/// Given an index, will return a PlayerID.
	/// \param[in] index Index should range between 0 and the maximum number of players allowed - 1.
	/// \return The PlayerID
	virtual PlayerID GetPlayerIDFromIndex( int index )=0;

	/// Bans an IP from connecting.  Banned IPs persist between connections.
	/// param[in] IP Dotted IP address. Can use * as a wildcard, such as 128.0.0.* will banAll IP addresses starting with 128.0.0
	/// \param[in] milliseconds how many ms for a temporary ban.  Use 0 for a permanent ban
	virtual void AddToBanList( const char *IP, unsigned int milliseconds=0 )=0;

	/// Allows a previously banned IP to connect. 
	/// param[in] Dotted IP address. Can use * as a wildcard, such as 128.0.0.* will banAll IP addresses starting with 128.0.0
	virtual void RemoveFromBanList( const char *IP )=0;

	/// Allows all previously banned IPs to connect.
	virtual void ClearBanList( void )=0;

	/// Returns true or false indicating if a particular IP is banned.
	/// \param[in] IP - Dotted IP address.
	/// \return true if IP matches any IPs in the ban list, accounting for any wildcards. false otherwise.
	virtual bool IsBanned( const char *IP )=0;

	// --------------------------------------------------------------------------------------------Pinging Functions - Functions dealing with the automatic ping mechanism--------------------------------------------------------------------------------------------
	/// Send a ping to the specified connected system.
	/// \pre The sender and recipient must already be started via a successful call to Initialize
	/// \param[in] target  who to ping
	virtual void Ping( const PlayerID target )=0;

	/// Send a ping to the specified unconnected system.The remote system, if it is Initialized, will respond with ID_PONG.The final ping time will be encoded in the following 4 bytes (2-5) as an unsigned intRequires:The sender and recipient must already be started via a successful call to Initialize
	/// \param[in] host Either a dotted IP address or a domain name.  Can be 255.255.255.255 for LAN broadcast.
	/// \param[in] remotePort Which port to connect to on the remote machine.
	/// \param[in] onlyReplyOnAcceptingConnections Only request a reply if the remote system is accepting connections
	virtual void Ping( const char* host, unsigned short remotePort, bool onlyReplyOnAcceptingConnections )=0;

	/// Returns the average of all ping times read for the specific player or -1 if none read yet
	/// \param[in] playerId Which player we are referring to
	/// \return The ping time for this player, or -1
	virtual int GetAveragePing( const PlayerID playerId )=0;

	/// Returns the last ping time read for the specific player or -1 if none read yet
	/// \param[in] playerId Which player we are referring to
	/// \return The last ping time for this player, or -1
	virtual int GetLastPing( const PlayerID playerId ) const=0;

	/// Returns the lowest ping time read or -1 if none read yet
	/// \param[in] playerId Which player we are referring to
	/// \return The lowest ping time for this player, or -1
	virtual int GetLowestPing( const PlayerID playerId ) const=0;

	/// Ping the remote systems every so often, or not. This is off by default.  Can be called anytime.
	/// \param[in] doPing True to start occasional pings.  False to stop them.
	virtual void SetOccasionalPing( bool doPing )=0;

	// --------------------------------------------------------------------------------------------Static Data Functions - Functions dealing with API defined synchronized memory--------------------------------------------------------------------------------------------
	/// All systems have a block of data associated with them, for user use.  This block of data can be used to easily 
	/// specify typical system data that you want to know on connection, such as the player's name.
	/// \param[in] playerId Which system you are referring to.  Pass the value returned by GetInternalID to refer to yourself
	/// \return The data passed to SetRemoteStaticData stored as a bitstream
	virtual RakNet::BitStream * GetRemoteStaticData( const PlayerID playerId )=0;

	/// All systems have a block of data associated with them, for user use.  This block of data can be used to easily
	/// specify typical system data that you want to know on connection, such as the player's name.
	/// \param[in] playerId Whose static data to change.  Use your own playerId to change your own static data
	/// \param[in] data a block of data to store
	/// \param[in] length The length of data in bytes 
	virtual void SetRemoteStaticData( const PlayerID playerId, const char *data, const int length )=0;

	/// Sends your static data to the specified system. This is automatically done on connection.
	/// You should call this when you change your static data.To send the static data of another system (such as relaying their data) you should do this
	/// normally with Send)
	/// \param[in] target Who to send your static data to.  Specify UNASSIGNED_PLAYER_ID to broadcast to all
	virtual void SendStaticData( const PlayerID target )=0;

	/// Sets the data to send along with a LAN server discovery or offline ping reply.
	/// \a length should be under 400 bytes, as a security measure against flood attacks
	/// \param[in] data a block of data to store, or 0 for none
	/// \param[in] length The length of data in bytes, or 0 for none
	/// \sa Ping.cpp
	virtual void SetOfflinePingResponse( const char *data, const unsigned int length )=0;

	//--------------------------------------------------------------------------------------------Network Functions - Functions dealing with the network in general--------------------------------------------------------------------------------------------
	/// Return the unique address identifier that represents you on the the network and is based on your local IP / port.
	/// Note that unlike in previous versions, this is a struct and is not sequential
	virtual PlayerID GetInternalID( void ) const=0;

	/// Return the unique address identifier that represents you on the the network and is based on your externalIP / port
	/// (the IP / port the specified player uses to communicate with you)
	/// \note that unlike in previous versions, this is a struct and is not sequential
	/// \param[in] target Which remote system you are referring to for your external ID
	virtual PlayerID GetExternalID( const PlayerID target ) const=0;

	/// Set the MTU per datagram.  It's important to set this correctly - otherwise packets will be needlessly split, decreasing performance and throughput.
	/// Maximum allowed size is MAXIMUM_MTU_SIZE.
	/// Too high of a value will cause packets not to arrive at worst and be fragmented at best.
	/// Too low of a value will split packets unnecessarily.
	/// sa MTUSize.h
	/// \pre Can only be called when not connected.
	/// \return false on failure (we are connected), else true
	virtual bool SetMTUSize( int size )=0;

	/// Returns the current MTU size
	/// \return The current MTU size
	virtual int GetMTUSize( void ) const=0;

	///Returns the number of IP addresses we have
	virtual unsigned GetNumberOfAddresses( void )=0;

	///Returns the dotted IP address for the specified playerId
	/// \param[in] playerId Any player ID other than UNASSIGNED_PLAYER_ID, even if that player is not currently connected
	virtual const char* PlayerIDToDottedIP( const PlayerID playerId ) const=0;

	///Converts a dotted IP to a playerId
	/// \param[in]  host Either a dotted IP address or a domain name
	/// \param[in]  remotePort Which port to connect to on the remote machine.
	/// \param[out] playerId  The result of this operation
	virtual void IPToPlayerID( const char* host, unsigned short remotePort, PlayerID *playerId )=0;

	///Returns an IP address at index 0 to GetNumberOfAddresses-1
	virtual const char* GetLocalIP( unsigned int index )=0;

	///Allow or disallow connection responses from any IP. Normally this should be false, but may be necessary
	/// when connection to servers with multiple IP addresses.
	/// \param[in] allow - True to allow this behavior, false to not allow. Defaults to false. Value persists between connections
	virtual void AllowConnectionResponseIPMigration( bool allow )=0;

	/// Sends a one byte message ID_ADVERTISE_SYSTEM to the remote unconnected system.
	/// This will tell the remote system our external IP outside the LAN, and can be used for NAT punch through
	/// \pre The sender and recipient must already be started via a successful call to Initialize
	/// \param[in] host Either a dotted IP address or a domain name
	/// \param[in] remotePort Which port to connect to on the remote machine.
	/// \param[in] data Optional data to append to the packet.
	/// \param[in] dataLength length of data in bytes.  Use 0 if no data.
	virtual void AdvertiseSystem( char *host, unsigned short remotePort, const char *data, int dataLength )=0;

	// --------------------------------------------------------------------------------------------Compression Functions - Functions related to the compression layer--------------------------------------------------------------------------------------------
	/// Enables or disables frequency table tracking.  This is required to get a frequency table, which is used in GenerateCompressionLayer()
	/// This value persists between connect calls and defaults to false (no frequency tracking)
	/// \pre You can call this at any time - however you SHOULD only call it when disconnected.  Otherwise you will only trackpart of the values sent over the network.
	/// \param[in] doCompile True to enable tracking 
	virtual void SetCompileFrequencyTable( bool doCompile )=0;

	/// Returns the frequency of outgoing bytes into output frequency table
	/// The purpose is to save to file as either a master frequency table from a sample game session for passing to
	/// GenerateCompressionLayer() 
	/// \pre You should only call this when disconnected. Requires that you first enable data frequency tracking by calling SetCompileFrequencyTable(true)
	/// \param[out] outputFrequencyTable  The frequency of each corresponding byte
	/// \return False (failure) if connected or if frequency table tracking is not enabled. Otherwise true (success)
	virtual bool GetOutgoingFrequencyTable( unsigned int outputFrequencyTable[ 256 ] )=0;

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
	virtual bool GenerateCompressionLayer( unsigned int inputFrequencyTable[ 256 ], bool inputLayer )=0;

	/// Delete the output or input layer as specified.  This is not necessary to call and is only valuable for freeing memory.
	/// \pre You should only call this when disconnected
	/// \param[in] inputLayer True to mean the inputLayer, false to mean the output layer
	/// \return false (failure) if connected.  Otherwise true (success)
	virtual bool DeleteCompressionLayer( bool inputLayer )=0;

	/// Returns the compression ratio. A low compression ratio is good.  Compression is for outgoing data
	/// \return The compression ratio
	virtual float GetCompressionRatio( void ) const=0;

	///Returns the decompression ratio.  A high decompression ratio is good.  Decompression is for incoming data
	/// \return The decompression ratio
	virtual float GetDecompressionRatio( void ) const=0;

	// --------------------------------------------------------------------------------------------Message Handler Functions--------------------------------------------------------------------------------------------
	/// Attatches a Plugin interface to run code automatically on message receipt in the Receive call
	/// \param[in] messageHandler Pointer to a message handler to attach
	virtual void AttachPlugin( PluginInterface *messageHandler )=0;

	///Detatches a Plugin interface to run code automatically on message receipt
	/// \param[in] messageHandler Pointer to a message handler to detatch
	virtual void DetachPlugin( PluginInterface *messageHandler )=0;

	// --------------------------------------------------------------------------------------------Miscellaneous Functions--------------------------------------------------------------------------------------------
	/// Retrieves the data you passed to the passwordData parameter in Connect
	/// \param[out] passwordData  Should point to a block large enough to hold the password data you passed to Connect
	/// \param[in] passwordDataLength Maximum size of the array passwordData.  Modified to hold the number of bytes actually written
	virtual void GetPasswordData( char *passwordData, int *passwordDataLength )=0;

	///Put a packet back at the end of the receive queue in case you don't want to deal with it immediately
	/// \param[in] packet The packet you want to push back.
	/// \param[in] pushAtHead True to push the packet so that the next receive call returns it.  False to push it at the end of the queue (obviously pushing it at the end makes the packets out of order)
	virtual void PushBackPacket( Packet *packet, bool pushAtHead )=0;

	/// \internal
	virtual RPCMap *GetRPCMap( const PlayerID playerId)=0;

	// --------------------------------------------------------------------------------------------Statistical Functions - Functions dealing with API performance--------------------------------------------------------------------------------------------

	/// Returns a structure containing a large set of network statistics for the specified system.
	/// You can map this data to a string using the C style StatisticsToString() function
	/// \param[in] playerId: Which connected system to get statistics for
	/// \return 0 on can't find the specified system.  A pointer to a set of data otherwise.
	/// \sa RakNetStatistics.h
	virtual RakNetStatisticsStruct * const GetStatistics( const PlayerID playerId )=0;

};

#endif


