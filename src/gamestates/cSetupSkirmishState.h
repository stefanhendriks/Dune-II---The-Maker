#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "definitions.h"
#include "gui/GuiButton.h"
#include "sMouseEvent.h"
#include "utils/cRectangle.h"
#include "map/cPreviewMaps.h"

#include <memory>
#include <functional>

struct SDL_Surface;
class cGame;
class cMouse;
class cPreviewMaps;
class cRandomMapGenerator;
class Graphics;
class SDLDrawer;
class cTextDrawer;
class cGameSettings;
class cGameInterface;

struct s_DataCampaign;

struct s_SkirmishPlayer {
    bool bHuman;
    bool bPlaying;
    int startingUnits;
    int iCredits;
    int iHouse;
    int team;
};

class cSetupSkirmishState : public cGameState {

public:
    explicit cSetupSkirmishState(sGameServices* services, std::shared_ptr<cPreviewMaps> previewMaps, s_DataCampaign* dataCompaign);
    ~cSetupSkirmishState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    cTextDrawer* m_textDrawer = nullptr;
    cGameSettings* m_settings = nullptr;
    cGameInterface* m_interface = nullptr;
    cGameObjectContext* m_objects = nullptr;
    s_DataCampaign* m_dataCampaign = nullptr;
    std::shared_ptr<cPreviewMaps> m_previewMaps;
    Graphics* m_gfxinter = nullptr;
    cMouse *m_mouse = nullptr;

    s_SkirmishPlayer skirmishPlayer[MAX_PLAYERS] = {};
    
    std::unique_ptr<cRandomMapGenerator> randomMapGenerator;

    bool guiDrawFrame(int x, int y, int width, int height) const ;
    bool guiDrawFramePressed(int x1, int y1, int width, int height) const;

    std::function<void()> nextFunction;
    std::function<void()> previousFunction;

    int mapStartingIndexToDisplay;          // index of which maps are currently being displayed in the map list
    int maxMapsInSelectArea = 0;            // maximum number of maps that can be displayed in the select area
    int maxMapsInSelectAreaHorizontally;    // maximum number of maps that can be displayed in the select area, horizontally
    int maxMapsInSelectAreaVertically;      // maximum number of maps that can be displayed in the select area, vertically
    int iSkirmishMap;			            // what map is selected

    int spawnWorms;
    bool spawnBlooms;
    bool detonateBlooms;
    int iStartingPoints;
    int techLevel;

    // Colors
    Color colorDarkishBackground;
    Color colorLightBackground;
    Color colorDarkishBorder;
    Color colorDarkerYellow;
    Color colorDisabled;
    Color colorOtherBorder;

    // Some basic layout coordinates
    int topBarHeight;
    int previewMapHeight;
    int previewMapWidth;
    int widthOfRightColumn;
    int mapItemButtonHeight;
    int mapItemButtonWidth;

    // screen
    int screen_x, screen_y;

    // Rects
    cRectangle topBar;
    cRectangle playerTitleBar;
    cRectangle topRightBox;
    cRectangle playerList;
    cRectangle mapList;
    cRectangle mapListTitle;
    cRectangle previewMapTitle;
    cRectangle previewMap;
    cRectangle previewMapRect;
    cRectangle startPointsRect;
    cRectangle wormsRect;
    cRectangle bloomsRect;
    cRectangle detonateBloomsRect;
    cRectangle techLevelRect;
    cRectangle selectArea;

    std::unique_ptr<GuiButton> backButton;
    std::unique_ptr<GuiButton> startButton;
    std::unique_ptr<GuiButton> modifyButton;
    std::unique_ptr<GuiButton> nextMapButton;
    std::unique_ptr<GuiButton> previousMapButton;

    // Functions
    void prepareSkirmishGameToPlayAndTransitionToCombatState(int iSkirmishMap);

    void onMouseLeftButtonClicked(const s_MouseEvent &event);
    void onMouseRightButtonClicked(const s_MouseEvent &event);    

    void drawMapList(const cRectangle &mapList) const;

    void drawWorms(const cRectangle &wormsRect) const;

    void drawBlooms(const cRectangle &bloomsRect) const;

    void drawDetonateBlooms(const cRectangle &detonateBloomsRect) const;

    void drawPreviewMapAndMore(const cRectangle &previewMapRect) const;

    void drawStartPoints(int iStartingPoints, const cRectangle &startPoints) const;

    void drawPlayerBrain(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &brainRect) const;

    void drawHouse(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &houseRec) const;

    void drawCredits(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &creditsRect) const;

    Color getTextColorForRect(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &rect) const;

    void drawStartingUnits(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &startingUnitsRect) const;

    void drawTeams(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &teamsRect) const;

    void drawTechLevel(const cRectangle &techRect) const;

    void onMouseMovedTo(const s_MouseEvent &event);

    void generateRandomMap();


    void onMouseLeftButtonClickedAtStartPoints();
    void onMouseRightButtonClickedAtStartPoints();

    void onMouseLeftButtonClickedAtWorms();
    void onMouseRightButtonClickedAtWorms();

    void onMouseLeftButtonClickedAtMapList(const cRectangle &selectMapRect);
    void onMouseLeftButtonClickedAtSpawnBlooms();
    void onMouseLeftButtonClickedAtDetonateBlooms();

    void onMouseLeftButtonClickedAtPlayerList();
    void onMouseRightButtonClickedAtPlayerList();

    void onMouseLeftButtonClickedAtTechLevel();
    void onMouseRightButtonClickedAtTechLevel();
};
