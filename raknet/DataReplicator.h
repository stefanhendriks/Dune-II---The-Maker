/// \file
/// \brief This class, based on the plugin system, is designed to automatically create and destroy your objects, transmit object scope, and synchronize memory.
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

#ifndef __DATA_REPLICATOR_H
#define __DATA_REPLICATOR_H

#include "BitStream.h"
#include "NetworkTypes.h"
#include "OrderedList.h"
#include "PluginInterface.h"
#include "PacketPriority.h"
#include "NetworkIDGenerator.h"
#include "BitStream.h"
#include "ArrayList.h"
#include "RakNetQueue.h"

class RakPeerInterface;
class ReplicatedObject;
class ReplicatedObjectFactory;

typedef unsigned char strToIndexMapType;
#define STR_TO_INDEX_MAP_MAX (strToIndexMapType)(strToIndexMapType(0)-1)

/// Maximum number of objects you can have waiting for creation requests at once
#define LOCAL_CREATION_REQUEST_TABLE_SIZE 64

/// \defgroup RAKNET_DATA_REPLICATOR Data Replicator Subsystem
/// \brief Automatically creates and destroys your objects and synchronizes the memory between them
///
/// The data replicator system is build on the plugin system.  is designed to automatically create and destroy your objects, transmit object scope, and synchronize memory.
/// \sa DataReplicatorTest.cpp

/// \brief This class, based on the plugin system, is designed to automatically create and destroy your objects, transmit object scope, and synchronize memory.
///
/// \ingroup RAKNET_DATA_REPLICATOR
/// This class is responsible for synchronizing memory and objects.
/// It solves the problems of:
/// 1. The concept of object scope, and relying scoping changes between systems.
/// 2. How to send a list of all synchronized objects and memory to newly connecting systems.
/// 3. Synchronizing object synchronization and desynchronization between systems.
/// 4. Tracking data values sent per-system, such that you can send only value changes if desired.
class DataReplicator : public PluginInterface
{
public:
	DataReplicator();
	~DataReplicator();

	// --------------------------------------------------------------------------------------------
	// Data replicator system participation functions
	// --------------------------------------------------------------------------------------------
	/// With BroadcastToAll true, all players that connect will have AddParticipant called on their playerId.  Defaults to false
	/// \param[in] value True or false, to broadcast to all or not.
	void SetBroadcastToAll(bool value);

	/// Returns the value passed to SetBroadcastToAll, or the default of false
	/// \return true or false
	bool GetBroadcastToAll(void) const;

	/// Adds a participant to the data replicator system.  This player will from then on will get memory and object updates.
	/// \param[in] playerId Which player you are referring to
	void AddParticipant(PlayerID playerId);

	/// Removes a participant from the data replicator system
	/// \param[in] playerId Which player you are referring to
	void RemoveParticipant(PlayerID playerId);

	/// Normally when a player is added as a participant, (either by calling AddParticipant or having BroadcastToAll as true)
	/// they willimmediately get object creation requests.  However, this is often inappropriate since that player may be
	/// loading or not yet in a playable modeCall this function with false to queue up all packets and not process them until
	/// this function is called with true.Defaults to true.
	/// \param[in] value false to queue replication, true (default) to immediately process replication
	void EnableReplication(bool value);

	// --------------------------------------------------------------------------------------------
	//	Object functions
	// --------------------------------------------------------------------------------------------
	/// Using object functions requires a callback that is able to create instances derivations of ReplicatedObject by class name.
	/// You can implement this through a switch / case or something more sophisticated if you wish.
	/// It is mandatory to call this functionif you want to automatically create objects through this system.
	/// \param[in] factoryFunc The callback.
	void SetClassFactory(ReplicatedObject* (* factoryFunc)(const char *className));

	/// This function causes an object that inherits from ReplicatedObject to send creation, destruction, and scope updates to all participants.
	/// \param[in] objectName The name of the class to create.  If object is 0, objectName must be filled in.  This name will be sent to your class factory.
	/// \param[in] object A pointer to the object if you already created it, or 0 if the object was not created yet and you want it to be created.
	/// \param[in] isObjectOwner Do we own this object?  If we do, then when we disconnect all other systems will get a call to ReplicatedObject::AcceptDereplicationRequest
	/// \param[in] priority Same as the parameter in RakPeer::Send
	/// \param[in] reliability Same as the parameter in RakPeer::Send
	/// \param[in] orderingChannel Same as the parameter in RakPeer::Send
	/// \param[in] maxUpdateFrequencyMS How often, at most, to test for scope changes and potentially send push notifications to other participants.  Pass 0 to check every call to RakPeer::Update.
	/// \return The object passed to the function, or the newly created object if no object was passed.
	ReplicatedObject* ReplicateObject(const char *objectName, ReplicatedObject *object, bool isObjectOwner, PacketPriority priority=HIGH_PRIORITY, PacketReliability reliability=RELIABLE_ORDERED, char orderingChannel=0, int maxUpdateFrequencyMS=0);

	/// \brief Unsynchronize an object passed to ReplicateObject.
	///
	/// This object and registered member variables will no longer be tracked by the DataReplicator system.
	/// Remote participants will get called ReplicatedObject::AcceptDereplicationRequest()
	/// If you want to delete an object you need to call this function first, or the system will crash on the next update cycle.
	/// \param[in] object The object to unsynchronize
	/// \param[in] sendNotificationPacket If you want other systems to get the AcceptDereplicationRequest event, pass true.  Otherwise the object will stop being tracked locally only.
	void DereplicateObject(ReplicatedObject *object, bool sendNotificationPacket);

	/// \brief Returns the last ReplicatedObject::InScope() calculation for an object.
	///
	/// \param[in] object The object we are referencing
	/// \param[in] target Who the object is in scope in relation to
	/// \return If the locally registered object is in scope on this remote system
	bool ObjectInScope(ReplicatedObject *object, PlayerID target);

