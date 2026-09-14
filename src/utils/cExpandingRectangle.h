/**
 * @file cExpandingRectangle.h
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

#include "cRectangle.h"

class cExpandingRectangle : public cRectangle {
public:
    cExpandingRectangle();

    void expand(float x, float y);
    void expandBy(float amount);

    bool isInitialized() const { return m_initialized; }

    int getWidth() const override;
    int getHeight() const override;

private:
    float m_minX, m_maxX, m_minY, m_maxY;
    bool m_initialized;

    void sync();
};
