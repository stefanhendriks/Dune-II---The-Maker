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
