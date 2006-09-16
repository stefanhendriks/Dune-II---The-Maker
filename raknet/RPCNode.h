/// \file
/// \brief \b [Internal] Holds information related to a RPC
///
/// \ingroup RAKNET_RPC
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

#ifndef __RPC_NODE
#define __RPC_NODE

#include "NetworkTypes.h"

class RakPeerInterface;


/// \defgroup RAKNET_RPC Remote Procedure Call Subsystem 
/// \brief A system to call C or object member procudures on other systems, and even to return return values.
///
/// \ingroup RAKNET_RPC 
/// \internal
/// 
/// \brief Map registered procedure inside of a peer.  
/// 
struct RPCNode
{
 
 	/// String identifier of the RPC
	char *uniqueIdentifier;
	
 /// Force casting of member functions to void *
	union
	{
		void ( *staticFunctionPointer ) ( RPCParameters *rpcParms );
		#ifdef __GNUC__
  		void (*memberFunctionPointer)(void* _this, RPCParameters *rpcParms);
		#else
		void (__cdecl *memberFunctionPointer)(void* _this, RPCParameters *rpcParms);
		#endif

		void *functionPointer;
	};
	
	/// Is this a member function pointer?  True if so.  If false it's a regular C function.
	bool isPointerToMember;
};

#endif



