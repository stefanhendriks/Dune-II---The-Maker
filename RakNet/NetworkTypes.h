/// \file
/// \brief Types used by RakNet, most of which involve user code.
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

#ifndef __NETWORK_TYPES_H
#define __NETWORK_TYPES_H 

/// Forward declaration
namespace RakNet
{
	class BitStream;
};

/// \sa NetworkIDGenerator.h
typedef unsigned short ObjectID;
typedef unsigned char UniqueIDType;
typedef unsigned short PlayerIndex;
typedef unsigned char RPCIndex;
const int MAX_RPC_MAP_SIZE=((RPCIndex)-1)-1;
const int UNDEFINED_RPC_INDEX=((RPCIndex)-1);

/// \brief Unique identifier for a system.
/// Corresponds to a network address
struct PlayerID
{
	///The peer address from inet_addr.
	unsigned int binaryAddress;
	///The port number
	unsigned short port;
	
	PlayerID& operator = ( const PlayerID& input )
	{
		binaryAddress = input.binaryAddress;
		port = input.port;
		return *this;
	}

	friend int operator==( const PlayerID& left, const PlayerID& right );
	friend int operator!=( const PlayerID& left, const PlayerID& right );
	friend int operator > ( const PlayerID& left, const PlayerID& right );
	friend int operator < ( const PlayerID& left, const PlayerID& right );
};
/// Size of PlayerID data
#define PlayerID_Size 6

/// This represents a user message from another system.
struct Packet
{
	/// Server only - this is the index into the player array that this playerId maps to
	PlayerIndex playerIndex;
	
	/// The system that send this packet. 
	PlayerID playerId;
	
	/// The length of the data in bytes
	/// \deprecated You should use bitSize.
	unsigned int length;
		
	/// The length of the data in bits
	unsigned int bitSize;
	
	/// The data from the sneder
	unsigned char* data;
	
	/// @internal
	/// Indicates whether to delete the packet, or to simply update the producer consumer
	bool deletePacket;
};

class RakPeerInterface;

/// All RPC functions have the same parameter list - this structure.
struct RPCParameters
{
	/// The data from the remote system
	char *input;
	
	/// How many bits long \a input is
	unsigned int numberOfBitsOfData;
	
	/// Which system called this RPC
	PlayerID sender;
	
	/// Which instance of RakPeer (or a derived RakServer or RakClient) got this call
	RakPeerInterface *recipient;
	
	/// If true, the first 4 bytes of input represents a timestamp corresonding to RakNet::GetTime() and shifted to be relative to this system.
	bool hasTimestamp;
	
	/// You can return values from RPC calls by writing them to this BitStream.
	/// This is only sent back if the RPC call originally passed a BitStream to receive the reply.
	/// If you do so and your send is reliable, it will block until you get a reply or you get disconnected from the system you are sending to, whichever is first.
	/// If your send is not reliable, it will block for triple the ping time, or until you are disconnected, or you get a reply, whichever is first.
	RakNet::BitStream *replyToSender;
};

///  Index of an unassigned player
const PlayerIndex UNASSIGNED_PLAYER_INDEX = 65535;

/// Index of an invalid PlayerID
const PlayerID UNASSIGNED_PLAYER_ID =
{
	0xFFFFFFFF, 0xFFFF
};

/// Unassigned object ID
const ObjectID UNASSIGNED_OBJECT_ID = 65535;

const int PING_TIMES_ARRAY_SIZE = 5;

/// @defgroup RAKNET_RPC Remote Procedure Call Subsystem. 
/// \brief RPC Function Implementation 
/// 
/// The Remote Procedure Call Subsystem provide the RPC paradigm to
/// RakNet user. It consists in providing remote function call over the
/// network.  A call to a remote function require you to prepare the
/// data for each parameter (using BitStream) for example. 
/// 
/// Use the following C function prototype for your callbacks
/// @code
/// void functionName(RPCParameters *rpcParms);
/// @endcode 
/// If you pass input data, you can parse the input data in two ways.
/// 1.
/// Cast input to a struct (such as if you sent a struct)
/// i.e. MyStruct *s = (MyStruct*) input;
/// Make sure that the sizeof(MyStruct) is equal to the number of bytes passed!
/// 2.
/// Create a BitStream instance with input as data and the number of bytes
/// i.e. BitStream myBitStream(input, (numberOfBitsOfData-1)/8+1)
/// (numberOfBitsOfData-1)/8+1 is how convert from bits to bytes
/// Full example:
/// @code
/// void MyFunc(RPCParameters *rpcParms) {}
/// RakClient *rakClient;
/// REGISTER_AS_REMOTE_PROCEDURE_CALL(rakClient, MyFunc);
/// This would allow MyFunc to be called from the server using  (for example)
/// rakServer->RPC("MyFunc", 0, clientID, false);
/// @endcode