	// --------------------------------------------------------------------------------------------
	//	Memory functions
	// --------------------------------------------------------------------------------------------
	/// \brief Associates memory with a string identifier.  Memory can either be a class that implements SynchronizedMemory, or it can be a raw pointer.
	///
	/// The best place to register memory is in ReplicatedObject::OnReplication, since this is called exactly once on every system
	/// when an object is registered.  You can call this more than once for the same pointer if you want to change the parameters used.
	/// Memory will be updated in the same order you call SynchronizeMemory on it.
	/// \param[in] stringId Identifier for this memory.  The set {stringId, ReplicatedObjectId} must form a unique pair.  It is easiest just to pass the variable name.
	/// \param[in] memory A pointer to a memory block that is passed around as the \a memoryPtr parameter to all the SetMemory*CB callbacks. The default behaviors assume this is a pointer to the memory you want copied and will perform memcpy on it.  If you overwrite all the callbacks, you can pass anything you want.
	/// \param[in] memoryByteLength The byte length of the data pointed to by memory.  This is only used by the default callbacks for SetMemoryCopyFromCB, SetMemoryMakeCopyCB(), SetMemoryDeserializeCB, and SetMemorySerializeCB.  If you set these callbacks yourself you can pass anything you want.
	/// \param[in] isMemoryOwner Are we the owner of the memory? If you own the memory, that means you are the main source of updates for this memory.  Among other things, you will send the actual value for interpolation sends while other systems will send the extrapolated value.
	/// \param[in] maxUpdateFrequencyMS How often, at most, to test for changes and potentially send packets.  Pass 0 to check every call to RakPeer::Update.  Unless you use the default behavior for SetMemorySerializeCB, I recommend you pass 0 and if you don't want to send just return false from the callback.
	/// \param[in] replicatedObject If this memory is a member variable of an object that derives from or has a parent class of ReplicatedObject, then pass it here.  Pass 0 if this memory is not a member variable of an object (i.e. a global variable).
	/// \param[in] priority Same as the parameter in RakPeer::Send
	/// \param[in] reliability Same as the parameter in RakPeer::Send
	/// \param[in] orderingChannel Same as the parameter in RakPeer::Send
	void SynchronizeMemory(const char *stringId, void *memory, unsigned memoryByteLength, bool isMemoryOwner, int maxUpdateFrequencyMS, ReplicatedObject *replicatedObject, PacketPriority priority=HIGH_PRIORITY, PacketReliability reliability=RELIABLE_ORDERED, char orderingChannel=0);

	/// \brief Unsynchronizes memory that was formerly synchronized with SynchronizeMemory.
	///
	/// \param[in] memoryPtr Pointer to the memory, NOT the stringId, that was passed to SynchronizeMemory
	/// \param[in] memory Pointer to the memory, NOT the stringId, that was passed to SynchronizeMemory
	/// \param[in] sendNotificationPacket Tell the other system we are unsynchronizing this object?  You should do this if you are deleting the object and want that deletion to transmit over the network.
	void UnsynchronizeMemory(const void *memoryPtr, bool sendNotificationPacket);

	/// \brief Sets the memory context to be passed to each memory callback.
	///
	/// This can be anything you want and is simply a pointer that is passed around for you.
	/// The intended use for this is to pass the ReplicatedObject that contains this member variable so you can process events
	/// on memory updates.Defaults to 0
	/// \param[in] memoryPtr The pointer to associate this context with
	/// \param[in] _context the data to associate
	void SetMemoryContext(const void *memoryPtr, void *_context);

	/// \brief [Optional] Set a callback for your memory pointer that will be called every tick after you get an update.
	///
	/// The intended use of this function is to interpolate the memory that is given to you between the \a currentValue and the \a targetValue
	/// This memory is treated as a special case for updates, such that you can interpolate a value without causing a send every tick.
	/// Instead, interpolation continues until the value changes from something other than interpolation, at which point an update gets sent.
	/// \param[in] _interpolateCB REQUIRED callback.  In it, you should adjust currentValue to be closer or equal to destinationValue.  Return true if you are done interpolating and this function will stop getting called until the next update arrives.
	/// \return True if you are done interpolating, false otherwise.
	void InterpolateMemory(const void *memoryPtr,
		bool (* _interpolateCB)(void *currentValue, void *targetValue, unsigned int currentTimeMS, unsigned int timePacketSentMS, unsigned int lastCallTimeMS, void *context)
		);

	/// \brief Interpolates a float using cumulative increments
	///
	/// A static math function that interpolates a value between the current position and the final position, given the starting time, current time, and last update time.
	/// The parameters here match the parameters in the callback to InterpolateMemory.
	/// All the parameters match the InterpolateMemory callback, so this is an easy way to do linear interpolation on a float.
	/// \param[in] position Pointer to the float you want to interpolate.  Both an in and an out parameter.
	/// \param[in] targetOrigin The origin of the destination you want to reach, not counting velocity
	/// \param[in] targetVelocity The velocity of the destination, or 0 if the destination is not moving
	/// \param[in] currentTimeMS The current time
	/// \param[in] timePacketSentMS The time we started moving position
	/// \param[in] lastUpdateTime The last time we updated position towards the target
	/// \param[in] interpolationTime How long to interpolate for (how long it takes to reach the destination)
	/// \param[in] updatePastInterpolationTime After reaching the target, should we continue to interpolate to stay at the location of the moving target?  Only meaninful if targetVelocity is nonzero
	/// \return True if we no longer need to interpolate.  Otherwise false.
	static bool InterpolateFloat(float *position, float targetOrigin, float targetVelocity, unsigned int currentTimeMS, unsigned int timePacketSentMS, unsigned int lastCallTimeMS, unsigned int interpolationTimeMS, bool updatePastInterpolationTime);

