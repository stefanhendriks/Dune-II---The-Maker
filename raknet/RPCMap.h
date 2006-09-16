/// \file
/// \brief \b [Internal] A container class for a list of RPCNodes
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

#ifndef __RPC_MAP
#define __RPC_MAP

#include "RPCNode.h"
#include "ArrayList.h"
#include "NetworkTypes.h"


/// \ingroup RAKNET_RPC 
/// \internal
/// \brief A container class for a list of RPCNodes
struct  RPCMap
{
public:
	RPCMap();
	~RPCMap();
	void Clear(void);
    RPCNode *GetNodeFromIndex(RPCIndex index);
	RPCNode *GetNodeFromFunctionName(char *uniqueIdentifier);
	RPCIndex GetIndexFromFunctionName(char *uniqueIdentifier);
	void AddIdentifierWithFunction(char *uniqueIdentifier, void *functionPointer, bool isPointerToMember);
	void AddIdentifierAtIndex(char *uniqueIdentifier, RPCIndex insertionIndex);
	void RemoveNode(char *uniqueIdentifier);
protected:
	BasicDataStructures::List<RPCNode *> rpcSet;
};

#endif