/// \def REGISTER_STATIC_RPC
/// \ingroup RAKNET_RPC
/// Register a C function as a Remote procedure. 
/// \param[in] networkObject Your instance of RakPeer, RakServer, or RakClient
/// \param[in] functionName The name of the C function to call
/// \attention 12/01/05 REGISTER_AS_REMOTE_PROCEDURE_CALL renamed to REGISTER_STATIC_RPC.  Delete the old name sometime in the future
#pragma deprecated(REGISTER_AS_REMOTE_PROCEDURE_CALL)
#define REGISTER_AS_REMOTE_PROCEDURE_CALL(networkObject, functionName) REGISTER_STATIC_RPC(networkObject, functionName)
#define REGISTER_STATIC_RPC(networkObject, functionName) (networkObject)->RegisterAsRemoteProcedureCall((#functionName),(functionName))

/// \def CLASS_MEMBER_ID
/// \ingroup RAKNET_RPC
/// \brief Concatenate two strings

/// \def REGISTER_CLASS_MEMBER_RPC
/// \ingroup RAKNET_RPC
/// \brief Register a member function of an instantiated object as a Remote procedure call.
/// RPC member Functions MUST be marked __cdecl!
/// \sa ObjectMemberRPC.cpp
/// \b CLASS_MEMBER_ID is a utility macro to generate a unique signature for a class and function pair and can be used for the Raknet functions RegisterClassMemberRPC(...) and RPC(...)
/// \b REGISTER_CLASS_MEMBER_RPC is a utility macro to more easily call RegisterClassMemberRPC
/// \param[in] networkObject Your instance of RakPeer, RakServer, or RakClient
/// \param[in] className The class containing the function
/// \param[in] functionName The name of the function (not in quotes, just the name)
#define CLASS_MEMBER_ID(className, functionName) #className "_" #functionName
#define REGISTER_CLASS_MEMBER_RPC(networkObject, className, functionName) {union {void (__cdecl className::*cFunc)( RPCParameters *rpcParms ); void* voidFunc;}; cFunc=&className::functionName; networkObject->RegisterClassMemberRPC(CLASS_MEMBER_ID(className, functionName),voidFunc);}

/// \def UNREGISTER_AS_REMOTE_PROCEDURE_CALL
/// \depreciated
/// \brief Only calls UNREGISTER_STATIC_RPC

/// \def UNREGISTER_STATIC_RPC 
/// \ingroup RAKNET_RPC
/// Unregisters a remote procedure call
/// RPC member Functions MUST be marked __cdecl!  See the ObjectMemberRPC example.
/// \param[in] networkObject The object that manages the function 
/// \param[in] functionName The function name 
// 12/01/05 UNREGISTER_AS_REMOTE_PROCEDURE_CALL Renamed to UNREGISTER_STATIC_RPC.  Delete the old name sometime in the future
#pragma deprecated(UNREGISTER_AS_REMOTE_PROCEDURE_CALL)
#define UNREGISTER_AS_REMOTE_PROCEDURE_CALL(networkObject,functionName) UNREGISTER_STATIC_RPC(networkObject,functionName)
#define UNREGISTER_STATIC_RPC(networkObject,functionName) (networkObject)->UnregisterAsRemoteProcedureCall((#functionName))

/// \def UNREGISTER_CLASS_INST_RPC 
/// \ingroup RAKNET_RPC
/// \brief Unregisters a member function of an instantiated object as a Remote procedure call.
/// \param[in] networkObject The object that manages the function 
/// \param[in] className The className that was originally passed to REGISTER_AS_REMOTE_PROCEDURE_CALL
/// \param[in] functionName The function name 
#define UNREGISTER_CLASS_MEMBER_RPC(networkObject, className, functionName) (networkObject)->UnregisterAsRemoteProcedureCall((#className "_" #functionName))

#endif

