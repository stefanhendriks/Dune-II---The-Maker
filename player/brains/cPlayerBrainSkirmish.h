#pragma once

#include "gameobjects/units/cUnit.h"
#include "player/brains/cPlayerBrain.h"
#include "player/brains/cPlayerBrainData.h"
#include "player/brains/missions/cPlayerBrainMission.h"

#include <set>

struct s_PlaceResult;

namespace brains {

    const int MISSION_IMPROVE_ECONOMY_BUILD_ADDITIONAL_HARVESTER = 98;
    const int MISSION_IMPROVE_ECONOMY_BUILD_ADDITIONAL_CARRYALL = 99;
    const int MISSION_SCOUT1 = 97;
    const int MISSION_SCOUT2 = 96;
    const int MISSION_SCOUT3 = 95;
    const int MISSION_GUARDFORCE = 94;
    const int MISSION_ATTACK1 = 93;
    const int MISSION_ATTACK2 = 92;
    const int MISSION_ATTACK3 = 91;
    const int MISSION_ATTACK4 = 90;
    const int MISSION_ATTACK5 = 89;

    const int SPECIAL_MISSION1 = 50;
    const int SPECIAL_MISSION2 = 51;
    const int SPECIAL_MISSION3 = 52;

    // 10 ticks = 1 second.
    // 100 ticks = 10 seconds.
    // 600 ticks = 60 seconds (1 minute)
    const int MOMENT_CONSIDER_ADDITIONAL_REFINERY = 300; // after 30 seconds consider building 2nd refinery

    const int MOMENT_PRODUCE_ADDITIONAL_UNITS = 1800; // after 3 minutes consider building additional units

    struct s_SkirmishPlayer_PlaceForStructure {
        int structureType;
        int cell;
    };

    /**
     * The cPlayerBrainSkirmish is responsible for being an AI brain during skirmish games.
     */
    class cPlayerBrainSkirmish : public cPlayerBrain {

    public:
        explicit cPlayerBrainSkirmish(cPlayer *player);

        ~cPlayerBrainSkirmish();

        void think() override;

        void thinkFast() override;

        void onNotifyGameEvent(const s_GameEvent &event) override;

        void addBuildOrder(S_buildOrder order) override;

        void log(const std::string & txt);

    private:
        ePlayerBrainState m_state;

        ePlayerBrainSkirmishThinkState m_thinkState;

        ePlayerBrainSkirmishEconomyState m_economyState;

        /**
         * A score between 0 (BAD) and 100 (GOOD) economy
         * A neutral economy score is between 30-70
         */
        int m_economyScore;

        int m_TIMER_rest;

        int m_TIMER_produceMissionCooldown;

        int m_TIMER_ai;

        int m_centerOfBaseCell;

        // at which cells did we detect an enemy? Remember those.
        std::set<int> m_discoveredEnemyAtCell;

        std::vector<cPlayerBrainMission> m_missions;
        std::vector<S_structurePosition> m_myBase;
        std::vector<S_buildOrder> m_buildOrders;

        void onMyStructureDestroyed(const s_GameEvent &event);

        void onMyStructureCreated(const s_GameEvent &event);

        void onMyStructureAttacked(const s_GameEvent &event);

        void onMyStructureDecayed(const s_GameEvent &event);

        void onMyUnitAttacked(const s_GameEvent &event);

        void thinkState_Base();

        void thinkState_Missions();

        void thinkState_Evaluate();

        void thinkState_EndGame();

        void thinkState_ProcessBuildOrders();

        void changeThinkStateTo(const ePlayerBrainSkirmishThinkState& newState);

        void changeEconomyStateTo(const ePlayerBrainSkirmishEconomyState& newState);

        bool allMissionsAreDoneGatheringResources();

