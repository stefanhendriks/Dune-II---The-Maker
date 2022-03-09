#pragma once

struct sReinforcement
{
    int iSeconds;       // FPS based, every second this decreases (0 = deliver)
    int iUnitType;      // what unit?
    int iPlayer;         // to who?
    int iCell;          // Where to?
};

#define MAX_REINFORCEMENTS 50    // max of 50 reinforcements

class cReinforcements
{
public:
    cReinforcements(){}
    ~cReinforcements(){}
    void INIT_REINFORCEMENT();
    int NEXT_REINFORCEMENT();
    void SET_REINFORCEMENT(int iCll, int iPlyr, int iTime, int iUType, int iPlyrGetHouse);
    void spendASecond();
    bool hasReinforcement();
    void getReinforcementAndDestroy(int &iCll, int &iPlyr, int &iTime, int &iUType);
private:
    sReinforcement reinforcements[MAX_REINFORCEMENTS];
};