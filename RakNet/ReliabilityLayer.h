/// \file
/// \brief \b [Internal] Datagram reliable, ordered, unordered and sequenced sends.  Flow control.  Message splitting, reassembly, and coalescence.
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

#ifndef __RELIABILITY_LAYER_H
#define __RELIABILITY_LAYER_H

#include "MTUSize.h"
#include "LinkedList.h"
#include "ArrayList.h"
#include "SocketLayer.h"
#include "PacketPriority.h"
#include "RakNetQueue.h"
#include "BitStream.h"
//#include "SimpleMutex.h"
#include "InternalPacket.h"
#include "InternalPacketPool.h"
#include "DataBlockEncryptor.h"
#include "RakNetStatistics.h"
//#include "SingleProducerConsumer.h"
#include "SHA1.h"

class PluginInterface;

/// Sizeof an UDP header in byte 
#define UDP_HEADER_SIZE 28 

/// Number of ordered streams available. You can use up to 32 ordered streams 
#define NUMBER_OF_ORDERED_STREAMS 32 // 2^5 

/// Timeout before killing a connection. If no response to a reliable packet for this long kill the connection
#ifdef _DEBUG
const unsigned int TIMEOUT_TIME = 30000; // In debug don't timeout for 30 seconds so the connection doesn't drop while debugging
#else
const unsigned int TIMEOUT_TIME = 10000; // In release timeout after the normal 10 seconds
#endif


#include "BitStream.h" 

/// Datagram reliable, ordered, unordered and sequenced sends.  Flow control.  Message splitting, reassembly, and coalescence.
class ReliabilityLayer
{
public:

	/// Constructor
	ReliabilityLayer();

	/// Destructor
	~ReliabilityLayer();

	/// Resets the layer for reuse
	void Reset( void );
	
	///Sets the encryption key.  Doing so will activate secure connections
	/// \param[in] key Byte stream for the encryption key
	void SetEncryptionKey( const unsigned char *key );
	
	/// Depreciated, from IO Completion ports
	/// \param[in] s The socket
	void SetSocket( SOCKET s );

	///	Returns what was passed to SetSocket
	/// \return The socket
	SOCKET GetSocket( void );
	
	/// Packets are read directly from the socket layer and skip the reliability layer because unconnected players do not use the reliability layer
	/// This function takes packet data after a player has been confirmed a sconnected.
	/// \param[in] buffer The socket data
	/// \param[in] length The length of the socket data
	/// \param[in] playerId The player that this data is from
	/// \param[in] messageHandlerList A list of registered plugins
	/// \retval true Success
	/// \retval false Modified packet
	bool HandleSocketReceiveFromConnectedPlayer( const char *buffer, int length, PlayerID playerId, BasicDataStructures::List<PluginInterface*> &messageHandlerList );

	/// This allocates bytes and writes a user-level message to those bytes.
	/// \param[out] data The message
	/// \return Returns number of BITS put into the buffer
	int Receive( char**data );

	/// Puts data on the send queue
	/// \param[in] data The data to send
	/// \param[in] numberOfBitsToSend The length of \a data in bits
	/// \param[in] priority The priority level for the send
	/// \param[in] reliability The reliability type for the send
	/// \param[in] orderingChannel 0 to 31.  Specifies what channel to use, for relational ordering and sequencing of packets.
	/// \param[in] makeDataCopy If true \a data will be copied.  Otherwise, only a pointer will be stored.
	/// \param[in] MTUSize maximum datagram size
	/// \param[in] currentTime Current time, as per RakNet::GetTime()
	/// \return True or false for success or failure.
	bool Send( char *data, int numberOfBitsToSend, PacketPriority priority, PacketReliability reliability, unsigned char orderingChannel, bool makeDataCopy, int MTUSize, unsigned int currentTime );

	/// Call once per game cycle.  Handles internal lists and actually does the send.
	/// \param[in] s the communication  end point 
	/// \param[in] playerId The Unique Player Identifier who shouldhave sent some packets
	/// \param[in] MTUSize maximum datagram size
	/// \param[in] time current system time
	/// \param[in] messageHandlerList A list of registered plugins	
	void Update(  SOCKET s, PlayerID playerId, int MTUSize, unsigned int time, BasicDataStructures::List<PluginInterface*> &messageHandlerList );

