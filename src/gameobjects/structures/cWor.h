/**
 * @file cWor.h
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

class cWor : public cAbstractStructure {
public:
    cWor();

    void thinkFast() override;

    void thinkSlow() override {}

    void think_animation() override;

    void think_guard() override;

    void startAnimating() override {}

    void draw() override {
        drawWithShadow();
    }

    int getType() const override;

    std::string getStatusForMessageBar() const override {
        return getDefaultStatusMessageBar();
    }

    void onNotifyGameEvent(const s_GameEvent &) override {}
};
