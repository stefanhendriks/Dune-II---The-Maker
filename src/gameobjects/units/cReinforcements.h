/**
 * @file cReinforcements.h
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

#include <vector>

class cGameObjectContext;
class cInfoContext;
class cGameInterface;
struct sGameServices;

class cReinforcement {
public:
    cReinforcement();
    explicit cReinforcement(int delayInSeconds, int unitType, int playerId, int targetCell, bool repeat);

    void substractSecondIfApplicable();

    [[nodiscard]] bool isReady() const;
    [[nodiscard]] bool isValid() const;
    [[nodiscard]] bool canBeRemoved() const;
    void repeatOrMarkForDeletion();
    void execute(cGameObjectContext* objects, cInfoContext* infos, cGameInterface* iface) const;

private:
    int m_delayInSeconds = -1;
    int m_unitType = -1;      // what unit to deliver?
    int m_playerId = -1;        // for which player?
    int m_cell = -1;          // where should it be delivered?
    bool m_repeat;           // repeat executing this reinforcement?
    bool m_removeMe;            // if set, it will be removed
    int m_originalDelay = -1; // to remember delay for repeating
};

class cReinforcements {
public:
    explicit cReinforcements();

    void init();
    void serviceInit(sGameServices* services);

    void addReinforcement(int playerId, int unitType, int targetCell, int delayInSeconds, bool repeat);

    void thinkSlow();

private:
    void substractSecondFromValidReinforcements();

    cGameObjectContext* m_objects = nullptr;
    cInfoContext* m_infos = nullptr;
    cGameInterface* m_interface = nullptr;
    std::vector<cReinforcement> reinforcements;
};


void REINFORCE(cGameObjectContext* objects, cInfoContext* infos, cGameInterface* iface, int iPlr, int iTpe, int iCll, int iStart);
void REINFORCE(cGameObjectContext* objects, cInfoContext* infos, cGameInterface* iface, const cReinforcement &reinforcement);

/**
 * Allows overriding reinforcement flag, ie used when a unit is reinforced by construction or other way, rather
 * than a 'real' reinforcement.
 */
void REINFORCE(cGameObjectContext* objects, cInfoContext* infos, cGameInterface* iface, int iPlr, int iTpe, int iCll, int iStart, bool isReinforcement);
