#pragma once


enum eGameEventType {
    GAME_EVENT_NONE,            
    GAME_EVENT_DESTROYED,       // an entity was destroyed
    GAME_EVENT_DEVIATED,        // an entity was deviated (switched player ownership)
    GAME_EVENT_DISCOVERED,      // an entity was discovered
    GAME_EVENT_CREATED,         // an entity was created
    GAME_EVENT_DAMAGED,         // damaged by projectile
    GAME_EVENT_DECAY,           // damaged by decay
    GAME_EVENT_SPECIAL_SELECT_TARGET,   // special weapon READY (ie deathhand can be launched)
    GAME_EVENT_LIST_ITEM_PLACE_IT,      // list item is ready for placement (structure)
    GAME_EVENT_SPECIAL_LAUNCH,          // LAUNCHes special weapon (ie deathhand)
    GAME_EVENT_SPECIAL_LAUNCHED,        // special weapon has been launched! (warning)
    GAME_EVENT_CANNOT_BUILD,            // cannot build requested thing
    GAME_EVENT_CANNOT_CREATE_PATH,      // cannot create a path to destination, help!
    GAME_EVENT_LIST_BECAME_AVAILABLE,   // list became available
    GAME_EVENT_LIST_BECAME_UNAVAILABLE, // list became unavailable
    GAME_EVENT_LIST_ITEM_ADDED,         // list item has been added (not necessarily available)
    /**
     * list item has been constructed & deployed, and became available again for construction.
     * Also fired when item is an upgrade and it is finished.
     */
    GAME_EVENT_LIST_ITEM_FINISHED,      
    GAME_EVENT_LIST_ITEM_CANCELLED,     // list item has been cancelled
    GAME_EVENT_ABOUT_TO_BEGIN,          // we're about to play the mission! (kind of init state)
    GAME_EVENT_SPICE_BLOOM_SPAWNED,     // spice bloom has been spawned on the map
    GAME_EVENT_SPICE_BLOOM_BLEW,        // spice bloom has been blown up
    GAME_EVENT_PLAYER_DEFEATED,         // a player got defeated
    GAME_EVENT_DEPLOY_UNIT,             // Notify a unit should be deployed
};