/**
 * @file cExpandingRectangle.cpp
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

#include "cExpandingRectangle.h"
#include <cmath>

cExpandingRectangle::cExpandingRectangle()
    : m_minX(0), m_maxX(0), m_minY(0), m_maxY(0), m_initialized(false)
{
}

void cExpandingRectangle::expand(float x, float y)
{
    if (!m_initialized) {
        m_minX = m_maxX = x;
        m_minY = m_maxY = y;
        m_initialized = true;
    } else {
        if (x < m_minX) m_minX = x;
        if (x > m_maxX) m_maxX = x;
        if (y < m_minY) m_minY = y;
        if (y > m_maxY) m_maxY = y;
    }
    sync();
}

void cExpandingRectangle::expandBy(float amount)
{
    m_minX -= amount;
    m_maxX += amount;
    m_minY -= amount;
    m_maxY += amount;
    sync();
}

int cExpandingRectangle::getWidth() const
{
    return static_cast<int>(std::round(m_maxX - m_minX));
}

int cExpandingRectangle::getHeight() const
{
    return static_cast<int>(std::round(m_maxY - m_minY));
}

void cExpandingRectangle::sync()
{
    move(static_cast<int>(m_minX), static_cast<int>(m_minY));
    resize(getWidth(), getHeight());
}
