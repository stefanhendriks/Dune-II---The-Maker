#pragma once

#include "cAbstractStructure.h"

class cGunTurret : public cAbstractStructure {
private:
    int m_iHeadFacing;        // (for turrets only) what is this structure facing at?
    int m_iShouldHeadFacing;  // where should we look face at?
    int m_iTargetID;           // target id (assumes is always a unit)

    int m_TIMER_fire;
    int m_TIMER_turn;
    int m_TIMER_guard;         // timed 'area scanning'

public:
    cGunTurret();

    // overloaded functions
    void thinkFast() override;

    void thinkSlow() override {}

    void think_animation() override;

    void think_guard() override;

    void think_fire();

    void think_attack();

    int getFacingAngles();

    void setShouldHeadFacing(int value) {
        m_iShouldHeadFacing = value;
    }

    void startAnimating() override {}

    void draw() override {
        drawWithShadow();
    }

    int getType() const override;

    int getHeadFacing() {
        return m_iHeadFacing;
    }

    void think_turning();

    bool isFacingTarget() const;

    std::string getStatusForMessageBar() const override {
        return getDefaultStatusMessageBar();
    }

    void onNotifyGameEvent(const s_GameEvent &) override {}
};

