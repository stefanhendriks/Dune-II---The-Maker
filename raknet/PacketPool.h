/// \file
/// \brief \b [Internal] A pool for the Packet class.  I don't believe this is used any longer.
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

#ifndef __PACKET_POOL
#define __PACKET_POOL
#include "SimpleMutex.h"
#include "RakNetQueue.h"

struct Packet;

class PacketPool
{

public:
	PacketPool();
	~PacketPool();

	static void AddReference(void);
	static void RemoveReference(void);
	Packet* GetPointer( void );
	void ReleasePointer( Packet *p );
	void ClearPool( void );
	static inline PacketPool* Instance()
	{
		return I;
	}

private:
	BasicDataStructures::Queue<Packet*> pool;
	SimpleMutex poolMutex;
	static PacketPool *I;
	static int referenceCount;
#ifdef _DEBUG
	int packetsReleased;
#endif
};

#endif



