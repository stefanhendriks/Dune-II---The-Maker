#pragma once

class cReinforcement {
public:
    cReinforcement();
    explicit cReinforcement(int delayInSeconds, int unitType, int playerId, int targetCell, bool repeat);

    void substractSecondIfApplicable();

    [[nodiscard]] bool isReady() const;
    [[nodiscard]] bool isValid() const;
    void invalidateOrRepeat();
    void execute() const;

private:
    int m_delayInSeconds = -1;
    int m_unitType = -1;      // what unit to deliver?
    int m_playerId = -1;        // for which player?
    int m_cell = -1;          // where should it be delivered?
    bool m_repeat;           // repeat executing this reinforcement?
    int m_originalDelay = -1; // to remember delay for repeating
};

class cReinforcements {
public:
    explicit cReinforcements();

    ~cReinforcements() {}

    void init();

    void addReinforcement(int targetCell, int playerId, int delayInSeconds, int unitType, bool repeat);

    void thinkSlow();

    cReinforcement getReinforcementAndDestroy();

private:
    void substractSecondFromValidReinforcements();

    std::vector<cReinforcement> reinforcements;
};


void REINFORCE(int iPlr, int iTpe, int iCll, int iStart);
void REINFORCE(const cReinforcement &reinforcement);

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
