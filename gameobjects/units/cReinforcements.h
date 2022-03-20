#pragma once

struct sReinforcement {
    int iSeconds = -1;
    int iUnitType = -1;      // what unit to deliver?
    int iPlayer = -1;        // for which player?
    int iCell = -1;          // where should it be delivered?
};

#define MAX_REINFORCEMENTS 50    // max of 50 reinforcements

class cReinforcements {
public:
    explicit cReinforcements();

    ~cReinforcements() {}

    void init();

    void SET_REINFORCEMENT(int iCll, int iPlyr, int iTime, int iUType, int iPlyrGetHouse);

    void thinkSlow();

//    bool hasReinforcement();

    sReinforcement getReinforcementAndDestroy();

private:
    void substractSecondFromValidReinforcements();

    int findNextUnusedId();

    sReinforcement reinforcements[MAX_REINFORCEMENTS];
};


void REINFORCE(int iPlr, int iTpe, int iCll, int iStart);
void REINFORCE(const sReinforcement &reinforcement);

/**
 * Allows overriding reinforcement flag, ie used when a unit is reinforced by construction or other way, rather
 * than a 'real' reinforcement.
 * @param iPlr
 * @param iTpe
 * @param iCll
 * @param iStart
 * @param isReinforcement
 */
void REINFORCE(int iPlr, int iTpe, int iCll, int iStart, bool isReinforcement);
