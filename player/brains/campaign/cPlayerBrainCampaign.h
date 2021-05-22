#ifndef D2TM_CPLAYERBRAINCAMPAIGN_H
#define D2TM_CPLAYERBRAINCAMPAIGN_H

#include "player/brains/cPlayerBrain.h"
#include "player/brains/cPlayerBrainData.h"
#include "player/brains/missions/cPlayerBrainMission.h"

namespace brains {

/**
 * The cPlayerBrainCampaign is responsible for being an AI brain during campaign missions. This is different
 * compared to the Skirmish brain.
 */
    class cPlayerBrainCampaign : public cPlayerBrain {

    public:
        cPlayerBrainCampaign(cPlayer *player);

        ~cPlayerBrainCampaign();

        void think() override;

        void onNotify(const s_GameEvent &event) override;

        void addBuildOrder(S_buildOrder order);

    private:
        ePlayerBrainScenarioState state;

        int TIMER_initialDelay;
        int TIMER_scanBase;

        std::vector<cPlayerBrainMission> missions;
        std::vector<S_structurePosition> myBase;
        std::vector<S_buildOrder> buildOrders;

        void onMyStructureDestroyed(const s_GameEvent &event);

        void onMyStructureCreated(const s_GameEvent &event);

        void onMyStructureAttacked(const s_GameEvent &event);

        void onMyStructureDecayed(const s_GameEvent &event);

        void thinkState_Waiting();

        void thinkState_ScanBase();

        void thinkState_Missions();

        void thinkState_Evaluate();

        void thinkState_EndGame();

        void thinkState_ProcessBuildOrders();
    };

}

#endif //D2TM_CPLAYERBRAINCAMPAIGN_H
