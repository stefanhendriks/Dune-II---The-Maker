#include "cGunTurret.h"

#include "d2tmc.h"
#include "definitions.h"
#include "gameobjects/particles/cParticle.h"
#include "gameobjects/projectiles/bullet.h"
#include "gameobjects/units/cUnit.h"
#include "player/cPlayer.h"

namespace {
    constexpr auto kTurretFacings = 8;
}

cGunTurret::cGunTurret() {
 // other variables (class specific)
 iHeadFacing=0;        // (for turrets only) what is this structure facing at?
 iShouldHeadFacing=0;  // where should we look face at?
 iTargetID=-1;           // target id

 TIMER_fire=0;
 TIMER_turn=0;
 TIMER_guard=0;         // timed 'area scanning'
}


int cGunTurret::getType() const {
	return TURRET;
}

void cGunTurret::thinkFast() {
    bool lowPower = !getPlayer()->bEnoughPower();
    if (lowPower && game.isTurretsDownOnLowPower()) {
        // don't do anything when low power and this flag is set
        return;
    }

    // turning & shooting
    if (iTargetID > -1) {
        think_attack();
    }

    if (!isFacingTarget()) {
        think_turning();
    }

    // last but not least, think like our abstraction
    cAbstractStructure::thinkFast();
}

// Specific Animation thinking (flag animation OR its deploy animation)
void cGunTurret::think_animation() {
	cAbstractStructure::think_animation();
}

void cGunTurret::think_attack() {
    cUnit &unitTarget = unit[iTargetID];
    if (unitTarget.isValid() && !unitTarget.isDead()) {
        int iCellX = map.getCellX(getCell());
        int iCellY = map.getCellY(getCell());

        int unitCell = unitTarget.getCell();

        int iTargetX = map.getCellX(unitCell);
        int iTargetY = map.getCellY(unitCell);

        int d = fDegrees(iCellX, iCellY, iTargetX, iTargetY);
        int facingAngle = faceAngle(d, kTurretFacings); // get the angle

        // set facing
        setShouldHeadFacing(facingAngle);

        if (isFacingTarget()) {
            think_fire();
        }
    } else {
        iTargetID = -1;
    }
}

bool cGunTurret::isFacingTarget() const {
    return iShouldHeadFacing == iHeadFacing;
}

void cGunTurret::think_turning() {
    TIMER_turn++;

    int iSlowDown = 125; // for 8 facings , TODO: make it configurable (turnSpeed)
    if (kTurretFacings > 8) {
        iSlowDown = 65; // for 16 facings , TODO: make it configurable (turnSpeed)
    }

    if (TIMER_turn > iSlowDown) {
        TIMER_turn = 0;
        int facingsZeroBased = kTurretFacings - 1;

        // incrementing means going clockwise (to the 'right' so to speak)
        int increment = 1;

        // check difference when we go over 'left' (counter-clockwise)
        int counterClockwiseSteps = (iHeadFacing + kTurretFacings) - iShouldHeadFacing;
        if (counterClockwiseSteps > facingsZeroBased) counterClockwiseSteps -= kTurretFacings;

        // and difference going clockwise
        int clockwiseSteps = abs(counterClockwiseSteps - kTurretFacings);

        // it does not matter which way we go, so pick random direction
        if (clockwiseSteps == counterClockwiseSteps) increment = -1 + (rnd(2));

        // counterClockwise is longer than clockwise, so go clockwise (+1)
        if (counterClockwiseSteps > clockwiseSteps) increment = 1;
        // clockwise is taking longer than counter-clockwise so go counter-clockwise
        if (clockwiseSteps > counterClockwiseSteps) increment = -1;

        iHeadFacing += increment;

        // deal with going around (index < 0 becomes max, and vice versa)
        if (iHeadFacing < 0) {
            iHeadFacing = facingsZeroBased;
        }
        if (iHeadFacing > facingsZeroBased) {
            iHeadFacing = 0;
        }
    } // turning
}

