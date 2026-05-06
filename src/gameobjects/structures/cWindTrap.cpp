#include "cWindTrap.h"

#include "game/cGame.h"
#include "include/d2tmc.h"
#include "utils/cLog.h"
#include "definitions.h"
#include "gameobjects/players/cPlayer.h"
#include "utils/RNG.hpp"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"
#include <format>

// Constructor
cWindTrap::cWindTrap()
{
    cLogger::getInstance()->log(LOG_DEBUG, COMP_STRUCTURES, "Setting cWindTrap", 
                std::format("(cWindTrap)(ID {}) Constructor", this->id));

    m_flagDir = true;
    iFrame = RNG::rnd(6); // start with random frame, so not all windtraps are in sync
}

int cWindTrap::getType() const
{
    return WINDTRAP;
}

cWindTrap::~cWindTrap()
{
    cLogger::getInstance()->log(LOG_DEBUG, COMP_STRUCTURES, "Destroying structure", 
            std::format("(cWindTrap)(ID {}) Destructor", this->id));
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
    float percentage = ((float) getHitPoints() / (float) game.m_infoContext->getStructureInfo(getType()).hp);
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