        static const char* ePlayerBrainSkirmishThinkStateString(const ePlayerBrainSkirmishThinkState &state) {
            switch (state) {
                case ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_EVALUATE: return "PLAYERBRAIN_SKIRMISH_STATE_EVALUATE";
                case ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_PROCESS_BUILDORDERS: return "PLAYERBRAIN_SKIRMISH_STATE_PROCESS_BUILDORDERS";
                case ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_BASE: return "PLAYERBRAIN_SKIRMISH_STATE_BASE";
                case ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_ENDGAME: return "PLAYERBRAIN_SKIRMISH_STATE_ENDGAME";
                case ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_MISSIONS: return "PLAYERBRAIN_SKIRMISH_STATE_MISSIONS";
                case ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_REST: return "PLAYERBRAIN_SKIRMISH_STATE_REST";
                default:
                    assert(false);
                    break;
            }
            return "";
        }

        static const char* ePlayerBrainSkirmishEconomyStateString(const ePlayerBrainSkirmishEconomyState &state) {
            switch (state) {
                case ePlayerBrainSkirmishEconomyState::PLAYERBRAIN_ECONOMY_STATE_NORMAL: return "PLAYERBRAIN_ECONOMY_STATE_NORMAL";
                case ePlayerBrainSkirmishEconomyState::PLAYERBRAIN_ECONOMY_STATE_IMPROVE: return "PLAYERBRAIN_ECONOMY_STATE_IMPROVE";
                case ePlayerBrainSkirmishEconomyState::PLAYERBRAIN_ECONOMY_STATE_GOOD: return "PLAYERBRAIN_ECONOMY_STATE_GOOD";
                case ePlayerBrainSkirmishEconomyState::PLAYERBRAIN_ECONOMY_STATE_BAD: return "PLAYERBRAIN_ECONOMY_STATE_BAD";
                case ePlayerBrainSkirmishEconomyState::PLAYERBRAIN_ECONOMY_STATE_SELL_FOR_CASH: return "PLAYERBRAIN_ECONOMY_STATE_SELL_FOR_CASH";
                default:
                    assert(false);
                    break;
            }
            return "";
        }

        static const char* ePlayerBrainStateString(const ePlayerBrainState &state) {
            switch (state) {
                case ePlayerBrainState::PLAYERBRAIN_ENEMY_DETECTED: return "PLAYERBRAIN_ENEMY_DETECTED";
                case ePlayerBrainState::PLAYERBRAIN_LOSING: return "PLAYERBRAIN_LOSING";
                case ePlayerBrainState::PLAYERBRAIN_PEACEFUL: return "PLAYERBRAIN_PEACEFUL";
                case ePlayerBrainState::PLAYERBRAIN_WINNING: return "PLAYERBRAIN_WINNING";
                default:
                    assert(false);
                    break;
            }
            return "";
        }

        void thinkState_Rest();

        void onEntityDiscoveredEvent(const s_GameEvent &event);

        void produceMissions();

        void addMission(ePlayerBrainMissionKind kind, std::vector<S_groupKind> &group, int initialDelay,
                        int id);

        bool hasMission(const int id);

        s_SkirmishPlayer_PlaceForStructure thinkAboutNextStructureToBuildAndPlace();

        int getStructureToBuild() const;

        int getStructureIdToBuildWithoutConsideringPowerUsage() const;

        bool hasBuildOrderQueuedForStructure();

        bool hasBuildOrderQueuedForUnit(int buildId);

        void evaluateEconomyState();

        void produceSkirmishGroundAttackMission(int missionId);

        void findNewLocationOrMoveAnyBlockingUnitsOrCancelBuild(S_buildOrder *pBuildOrder, cBuildingListItem *pItem, const s_PlaceResult &placeResult);

        bool findNewPlaceToPlaceStructureOrCancelBuild(S_buildOrder *pBuildOrder, cBuildingListItem *pItem);

        void addBuildOrderForUnit(int type);

        void buildUnitIfICanAndNotAlreadyQueued(int type);

        void logMissions();

        void produceMissionsDuringPeacetime(int scoutingUnitType);

        void produceAttackingMissions();

        void produceSuperWeaponMissionsWhenApplicable();

        void produceEconomyImprovingMissions();

        void respondToThreat(cUnit *threat, cUnit *victim, int cellOriginOfThreat, int maxUnitsToOrder);

        ePlayerBrainSkirmishEconomyState determineEconomyState();
    };

}
