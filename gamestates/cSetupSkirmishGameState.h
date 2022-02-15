#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "definitions.h"
#include "drawers/cTextDrawer.h"
#include "gui/cGuiButton.h"
#include "sMouseEvent.h"
#include "utils/cRectangle.h"

class BITMAP;
class cGame;
class cMouse;

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
    explicit cSetupSkirmishGameState(cGame& theGame);
    ~cSetupSkirmishGameState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    cTextDrawer textDrawer;
    s_SkirmishPlayer skirmishPlayer[MAX_PLAYERS];
    cMouse * mouse;

    int iSkirmishMap;			// what map is selected
    int spawnWorms;
    bool spawnBlooms;
    bool detonateBlooms;
    int iStartingPoints;


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
    cRectangle mapList;
    cRectangle mapListTitle;
    cRectangle previewMapRect;
    cRectangle startPointsRect;
    cRectangle wormsRect;
    cRectangle bloomsRect;
    cRectangle detonateBloomsRect;

    cGuiButton * backButton;
    cGuiButton * startButton;

    // Functions
    void prepareSkirmishGameToPlayAndTransitionToCombatState(int iSkirmishMap);

    void onMouseLeftButtonClicked(const s_MouseEvent &event);

    void drawMapList(const cRectangle &mapList) const;

    void drawWorms(const cRectangle &wormsRect) const;

    void drawBlooms(const cRectangle &bloomsRect) const;

    void drawDetonateBlooms(const cRectangle &detonateBloomsRect) const;

    void drawPreviewMapAndMore(const cRectangle &previewMapRect) const;

    void drawStartPoints(int iStartingPoints, const cRectangle &startPoints) const;

    void drawPlayerBrain(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &brainRect) const;

    void drawHouse(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &houseRec) const;

    void drawCredits(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &creditsRect) const;

    int getTextColorForRect(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &rect) const;

    void drawStartingUnits(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &startingUnitsRect) const;

    void drawTeams(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &teamsRect) const;

    void onMouseMovedTo(const s_MouseEvent &event);

    void generateRandomMap();

    void onMouseLeftButtonClickedAtMapList();

    void onMouseLeftButtonClickedAtStartPoints();

    void onMouseRightButtonClicked(const s_MouseEvent &event);

    void onMouseRightButtonClickedAtStartPoints();

    void onMouseLeftButtonClickedAtStartButton();

    void onMouseLeftButtonClickedAtWorms();

    void onMouseRightButtonClickedAtWorms();

    void onMouseLeftButtonClickedAtSpawnBlooms();

    void onMouseLeftButtonClickedAtDetonateBlooms();

    void onMouseRightButtonClickedAtPlayerList();

    void onMouseLeftButtonClickedAtPlayerList();
};
