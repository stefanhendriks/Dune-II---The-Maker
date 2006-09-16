/// \file
/// \brief \b [Internal] Encapsulates a mutex
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

#ifndef __SIMPLE_MUTEX_H
#define __SIMPLE_MUTEX_H

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <sys/types.h>
#endif

/// \brief An easy to use mutex.
/// 
/// I wrote this because the version that comes with Windows is too complicated and requires too much code to use.
/// @remark Previously I used this everywhere, and in fact for a year or two RakNet was totally threadsafe.  While doing profiling, I saw that this function was incredibly slow compared to the blazing performance of everything else, so switched to single producer / consumer everywhere.  Now the user thread of RakNet is not threadsafe, but it's 100X faster than before.
class SimpleMutex
{
public:

	/// Constructor
	SimpleMutex();
	
	// Destructor
	~SimpleMutex();
	
	// Locks the mutex.  Slow!
	void Lock(void);
	
	// Unlocks the mutex.
	void Unlock(void);
private:
	#ifdef _WIN32
	CRITICAL_SECTION criticalSection; /// Docs say this is faster than a mutex for single process access
	#else
	pthread_mutex_t hMutex;
	#endif
};

#endif