	/// If Read returns -1 and this returns true then a modified packetwas detected
	/// \return true when a modified packet is detected
	bool IsCheater( void ) const;
	
	/// Were you ever unable to deliver a packet despite retries?
	/// \return true means the connection has been lost.  Otherwise not.
	bool IsDeadConnection( void ) const;

	/// Causes IsDeadConnection to return true
	void KillConnection(void);

	/// Sets the ping, which is used by the reliability layer to determine how long to wait for resends.  Mostly for flow control.
	/// \param[in] The ping time.
	void SetPing( unsigned int i );

	/// Get Statistics
	/// \return A pointer to a static struct, filled out with current statistical information.
	RakNetStatisticsStruct * const GetStatistics( void );

	///Are we waiting for any data to be sent out or be processed by the player?
	bool IsDataWaiting(void);

private:
	/// Returns true if we can or should send a datagram.  False if we should not
	/// \param[in] time The current time
	/// \return true if we can or should send a frame. 
	bool IsDatagramReady( unsigned int time, int MTUSize);

	/// Generates a datagram (coalesced packets)
	/// \param[out] output The resulting BitStream 
	/// \param[in] Current MTU size 
	/// \param[out] reliableDataSent Set to true or false as a return value as to if reliable data was sent.
	/// \param[in] time Current time
	/// \param[in] playerId Who we are sending to
	/// \param[in] messageHandlerList A list of registered plugins	
	void GenerateDatagram( RakNet::BitStream *output, int MTUSize, bool *reliableDataSent, unsigned int time, PlayerID playerId, BasicDataStructures::List<PluginInterface*> &messageHandlerList );

	/// Send the contents of a bitstream to the socket
	/// \param[in] s The socket used for sending data 
	/// \param[in] playerId The address and port to send to
	/// \param[in] bitStream The data to send. 
	void SendBitStream( SOCKET s, PlayerID playerId, RakNet::BitStream *bitStream );
	
	///Parse an internalPacket and create a bitstream to represent this dataReturns number of bits used
	int WriteToBitStreamFromInternalPacket( RakNet::BitStream *bitStream, const InternalPacket *const internalPacket );

	/// Parse a bitstream and create an internal packet to represent this data
	InternalPacket* CreateInternalPacketFromBitStream( RakNet::BitStream *bitStream, unsigned int time );

	/// Does what the function name says
	void RemovePacketFromResendQueueAndDeleteOlderReliableSequenced( const MessageNumberType messageNumber, unsigned int time );

	/// Acknowledge receipt of the packet with the specified messageNumber
	void SendAcknowledgementPacket( const MessageNumberType messageNumber, unsigned int time );

	/// This will return true if we should not send at this time
	bool IsSendThrottled( int MTUSize );

	/// We lost a packet
	void UpdateWindowFromPacketloss( unsigned int time );

	/// Increase the window size
	void UpdateWindowFromAck( unsigned int time );

	/// Parse an internalPacket and figure out how many header bits would be written.  Returns that number
	int GetBitStreamHeaderLength( const InternalPacket *const internalPacket );

	/// Get the SHA1 code
	void GetSHA1( unsigned char * const buffer, unsigned int nbytes, char code[ SHA1_LENGTH ] );

	/// Check the SHA1 code
	bool CheckSHA1( char code[ SHA1_LENGTH ], unsigned char * const buffer, unsigned int nbytes );

	/// Search the specified list for sequenced packets on the specified ordering channel, optionally skipping those with splitPacketId, and delete them
	void DeleteSequencedPacketsInList( unsigned char orderingChannel, BasicDataStructures::List<InternalPacket*>&theList, int splitPacketId = -1 );
	
	/// Search the specified list for sequenced packets with a value less than orderingIndex and delete them
	void DeleteSequencedPacketsInList( unsigned char orderingChannel, BasicDataStructures::Queue<InternalPacket*>&theList );

	/// Returns true if newPacketOrderingIndex is older than the waitingForPacketOrderingIndex
	bool IsOlderOrderedPacket( OrderingIndexType newPacketOrderingIndex, OrderingIndexType waitingForPacketOrderingIndex );

	/// Split the passed packet into chunks under MTU_SIZE bytes (including headers) and save those new chunks
	void SplitPacket( InternalPacket *internalPacket, int MTUSize );

	/// Insert a packet into the split packet list
	void InsertIntoSplitPacketList( InternalPacket * internalPacket );

