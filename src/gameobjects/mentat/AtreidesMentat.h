/**
 * @file AtreidesMentat.h
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

#include "AbstractMentat.h"

class AtreidesMentat : public AbstractMentat {
public:
    AtreidesMentat(GameContext* ctx, bool allowMissionSelect);
    void think() override;

    protected:
    void draw_mouth() override;
    void draw_eyes() override;
    void draw_other() override;
    void draw() override;
};