	/// \brief Returns the percentile of the remaining time that just elapsed.
	///
	/// For example, if you interpolate over 10 seconds, 5 seconds has already passed, and this function was called 2.5 seconds later,
	/// you just used 50% of the remaining time.  The function will return .5, so for example if you are interpolating position,
	/// you move halfway towards the destination. This is very useful for interpolation as it tells you the percentile of the remaining
	/// distance/angle/time to move towards the target.
	/// The percentile is capped to the total interpolation time
	/// \param[in] currentTimeMS The current time
	/// \param[in] timePacketSentMS The time we started moving position
	/// \param[in] lastUpdateTime The last time we updated position towards the target
	/// \param[in] interpolationTime How long to interpolate for (how long it takes to reach the destination)
	/// \return 1.0f means all the remaining interpolation time has been used.  0.0 means don't interpolate at all.
	static float GetCumulativeInterpolationPercentile(unsigned int currentTimeMS, unsigned int timePacketSentMS, unsigned int lastCallTimeMS, unsigned int interpolationTimeMS);

	/// \brief Returns the percentile of the total time that just elapsed, up to the maximum of interpolationTimeMS, or 0.0f if no time is remaining.
	///
	/// If you interpolate over 10 seconds and 6 seconds have passed then this function would return .6.  On your next call, if another
	/// 2 seconds have passed, this function would return .2.  This is capped to the total interpolation time, so if on the final call
	/// 4 seconds have passed, for a total of 12 seconds, only the last 2 seconds would count, and the function would return .2.
	/// \param[in] currentTimeMS The current time
	/// \param[in] timePacketSentMS The time we started moving position
	/// \param[in] lastUpdateTime The last time we updated position towards the target
	/// \param[in] interpolationTime How long to interpolate for (how long it takes to reach the destination)
	/// \return 0.0f means interpolation is complete.  Otherwise returns the amount to interpolate.
	static float GetLinearInterpolationPercentile(unsigned int currentTimeMS, unsigned int timePacketSentMS, unsigned int lastCallTimeMS, unsigned int interpolationTimeMS);

	/// \brief Serializes memory to a bitstream.
	///
	/// This callback is responsible for sending a bitstream to a remote system, that can represent a change in value to
	/// the data pointed to by the source pointer. Whatever you write to the bitstream will be received by the callback
	/// passed to SetMemoryDeserializeCB.Note that while the intended purpose is to serialize the data pointed to by source,
	/// you can send anything you want and whatever you send will arrive in the deserialize callback.
	/// This callback is only called if the callback passed to SetMemoryShouldSendUpdateCB returns true.
	/// The last sent value and last sent time is given to you in case you want to use delta compression.If you want to trigger
	/// events to occur or to save values everytime you send to a particular system this is the place to do it.
	/// The default behavior is to do a bitstream write from source of length memoryByteLength
	/// \param[in] memoryPtr Which memory to associate this callback with
	/// \param[in] cbFunc Function pointer to the callback
	/// \param[in] source The parameter \a SynchronizeMemory::memory.
	/// \param[in] memoryByteLength The parameter \a SynchronizeMemory::memoryByteLength.
	/// \param[in] context The parameter \a SetMemoryContext::_context, or 0 if that function was not called.
	/// \param[in] bitstream This bitstream will be sent to playerId when this function returns.  The intended usage is to serialize memoryPtr to this bitstream, but you can do whatever you want.
	/// \param[in] lastSentValue When this function returns, the callback passed to either SetMemoryMakeCopyCB or SetMemoryCopyFromCB to make a copy of \a source.  This essentially tracks the last value sent to playerId.
	/// \param[in] currentTime The time RakPeer::Receive was called that triggered the update to the DataReplicator system.  This should be close to the current value of RakNet::GetTime and can be used to avoid calling that slow function.
	/// \param[in] lastSendTime The last time that this function was called for this playerId.  Matches the prior value of currentTime.
	/// \param[in] playerId The player that we are sending bitStream to.
	/// \param[in] includeTimestamp If you set this pointer to true, then a timestamp will be included in the packet and will be reflected in the \a timePacketSent parameter of the callback from SetMemoryDeserializeCB.  Defaults to false.
	void SetMemorySerializeCB(const void *memoryPtr, void (* cbFunc)(void *source, unsigned memoryByteLength, void *context, RakNet::BitStream *bitstream, void *lastSentValue, unsigned int currentTime, unsigned int lastSendTime, PlayerID playerId, bool *includeTimestamp) );

	/// \brief Write to memory from a bitstream.
	///
	/// The callback passed to this function is called when we get an update from the callback passed to SetMemorySerializeCB().
	/// Whatever was written to the parameter \a bitstream in SetMemorySerializeCB() will be in the bitstream in the callback.
	/// If you want to do processing or events based on getting an update for a memory block, this is the place to do it.
	/// The default behavior is to copy the data in bitstream to memoryPtr.
	/// \param[in] memoryPtr Which memory to associate this callback with
	/// \param[in] cbFunc Function pointer to the callback
	/// \param[in] destination The parameter \a SynchronizeMemory::memory.
	/// \param[in] memoryByteLength The parameter \a SynchronizeMemory::memoryByteLength.
	/// \param[in] context The parameter \a SetMemoryContext::_context, or 0 if that function was not called.
	/// \param[in] bitstream The data passed to the parameter \a bitstream in SetMemorySerializeCB.
	/// \param[in] timePacketSent If includeTimestamp was set to true in the SetMemorySerializeCB callback, then this is when the packet was sent.  Otherwise it is the time the time we got the packet.
	/// \param[in] playerId The player that we got bitstream from.
	void SetMemoryDeserializeCB(const void *memoryPtr, void (* cbFunc)(void *destination, unsigned memoryByteLength, void *context, RakNet::BitStream *bitstream, unsigned int timePacketSent, PlayerID playerId));

