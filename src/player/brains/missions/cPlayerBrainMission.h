#pragma once

#include "cPlayerBrainMissionKind.h"
#include "observers/cScenarioObserver.h"
#include "player/brains/cPlayerBrainData.h"

#include <vector>

namespace brains {

    class cPlayerBrain;
    class cPlayerBrainMissionKind;

    enum ePlayerBrainMissionState {
        /**
         * Use an initial delay to play with the synchronisity between missions
         */
        PLAYERBRAINMISSION_STATE_INITIAL_DELAY,

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
         * Start executing the "Mission kind"
         */
        PLAYERBRAINMISSION_STATE_EXECUTE,

        /**
         * Mission ended
         */
        PLAYERBRAINMISSION_STATE_ENDED,

    };

    struct S_groupKind {
        /**
         * Which kind of thing to build? (UNIT/STRUCTURE/SPECIAL?)
         */
        eBuildType buildType;

        /**
         * Which type of that buildType? (ie , in case buildType == UNIT, this is QUAD for example)
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


    /**
     * A mission is something where resources are gathered, and used for some kind of action to produce a desired
     * result. The cPlayerBrainMission takes care of gathering resources and when needed executing the actual
     * mission (the mission "kind"). The mission kind is a separate object that is constructed based on the
     * ePlayerBrainMissionKind enum.
     */
    class cPlayerBrainMission : public cScenarioObserver {

    public:

        cPlayerBrainMission(cPlayer *player, const ePlayerBrainMissionKind &kind, cPlayerBrain *brain,
                            std::vector<S_groupKind> group, int initialDelay, int uniqueId);

        ~cPlayerBrainMission() override;

        /**
         * Copy constructor
         * @param src
         */
        cPlayerBrainMission(const cPlayerBrainMission &src);

        /**
         * Assignment operator
         * @param rhs
         * @return
         */
        cPlayerBrainMission& operator=(const cPlayerBrainMission &rhs);

        void thinkFast();

        void onNotifyGameEvent(const s_GameEvent &event);

        bool isEnded() const;

        void changeState(ePlayerBrainMissionState newState);

        std::vector<int> & getUnits();

        int getUniqueId() const {
            return uniqueIdentifier;
        }

        bool isDoneGatheringResources();

        void log(const char *txt);
    private:

        int TIMER_delay;

        int TIMER_awaitingGatheringResoures;

        int uniqueIdentifier;

        cPlayer *player;

        ePlayerBrainMissionKind kind;

        cPlayerBrainMissionKind *missionKind;

        cPlayerBrain *brain;

        ePlayerBrainMissionState state;

        bool specialEventMakesStateSwitchToSelectTarget;

        bool missionWithUnits;

        // store which units where created (and thus part of this team)
        std::vector<int> units;

        // the desired group of units
        std::vector<S_groupKind> group;

        void thinkState_InitialDelay();

        void thinkState_PrepareGatherResources();

        void thinkState_SelectTarget();

        void thinkState_Execute();

        void thinkState_PrepareAwaitResources();

        void removeUnitIdFromListIfPresent(int unitIdToRemove);

        void onEventCreated(const s_GameEvent &event);

        void onEventDestroyed(const s_GameEvent &event);

        void onEventDeviated(const s_GameEvent &event);

        static const char* ePlayerBrainMissionStateString(const ePlayerBrainMissionState &state) {
            switch (state) {
                case ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_SELECT_TARGET: return "PLAYERBRAINMISSION_STATE_SELECT_TARGET";
                case ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_ENDED: return "PLAYERBRAINMISSION_STATE_ENDED";
                case ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_GATHER_RESOURCES: return "PLAYERBRAINMISSION_STATE_PREPARE_GATHER_RESOURCES";
                case ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES: return "PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES";
                case ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_EXECUTE: return "PLAYERBRAINMISSION_STATE_EXECUTE";
                case ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_INITIAL_DELAY: return "PLAYERBRAINMISSION_STATE_INITIAL_DELAY";
                default:
                    assert(false);
                    break;
            }
            return "";
        }

        void onEventCannotBuild(const s_GameEvent &event);

        bool producedAllRequiredUnits();

        void onEventCannotCreatePath(const s_GameEvent &event);

        void logUnits();
    };

}
