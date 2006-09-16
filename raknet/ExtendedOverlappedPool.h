/// \file
/// \brief \b [Depreciated] This was used for IO completion ports.
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

#ifdef __USE_IO_COMPLETION_PORTS
#ifndef __EXTENDED_OVERLAPPED_POOL
#define __EXTENDED_OVERLAPPED_POOL
#include "SimpleMutex.h"
#include "ClientContextStruct.h"
#include "RakNetQueue.h"

/// Depreciated - for IO completion ports
class ExtendedOverlappedPool
{

public:
	ExtendedOverlappedPool();
	~ExtendedOverlappedPool();
	ExtendedOverlappedStruct* GetPointer( void );
	void ReleasePointer( ExtendedOverlappedStruct *p );
	static inline ExtendedOverlappedPool* Instance()
	{
		return & I;
	}
	
private:
	BasicDataStructures::Queue<ExtendedOverlappedStruct*> pool;
	SimpleMutex poolMutex;
	static ExtendedOverlappedPool I;
};

#endif
#endif

