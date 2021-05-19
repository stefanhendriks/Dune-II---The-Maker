#ifndef D2TM_CPLAYERBRAINSCENARIO_H
#define D2TM_CPLAYERBRAINSCENARIO_H

#include "cPlayerBrain.h"
#include "cPlayerBrainData.h"
#include "cPlayerBrainMission.h"

class cPlayerBrainScenario : public cPlayerBrain {

public:
    cPlayerBrainScenario(cPlayer * player);
    ~cPlayerBrainScenario();

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


#endif //D2TM_CPLAYERBRAINSCENARIO_H