	/// \brief Sets the callback to be called when we need to allocate and copy of The parameter \a SynchronizeMemory::memory.
	///
	/// Whatever you return will be passed to \a lastSentValue in the callback for SetMemorySerializeCB and \a source in
	/// the callback for SetMemoryCopyFromCB.
	/// \note You aren't restricted to exactly copying \a source as the system does not use what you return -
	/// it only passes it to those functions.For example, if you are pointing to a structure that contains pointers,
	/// to make a copy of the last sent value you don't want to copy the pointers -
	/// you want to allocate memory to hold the values of those pointers.This is used to track \a lastSentValue for SetMemorySerializeCB()
	/// as well as the pre-interpolation value to track if something other than interpolation changed this memory.
	/// Defaults to doing a simple char* allocation with a memcpy
	/// \param[in] memoryPtr Which memory to associate this callback with
	/// \param[in] cbFunc Function pointer to the callback
	/// \param[in] destination The parameter \a SynchronizeMemory::memory.
	/// \param[in] memoryByteLength The parameter \a SynchronizeMemory::memoryByteLength.
	/// \param[in] context The parameter \a SetMemoryContext::_context, or 0 if that function was not called.
	void SetMemoryMakeCopyCB(const void *memoryPtr, void* (* cbFunc)(void *source, unsigned memoryByteLength, void *context));

	/// \brief Write one memory block to antoher
	///
	/// Sets the callback to be called when we need to only copy The parameter \a SynchronizeMemory::memory.
	/// Otherwise identical to SetMemoryMakeCopyCB().
	/// \param[in] memoryPtr Which memory to associate this callback with
	/// \param[in] cbFunc Function pointer to the callback
	/// \param[in] destination A block of memory - whatever structure and size was returned by SetMemoryMakeCopyCB(), that needs a copy of \a source.  This is used to track \a lastSentValue for SetMemorySerializeCB as well as the pre-interpolation value to track if something other than interpolation changed this memory.
	/// \param[in] source The parameter \a SynchronizeMemory::memory.
	/// \param[in] memoryByteLength The parameter \a SynchronizeMemory::memoryByteLength. Isn't necessarily the size of source or destination - the real allocated size of those pointers depends on what you returned from the SetMemoryMakeCopyCB() callback.
	/// \param[in] context The parameter \a SetMemoryContext::_context, or 0 if that function was not called.
	void SetMemoryCopyFromCB(const void *memoryPtr, void (* cbFunc)(void *destination, void *source, unsigned memoryByteLength, void *context));

	/// \brief Delete a block of memory
	///
	/// Set the callback to be called when the system no longer needs a copy of allocated memory.
	/// This occurs on shutdown, and when a participant is dropped such that the last sent value to that participant (if non-unified memory),
	/// or when the last reference to a block of sent memory (for unified memory) is no longer needed
	/// Defaults to
	/// \code
	/// delete memory;
	/// \endcode
	/// You can leave this at the default unless you did complex allocation in the SetMemoryMakeCopyCB() callback
	/// \param[in] memoryPtr Which memory to associate this callback with
	/// \param[in] cbFunc Function pointer to the callback
	/// \param[in] memory The pointer to delete
	/// \param[in] memoryByteLength The parameter \a SynchronizeMemory::memoryByteLength. Isn't necessarily the size of source or destination - the real allocated size of those pointers depends on what you returned from the SetMemoryMakeCopyCB() callback.
	/// \param[in] context The parameter \a SetMemoryContext::_context, or 0 if that function was not called.
	void SetMemoryFreeMemoryCB(const void *memoryPtr, void (* cbFunc)(void *memory, unsigned memoryByteLength, void *context));

	/// \brief Return true or false if we should send a memory update
	///
	/// Set the callback to be called when the system needs to know if we should send another update to a remote system.
	/// Generally you will compare \a memory and \a lastSentValue and return true if they are different and enough time
	/// has passed between \a currentTime and \a lastSendTime/
	/// Defaults to
	/// \code
	/// return memcmp(memory, lastSentValue)!=0;
	/// \endcode
	/// \param[in] memoryPtr Which memory to associate this callback with
	/// \param[in] cbFunc Function pointer to the callback
	/// \param[in] lastSentValue The pointer returned from the SetMemoryMakeCopyCB() callback.
	/// \param[in] memory The parameter \a SynchronizeMemory::memory.
	/// \param[in] memoryByteLength The parameter \a SynchronizeMemory::memoryByteLength. Isn't necessarily the size of source or destination - the real allocated size of those pointers depends on what you returned from the SetMemoryMakeCopyCB() callback.
	/// \param[in] destinationSystem The player that we are considering sending to
	/// \param[in] currentTime The time RakPeer::Receive was called that triggered the update to the DataReplicator system.  This should be close to the current value of RakNet::GetTime and can be used to avoid calling that slow function.
	/// \param[in] lastSendTime The last time that this function was called for this playerId.  Matches the prior value of currentTime.
	/// \param[in] context The parameter \a SetMemoryContext::_context, or 0 if that function was not called.
	void SetMemoryShouldSendUpdateCB(const void *memoryPtr, bool (* cbFunc)(void *memory, void *lastSentValue, unsigned memoryByteLength, PlayerID destinationSystem,  unsigned int currentTime, unsigned int lastSendTime, void *context));

