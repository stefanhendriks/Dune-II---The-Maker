#ifndef D2TM_CSETUPSKIRMISHGAMESTATE_H
#define D2TM_CSETUPSKIRMISHGAMESTATE_H

class cGame;

#include <drawers/cTextDrawer.h>
#include "cGameState.h"

struct s_SkirmishPlayer {
    bool bPlaying;
    int startingUnits;
    int iCredits;
    int iHouse;
    int team;
};

class cSetupSkirmishGameState : public cGameState {

public:
    cSetupSkirmishGameState(cGame& theGame);
    ~cSetupSkirmishGameState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;

    eGameStateType getType() override;

private:
    cTextDrawer textDrawer;
    s_SkirmishPlayer skirmishPlayer[MAX_PLAYERS];
    cMouse * mouse;

    int spawnWorms;
    bool spawnBlooms;
    bool detonateBlooms;

    // Colors
    int colorDarkishBackground;
    int colorDarkishBorder;
    int colorWhite;
    int colorRed;
    int colorYellow;
    int colorDarkerYellow;
    int colorDisabled;

    // Some basic layout coordinates
    int topBarHeight;
    int previewMapHeight;
    int previewMapWidth;
    int widthOfSomething;

    // screen
    int screen_x, screen_y;

    // Functions
    void prepareSkirmishGameToPlayAndTransitionToCombatState(int iSkirmishMap);

    void onMouseLeftButtonClicked(const s_MouseEvent &event);

    void drawMapList(cRectangle &mapList, cRectangle &mapListFrame) const;

    void drawWorms(cRectangle &wormsRect) const;

    void drawBlooms(cRectangle &bloomsRect) const;

    void drawDetonateBlooms(cRectangle &detonateBloomsRect) const;

    void drawPreviewMapAndMore(cRectangle &previewMapRect, int &iStartingPoints, int &iSkirmishMap) const;

    bool drawStartPoints(int iStartingPoints, cRectangle &startPoints) const;
};

#endif //D2TM_CSETUPSKIRMISHGAMESTATE_H
