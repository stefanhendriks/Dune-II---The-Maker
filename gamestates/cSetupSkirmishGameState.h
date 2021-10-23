#ifndef D2TM_CSETUPSKIRMISHGAMESTATE_H
#define D2TM_CSETUPSKIRMISHGAMESTATE_H

class cGame;

#include <drawers/cTextDrawer.h>
#include "cGameState.h"

struct s_SkirmishPlayer {
    bool bHuman;
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

    BITMAP *background;

    // screen
    int screen_x, screen_y;

    // Rects
    cRectangle topBar;
    cRectangle playerTitleBar;
    cRectangle topRightBox;
    cRectangle playerList;
    cRectangle mapListTop;
    cRectangle mapListFrame;
    cRectangle previewMapRect;
    cRectangle startPoints;
    cRectangle wormsRect;
    cRectangle bloomsRect;
    cRectangle detonateBloomsRect;

    // Functions
    void prepareSkirmishGameToPlayAndTransitionToCombatState(int iSkirmishMap);

    void onMouseLeftButtonClicked(const s_MouseEvent &event);

    void drawMapList(const cRectangle &mapList, const cRectangle &mapListFrame) const;

    void drawWorms(const cRectangle &wormsRect) const;

    void drawBlooms(const cRectangle &bloomsRect) const;

    void drawDetonateBlooms(const cRectangle &detonateBloomsRect) const;

    void drawPreviewMapAndMore(const cRectangle &previewMapRect, int &iStartingPoints, int &iSkirmishMap) const;

    bool drawStartPoints(int iStartingPoints, const cRectangle &startPoints) const;

    void drawPlayerBrain(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &brainRect) const;

    void drawHouse(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &houseRec) const;

    void drawCredits(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &creditsRect) const;

    int getTextColorForRect(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &rect) const;

    void drawStartingPoints(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &startingUnitsRect) const;

    void drawTeams(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &teamsRect) const;
};

#endif //D2TM_CSETUPSKIRMISHGAMESTATE_H