	/// \brief Return the last value sent for a memory block
	///
	/// Returns the \a lastSentValue parameter passed to the SetMemoryShouldSendUpdateCB() callback for a given playerId
	/// \param[in] memoryPtr The pointer returned from the SetMemoryMakeCopyCB() callback.
	/// \param[in] playerId Specifies which player's memory copy copy we are referring to
	/// \return Either a pointer to a SynchronizedMemory object or a pointer to a copy of some raw data, or 0 if that memory block or player is not registered
	void* GetLastSentMemoryValue(const void *memoryPtr, PlayerID playerId);

	/// \brief Returns the last time this memory value was sent.
	///
	/// This function is useful for linking sends, since the time returned from this function will be equal to the
	/// currentTime parameter in the memory serialize callback for all serializations that occur during the same update.
	/// For example, if you are processing the memory block "position" and want to always send position if you also send "controls"
	/// then you should check the last time whichever one was first registered was sent inside the callback for the one registered later.
	/// If the times are equal then you would go ahead and allow the send in the SetMemoryShouldSendUpdateCB callback.
	/// \param[in] memoryPtr The pointer returned from the SetMemoryMakeCopyCB() callback.
	/// \param[in] playerId Specifies which player's memory copy copy we are referring to
	/// \return The last time a send was done for \a memoryPtr for the player \a playerId.  Returns 0 if no sends were ever done, or this memory block is not found.
	unsigned int GetLastSendTime(const void *memoryPtr, PlayerID playerId);


	// --------------------------------------------------------------------------------------------
	// General functions
	// --------------------------------------------------------------------------------------------
	/// Frees all memory and releases all participants
	void Clear(void);

	// --------------------------------------------------------------------------------------------
	// Overridable event callbacks
	// --------------------------------------------------------------------------------------------
	/// Too much data was in a packet.  User should override in a derived class to handle this.  Default behavior is to assert.
	virtual void OnDataOverflow(RakPeerInterface *peer, Packet *packet);

	/// Not enough data was in a packet.  User should override in a derived class to handle this.  Default behavior is to assert.
	virtual void OnDataUnderflow(RakPeerInterface *peer, Packet *packet);

	/// Packet data that just doesn't make sense.  Default behavior is to assert.
	virtual void OnInvalidPacket(RakPeerInterface *peer, Packet *packet);

// --------------------------------------------------------------------------------------------
//				Everything which follows in this class is for internal use
// --------------------------------------------------------------------------------------------

	// Used as a container for all the shared synchronized data - both memory and objects
	struct BaseData
	{
		// Store all the parameters from the synchronize call
		PacketPriority priority;
		PacketReliability reliability;
		char orderingChannel;

		// This is so we check maxUpdateFrequencyMS once and update it immediately
		bool doUpdate;

		// Keeps track of when we updated so we know when to update again
		int maxUpdateFrequencyMS;
		unsigned int lastUpdateTime;
		//bool autoAssignPropertyFlags;
		bool isOwner;

	};

	// Specific data for locally synchronized memory.  Sorted by localKey for fast lookups on memory updates
	struct BaseMemoryData : public BaseData
	{
		BaseMemoryData();

		// deletes identifier, which is an allocated string
		~BaseMemoryData();

		// Memory can be associated with objectIDs.  This will be used to see if an object is in scope
		ReplicatedObject* associatedObject;

		// References the actual memory in the game
		void *synchronizedMemory;

		// Memory across systems share unique identifiers so we can do lookups.
		char *localIdentifier;

		// Rather than send identifiers, we send the local key of each system to save bandwidth
		strToIndexMapType localKey;

		// Used for unified memory.  Records if we did a comparison check and a copy or not via didShouldSend.
		// This way only one compare and copy is done, and if one has been done we use the recorded value.
		//bool didComparisonAndCopy, dataDifferent;

		// 02/17/06 Disabled because a server getting data from one client and sending to others is always going to report the data is the same
		// True means we only need one copy of this memory.  False means we need more than one copy.
		// More than one copy is used if we send different values of this memory to different systems.
		//bool isUnifiedMemory;

		// Context to pass to each callback
		void *context;

		// ByteLength to pass to each callback
		unsigned byteLength;

		// Various callbacks for how to synchronize things.
		void (*serializeCB)(void *memory, unsigned memoryByteLength, void *context, RakNet::BitStream *bitstream, void *lastSentValue, unsigned int currentTime, unsigned int lastSendTime, PlayerID playerId, bool *includeTimestamp);
		void (*deserializeCB)(void *memory, unsigned memoryByteLength, void *context, RakNet::BitStream *bitstream, unsigned int timePacketSent, PlayerID playerId);
		void* (*makeCopyCB)(void *memory, unsigned memoryByteLength, void *context);
		void (*copyFromCB)(void *destination, void *source, unsigned memoryByteLength, void *context);
		void (*freeMemoryCB)(void *memory,unsigned byteLength, void *context);
		bool (*shouldSendUpdateCB)(void *memory, void *lastSentValue, unsigned memoryByteLength, PlayerID destinationSystem,  unsigned int currentTime, unsigned int lastSendTime, void *context);

		// Callbacks and data related to interpolation
		unsigned int lastInterpolateCallTime;
		unsigned int timePacketSent;
		bool (* interpolateCB)(void *currentValue, void *targetValue, unsigned int currentTimeMS, unsigned int timePacketSentMS, unsigned int lastCallTimeMS, void *context);
		void *lastReceivedValue;
		void *lastNonInterpValue; // Used so we can tell if memory changed from something other than interpolation.  If so, we disable interpolation.
		bool needsInterpolation; // Means we got a value so need to interpolate it.
		bool gotFirstUpdate; // Did we ever get a memory update?

		// This is done to differentiate between comparing between the last packet received and the synchronized memory.
		// If true, we don't run interpolation but we do continue to check against the last received value.
		bool userInterpolationFinished;
	};

