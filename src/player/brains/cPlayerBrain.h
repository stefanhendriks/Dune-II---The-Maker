#pragma once

#include "cPlayerBrainData.h"
#include "observers/cScenarioObserver.h"

class cPlayer;
class cAbstractStructure;

namespace {

constexpr int kScanRadius = 20; // hard-coded for now, make player property later?

}

namespace brains {

class cPlayerBrain : public cScenarioObserver {

public:
    // AI brain tick is every 100ms, so rest-time of 10 means resting 1 second.
    const static int RestTime = 10;

    explicit cPlayerBrain(cPlayer *player);

    virtual ~cPlayerBrain() = default;

    /**
     * called, every 100 ms
     */
    virtual void think() = 0;

    /**
     * called, every 5 ms
     */
    virtual void thinkFast() = 0;

    virtual void addBuildOrder(S_buildOrder order) = 0;

protected:
    cPlayer *player;

private:


};
}
