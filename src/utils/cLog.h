#pragma once

enum eLogLevel {
    LOG_INFO,
    LOG_TRACE,
    LOG_DEBUG,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
};

enum eLogComponent {
    COMP_UNITS,
    COMP_STRUCTURES,
    COMP_GAMERULES,
    COMP_SCENARIOINI,
    COMP_PARTICLE,
    COMP_BULLET,
    COMP_AI,
    COMP_UPGRADE_LIST,
    COMP_BUILDING_LIST_UPDATER,
    COMP_SIDEBAR,
    COMP_MAP,
    COMP_GAMEOBJECTS,
    COMP_GAME,
    COMP_NONE,
    COMP_INIT,
    // COMP_ALLEGRO,		/** Use for allegro specific calls **/
    COMP_SDL2,        /** Use for SDL2 specific calls **/
    COMP_SETUP,
    COMP_VERSION,		/** version specific loggin messages **/
    COMP_SKIRMISHSETUP, /** When skirmish game is being set up **/
    COMP_ALFONT, 		/** ALFONT library specific **/
    COMP_SOUND, 		/** Sound related **/
    COMP_REGIONINI,		/** Used for regions **/
    COMP_PLAYER,			/** Player related **/
    COMP_GAMESTATE,			/** Used for game state changes **/
    COMP_CHEATS,            /** Used for cheat related logs **/
    COMP_BUILDER
};