	// Specific data for locally synchronized objects.  Sorted by ObjectID
	struct BaseObjectData : public BaseData
	{
		// The object referenced in the game.  We do lookups by objectID, which is in inherent property of the object.
		ReplicatedObject *replicatedObject;

		// Do we need to send an object creation request?
		bool needsCreationRequest;

		// This is here because we need to call onReplicate but can't call it from DataReplicator::ReplicateObject because that function may be
		// called from the object constructor, and in the constructor no networkID was assigned yet.  That function needs a network ID assigned,
		// because it generates the memory ID string based on part from the network ID value.
		bool calledOnReplication;
	};

	// Base class for per-remote system data.  Used to track what we last sent to each system so we can have system contextual sends
	// rather than always broadcasting.
	struct ExtendedData
	{
		// Extended data only holds the specific per-system data, and not the base object we are sending.
		// baseData points to the base object so we can do lookups.
		BaseData *baseData;
	};

	// Copies of the memory we sent to each remote system.  If unified, all copies point to the same memory block.
	struct ExtendedMemoryData : public ExtendedData
	{
		// Track what we last sent to this system
		void *lastMemorySend;
		unsigned int lastSendTime;

		// This is the local key on the remote system. 65535 means undefined.
		// This is initially sent to us when the remote system asks us to send us this memory.
		// We use this, rather than strings, to refer to memory blocks to save bandwidth
		strToIndexMapType remoteKey;
	};

	struct ExtendedObjectData : public ExtendedData
	{
		// Tracks the state of the object on the remote system so we know when to send or not.
		bool objectInstantiated;
		bool objectInScope;
	};

	struct ParticipantStruct
	{
		ParticipantStruct();
		~ParticipantStruct();

		// Which player does this struct represent?
		PlayerID playerId;

		// Sorted by base pointer integer value.
		BasicDataStructures::OrderedList<ExtendedMemoryData*, BaseData *> extendedMemoryList;
		BasicDataStructures::OrderedList<ExtendedObjectData*, BaseData *> extendedObjectList;

		// Unsorted
		BasicDataStructures::List<char*> remoteObjectNameTable;
		BasicDataStructures::List<char*> remoteMemoryNameTable;

		unsigned GetExtendedMemoryIndexByKey(strToIndexMapType key);
		unsigned objectRequestCount;
	};

protected:
	void OnAttach(RakPeerInterface *peer);
	void OnUpdate(RakPeerInterface *peer);
	int OnReceive(RakPeerInterface *peer, Packet *packet);
	void OnDisconnect(RakPeerInterface *peer);
	bool PropagateToGame(Packet *packet) const;

	friend int BaseObjectDataComp( BaseObjectData* data, ReplicatedObject* key );
	friend int BaseMemoryDataComp( BaseMemoryData* data, strToIndexMapType key );
	friend int ExtendedMemoryDataComp( ExtendedMemoryData* data, DataReplicator::BaseData * key );
	friend int ExtendedObjectDataComp( ExtendedObjectData* data, DataReplicator::BaseData * key );
	friend int ParticipantStructComp( ParticipantStruct* data, PlayerID key );
	void RequestObjectCreation(const char *objectName, RakNet::BitStream *context, ReplicatedObject *object, PacketPriority priority, PacketReliability reliability, char orderingChannel, PlayerID targetSystem);
	bool SendImmediateMemoryStartRequest(BaseMemoryData *memoryData, ParticipantStruct *participantStruct);
	void EncodeClassName(const char *strIn, PlayerID playerId, RakNet::BitStream *bitStream);
	void OnStringMapIndex(RakPeerInterface *peer, Packet *packet);
	void OnStringMapMemoryIndex(RakPeerInterface *peer, Packet *packet);
	bool DecodeClassName(char *strOut, ParticipantStruct *participantStruct, RakNet::BitStream *bitStream, RakPeerInterface *peer, Packet *packet);
	void DereplicateObjectInt(ReplicatedObject *object, bool sendNotificationPacket, bool remoteCall);
	void AddParticipantInt(PlayerID playerId);
	void AddExtendedMemoryDataToParticipant(BaseMemoryData *baseMemoryData, ParticipantStruct *participantStruct);
	void AddExtendedObjectDataToParticipant(BaseObjectData *baseObjectData, ParticipantStruct *participantStruct);
	void RemoveExtendedDataFromParticipant(BaseData *baseData, ParticipantStruct *participantStruct, bool isMemory, bool sendNotificationPacket);
	void SendObjectStopRequest(ObjectID networkID, PlayerID target);
	void SendMemoryStopRequest(char *localIdentifier, PlayerID target);
	//void SendDataStopRequest(BaseData *baseData, PlayerID target, bool isMemory);
	void ValidatedSend(ExtendedData *extendedData, ParticipantStruct *participant, bool isMemory, unsigned int currentTime);
	BaseMemoryData* GetBaseMemoryDataByString(const char *str);
	ParticipantStruct *GetParticipantByPlayerID(PlayerID playerId);
	unsigned GetUnifiedMemoryCount(void *memory);
	void* GetSynchronizedMemoryCopy(BaseMemoryData *baseMemoryData, void *source);
	strToIndexMapType GenerateMemoryStringMappingKey(void);
	BaseMemoryData *GetMemoryDataByKey(strToIndexMapType key);
	BaseObjectData* GetBaseObjectByObjectID(ObjectID objectId);
	BaseObjectData* GetBaseObjectByReplicatedObject(ReplicatedObject *object);
	void UnsynchronizeMemoryInt(BaseMemoryData *baseMemoryData, bool sendNotificationPacket);
	void OnCreationCommand(ReplicatedObject* newObject, PlayerID senderPlayerId, ObjectID objectId, PlayerID objectOwner, BaseObjectData *baseObjectData);
	//void SendMemoryStartRequest(BaseMemoryData *baseData, PlayerID target);
	void AddToParticipantList(PlayerID playerId);
	void RemoveFromParticipantList(PlayerID playerId, bool sendDataStopRequests, bool fromNetwork);
	void RemoveAllParticipants(void);
	unsigned GetParticipantIndexByPlayerID(PlayerID playerId);
	ExtendedObjectData* GetExtendedObjectByReplicatedObject(ReplicatedObject *object, ParticipantStruct *participant);
	ExtendedObjectData* GetExtendedObjectByObjectID(ObjectID objectId, ParticipantStruct *participant);
	ExtendedMemoryData* GetExtendedMemoryByBaseMemory(BaseMemoryData *baseData, ParticipantStruct *participant);
	BaseMemoryData *GetBaseMemoryDataBySynchronizedMemory(const void *memory);
	void OnDataReplicateSendMemory(RakPeerInterface *peer, Packet *packet);
	void OnSendReplicationPushPacket(RakPeerInterface *peer, Packet *packet);
	void OnDataReplicateSendObjectScope(RakPeerInterface *peer, Packet *packet);
	void OnMemoryReplicateStart(RakPeerInterface *peer, Packet *packet);
	void OnDataReplicateStop(RakPeerInterface *peer, Packet *packet);
	void OnNewPlayer(RakPeerInterface *peer, Packet *packet);
	void OnLostPlayer(RakPeerInterface *peer, Packet *packet);
	void OnObjectCreationRequest(RakPeerInterface *peer, Packet *packet);
	void OnObjectCreationRequestResponse(RakPeerInterface *peer, Packet *packet);
	ReplicatedObject *CreateObject(const char *className);
	void AutoAssignPropertyFlags(BaseObjectData *baseObjectData, ReplicatedObject *newObject, bool isObjectOwner);
	bool ParsePacket(RakPeerInterface *peer, Packet *packet);

