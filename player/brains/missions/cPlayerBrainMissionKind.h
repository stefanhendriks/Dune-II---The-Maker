#ifndef D2TM_CPLAYERBRAINMISSIONKIND_H
#define D2TM_CPLAYERBRAINMISSIONKIND_H

#include <observers/cScenarioObserver.h>

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

    };

    class cPlayerBrainMissionKind : public cScenarioObserver {

    public:

        cPlayerBrainMissionKind(cPlayer *player, cPlayerBrainMission * mission);

        virtual ~cPlayerBrainMissionKind() = 0;

        virtual void think_SelectTarget() = 0;

        virtual void think_Execute() = 0;

        virtual cPlayerBrainMissionKind * clone(cPlayer *player, cPlayerBrainMission * mission) = 0;

        virtual void onNotify(const s_GameEvent &event) override = 0;

    protected:

        cPlayer *player;
        cPlayerBrainMission *mission;
    };

}

#endif //D2TM_CPLAYERBRAINMISSIONKIND_H
