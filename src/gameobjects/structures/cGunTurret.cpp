#include "cGunTurret.h"

#include "game/cGame.h"
#include "include/d2tmc.h"
#include "map/cMap.h"
#include "definitions.h"
#include "gameobjects/particles/cParticle.h"
#include "gameobjects/projectiles/bullet.h"
#include "gameobjects/units/cUnit.h"
#include "utils/d2tm_math.h"
#include "player/cPlayer.h"
#include "utils/RNG.hpp"

namespace {
constexpr auto kTurretFacings = 8;
}

cGunTurret::cGunTurret()
{
// other variables (class specific)
    m_iHeadFacing=0;        // (for turrets only) what is this structure facing at?
    m_iShouldHeadFacing=0;  // where should we look face at?
    m_iTargetID=-1;           // target id

    m_TIMER_fire=0;
    m_TIMER_turn=0;
    m_TIMER_guard=0;         // timed 'area scanning'
}


int cGunTurret::getType() const
{
    return TURRET;
}

void cGunTurret::thinkFast()
{
    bool lowPower = !getPlayer()->bEnoughPower();
    if (lowPower && game.isTurretsDownOnLowPower()) {
        // don't do anything when low power and this flag is set
        return;
    }

    // turning & shooting
    if (m_iTargetID > -1) {
        think_attack();
    }

    if (!isFacingTarget()) {
        think_turning();
    }

    // last but not least, think like our abstraction
    cAbstractStructure::thinkFast();
}

// Specific Animation thinking (flag animation OR its deploy animation)
void cGunTurret::think_animation()
{
    cAbstractStructure::think_animation();
}

void cGunTurret::think_attack()
{
    cUnit &unitTarget = game.getUnit(m_iTargetID);
    if (unitTarget.isValid() && !unitTarget.isDead()) {
        int iCellX = game.m_map.getCellX(getCell());
        int iCellY = game.m_map.getCellY(getCell());

        int unitCell = unitTarget.getCell();

        int iTargetX = game.m_map.getCellX(unitCell);
        int iTargetY = game.m_map.getCellY(unitCell);

        int d = fDegrees(iCellX, iCellY, iTargetX, iTargetY);
        int facingAngle = faceAngle(d, kTurretFacings); // get the angle

        // set facing
        setShouldHeadFacing(facingAngle);

        if (isFacingTarget()) {
            think_fire();
        }
    }
    else {
        m_iTargetID = -1;
    }
}

bool cGunTurret::isFacingTarget() const
{
    return m_iShouldHeadFacing == m_iHeadFacing;
}

void cGunTurret::think_turning()
{
    m_TIMER_turn++;

    int iSlowDown = 125; // for 8 facings , TODO: make it configurable (turnSpeed)
    if (kTurretFacings > 8) {
        iSlowDown = 65; // for 16 facings , TODO: make it configurable (turnSpeed)
    }

    if (m_TIMER_turn > iSlowDown) {
        m_TIMER_turn = 0;
        int facingsZeroBased = kTurretFacings - 1;

        // incrementing means going clockwise (to the 'right' so to speak)
        int increment = 1;

        // check difference when we go over 'left' (counter-clockwise)
        int counterClockwiseSteps = (m_iHeadFacing + kTurretFacings) - m_iShouldHeadFacing;
        if (counterClockwiseSteps > facingsZeroBased) counterClockwiseSteps -= kTurretFacings;

        // and difference going clockwise
        int clockwiseSteps = abs(counterClockwiseSteps - kTurretFacings);

        // it does not matter which way we go, so pick random direction
        if (clockwiseSteps == counterClockwiseSteps) increment = -1 + (RNG::rnd(2));

        // counterClockwise is longer than clockwise, so go clockwise (+1)
        if (counterClockwiseSteps > clockwiseSteps) increment = 1;
        // clockwise is taking longer than counter-clockwise so go counter-clockwise
        if (clockwiseSteps > counterClockwiseSteps) increment = -1;

        m_iHeadFacing += increment;

        // deal with going around (index < 0 becomes max, and vice versa)
        if (m_iHeadFacing < 0) {
            m_iHeadFacing = facingsZeroBased;
        }
        if (m_iHeadFacing > facingsZeroBased) {
            m_iHeadFacing = 0;
        }
    } // turning
}