	// Copy of the interface this plugin is attached to
	RakPeerInterface *rakPeer;

	// participantList is sorted by PlayerID because we do frequent lookups to make sure packet senders are participants.
	BasicDataStructures::OrderedList<ParticipantStruct*, PlayerID> participantList;
	BasicDataStructures::OrderedList<BaseMemoryData*, strToIndexMapType> memoryList;
	BasicDataStructures::OrderedList<BaseObjectData*, ReplicatedObject*> objectList;

	// If enableReplication==false, this will store all packets that deal with replication for later processing
	BasicDataStructures::Queue<Packet *> packetQueue;

	// For objects: This will be undefined at first.  When the remote system sends us an entire object name,
	// We reply with the key to use from then on.
	BasicDataStructures::List<char*> localObjectNameTable;

	static strToIndexMapType memoryToStringMappingKey;

    unsigned char localCreationRequestIndex;
	ReplicatedObject *localCreationRequestTable[LOCAL_CREATION_REQUEST_TABLE_SIZE];

	bool objectCreatedFromNetwork, ignoreReplicateObjectCalls;
	bool broadcastToAll;

	// Stores the value passed to the function of the same name
	bool enableReplication;
	
	ReplicatedObject* (*objectFactoryCallback)(const char *className);
};

/// \brief The result you result from a replication push callback for both the sender and the receiver
///
/// \ingroup RAKNET_DATA_REPLICATOR
enum ObjectReplicationPushResult
{
	/// Go ahead and push or create the object right away
	OBJECT_REPLICATION_OK,
	/// Ask me again next tick
	OBJECT_REPLICATION_PUSH_LATER,
	/// Never push or create this object
	OBJECT_REPLICATION_NEVER_PUSH,
};

/// \brief This class provides an interface so RakNet can do object initial serializations and notifications of scope changes.
///
/// \ingroup RAKNET_DATA_REPLICATOR
class ReplicatedObject : public NetworkIDGenerator
{
public:
	ReplicatedObject();
	virtual ~ReplicatedObject();

	/// Return true if this object is in-scope relevant to the system designated by playerId
	/// \param[in] playerId The remote system
	/// \param[in] dataReplicator The instance of the DataReplicator class this object was synchronized with.
	/// \return true if this object is in-scope relevant to the system designated by playerId.
	virtual bool InScope(PlayerID playerId, DataReplicator *dataReplicator);

	/// This object is either now in or now out of scope in relation to the system designated by playerId
	/// \param[in] sender The remote system
	/// \param[in] isNowInScope isNowInScope==true means that the InScope call returns true for your system, while false means it returns false.
	/// \param[in] dataReplicator The instance of the DataReplicator class this object was synchronized with.
	virtual void OnScopeChange(PlayerID sender, bool isNowInScope, DataReplicator *dataReplicator);

	/// A remote system has Unsynchronized an object and is notifying us of the desynchronizationIf you return true the DataReplicator will stop sending updates to that system if we send updates.If we do not send updates and we receive updates, the object will be Unsynchronized.  Note that the object is not deleted.
	/// \param[in] sender The remote system
	/// \param[in] dataReplicator The instance of the DataReplicator class this object was synchronized with.
	/// \return true to accept the deletion, false to ignore it.
	virtual bool AcceptDereplicationRequest(PlayerID sender, DataReplicator *dataReplicator);

	/// Convenience function called once, when the object is synchronized.  If the object contains synchronized memory, this function is a good placeto call SynchronizeMemory
	virtual void OnReplication(DataReplicator *dataReplicator);

	/// Function called every time the object is dereplicated by remote systems that send those objects.
	/// \param[in] dataReplicator The instance of the DataReplicator class this object was synchronized with.
	virtual void OnRemoteDereplication(DataReplicator *dataReplicator);

	/// Return the name of the class of this object.
	/// \return class name.
	virtual char* GetObjectClassName(void) const=0;

