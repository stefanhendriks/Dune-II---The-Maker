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
    // other variables (class specific)
    iFade = RNG::rnd(63);
    bFadeDir = true;

    // Timers
    TIMER_fade = 0;
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
    TIMER_fade++;

    int iTime; // the speed of fading

    // depending on fade direction, fade in slower/faster
    if (bFadeDir) { // go to blue
        iTime = 3;
    }
    else {   // go to black
        iTime = 4;
    }

    // time passed, we may change fade color
    if (TIMER_fade > iTime) {
        if (bFadeDir) {
            iFade++;

            if (iFade > 254) {
                bFadeDir = false;
            }
        }
        else {
            iFade--;

            if (iFade < 1) {
                bFadeDir = true;
            }
        }

        TIMER_fade = 0;
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