#include "cWindTrap.h"

#include "game/cGame.h"
#include "include/d2tmc.h"
#include "utils/cLog.h"
#include "definitions.h"
#include "player/cPlayer.h"
#include "utils/RNG.hpp"
#include <format>

// Constructor
cWindTrap::cWindTrap()
{
    cLogger::getInstance()->log(LOG_DEBUG, COMP_STRUCTURES, "Setting cWindTrap", 
                std::format("(cWindTrap)(ID {}) Constructor", this->id));
    // other variables (class specific)
    m_iFade = RNG::rnd(63);
    m_bFadeDir = true;

    // Timers
    m_TIMER_fade = 0;
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
}

void cWindTrap::think_fade()
{
    m_TIMER_fade++;

    int iTime; // the speed of fading

    // depending on fade direction, fade in slower/faster
    if (m_bFadeDir) { // go to blue
        iTime = 3;
    }
    else {   // go to black
        iTime = 4;
    }

    // time passed, we may change fade color
    if (m_TIMER_fade > iTime) {
        if (m_bFadeDir) {
            m_iFade++;

            if (m_iFade > 254) {
                m_bFadeDir = false;
            }
        }
        else {
            m_iFade--;

            if (m_iFade < 1) {
                m_bFadeDir = true;
            }
        }

        m_TIMER_fade = 0;
    }
}

void cWindTrap::think_animation()
{
    cAbstractStructure::think_animation();
    think_fade(); // windtrap specific blue fading
    cAbstractStructure::think_flag_new();
}

void cWindTrap::think_guard()
{

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */

int cWindTrap::getPowerOut() const
{
    float percentage = ((float) getHitPoints() / (float) game.structureInfos[getType()].hp);
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