	/// Take all split chunks with the specified splitPacketId and try to reconstruct a packet. If we can, allocate and return it.  Otherwise return 0
	InternalPacket * BuildPacketFromSplitPacketList( unsigned int splitPacketId, unsigned int time );

	/// Delete any unreliable split packets that have long since expired
	void DeleteOldUnreliableSplitPackets( unsigned int time );

	/// Creates a copy of the specified internal packet with data copied from the original starting at dataByteOffset for dataByteLength bytes.
	/// Does not copy any split data parameters as that information is always generated does not have any reason to be copied
	InternalPacket * CreateInternalPacketCopy( InternalPacket *original, int dataByteOffset, int dataByteLength, unsigned int time );

	/// Get the specified ordering list
	BasicDataStructures::LinkedList<InternalPacket*> *GetOrderingListAtOrderingStream( unsigned char orderingChannel );

	/// Add the internal packet to the ordering list in order based on order index
	void AddToOrderingList( InternalPacket * internalPacket );

	/// Inserts a packet into the resend list in order
	void InsertPacketIntoResendQueue( InternalPacket *internalPacket, unsigned int time, bool makeCopyOfInternalPacket, bool firstResend );

	/// Memory handling
	void FreeMemory( bool freeAllImmediately );
	
	/// Memory handling
	void FreeThreadedMemory( void );
	
	/// Memory handling
	void FreeThreadSafeMemory( void );

	// Initialize the variables
	void InitializeVariables( void );

	/// Given the current time, is this time so old that we should consider it a timeout?
	bool IsExpiredTime(unsigned int input, unsigned int currentTime) const;
	
	/// Does this packet number represent a packet that was skipped (out of order?)
	unsigned int IsReceivedPacketHole(unsigned int input, unsigned int currentTime) const;
	
	/// Skip an element in the received packets list
	unsigned int MakeReceivedPacketHole(unsigned int input) const;

	/// How many elements are waiting to be resent?
	unsigned int GetResendQueueDataSize(void) const;
	
	/// Update all memory which is not threadsafe
	void UpdateThreadedMemory(void);


	// 10/17/05 - Don't need any of this now that all interactions are from the main network thread
	/*
	// STUFF TO MUTEX HERE
	enum
	{
	// splitPacketList_MUTEX, // We don't have to mutex this as long as Update and HandleSocketReceiveFromConnectedPlayer are called by the same thread
	//sendQueueSystemPriority_MUTEX, // Replaced with producer / consumer
	//sendQueueHighPriority_MUTEX, // Replaced with producer / consumer
	//sendQueueMediumPriority_MUTEX, // Replaced with producer / consumer
	//sendQueueLowPriority_MUTEX, // Replaced with producer / consumer
	//resendQueue_MUTEX,// We don't have to mutex this as long as Update and HandleSocketReceiveFromConnectedPlayer are called by the same thread
	//orderingList_MUTEX,// We don't have to mutex this as long as Update and HandleSocketReceiveFromConnectedPlayer are called by the same thread
	//acknowledgementQueue_MUTEX,// We don't have to mutex this as long as Update and HandleSocketReceiveFromConnectedPlayer are called by the same thread
	// outputQueue_MUTEX,// We don't have to mutex this as long as Recieve and HandleSocketReceiveFromConnectedPlayer are called by the same thread
	messageNumber_MUTEX,
	// windowSize_MUTEX, // Causes long delays for some reason
	//lastAckTime_MUTEX,// We don't have to mutex this as long as Update and HandleSocketReceiveFromConnectedPlayer are called by the same thread
	//updateBitStream_MUTEX,// We don't have to mutex this as long as Update and HandleSocketReceiveFromConnectedPlayer are called by the same thread
	waitingForOrderedPacketWriteIndex_MUTEX,
	waitingForSequencedPacketWriteIndex_MUTEX,
	NUMBER_OF_RELIABILITY_LAYER_MUTEXES
	};
	SimpleMutex reliabilityLayerMutexes[ NUMBER_OF_RELIABILITY_LAYER_MUTEXES ];
	*/

