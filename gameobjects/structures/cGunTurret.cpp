#include "../../include/d2tmh.h"

namespace {
    constexpr auto kTurretFacings = 8;
}

// Constructor
cGunTurret::cGunTurret() {
 // other variables (class specific)
 iHeadFacing=FACE_UP;        // (for turrets only) what is this structure facing at?
 iShouldHeadFacing=FACE_UP;  // where should we look face at?
 iTargetID=-1;           // target id

 TIMER_fire=0;
 TIMER_turn=0;
 TIMER_guard=0;         // timed 'area scanning'
}


int cGunTurret::getType() const {
	return TURRET;
}


cGunTurret::~cGunTurret()
{

}

void cGunTurret::thinkFast() {
	if (!getPlayer()->bEnoughPower()) {
        return;
	}

    // turning & shooting
    if (iTargetID > -1) {
        think_attack();
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
        int f = face_angle(d); // get the angle

        // set facing
        iShouldHeadFacing = f;

        // if attacking an airunit, then don't care about facing (we use homing missiles)
        bool isFacingTarget = iShouldHeadFacing == iHeadFacing;

        if (isFacingTarget) {
            TIMER_fire++;

            int iDistance = map.distance(getCell(), unitCell);

            if (iDistance > getSight()) {
                iTargetID = -1;
                // went out of sight, unfortunately
                return;
            }

            // TODO: Move to property 'fireRate' ?
            int iSlowDown = 200; // fire-rate of turret
            if (TIMER_fire > iSlowDown) {
                int iTargetCell = unitTarget.getCell();

                int bulletType = BULLET_TURRET; // short range bullet
                if (unitTarget.isAirbornUnit()) {
                    bulletType = ROCKET_RTURRET;
                } else {
                    // TODO: move '3' to property (distanceForSecondaryFire?)
                    if (getType() == RTURRET && iDistance > 3) {
                        bulletType = ROCKET_RTURRET; // long-range bullet,
                    } else {
                        int half = 16;
                        int iShootX = pos_x() + half;
                        int iShootY = pos_y() + half;
                        int bmp_head = convert_angle(iHeadFacing);
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
        } else { // not yet facing target
            think_turning();
        }
    } else {
        iTargetID = -1;
    }
}

void cGunTurret::think_turning() {
    TIMER_turn++;

    int iSlowDown = 125; // for 8 facings , TODO: make it configurable (turnSpeed)

    if (TIMER_turn > iSlowDown) {
        TIMER_turn = 0;

        int d = 1;

        int toleft = (iHeadFacing + kTurretFacings) - iShouldHeadFacing;
        if (toleft > 7) toleft -= kTurretFacings;

        int toright = abs(toleft - kTurretFacings);

        if (toright == toleft) d = -1 + (rnd(2));
        if (toleft > toright) d = 1;
        if (toright > toleft) d = -1;

        iHeadFacing += d;

        if (iHeadFacing < 0)
            iHeadFacing = (kTurretFacings - 1);

        if (iHeadFacing > (kTurretFacings - 1))
            iHeadFacing = 0;
    } // turning
}

void cGunTurret::think_guard() {
    if (!getPlayer()->bEnoughPower()) {
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
            }

            int c1 = cUnit.getCell();
            int distance = ABS_length(iCellX, iCellY, map.getCellX(c1), map.getCellY(c1));

            if (distance <= getSight()) {
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

/*  STRUCTURE SPECIFIC FUNCTIONS  */
