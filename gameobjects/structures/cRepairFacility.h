#pragma once

#include "cAbstractStructure.h"
#include "enums.h"

class cRepairFacility : public cAbstractStructure {
  private:
    void think_animation_unitDeploy();

    int TIMER_repairunit;

    eAnimationDirection animDir;

  public:
    cRepairFacility();

    void thinkFast() override;

    void thinkSlow() override {}

    void think_animation() override;

    void think_guard() override;

    int getType() const override;

    void think_repairUnit();

    void startAnimating() override;

    void draw() override { drawWithShadow(); }

    std::string getStatusForMessageBar() const override { return getDefaultStatusMessageBar(); }

    void onNotifyGameEvent(const s_GameEvent &) override {}
};
