#ifndef D2TM_CSELECTYOURNEXTCONQUESTSTATE_H
#define D2TM_CSELECTYOURNEXTCONQUESTSTATE_H

class cGame;

#include "cGameState.h"

enum eRegionState {
    REGSTATE_INIT,                   // Initialization
    REGSTATE_INTRODUCTION,           // The very beginning, ie "3 houses have come to Dune" and ends with drawing the dune regions
    REGSTATE_CONQUER_REGIONS,        // here we change state of regions, ie some regions get conquered / change sides
    REGSTATE_SELECT_NEXT_CONQUEST,   // here the user may select the next area to attack
    REGSTATE_FADEOUT                 // the player clicked on a region and we fade out
};

// these are the 3 iconic 'scenes' the campaign overview shows, and ends with "Select your next conquest
enum eRegionSceneState {
    SCENE_INIT,                         // load data first
    SCENE_THREE_HOUSES_COME_FOR_DUNE,   // ...
    SCENE_TO_TAKE_CONTROL_OF_THE_LAND,  // ...
    SCENE_THAT_HAS_BECOME_DIVIDED,      // ...
    SCENE_SELECT_YOUR_NEXT_CONQUEST     // this transitions to the next regionState "CONQUER_REGIONS"
};

class cRegion {
public:
    int iHouse;         // who (which house) owns this?
    int x, y;           // x and y position of the region
    int iAlpha;         // Alpha (0 = not visible, > 0 goes up to 255)
    int iTile;          // tile refering to gfxworld.dat
    bool bSelectable;   // selectable for attacking (default = false)
    BITMAP *bmp;        // the original bmp
    BITMAP *bmpHighBit; // the bmp (created! and thus must be destroyed) on same bitdepth as bmp_screen
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

    void REGION_SETUP_LOST_MISSION();
    void REGION_SETUP_NEXT_MISSION(int iMission, int iHouse);
    void INSTALL_WORLD();

    void conquerRegions();

    void destroy();

    void calculateOffset();

private:

    eRegionState state;

    eRegionSceneState regionSceneState;
    int iRegionSceneAlpha;          // scene alpha

    int iRegionConquer[MAX_REGIONS];     // INDEX = REGION NR , > -1 means conquered..
    int iRegionHouse[MAX_REGIONS];       // I guess... this is the house that the conquered region will become

    char cRegionText[MAX_REGIONS][255]; // text for this

    void REGION_DRAW(cRegion &regionPiece);
    int REGION_OVER();
    void REGION_NEW(int x, int y, int iAlpha, int iHouse, int iTile);

    void drawStateSelectYourNextConquest();

    void drawStateConquerRegions();

    void drawStateIntroduction();

    void drawLogoInFourCorners(int iHouse) const;

    void transitionToNextRegionSceneState(eRegionSceneState newSceneState);

    void drawRegion(cRegion &regionPiece) const;

    int offsetX;
    int offsetY;

    int selectNextConquestAlpha;

    BITMAP *regionClickMapBmp;  // this is the bmp that
};

#endif //D2TM_CSELECTYOURNEXTCONQUESTSTATE_H
