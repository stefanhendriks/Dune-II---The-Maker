/// \file
/// \brief \b [Internal] Memory pool for InternalPacket*
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

#ifndef __INTERNAL_PACKET_POOL
#define __INTERNAL_PACKET_POOL
#include "RakNetQueue.h"
#include "InternalPacket.h"

/// Handles of a pool of InternalPacket pointers.  This is only here for efficiency.
/// \sa InternalPacket.h
class InternalPacketPool
{
public:
	
	/// Constructor	
	InternalPacketPool();
	
	/// Destructor	
	~InternalPacketPool();
	
	/// Get an InternalPacket pointer.  Will either allocate a new one or return one from the pool
	/// \return An InternalPacket pointer.
	InternalPacket* GetPointer( void );
	
	/// Return an InternalPacket pointer to the pool.
	/// \param[in] p A pointer to an InternalPacket you no longer need.
	void ReleasePointer( InternalPacket *p );
	
	// Delete all InternalPacket pointers in the pool.	
	void ClearPool( void );

private:
	int packetsReleased;
	/// Queue of internal packets
	BasicDataStructures::Queue<InternalPacket*> pool;
};

#endif

