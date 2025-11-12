#include "utils/cTimeCounter.h"

cTimeCounter::cTimeCounter()
        : durationTimer(0), isPartyTimer(false)
{}
 
cTimeCounter::~cTimeCounter()
{}

void cTimeCounter::start()
{
    isPartyTimer = true;
    durationTimer = 0;
}

void cTimeCounter::pause()
{
    isPartyTimer = false;
}

void cTimeCounter::restart()
{
    isPartyTimer = true;
}

void  cTimeCounter::addTime(uint64_t delta)
{
    if (isPartyTimer) {
        durationTimer += delta;
    }
}


uint64_t cTimeCounter::getTime() const
{
    return durationTimer;
}