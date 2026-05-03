#pragma once

#include "cPlayerBrain.h"
#include "gameobjects/units/cUnit.h"

namespace brains {

class cPlayerBrainSandworm : public cPlayerBrain {

public:
    explicit cPlayerBrainSandworm(cPlayer *player);
    ~cPlayerBrainSandworm();

    void think() override;

    void thinkFast() override;

    void onNotifyGameEvent(const s_GameEvent &event) override;

    void addBuildOrder(s_buildOrder order) override;

private:
    int TIMER_think;

    void findRandomValidLocationToMoveToAndGoThere(cUnit *pSandWorm) const;
};

}
