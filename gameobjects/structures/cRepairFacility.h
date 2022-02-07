#pragma once

class cRepairFacility : public cAbstractStructure {

private:
    void think_animation_unitDeploy();

    int TIMER_repairunit;

    eAnimationDirection animDir;
public:
    cRepairFacility();

    // overloaded functions    
    void thinkFast() override;

    void thinkSlow() override {}

    void think_animation() override;

    void think_guard() override;

    int getType() const override;

    void think_repairUnit();

    void startAnimating() override;

    void draw() override { drawWithShadow(); }

};