	BasicDataStructures::List<InternalPacket*> splitPacketList;
	BasicDataStructures::List<BasicDataStructures::LinkedList<InternalPacket*>*> orderingList;
	BasicDataStructures::Queue<InternalPacket*> acknowledgementQueue, outputQueue;
	BasicDataStructures::Queue<InternalPacket*> resendQueue;
	BasicDataStructures::Queue<InternalPacket*> sendPacketSet[ NUMBER_OF_PRIORITIES ];
	MessageNumberType messageNumber;
	//unsigned int windowSize;
	unsigned int lastAckTime;
	RakNet::BitStream updateBitStream;
	OrderingIndexType waitingForOrderedPacketWriteIndex[ NUMBER_OF_ORDERED_STREAMS ], waitingForSequencedPacketWriteIndex[ NUMBER_OF_ORDERED_STREAMS ];
	// Used for flow control (changed to regular TCP sliding window)
	// unsigned int maximumWindowSize, bytesSentSinceAck;
	// unsigned int outputWindowFullTime; // under linux if this last variable is on the line above it the delete operator crashes deleting this class!

	// STUFF TO NOT MUTEX HERE (called from non-conflicting threads, or value is not important)
	OrderingIndexType waitingForOrderedPacketReadIndex[ NUMBER_OF_ORDERED_STREAMS ], waitingForSequencedPacketReadIndex[ NUMBER_OF_ORDERED_STREAMS ];
	bool deadConnection, cheater;
	// unsigned int lastPacketSendTime,retransmittedFrames, sentPackets, sentFrames, receivedPacketsCount, bytesSent, bytesReceived,lastPacketReceivedTime;
	unsigned int ping;
	unsigned int splitPacketId;
//	int TIMEOUT_TIME; // How long to wait in MS before timing someone out
	//int MAX_AVERAGE_PACKETS_PER_SECOND; // Name says it all
//	int RECEIVED_PACKET_LOG_LENGTH, requestedReceivedPacketLogLength; // How big the receivedPackets array is
//	unsigned int *receivedPackets;
	unsigned int blockWindowIncreaseUntilTime;
	RakNetStatisticsStruct statistics;

	/// New memory-efficient receivedPackets algorithm:
	/// Store times in a queue.  Store the base offset.
	/// On time insert:
	///   Pop all expired times.  For each expired time popped, increase the baseIndex
	///   If the packet number is >= the baseIndex and (messageNumber - baseIndex) < queue size
	///     We got a duplicate packet.
	///   else
	///     Add 0 times to the queue until (messageNumber - baseIndex) < queue size.
	BasicDataStructures::Queue<unsigned int> receivedPackets;
	MessageNumberType receivedPacketsBaseIndex;
	bool resetReceivedPackets;

/*
	// Windowing algorithm:
	// Start at a minimum size
	// Set the lossy window size to INFINITE
	// If the current window size is lower than the lossy window size, then increase the window size by 1 per frame if the number of acks is >= the window size and data is waiting to go out.
	// Otherwise, do the same, but also apply the limit of 1 frame per second. If we are more than 5 over the lossy window size, increase the lossy window size by 1
	// If we lose a frame, decrease the window size by 1 per frame lost.  Record the new window size as the lossy window size.
	int windowSize;
	int lossyWindowSize, consecutivePacketsLost;
	unsigned int lastWindowIncreaseSizeTime;
	*/

	/// New windowing algorithm:
	/// For each packet containing reliable messages, assign the packet number to that message
	/// For each reliable message resent, if that packet number has not yet caused the window to drop by half, do so.
	/// For each reliable message acknowledged, if that packet number has not yet caused the window to increase by 1, do so.
	int windowSize;
	/// Limit how fast the window can drop, so many fast resends doesn't drop the window to nothing immediately
	unsigned int lastPacketLossTime, lastWindowDropTime;
	unsigned int lastWindowIncreaseTime;

	
	DataBlockEncryptor encryptor;
	unsigned sendPacketCount, receivePacketCount;


#ifdef __USE_IO_COMPLETION_PORTS
	///\note Windows Port only 
	SOCKET readWriteSocket;
#endif
	///This variable is so that free memory can be called by only theupdate thread so we don't have to mutex things so much 
	bool freeThreadedMemoryOnNextUpdate;

#ifdef _INTERNET_SIMULATOR 
	struct DataAndTime
	{
		char data[ 2000 ];
		int length;
		unsigned int sendTime;
	};
	BasicDataStructures::List<DataAndTime*> delayList;
#endif

	// This has to be a member because it's not threadsafe when I removed the mutexes
	InternalPacketPool internalPacketPool;
};

#endif

