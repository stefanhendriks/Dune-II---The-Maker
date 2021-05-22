#ifndef D2TM_CPLAYERBRAINMISSION_H
#define D2TM_CPLAYERBRAINMISSION_H

#include <vector>
#include "player/brains/cPlayerBrainData.h"
#include <observers/cScenarioObserver.h>

namespace brains {

    class cPlayerBrainCampaign;

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

    enum ePlayerBrainMissionState {
        /**
         * Gather required resources (units), either via selecting existing ones or by building new ones
         */
        PLAYERBRAINMISSION_STATE_PREPARE_GATHER_RESOURCES,

        /**
         * Await construction of all units
         */
        PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES,

        /**
         * Determine which target...
         */
        PLAYERBRAINMISSION_STATE_SELECT_TARGET,

        /**
         * Start executing the mission
         */
        PLAYERBRAINMISSION_STATE_EXECUTE,

        /**
         * Mission ended
         */
        PLAYERBRAINMISSION_STATE_ENDED,

    };

    struct S_groupKind {
        /**
         * Which kind of unit?
         */
        int type;

        /**
         * How many of that type of unit do we want?
         */
        int required;

        /**
         * How many have we ordered so far?
         */
        int ordered;

        /**
         * How many are really produced?
         */
        int produced;
    };


    class cPlayerBrainMission : public cScenarioObserver {

    public:

        cPlayerBrainMission(cPlayer *player, const ePlayerBrainMissionKind &kind, cPlayerBrainCampaign *brain,
                            std::vector<S_groupKind> group);

        ~cPlayerBrainMission() override;

        void think();

        void onNotify(const s_GameEvent &event);

        bool isEnded() const;

    private:

        int target;

        int targetStructureID;
        int targetUnitID;

        cPlayer *player;

        ePlayerBrainMissionKind kind;

        cPlayerBrainCampaign *brain;

        ePlayerBrainMissionState state;

        // store which units where created (and thus part of this team)
        std::vector<int> units;

        // the desired group of units
        std::vector<S_groupKind> group;

        void thinkState_PrepareGatherResources();

        void thinkState_SelectTarget();

        void thinkState_Execute();

        void thinkState_PrepareAwaitResources();
    };

}

#endif //D2TM_CPLAYERBRAINMISSION_H