void cGunTurret::think_fire()
{
    bool lowPower = !getPlayer()->bEnoughPower();

    cUnit &unitTarget = game.getUnit(m_iTargetID);
    if (unitTarget.isValid() && !unitTarget.isDead()) {
        m_TIMER_fire++;

        int iDistance = game.m_map.distance(getCell(), unitTarget.getCell());

        if (iDistance > getSight()) {
            m_iTargetID = -1;
            // went out of sight, unfortunately
            return;
        }

        if (lowPower) {
            if (unitTarget.isAirbornUnit()) {
                // no longer able, forget it
                m_iTargetID = -1;
                return;
            }
        }

        // TODO: move '3' to property (distanceForSecondaryFire?)
        int distanceForSecondaryFire = 3;
        int iSlowDown = getStructureInfo().fireRate;
        if (m_TIMER_fire > iSlowDown) {
            int iTargetCell = unitTarget.getCell();

            int bulletType = BULLET_TURRET; // short range bullet

            if (unitTarget.isAirbornUnit()) {
                bulletType = ROCKET_RTURRET;
            }
            else {
                if (!lowPower && getType() == RTURRET && iDistance > distanceForSecondaryFire) {
                    bulletType = ROCKET_RTURRET; // long-range bullet,
                }
                else {
                    int half = 16;
                    int iShootX = pos_x() + half;
                    int iShootY = pos_y() + half;
                    int bmp_head = convertAngleToDrawIndex(m_iHeadFacing);
                    cParticle::create(iShootX, iShootY, D2TM_PARTICLE_TANKSHOOT, -1, bmp_head);
                }
            }

            int iBull = createBullet(bulletType, getCell(), iTargetCell, -1, id);

            // only rockets are homing
            if (unitTarget.isAirbornUnit()) {
                if (iBull > -1 && bulletType == ROCKET_RTURRET) {
                    // it is a homing missile!
                    game.g_Bullets[iBull].iHoming = m_iTargetID;
                    // TODO: property for homing?
                    game.g_Bullets[iBull].TIMER_homing = 200;
                }
            }

            m_TIMER_fire = 0;
        }
    }
}

void cGunTurret::think_guard()
{
    bool lowPower = !getPlayer()->bEnoughPower();
    if (lowPower && game.isTurretsDownOnLowPower()) {
        // don't do anything when low power and this flag is set
        return;
    }

    m_TIMER_guard++;

    if (m_TIMER_guard > 10) {
        m_TIMER_guard=0-RNG::rnd(20);

        int c = getCell();
        int iCellX = game.m_map.getCellX(c);
        int iCellY = game.m_map.getCellY(c);

        int iDistance=9999; // closest distance

        int iAir=-1;        // aircraft (prioritized!)
        int iWorm=-1;       // worm lowest priority
        int iDanger=-1;     // danger id (unit to attack)

        int prevTarget = m_iTargetID;

        m_iTargetID=-1;       // no target

        int distanceForAttacking = getSight();
        if (lowPower && getType() == RTURRET) {
            distanceForAttacking = game.structureInfos[TURRET].sight; // HACK HACK: way to reduce distance for rturret on low power
        }

        // scan area for units
        for (int i = 0; i < game.m_Units.size(); i++) {
            // is valid
            cUnit &cUnit = game.getUnit(i);
            if (!cUnit.isValid()) continue;
            if (cUnit.iPlayer == getOwner()) continue; // skip own units
            if (cUnit.getPlayer()->isSameTeamAs(getPlayer())) continue; // skip allied units
            if (!game.m_map.isVisible(cUnit.getCell(), getPlayer())) continue; // skip not visible

            if (!canAttackAirUnits()) {
                if (cUnit.isAirbornUnit()) {
                    continue; // it was airborn, and turrets which can't attack air units cannot hit this, so skip
                }
            }
            else {
                // we can attack air units, but we are low on power, hence we don't attack them
                if (lowPower && game.isRocketTurretsDownOnLowPower()) {
                    // do not aim for air units when low power
                    if (cUnit.isAirbornUnit()) {
                        continue; // it was airborn, and this turret is down on power, so don't fire air units
                    }
                }
            }

            int c1 = cUnit.getCell();
            int distance = ABS_length(iCellX, iCellY, game.m_map.getCellX(c1), game.m_map.getCellY(c1));

            if (distance <= distanceForAttacking) {
                if (cUnit.isAttackableAirUnit()) {
                    iAir = i;
                }
                else if (cUnit.isSandworm()) {
                    iWorm = i;
                }
                else if (distance < iDistance) {
                    // ATTACK
                    iDistance = distance;
                    iDanger = i;
                }
            }
        }

        // set target
        if (iAir > -1) {
            m_iTargetID = iAir;
        }
        else if (iDanger > -1) {
            m_iTargetID = iDanger;
        }
        else if (iWorm > -1) {
            m_iTargetID = iWorm;
        }

        // discovered a new target
        if (m_iTargetID > -1 && m_iTargetID != prevTarget) {
            cUnit &unitToAttack = game.getUnit(m_iTargetID);
            if (unitToAttack.isValid()) {
                s_GameEvent event {
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

int cGunTurret::getFacingAngles()
{
    return kTurretFacings;
}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
