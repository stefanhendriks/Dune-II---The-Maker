#include "cRefinery.h"

#include "game/cGame.h"
#include "include/d2tmc.h"
#include "definitions.h"
#include "data/gfxaudio.h"
#include "gameobjects/units/cUnit.h"
#include "player/cPlayer.h"
#include "utils/cSoundPlayer.h"

cRefinery::cRefinery()
{
    // other variables (class specific)
    shouldAnimateWhenUnitHeadsTowardsStructure = true;
}

int cRefinery::getType() const
{
    return REFINERY;
}

void cRefinery::thinkFast()
{
    if (hasUnitWithin()) { // unit has entered structure
        think_unit_occupation();
    }

    // think like base class
    cAbstractStructure::thinkFast();
}

void cRefinery::think_unit_occupation()
{
    int iUnitID = getUnitIdWithin();
    cUnit &cUnit = game.getUnit(iUnitID);

    // the unit id is filled in, that means the unit is IN this structure
    // the TIMER_harvest of the unit will be used to dump the harvest in the
    // refinery
    cUnit.harvestTimer.increment();

    cPlayer *pPlayer = getPlayer();
    cPlayerDifficultySettings *difficultySettings = pPlayer->getDifficultySettings();

    if (cUnit.harvestTimer.get() < difficultySettings->getDumpSpeed(10)) return;

    cUnit.harvestTimer.zero();

    // dump credits
    if (cUnit.canUnload()) {
        int iAmount = 5;

        // cap at max
        s_UnitInfo &unitType = game.m_infoContext.getUnitInfo(cUnit.iType);

        if (cUnit.iCredits > unitType.credit_capacity) {
            cUnit.iCredits = unitType.credit_capacity;
            // this fixes the upper bound (so no unit can cheat !?)
        }

        // can substract credits? if not, choose remaining
        if ((cUnit.iCredits - iAmount) < 0) {
            iAmount = cUnit.iCredits;
        }

        pPlayer->dumpCredits(iAmount);
        cUnit.iCredits -= iAmount;
        return;
    }

    // Dumping credits is finished
    unitLeavesStructure();

    setAnimating(false);

    // let player know...
    if (pPlayer->isHuman()) {
        game.playVoice(SOUND_VOICE_02_ATR, pPlayer->getId());
    }

    // perhaps we can find a carryall to help us out
    int iHarvestCell = cUnit::findHarvestSpot(iUnitID);

    if (iHarvestCell > -1) {
        int iCarry = cUnit::carryallTransfer(iUnitID, iHarvestCell);

        if (iCarry > -1) {
            cUnit.movewaitTimer.reset(500);
            cUnit.thinkwaitTimer.reset(500);
        }
    }
}

void cRefinery::think_harvester_deploy()
{
    if (hasUnitWithin()) {
        iFrame = 7;
        return;
    }
    else {
        if (!isAnimating()) {
            iFrame = 0;
        }
    }

    if (!isAnimating()) return; // do nothing when not animating

    // harvester stuff
    if (iFrame < 0) {
        iFrame = 0;
    }

    TIMER_flag++;

    if (TIMER_flag > 40) {
        TIMER_flag = 0;

        iFrame++;
        if (iFrame > 6) {
            iFrame = 0;
        }
    }
}

void cRefinery::think_animation()
{
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
    think_harvester_deploy();
}

void cRefinery::think_guard()
{

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
int cRefinery::getSpiceSiloCapacity()
{
    float percentage = ((float) getHitPoints() / (float) game.m_infoContext.getStructureInfo(getType()).hp);
    return 1000 * percentage;
}
