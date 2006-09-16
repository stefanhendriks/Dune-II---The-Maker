/// \file
/// \brief Factory class for RakServerInterface, RakClientInterface, and RakPeerInterface
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

#ifndef __RAK_NETWORK_FACTORY_H
#define __RAK_NETWORK_FACTORY_H

class RakClientInterface;

class RakServerInterface;

class RakPeerInterface;

#ifdef _WIN32
#define RAK_DLL_EXPORT __declspec(dllexport)
#else 
// Unix needs no export, but for name mangling, keep the function name
// clean. If you omit the 'extern "C"', the .so names will be
// compiler dependent.
#define RAK_DLL_EXPORT extern "C"
#endif

#if defined(DLL_EXPORTS) || defined(_USRDLL)

class RAK_DLL_EXPORT RakNetworkFactory
#else 
//class __declspec( dllimport ) RakNetworkFactor
/// \brief Factory class for RakServerInterface, RakClientInterface, and RakPeerInterface
///
/// Only necessary if you use DLLs
class RakNetworkFactory
#endif
{
public:
	
	/// Returns a new instance of the network client.
	static RakClientInterface* GetRakClientInterface( void );
	
	/// Returns a new instance of the network server.
	static RakServerInterface* GetRakServerInterface( void );
	
	/// Returns a new instance of the network server.
	static RakPeerInterface* GetRakPeerInterface( void );
	
	/// Destroys an instance of the network client.
	static void DestroyRakClientInterface( RakClientInterface* i );
	
	/// Destroys an instance of the network server.
	static void DestroyRakServerInterface( RakServerInterface* i );
	
	/// Destroys an instance of the network server.
	static void DestroyRakPeerInterface( RakPeerInterface* i );
};

#endif