	/// A remote system has asked that an instance of this class be created.To accept, return true.  You will probably want to synchronize this class as well [Default does this already]
	/// \param[in] sender The remote system
	/// \param[in] inContext Data passed by the sender
	/// \param[in] outContext Data to return to the sender
	/// \param[in] objectOwner [OutParam] Which system owns the object, assuming it is created.  If you don't set this it defaults to this system
	/// \param[in] dataReplicator The instance of the DataReplicator class this object was synchronized with.
	/// \return true to accept creation of this class, false to delete it.
	virtual bool OnReceiveCreationRequest(PlayerID sender, RakNet::BitStream *inContext, RakNet::BitStream *outContext, PlayerID &objectOwner, DataReplicator *dataReplicator);

	/// Our request to create an object has been accepted.
	/// \param[in] sender The remote system
	/// \param[in] newObjectId The object ID of the new object.  You can set this yourself, or it will be set after the function returns if you do not.
	/// \param[in] inContext The outContext reply from OnCreationRequest
	/// \param[in] objectOwner [OutParam] Which system owns the object, assuming it is created.  If you don't set this it defaults to this system
	/// \param[in] dataReplicator The instance of the DataReplicator class this object was synchronized with.
	/// \return true to accept final creation of this class, false to delete it.
	virtual bool OnCreationRequestAcceptance(PlayerID sender, ObjectID newObjectId, RakNet::BitStream *inContext, PlayerID &objectOwner, DataReplicator *dataReplicator);

	/// Our request to create an object has been rejected.
	/// \param[in] sender The remote system
	/// \param[in] inContext The outContext reply from OnCreationRequest
	/// \param[in] dataReplicator The instance of the DataReplicator class this object was synchronized with.
	virtual void OnCreationRequestRejection(PlayerID sender, RakNet::BitStream *inContext, DataReplicator *dataReplicator);

	/// Called when we need this object created on the authoritative system
	/// \param[in] destination The system to send to
	/// \param[in] outContext Contextual data to include with the packet. It will passed to OnReceiveReplicationPush::inContext
	/// \param[in] dataReplicator The instance of the DataReplicator class this object was synchronized with.
	/// \return OBJECT_REPLICATION_OK to send a serialized copy of ReplicatedObject to that system immediately.
	/// \return OBJECT_REPLICATION_PUSH_LATER to delay send.  OnSendReplicationPush will be called again the next Update assuming that the object is still in scope.
	/// \return OBJECT_REPLICATION_NEVER_PUSH to permanently remove that system as a potential recipient of objectToReplicate
	virtual ObjectReplicationPushResult OnSendCreationRequest(PlayerID destination, RakNet::BitStream *outContext, DataReplicator *dataReplicator);

	/// Called when we have this object synchronized and the destination system does not.
	/// \param[in] destination The system to send to
	/// \param[in] outContext Contextual data to include with the packet. It will passed to OnReceiveReplicationPush::inContext
	/// \param[in] dataReplicator The instance of the DataReplicator class this object was synchronized with.
	/// \return OBJECT_REPLICATION_OK to send a serialized copy of ReplicatedObject to that system immediately.
	/// \return OBJECT_REPLICATION_PUSH_LATER to delay send.  OnSendReplicationPush will be called again the next Update assuming that the object is still in scope.
	/// \return OBJECT_REPLICATION_NEVER_PUSH to permanently remove that system as a potential recipient of objectToReplicate
	virtual ObjectReplicationPushResult OnSendReplicationPush(PlayerID destination, RakNet::BitStream *outContext, DataReplicator *dataReplicator);

	/// The inverse of OnSendReplicationPush, the other system wants to send an object to us that we didn't request.
	/// \param[in] sender The system that sent this object to us
	/// \param[in] inContext What was passed to OnSendReplicationPush::outContext
	/// \param[in] newObjectId The object ID to use for this new object.
	/// \param[in] objectOwner [OutParam] Which system owns the object, assuming it is created.  If you don't set this it defaults to this system
	/// \param[in] dataReplicator The instance of the DataReplicator class this object was synchronized with.
	/// \return true to accept the object, false to tell the caller not to push the object to us and to delete it.
	virtual bool OnReceiveReplicationPush(PlayerID sender, RakNet::BitStream *inContext, ObjectID newObjectId, PlayerID &objectOwner, DataReplicator *dataReplicator);

	/// Sets the owner
	/// \param[in] _owner Owner of the object.  When this system disconnects, the objects gets unsynchronized
	void SetOwner(PlayerID _owner);

	/// Gets the owner
	/// \return The value passed to SetOwner
	PlayerID GetOwner(void) const;

	/// Returns if this object is waiting for a response from the network ID authority to see if it can be created.
	/// \return if this object is waiting for a request creation response.
	bool GetWaitingForCreationResponse(void);

	/// For every group of systems, one system needs to be responsible for creating unique IDs for all objects created on all systems.This way, systems can send that id in packets to refer to objects (you can't send pointers because the memory allocations may be different).You must override this function to return the playerID of that system.
	/// \return PlayerID of the network ID assigning system.
	virtual PlayerID GetNetworkIDAuthority(void) const=0;

	/// For every group of systems, one system needs to be responsible for creating unique IDs for all objects created on all systems.This way, systems can send that id in packets to refer to objects (you can't send pointers because the memory allocations may be different).In a client/server enviroment, the system that creates unique IDs would be the server.If this system is responsible for creating unique IDs, return true.  Otherwise return false.
	/// \return true if this is the server, or a hosting peer.  Otherwise false.
	virtual bool IsNetworkIDAuthority(void) const=0;

	protected:
		/// This is the system that owns a particular object, which means when that system disconnects, other instances of this object on remote systems will
		/// Get a call to OnRemoteDereplication
		PlayerID owner;
};


#endif


