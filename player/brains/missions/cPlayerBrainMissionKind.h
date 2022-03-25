#pragma once

#include "observers/cScenarioObserver.h"

class cPlayer;

namespace brains {

    class cPlayerBrainMission;

    enum ePlayerBrainMissionKind {
        /**
         * Attack enemy player (generally)
         */
        PLAYERBRAINMISSION_KIND_ATTACK,

        /**
         * Explore the map
         */
        PLAYERBRAINMISSION_KIND_EXPLORE,

        /**
         * Improve economy (ie, build Carry-All's, harvesters, etc)
         */
        PLAYERBRAINMISSION_IMPROVE_ECONOMY,

        /**
         * Explore the map, find spice patch(es)
         */
        PLAYERBRAINMISSION_KIND_FIND_SPICE,

        /**
         * Attack enemy player (generally) from the air
         */
        PLAYERBRAINMISSION_KIND_AIRSTRIKE,

        /**
         * Defend area?
         */
        PLAYERBRAINMISSION_KIND_DEFEND,

        /**
         * Harass enemy ... (harvesters?)
         */
        PLAYERBRAINMISSION_KIND_HARASS,

        /**
         * Super weapon usage: Fremen
         */
        PLAYERBRAINMISSION_KIND_SUPERWEAPON_FREMEN,

        /**
         * Super weapon usage: Saboteur
         */
        PLAYERBRAINMISSION_KIND_SUPERWEAPON_SABOTEUR,

        /**
         * Super weapon usage: Deathhand
         */
        PLAYERBRAINMISSION_KIND_SUPERWEAPON_DEATHHAND,

    };

    const char* ePlayerBrainMissionKindString(const ePlayerBrainMissionKind &kind);
    
    class cPlayerBrainMissionKind : public cScenarioObserver {

    public:

        cPlayerBrainMissionKind(cPlayer *player, cPlayerBrainMission * mission);

        ~cPlayerBrainMissionKind();

        virtual bool think_SelectTarget() = 0;

        virtual void think_Execute() = 0;

        virtual cPlayerBrainMissionKind * clone(cPlayer *player, cPlayerBrainMission * mission) = 0;

        virtual void onNotify_SpecificStateSwitch(const s_GameEvent &event) = 0;

        void onNotifyGameEvent(const s_GameEvent &event);

        virtual const char *toString() = 0;

        void log(const char*txt);

    protected:

        cPlayer *player;
        cPlayerBrainMission *mission;

        // This adds extra abilities to switch to a different state, without getting timed-out.
        cPlayer *specificPlayerForEventToGoToSelectTargetState; // by default == owning player

        // this is an event that should happen for the player (specificPlayerForEventToGoToSelectTargetState)
        eGameEventType specificEventTypeToGoToSelectTargetState;

        // if provided, the event (specificEventTypeToGoToSelectTargetState) - should also have these specific properties
        eBuildType specificBuildTypeToGoToSelectTargetState;

        int specificBuildIdToGoToSelectTargetState;

        void onExecuteSpecificStateSwitch(const s_GameEvent &event);
    };

}