void cGunTurret::think_fire() {
    bool lowPower = !getPlayer()->bEnoughPower();

    cUnit &unitTarget = unit[iTargetID];
    if (unitTarget.isValid() && !unitTarget.isDead()) {
        TIMER_fire++;

        int iDistance = map.distance(getCell(), unitTarget.getCell());

        if (iDistance > getSight()) {
            iTargetID = -1;
            // went out of sight, unfortunately
            return;
        }

        if (lowPower) {
            if (unitTarget.isAirbornUnit()) {
                // no longer able, forget it
                iTargetID = -1;
                return;
            }
        }

        // TODO: move '3' to property (distanceForSecondaryFire?)
        int distanceForSecondaryFire = 3;
        int iSlowDown = getStructureInfo().fireRate;
        if (TIMER_fire > iSlowDown) {
            int iTargetCell = unitTarget.getCell();

            int bulletType = BULLET_TURRET; // short range bullet

            if (unitTarget.isAirbornUnit()) {
                bulletType = ROCKET_RTURRET;
            } else {
                if (!lowPower && getType() == RTURRET && iDistance > distanceForSecondaryFire) {
                    bulletType = ROCKET_RTURRET; // long-range bullet,
                } else {
                    int half = 16;
                    int iShootX = pos_x() + half;
                    int iShootY = pos_y() + half;
                    int bmp_head = convertAngleToDrawIndex(iHeadFacing);
                    cParticle::create(iShootX, iShootY, D2TM_PARTICLE_TANKSHOOT, -1, bmp_head);
                }
            }

            int iBull = create_bullet(bulletType, getCell(), iTargetCell, -1, id);

            // only rockets are homing
            if (unitTarget.isAirbornUnit()) {
                if (iBull > -1 && bulletType == ROCKET_RTURRET) {
                    // it is a homing missile!
                    bullet[iBull].iHoming = iTargetID;
                    // TODO: property for homing?
                    bullet[iBull].TIMER_homing = 200;
                }
            }

            TIMER_fire = 0;
        }
    }
}

void cGunTurret::think_guard() {
    bool lowPower = !getPlayer()->bEnoughPower();
    if (lowPower && game.isTurretsDownOnLowPower()) {
        // don't do anything when low power and this flag is set
        return;
    }

    TIMER_guard++;

    if (TIMER_guard > 10) {
        TIMER_guard=0-rnd(20);

        int c = getCell();
        int iCellX = map.getCellX(c);
        int iCellY = map.getCellY(c);

        int iDistance=9999; // closest distance

        int iAir=-1;        // aircraft (prioritized!)
        int iWorm=-1;       // worm lowest priority
        int iDanger=-1;     // danger id (unit to attack)

        int prevTarget = iTargetID;

        iTargetID=-1;       // no target

        int distanceForAttacking = getSight();
        if (lowPower && getType() == RTURRET) {
            distanceForAttacking = sStructureInfo[TURRET].sight; // HACK HACK: way to reduce distance for rturret on low power
        }

        // scan area for units
        for (int i = 0; i < MAX_UNITS; i++) {
            // is valid
            cUnit &cUnit = unit[i];
            if (!cUnit.isValid()) continue;
            if (cUnit.iPlayer == getOwner()) continue; // skip own units
            if (cUnit.getPlayer()->isSameTeamAs(getPlayer())) continue; // skip allied units
            if (!map.isVisible(cUnit.getCell(), getPlayer())) continue; // skip not visible

            if (!canAttackAirUnits()) {
                if (cUnit.isAirbornUnit()) {
                    continue; // it was airborn, and turrets which can't attack air units cannot hit this, so skip
                }
            } else {
                // we can attack air units, but we are low on power, hence we don't attack them
                if (lowPower && game.isRocketTurretsDownOnLowPower()) {
                    // do not aim for air units when low power
                    if (cUnit.isAirbornUnit()) {
                        continue; // it was airborn, and this turret is down on power, so don't fire air units
                    }
                }
            }

            int c1 = cUnit.getCell();
            int distance = ABS_length(iCellX, iCellY, map.getCellX(c1), map.getCellY(c1));

            if (distance <= distanceForAttacking) {
                if (cUnit.isAttackableAirUnit()) {
                    iAir = i;
                } else if (cUnit.isSandworm()) {
                    iWorm = i;
                } else if (distance < iDistance) {
                    // ATTACK
                    iDistance = distance;
                    iDanger = i;
                }
            }
        }

        // set target
        if (iAir > -1) {
            iTargetID = iAir;
        } else if (iDanger > -1) {
            iTargetID = iDanger;
        } else if (iWorm > -1) {
            iTargetID = iWorm;
        }

        // discovered a new target
        if (iTargetID > -1 && iTargetID != prevTarget) {
            cUnit &unitToAttack = unit[iTargetID];
            if (unitToAttack.isValid()) {
                s_GameEvent event{
                        .eventType = eGameEventType::GAME_EVENT_DISCOVERED,
                        .entityType = eBuildType::UNIT,
                        .entityID = unitToAttack.iID,
                        .player = getPlayer(),
                        .entitySpecificType = unitToAttack.getType(),
                        .atCell = unitToAttack.getCell()
                };

                game.onNotifyGameEvent(event);
            }
        }
    }
}

int cGunTurret::getFacingAngles() {
    return kTurretFacings;
}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
