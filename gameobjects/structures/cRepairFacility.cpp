#include "../../include/d2tmh.h"


// Constructor
cRepairFacility::cRepairFacility() {

    // other variables (class specific)
    TIMER_repairunit = 0;
}

int cRepairFacility::getType() const {
    return REPAIR;
}

cRepairFacility::~cRepairFacility() {

}


void cRepairFacility::think() {

    // Repair unit here (if any)
    if (hasUnitWithin()) {
        think_repairUnit();
    }

    // think like base class
    cAbstractStructure::think();

}

void cRepairFacility::think_repairUnit() {// must repair...
    int iUnitID = getUnitIdWithin();
    cUnit &unitToRepair = unit[iUnitID];
    int maxHpForUnitType = sUnitInfo[unitToRepair.iType].hp;

    if (unitToRepair.iTempHitPoints < maxHpForUnitType) {
        TIMER_repairunit++;

        // TODO: move to structure info? (or unit info?)
        int REPAIR_SPEED = 15;

        if (TIMER_repairunit < REPAIR_SPEED) return;

        TIMER_repairunit = 0;

        cPlayer *pPlayer = getPlayer();
        int REPAIR_COST_PER_TICK = 1;

        if (pPlayer->hasEnoughCreditsFor(REPAIR_COST_PER_TICK)) {
            // TODO: Move repair rate per tick (how much hp per tick will be increased?) into unit info?
            int REPAIR_RATE_HP_PER_TICK = 3; // ie, unit with 300 HP, costs 100 to repair

            unitToRepair.iTempHitPoints += REPAIR_RATE_HP_PER_TICK;

            // TODO: Move to unit info
            // Cost per tick to repair unit for REPAIR_RATE_HP_PER_TICK amount
            pPlayer->substractCredits(REPAIR_COST_PER_TICK);
        }
    } else if (unitToRepair.iTempHitPoints >= maxHpForUnitType) {
        unitToRepair.setMaxHitPoints();

        // dump unit, get rid of it
        unitLeavesStructure();

        // show door open/close thing
        setAnimating(true);
    }
}

void cRepairFacility::think_animation() {
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
    think_animation_unitDeploy();
}

void cRepairFacility::think_animation_unitDeploy() {
    if (!isAnimating()) return; // do nothing when not animating
    TIMER_flag++;
    int waitLimit = animDir == eAnimationDirection::ANIM_SPAWN_UNIT ? 120 : 30;
    if (TIMER_flag > waitLimit) {

        switch (animDir) {
            case eAnimationDirection::ANIM_OPEN:
                iFrame++;
                if (iFrame > 6) {
                    animDir = eAnimationDirection::ANIM_SPAWN_UNIT;
                }
                break;
            case eAnimationDirection::ANIM_SPAWN_UNIT:
                animDir = eAnimationDirection::ANIM_CLOSE; // and close again
                break;
            case eAnimationDirection::ANIM_CLOSE:
                iFrame--;
                if (iFrame < 1) {
                    // no longer animating
                    animDir = eAnimationDirection::ANIM_NONE;
                    setAnimating(false);
                }
                break;
            default:
                break;
        }

        TIMER_flag = 0;
    }

}

void cRepairFacility::think_guard() {

}

void cRepairFacility::startAnimating() {
    animDir = isAnimating() ? eAnimationDirection::ANIM_OPEN : eAnimationDirection::ANIM_NONE;
}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
