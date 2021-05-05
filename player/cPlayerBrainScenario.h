#ifndef D2TM_CPLAYERBRAINSCENARIO_H
#define D2TM_CPLAYERBRAINSCENARIO_H

#include "cPlayerBrain.h"

struct S_structurePosition {
    int cell;
    int type;
    int structureId;
    bool isDestroyed;
};

struct S_buildOrder {
    eBuildType buildType; // ie UNIT
    int priority;       // the higher, the more priority to build this
    int buildId;        // ie QUAD
    int placeAt;        // if buildType = STRUCTURE, this is the cell it should be placed at
    buildOrder::eBuildOrderState state;
};

class cPlayerBrainScenario : public cPlayerBrain {

public:
    cPlayerBrainScenario(cPlayer * player);
    ~cPlayerBrainScenario();

    void think();

private:
    int TIMER_initialDelay;
    int TIMER_scanBase;
    int TIMER_processBuildOrders;
    bool shouldScanBaseAndStoreIt;

    void storeMyBase();
    void scanBase();
    void processBuildOrders();
    void addBuildOrder(S_buildOrder order);

    std::vector<S_structurePosition> myBase;
    std::vector<S_buildOrder> buildOrders;
};


#endif //D2TM_CPLAYERBRAINSCENARIO_H
