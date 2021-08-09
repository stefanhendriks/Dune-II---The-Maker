#ifndef D2TM_CPLAYERBRAINSKIRMISH_H
#define D2TM_CPLAYERBRAINSKIRMISH_H

#include "player/brains/cPlayerBrain.h"
#include "player/brains/cPlayerBrainData.h"
#include "player/brains/missions/cPlayerBrainMission.h"
#include <set>
#include <player/cPlayer.h>

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

    // 10 = 1 second.
    // 100 = 10 seconds.
    // 600 = 60 seconds (1 minute)
    const int MOMENT_CONSIDER_ADDITIONAL_REFINERY = 1200; // after 2 minutes consider building 2nd refinery

    const int MOMENT_PRODUCE_ADDITIONAL_UNITS = 3000; // after 5 minutes consider building additional units

    struct s_SkirmishPlayer_PlaceForStructure {
        int structureType;
        int cell;
    };

    /**
     * The cPlayerBrainSkirmish is responsible for being an AI brain during skirmish games.
     */
    class cPlayerBrainSkirmish : public cPlayerBrain {

    public:
        cPlayerBrainSkirmish(cPlayer *player);

        ~cPlayerBrainSkirmish();

        void think() override;

        void onNotify(const s_GameEvent &event) override;

        void addBuildOrder(S_buildOrder order);

    private:
        ePlayerBrainState state;

        ePlayerBrainSkirmishThinkState thinkState;

        ePlayerBrainSkirmishEconomyState economyState;

        int COUNT_badEconomy;

        int TIMER_rest;

        int TIMER_ai;

        // at which cells did we detect an enemy? Remember those.
        std::set<int> discoveredEnemyAtCell;

        std::vector<cPlayerBrainMission> missions;
        std::vector<S_structurePosition> myBase;
        std::vector<S_buildOrder> buildOrders;

        void onMyStructureDestroyed(const s_GameEvent &event);

        void onMyStructureCreated(const s_GameEvent &event);

        void onMyStructureAttacked(const s_GameEvent &event);

        void onMyStructureDecayed(const s_GameEvent &event);

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

        void addMission(ePlayerBrainMissionKind kind, const std::vector<S_groupKind> &group, int initialDelay,
                        int id);

        bool hasMission(const int id);

        s_SkirmishPlayer_PlaceForStructure thinkAboutNextStructureToBuildAndPlace();

        int getStructureToBuild() const;

        int getStructureIdToBuildWithoutConsideringPowerUsage() const;

        bool hasBuildOrderQueuedForStructure();

        bool hasBuildOrderQueuedForUnit(int buildId);

        void evaluateEconomyState();

        void produceSkirmishGroundAttackMission(int trikeKind, int missionId);

        void findNewLocationOrMoveAnyBlockingUnitsOrCancelBuild(S_buildOrder *pBuildOrder, cBuildingListItem *pItem, const s_PlaceResult &placeResult);

        bool findNewPlaceToPlaceStructureOrCancelBuild(S_buildOrder *pBuildOrder, cBuildingListItem *pItem);

        void addBuildOrderForUnit(int type);

        void buildUnitIfICanAndNotAlreadyQueued(int type);
    };

}

#endif //D2TM_CPLAYERBRAINSKIRMISH_H
