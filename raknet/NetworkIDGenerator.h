/// \file
/// \brief A class you can derive from to make it easier to represent every networked object with an integer.  This way you can refer to objects over the network.
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

#if !defined(__NETWORK_ID_GENERATOR)
#define      __NETWORK_ID_GENERATOR

#include "BinarySearchTree.h"
#include "NetworkTypes.h"

class NetworkIDGenerator;

/// \internal
/// \brief A node in the AVL tree that holds the mapping between ObjectID and pointers.
struct ObjectIDNode
{
	ObjectID objectID;
	NetworkIDGenerator *object;
	ObjectIDNode();
	ObjectIDNode( ObjectID _objectID, NetworkIDGenerator *_object );
	friend int operator==( const ObjectIDNode& left, const ObjectIDNode& right );
	friend int operator > ( const ObjectIDNode& left, const ObjectIDNode& right );
	friend int operator < ( const ObjectIDNode& left, const ObjectIDNode& right );
};

/// \brief Unique shared ids for each object instance
///
/// A class you can derive from to make it easier to represent every networked object with an integer.  This way you can refer to objects over the network.
/// One system should return true for IsNetworkIDAuthority() and the rest should return false.  When an object needs to be created, have the the one system create the object.
/// Then have that system send a message to all other systems, and include the value returned from GetNetworkID() in that packet.  All other systems should then create the same
/// class of object, and call SetNetworkID() on that class with the ObjectID in the packet.
/// \see the manual for more information on this.
class NetworkIDGenerator
{
public:
	
	/// Constructor.  ObjectIDs, if IsNetworkIDAuthority() is true, are created here.
	NetworkIDGenerator();

	/// Destructor.  Used ObjectIDs, if any, are freed here.
	virtual ~NetworkIDGenerator();
	
	/// Returns the ObjectID that you can use to refer to this object over the network.
	/// \retval UNASSIGNED_OBJECT_ID UNASSIGNED_OBJECT_ID is returned IsNetworkIDAuthority() is false and SetNetworkID() was not previously called.  This is also returned if you call this function in the constructor.
	/// \retval 0-65534 Any other value is a valid ObjectID.  ObjectIDs start at 0 and go to 65534, wrapping at that point.
	virtual ObjectID GetNetworkID( void );
	
	/// Sets the ObjectID for this instance.  Usually this is called by the clients and determined from the servers.  However, if you save multiplayer games you would likely use
	/// This on load as well.	
	virtual void SetNetworkID( ObjectID id );
	
	/// Your class does not have to derive from NetworkIDGenerator, although that is the easiest way to implement this.
	/// If you want this to be a member object of another class, rather than inherit, then call SetParent() with a pointer to the parent class instance.
	/// GET_OBJECT_FROM_ID will then return the parent rather than this instance.
	virtual void SetParent( void *_parent );
	
	/// Return what was passed to SetParent
	/// \return The value passed to SetParent, or 0 if it was never called.
	virtual void* GetParent( void ) const;
	
	/// This AVL tree holds the pointer to ObjectID mappings	
	static BasicDataStructures::AVLBalancedBinarySearchTree<ObjectIDNode> IDTree;
	
	/// These function is only meant to be used when saving games as you
	/// should save the HIGHEST value staticItemID has achieved upon save
	/// and reload it upon load.  Save AFTER you've created all the items
	/// derived from this class you are going to create.  
	/// \return the HIGHEST Object Id currently used 
	static ObjectID GetStaticNetworkID( void );
	
	/// These function is only meant to be used when loading games. Load
	/// BEFORE you create any new objects that are not SetIDed based on
	/// the save data. 
	/// \param[in] i the highest number of NetworkIDGenerator reached. 
	static void SetStaticNetworkID( ObjectID i );
	
	/// For every group of systems, one system needs to be responsible for creating unique IDs for all objects created on all systems.
	/// This way, systems can send that id in packets to refer to objects (you can't send pointers because the memory allocations may be different).
	/// In a client/server enviroment, the system that creates unique IDs would be the server.
	/// return true if this system is responsible for creating unique IDs (probably the server), return true.  Otherwise return false.
	virtual bool IsNetworkIDAuthority(void) const=0;
	
	/// Overload this function and return true if you require that SetParent is called before this object is used.  You should do this if you want this to be
	/// a member object of another class rather than derive from this class.
	virtual bool RequiresSetParent(void) const;
	
	/// If you use a parent, returns this instance rather than the parent object.
	static void* NetworkIDGenerator::GET_BASE_OBJECT_FROM_ID( ObjectID x );
	
	/// Returns the parent object, or this instance if you don't use a parent.
	static void* NetworkIDGenerator::GET_OBJECT_FROM_ID( ObjectID x );

protected:
	
	/// The  network ID of this object
	ObjectID objectID;
	
	/// Store whether or not its a server assigned ID 
	bool serverAssignedID;
	
	/// The parent set by SetParent()
	void *parent;
	
	/// Internal function to generate an ID when needed.  This is deferred until needed and is not called from the constructor.
	void GenerateID(void);
	
	/// This is crap but is necessary because virtual functions don't work in the constructor
	bool callGenerationCode; 
	
private:
	
	static ObjectID staticItemID;
};

#endif


