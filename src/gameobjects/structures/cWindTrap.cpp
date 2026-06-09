#include "cWindTrap.h"

#include "utils/Log.h"
#include "definitions.h"
#include "gameobjects/players/cPlayer.h"
#include "utils/RNG.hpp"
#include "context/cInfoContext.h"
#include <format>

// Constructor
cWindTrap::cWindTrap()
{
    Logger::debug(COMP_STRUCTURES, "cWindTrap", "(cWindTrap)(ID {}) Constructor", this->id);

    m_flagDir = true;
    iFrame = RNG::rnd(6); // start with random frame, so not all windtraps are in sync
}

int cWindTrap::getType() const
{
    return WINDTRAP;
}

cWindTrap::~cWindTrap()
{
    Logger::debug(COMP_STRUCTURES, "cWindTrap", "(cWindTrap)(ID {}) Destructor", this->id);
}

void cWindTrap::thinkFast()
{
    // think like base class
    cAbstractStructure::thinkFast();
    TIMER_flag++;

    if (TIMER_flag > 108) {
        if (m_flagDir) {
            iFrame++;
        }
        else {
            iFrame--;
        }
        // switch between 2 and 6.
        if (iFrame >= 6) {
            m_flagDir = false;
        }
        else if (iFrame <= 2) {
            m_flagDir = true;
        }
        TIMER_flag=0;
    }

}

void cWindTrap::think_animation()
{
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
}

void cWindTrap::think_guard()
{

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
int cWindTrap::getPowerOut() const
{
    float percentage = ((float) getHitPoints() / (float) m_info->getStructureInfo(getType()).hp);
    return getMaxPowerOut() * percentage;
}

int cWindTrap::getMaxPowerOut() const
{
    return getStructureInfo().power_give;
}

std::string cWindTrap::getStatusForMessageBar() const
{
    int powerProduction = ((float)getPowerOut() / (float)getMaxPowerOut()) * (float)100;
    return std::format("{} and producing at {} percent capacity", cAbstractStructure::getDefaultStatusMessageBar(), powerProduction);
}