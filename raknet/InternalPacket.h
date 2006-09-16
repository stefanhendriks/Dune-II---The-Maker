/// \file
/// \brief \b [Internal] A class which stores a user message, and all information associated with sending and receiving that message.
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

#ifndef __INTERNAL_PACKET_H
#define __INTERNAL_PACKET_H

#include "PacketPriority.h"
#ifdef _DEBUG
#include "NetworkTypes.h"
#endif

/// This is the counter used for holding packet numbers, so we can detect duplicate packets.  It should be large enough that if the variables
/// were to wrap, the newly wrapped values would no longer be in use.  Warning: Too large of a value wastes bandwidth!
typedef unsigned short MessageNumberType;

/// This is the counter used for holding ordered packet numbers, so we can detect out-of-order packets.  It should be large enough that if the variables
/// were to wrap, the newly wrapped values would no longer be in use.  Warning: Too large of a value wastes bandwidth!
typedef unsigned short OrderingIndexType;

/// Holds a user message, and related information
struct InternalPacket
{
	///True if this is an acknowledgment packet
	bool isAcknowledgement;
	
	///A unique numerical identifier given to this user message
	MessageNumberType messageNumber;
	/// Used only for tracking packetloss and windowing internally, this is the aggreggate packet number that a message was last sent in
	unsigned packetNumber;
	/// Was this packet number used this update to track windowing drops or increases?  Each packet number is only used once per update.
	bool allowWindowUpdate;
	///The priority level of this packet
	PacketPriority priority;
	///What type of reliability algorithm to use with this packet
	PacketReliability reliability;
	///What ordering channel this packet is on, if the reliability type uses ordering channels
	unsigned char orderingChannel;
	///The ID used as identification for ordering channels
	OrderingIndexType orderingIndex;
	///The ID of the split packet, if we have split packets
	unsigned int splitPacketId;
	///If this is a split packet, the index into the array of subsplit packets
	unsigned int splitPacketIndex;
	///The size of the array of subsplit packets
	unsigned int splitPacketCount;
	///When this packet was created
	unsigned int creationTime;
	///The next time to take action on this packet
	unsigned int nextActionTime;
	///How many bits the data is
	unsigned int dataBitLength;
	///Buffer is a pointer to the actual data, assuming this packet has data at all
	char *data;
};

#endif



