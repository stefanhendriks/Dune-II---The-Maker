/**
 * @file cPlayerBrain.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include "observers/cScenarioObserver.h"
#include "sAiParameters.h"

#include <map>

class cPlayer;
class cUnit;
class cAbstractStructure;
class cGameObjectContext;
class cInfoContext;
class cGameSettings;
class cEventEmitter;
struct sGameServices;

namespace brains {

struct s_buildOrder;

class cPlayerBrain : public cScenarioObserver {

public:
    explicit cPlayerBrain(cPlayer *player);

    virtual ~cPlayerBrain();

    virtual void serviceInit(sGameServices* services);

    /**
     * called, every 100 ms
     */
    virtual void think() = 0;

    /**
     * called, every 5 ms
     */
    virtual void thinkFast() = 0;

    virtual void addBuildOrder(s_buildOrder order) = 0;

    const s_AiParameters &getAiParameters() const {
        return m_aiParameters;
    }

protected:
    cPlayer *player;
    cGameObjectContext *m_objects = nullptr;
    cInfoContext *m_info = nullptr;
    cGameSettings *m_settings = nullptr;
    cEventEmitter *m_eventEmitter = nullptr;
    s_AiParameters m_aiParameters;

    /**
     * Sends a damaged ground vehicle to a Repair Facility once its health drops below the recall threshold
     * (lower when friendly units are fighting near it). Returns true when the unit was sent.
     */
    bool recallForRepairWhenBadlyDamaged(cUnit *unit);

    /**
     * When the attacked structure calls for it (see the repairEject* AI parameters), units in a Repair Facility
     * that can hit the attacker are ejected and attack it.
     */
    void ejectRepairingUnitsToFight(int attackedStructureType, cUnit *attacker);

    /**
     * Sends ejected units back to repair once they have been idle long enough after their fight. Call every think().
     */
    void thinkUnitsEjectedFromRepair();

private:
    // unit id of a unit ejected from a Repair Facility -> brain ticks it has been idle since its fight
    std::map<int, int> m_idleTicksOfUnitsEjectedFromRepair;

    bool hasFriendlyUnitFightingNear(cUnit *unit) const;


};
}
