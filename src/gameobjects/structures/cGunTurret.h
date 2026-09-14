/**
 * @file cGunTurret.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include "cAbstractStructure.h"

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

    // overloaded functions
    void thinkFast() override;

    void thinkSlow() override {}

    void think_animation() override;

    void think_guard() override;

    void think_fire();

    void think_attack();

    int getFacingAngles();

    void setShouldHeadFacing(int value) {
        iShouldHeadFacing = value;
    }

    void startAnimating() override {}

    void draw() override {
        drawWithShadow();
    }

    int getType() const override;

    int getHeadFacing() {
        return iHeadFacing;
    }

    void think_turning();

    bool isFacingTarget() const;

    std::string getStatusForMessageBar() const override {
        return getDefaultStatusMessageBar();
    }

    void onNotifyGameEvent(const s_GameEvent &) override {}
};

