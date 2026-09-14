/**
 * @file cWindTrap.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include "cAbstractStructure.h"

class cWindTrap : public cAbstractStructure {
public:
    cWindTrap();

    ~cWindTrap();

    void thinkFast() override;

    void thinkSlow() override {}

    void think_animation() override;

    void think_guard() override;

    void startAnimating() override {}

    void draw() override {
        drawWithShadow();
    }

    int getType() const override;

    int getPowerOut() const;

    int getMaxPowerOut() const;

    std::string getStatusForMessageBar() const override;

    void onNotifyGameEvent(const s_GameEvent &) override {}
private:
    bool m_flagDir = true;      // Fading direction (TRUE -> up, FALSE -> down)
};

