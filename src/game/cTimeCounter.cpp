/**
 * @file cTimeCounter.cpp
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

#include "game/cTimeCounter.h"

cTimeCounter::cTimeCounter()
        : m_durationTimer(0), m_isPartyTimer(false)
{}

cTimeCounter::~cTimeCounter()
{}

void cTimeCounter::start()
{
    m_isPartyTimer = true;
    m_durationTimer = 0;
}

void cTimeCounter::pause()
{
    m_isPartyTimer = false;
}

void cTimeCounter::restart()
{
    m_isPartyTimer = true;
}

void  cTimeCounter::addTime(uint64_t delta)
{
    if (m_isPartyTimer) {
        m_durationTimer += delta;
    }
}


uint64_t cTimeCounter::getTime() const
{
    return m_durationTimer;
}
