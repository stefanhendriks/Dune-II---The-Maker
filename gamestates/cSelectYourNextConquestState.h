#ifndef D2TM_CSELECTYOURNEXTCONQUESTSTATE_H
#define D2TM_CSELECTYOURNEXTCONQUESTSTATE_H

class cGame;

#include "cGameState.h"

class cRegion
{
public:
    int iHouse;         // who owns this?
    int x, y;           // x and y position of the region
    int iAlpha;         // Alpha (0 = not visible, > 0 goes up to 255)
    int iTile;          // tile refering to gfxworld.dat
    bool bSelectable;   // selectable for attacking (default = false)
};

class cSelectYourNextConquestState : public cGameState {

public:
    cSelectYourNextConquestState(cGame& theGame);
    ~cSelectYourNextConquestState() override;

    void think() override;
    void draw() override;
    void interact() override;

    eGameStateType getType() override;

    // !? need to figure out what this does exactly
    void setRegionConquer(int iRegionIndex, int iRegion) {
        iRegionConquer[iRegionIndex] = iRegion;
    }

    void setRegionHouse(int iRegionIndex, int iHouse) {
        iRegionHouse[iRegionIndex] = iHouse;
    }

    void setRegionText(int iRegionIndex, char* txt) {
        sprintf(cRegionText[iRegionIndex], "%s", txt);
    }

    void REGION_SETUP(int iMission, int iHouse);
    void INSTALL_WORLD();
private:

    int iRegionState;           //
    int iRegionScene;           // scene
    int iRegionSceneAlpha;           // scene alpha
    int iRegionConquer[MAX_REGIONS];     // INDEX = REGION NR , > -1 means conquered..
    int iRegionHouse[MAX_REGIONS];
    char cRegionText[MAX_REGIONS][255]; // text for this

    void REGION_DRAW(cRegion &regionPiece);
    int REGION_OVER();
    void REGION_NEW(int x, int y, int iAlpha, int iHouse, int iTile);
};

#endif //D2TM_CSELECTYOURNEXTCONQUESTSTATE_H
