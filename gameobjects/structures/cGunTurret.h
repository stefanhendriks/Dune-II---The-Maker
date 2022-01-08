#pragma once

class cGunTurret : public cAbstractStructure {

private:
    int iHeadFacing;        // (for turrets only) what is this structure facing at?
    int iShouldHeadFacing;  // where should we look face at?
    int iTargetID;           // target id (assumes is always a unit)

    int TIMER_fire;
    int TIMER_turn;
    int TIMER_guard;         // timed 'area scanning'

public:
    cGunTurret();

    ~cGunTurret();

    // overloaded functions    
    void think() override;

    void think_animation() override;

    void think_guard() override;

    void think_attack();

    void startAnimating() override {}

    void draw() override { drawWithShadow(); }

    int getType() const override;

    int getHeadFacing() { return iHeadFacing; }

    void think_turning();
};

