#include "gamestates/cSetupSkirmishState.h"

#include "controls/eKeyAction.h"
#include "controls/cMouse.h"
#include "data/gfxinter.h"
#include "drawers/SDLDrawer.hpp"
#include "drawers/cTextDrawer.h"
#include "managers/cDrawManager.h"
#include "gameobjects/map/cMapCamera.h"
#include "gameobjects/map/cMapEditor.h"
#include "gameobjects/map/cRandomMapGenerator.h"
#include "gameobjects/map/cPreviewMaps.h"
#include "player/brains/cPlayerBrainSandworm.h"
#include "player/brains/cPlayerBrainSkirmish.h"
#include "player/brains/superweapon/cPlayerBrainFremenSuperWeapon.h"
#include "player/cPlayer.h"
#include "utils/cLog.h"
#include "utils/Graphics.hpp"
#include "utils/RNG.hpp"
#include "context/GameContext.hpp"
#include "context/GraphicsContext.hpp"
#include "context/cGameObjectContext.h"
#include "game/cGameInterface.h"
#include "include/sDataCampaign.h"
#include "gameobjects/units/cUnits.h"
#include "include/Texture.hpp"
#include "gui/GuiButton.h"

#include <format>
#include <algorithm>
#include <utility>
#include <cassert>

#include "config.h"

static bool mouse_within_rect(cMouse* mouse, int x, int y, int width, int height)
{
    return ((mouse->getX() >= x && mouse->getX() < (x + width))
            && (mouse->getY() >= y && mouse->getY() <= (y + height)));
}

cSetupSkirmishState::cSetupSkirmishState(sGameServices* services, std::shared_ptr<cPreviewMaps> previewMaps,s_DataCampaign* dataCompaign) :
    cGameState(services),
    m_textDrawer(m_ctx->getTextContext()->getBeneTextDrawer()),
    m_settings(services->settings),
    m_interface(m_ctx->getGameInterface()),
    m_objects(services->objects),
    m_dataCampaign(dataCompaign),
    m_previewMaps(std::move(previewMaps)),
    m_gfxinter(m_ctx->getGraphicsContext()->gfxinter.get())
{
    assert(m_textDrawer != nullptr);
    assert(m_settings != nullptr);
    assert(m_interface != nullptr);
    assert(m_objects != nullptr);
    assert(m_dataCampaign != nullptr);
    assert(m_gfxinter != nullptr);
    for (int i = 0; i < MAX_PLAYERS; i++) {
        s_SkirmishPlayer &sSkirmishPlayer = skirmishPlayer[i];
        // index 0 == human player, but to keep our lives sane we don't change the index.

        // player 0 (HUMAN) is always playing,
        sSkirmishPlayer.bHuman = i == HUMAN;

        // and 1 additional AI is minimally required to play
        sSkirmishPlayer.bPlaying = (i <= 1);

        // just some defaults
        sSkirmishPlayer.startingUnits = 3;
        sSkirmishPlayer.iCredits = 2500;
        sSkirmishPlayer.iHouse = 0; // random house
        sSkirmishPlayer.team = (i + 1); // all different team
    }

    iStartingPoints = 2;
    iSkirmishMap = -1;
    mapStartingIndexToDisplay = 0;

    randomMapGenerator = std::make_unique<cRandomMapGenerator>();
    generateRandomMap();

    m_mouse = m_interface->getMouse();

    spawnWorms = 2;
    techLevel = 9;
    spawnBlooms = true;
    detonateBlooms = true;

    auto theme = cGuiThemeBuilder().light().build();

    // Colors
    colorDarkishBackground = theme.background;
    colorDarkishBorder = theme.borderLight;
    colorDarkerYellow = theme.textDarkColor;
    colorDisabled = theme.disabled;
    colorLightBackground = theme.fillColor;
    colorOtherBorder = theme.borderDark;

    // Basic coordinates
    topBarHeight = 21;
    previewMapHeight = 129;
    previewMapWidth = 129;
    widthOfRightColumn = 300;
    mapItemButtonHeight = 175;
    mapItemButtonWidth = 145;

    // Screen
    screen_x = m_settings->getScreenW();
    screen_y = m_settings->getScreenH();

    // Rectangles for GUI interaction
    int topBarWidth = screen_x + 4;

    // title box
    topBar = cRectangle(-1, -1, topBarWidth, topBarHeight);

    // Players title bar
    int widthOfSidebar = widthOfRightColumn + 2;
    int playerTitleBarWidth = screen_x - widthOfSidebar;
    int playerTitleBarHeight = topBarHeight;
    int playerTitleBarX = 0;
    int playerTitleBarY = topBarHeight;
    playerTitleBar = cRectangle(playerTitleBarX, playerTitleBarY, playerTitleBarWidth, playerTitleBarHeight);

    // this is the box at the right from the Player list
    int topRightBoxWidth = widthOfSidebar;
    int topRightBoxHeight = playerTitleBarHeight + previewMapHeight+playerTitleBarHeight;
    int topRightBoxX = screen_x - topRightBoxWidth;
    int topRightBoxY = topBarHeight;
    topRightBox = cRectangle(topRightBoxX, topRightBoxY, topRightBoxWidth, topRightBoxHeight);

    // player list
    int playerListBarWidth = playerTitleBarWidth;
    int playerListBarHeight = playerTitleBarHeight + previewMapHeight;
    int playerListBarX = 0;
    int playerListBarY = playerTitleBarY + topBarHeight;
    playerList = cRectangle(playerListBarX, playerListBarY, playerListBarWidth, playerListBarHeight);

    // map list

    // title bar
    int mapListFrameX = 0;
    int mapListFrameY = playerListBarY + playerListBarHeight;
    int mapListFrameHeight = topBarHeight;

    // rectangle for map list
    mapListTitle = cRectangle(mapListFrameX, mapListFrameY, playerTitleBarWidth, mapListFrameHeight);
    cRectangle previousMaps(mapListFrameX + 4, mapListFrameY, 60, mapListFrameHeight);
    cRectangle nextMaps(mapListFrameX + playerTitleBarWidth - 48, mapListFrameY, 50, mapListFrameHeight);

    // preview map
    int previewMapFrameX = screen_x - widthOfSidebar;
    int previewMapFrameY = playerListBarY + playerListBarHeight;
    int previewMapFrameWidth = widthOfSidebar;
    int previewMapFrameHeight = topBarHeight;
    previewMapTitle = cRectangle(previewMapFrameX, previewMapFrameY,previewMapFrameWidth, previewMapFrameHeight);

    // actual list of maps
    int mapListTopY = mapListTitle.getY() + mapListTitle.getHeight();
    int previewMapY = playerListBarY + playerListBarHeight+previewMapFrameHeight;
    int previewMapX = screen_x - widthOfSidebar;
    previewMap = cRectangle(previewMapX, previewMapY, widthOfRightColumn, widthOfRightColumn+65);
    previewMapRect = cRectangle(previewMapX+25, previewMapY+25, widthOfRightColumn-50, widthOfRightColumn-50);

    selectArea = cRectangle(0, mapListTopY, screen_x -widthOfRightColumn-2, screen_y-topBarHeight-mapListTopY);
    int margin = 10;

    maxMapsInSelectAreaHorizontally = selectArea.getWidth() / (mapItemButtonWidth+margin);
    maxMapsInSelectAreaVertically = selectArea.getHeight() / (mapItemButtonHeight+5);
    maxMapsInSelectArea = maxMapsInSelectAreaHorizontally * maxMapsInSelectAreaVertically;

    nextFunction = [this]() {
        // Go to the next map
        int intendedNextStartIndex = mapStartingIndexToDisplay + maxMapsInSelectArea;
        if (intendedNextStartIndex <= m_previewMaps->getMapCount()) {
            mapStartingIndexToDisplay = intendedNextStartIndex;
        }
    };
    previousFunction = [this]() {
        // Go back to the previous map
        if (mapStartingIndexToDisplay > 0) {
            mapStartingIndexToDisplay -= maxMapsInSelectArea;
        }
    };

    nextMapButton = GuiButtonBuilder()
        .withRect(nextMaps)
        .withLabel("Next")
        .withTextDrawer(m_textDrawer)
        .withRenderer(m_renderDrawer)
        .withTheme(theme)
        .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
        .onClick(nextFunction)
        .build();

    previousMapButton = GuiButtonBuilder()
            .withRect(previousMaps)
            .withLabel("Previous")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withTheme(theme)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick(previousFunction)
            .build();

    // Top right skirmish game properties:

    // Starting positions
    int startPointsX = screen_x - widthOfRightColumn;
    int startPointsY = topBarHeight + 6;
    int startPointHitBoxWidth = 130;
    int startPointHitBoxHeight = 16;
    startPointsRect = cRectangle(startPointsX, startPointsY, startPointHitBoxWidth, startPointHitBoxHeight);

    // Worms
    int wormsX = screen_x - widthOfRightColumn;
    int wormsY = startPointsY + 32;
    int wormsHitBoxWidth = 130;
    int wormsHitBoxHeight = 16;
    wormsRect = cRectangle(wormsX, wormsY, wormsHitBoxWidth, wormsHitBoxHeight);

    // Spice Blooms
    int bloomsX = screen_x - widthOfRightColumn;
    int bloomsY = wormsY + 32;
    int bloomsHitBoxWidth = 130;
    int bloomsHitBoxHeight = 16;
    bloomsRect = cRectangle(bloomsX, bloomsY, bloomsHitBoxWidth, bloomsHitBoxHeight);

    int detonateX = screen_x - widthOfRightColumn;
    int detonateY = bloomsY + 32;
    int detonateHitBoxWidth = 130;
    int detonateHitBoxHeight = 16;
    detonateBloomsRect = cRectangle(detonateX, detonateY, detonateHitBoxWidth, detonateHitBoxHeight);

    int techLevelX = screen_x - widthOfRightColumn;
    int techLevelY = detonateY + 32;
    int techLevelHitBoxWidth = 130;
    int techLevelHitBoxHeight = 16;
    techLevelRect = cRectangle(techLevelX, techLevelY, techLevelHitBoxWidth, techLevelHitBoxHeight);

    int backButtonWidth = m_textDrawer->getTextLength(" BACK");
    int backButtonHeight = topBarHeight;
    int backButtonY = screen_y - topBarHeight;
    int backButtonX = 0;
    cRectangle backButtonRect(backButtonX, backButtonY, backButtonWidth, backButtonHeight);
    backButton = GuiButtonBuilder()
            .withRect(backButtonRect)        
            .withLabel("BACK")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withTheme(theme)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this]() {
                m_interface->setTransitionToWithFadingOut(GAME_MENU);
            })
            .build();

    // Start skirmish mission (bottom right)
    int startButtonWidth = m_textDrawer->getTextLength("START");
    int startButtonHeight = topBarHeight;
    int startButtonY = screen_y - topBarHeight;
    int startButtonX = screen_x - startButtonWidth;
    cRectangle startButtonRect = cRectangle(startButtonX, startButtonY, startButtonWidth, startButtonHeight);
    startButton = GuiButtonBuilder()
            .withRect(startButtonRect)        
            .withLabel("START")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withTheme(theme)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this]() {
                if (iSkirmishMap > -1) {
                    prepareSkirmishGameToPlayAndTransitionToCombatState(iSkirmishMap);
                }
            })
            .build();

    // Modifiymap mission (bottom right)
    int modifyButtonWidth = m_textDrawer->getTextLength("Modify");
    int modifyButtonHeight = topBarHeight;
    int modifyButtonY = screen_y - topBarHeight;
    int modifyButtonX = screen_x - startButtonWidth-modifyButtonWidth - 15;
    cRectangle modifyButtonRect = cRectangle(modifyButtonX, modifyButtonY, modifyButtonWidth, modifyButtonHeight);
    modifyButton = GuiButtonBuilder()
            .withRect(modifyButtonRect)        
            .withLabel("Modify")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withTheme(theme)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this]() {
                if (iSkirmishMap > -1) {
                    m_interface->loadMapFromEditor(iSkirmishMap);
                    m_interface->initiateFadingOut();
                }
            })
            .build();
}

cSetupSkirmishState::~cSetupSkirmishState()
{
    // Smart pointers handle cleanup
}

bool cSetupSkirmishState::guiDrawFrame(int x, int y, int width, int height) const
{
    cRectangle rect = cRectangle(x, y, width, height);
    m_renderDrawer->gui_DrawRect(rect, colorLightBackground, colorDarkishBorder, colorOtherBorder);
    return mouse_within_rect(m_mouse, x, y, width, height);
}

bool cSetupSkirmishState::guiDrawFramePressed(int x1, int y1, int width, int height) const
{
    m_renderDrawer->renderRectFillColor(x1, y1, width, height, colorLightBackground);
    m_renderDrawer->renderRectColor(x1, y1, width, height, colorOtherBorder);

    // lines to darken the right sides
    m_renderDrawer->renderLine(x1+width, y1, x1+width, y1+height, colorDarkishBorder);
    m_renderDrawer->renderLine(x1, y1+height, x1+width, y1+height, colorDarkishBorder);

    return mouse_within_rect(m_mouse, x1, y1, width, height);
}

void cSetupSkirmishState::thinkFast()
{
}

void cSetupSkirmishState::draw() const
{
    // @Mira rewrite it on Texture
    m_renderDrawer->gui_DrawRect(topBar, colorLightBackground, colorDarkishBorder, colorOtherBorder);

    m_textDrawer->drawTextCentered("Skirmish", 1);

    m_renderDrawer->gui_DrawRect(playerTitleBar, colorDarkishBackground, Color::White, Color::White);
    m_renderDrawer->gui_DrawRect(topRightBox, colorLightBackground, colorDarkishBorder, colorOtherBorder);
    m_renderDrawer->gui_DrawRect(playerList, colorDarkishBackground, Color::White, Color::White);
    m_renderDrawer->gui_DrawRect(mapListTitle, colorDarkishBackground, colorDarkishBorder, colorDarkishBorder);

    m_textDrawer->drawTextCentered("Maps", mapListTitle.getX(), mapListTitle.getWidth(), mapListTitle.getY() + 4, Color::Yellow);
    m_renderDrawer->gui_DrawRect(previewMapTitle, colorDarkishBackground, colorDarkishBorder, colorDarkishBorder);    //renderDrawer->gui_DrawRect(previewMap, colorDarkishBackground, colorDarkishBorder, colorDarkishBorder);      
    m_textDrawer->drawTextCentered("Preview", previewMapTitle.getX(), previewMapTitle.getWidth(), previewMapTitle.getY() + 4, Color::Yellow);
    m_renderDrawer->gui_DrawRect(previewMap, colorDarkishBackground, colorDarkishBorder, colorDarkishBorder);
    m_renderDrawer->gui_DrawRect(selectArea, colorDarkishBackground, colorDarkishBorder, colorDarkishBorder);
    ///////
    /// DRAW PREVIEW MAP
    //////

    // iSkirmishMap holds an index of which map to load, where index 0 means random map generated, although
    // this is only meaningful for rendering, the loading (more below) of that map does not care if it is
    // randomly generated or not.
    drawPreviewMapAndMore(previewMapRect);

    drawStartPoints(iStartingPoints, startPointsRect);

    drawWorms(wormsRect);
    drawBlooms(bloomsRect);

    drawDetonateBlooms(detonateBloomsRect);
    drawTechLevel(techLevelRect);
    drawMapList(selectArea);

    // Header text for players
    m_textDrawer->drawText(4, 25, "Player      House      Credits       Units    Team");

    // draw players who will be playing ;)
    for (int p = 0; p < (AI_WORM - 1); p++) {
        const int iDrawY = playerList.getY() + 4 + (p * 22);
        const int iDrawX = 4;

        const s_SkirmishPlayer &sSkirmishPlayer = skirmishPlayer[p];

        if (p < iStartingPoints) {
            // player playing or not
            cRectangle brainRect = cRectangle(iDrawX, iDrawY, 73, 16);
            drawPlayerBrain(sSkirmishPlayer, brainRect);

            // HOUSE
            const int houseX = 74;
            const int houseY = iDrawY;
            cRectangle houseRec = cRectangle(houseX, houseY, 76, 16);

            drawHouse(sSkirmishPlayer, houseRec);

            // Credits
            const int creditsX = 174;
            const int creditsY = iDrawY;
            cRectangle creditsRect = cRectangle(creditsX, creditsY, 56, 16);

            drawCredits(sSkirmishPlayer, creditsRect);

            // Units
            const int startingUnitsX = 272;
            const int startingUnitsY = iDrawY;
            cRectangle startingUnitsRect = cRectangle(startingUnitsX, startingUnitsY, 21, 16);
            drawStartingUnits(sSkirmishPlayer, startingUnitsRect);

            // Credits
            const int teamsX = 340;
            const int teamsY = iDrawY;
            cRectangle teamsRect = cRectangle(teamsX, teamsY, 21, 16);
            drawTeams(sSkirmishPlayer, teamsRect);
        }
    }

    cRectangle bottomBarRect = cRectangle(-1, screen_y - topBarHeight, screen_x + 2, topBarHeight + 2);
    m_renderDrawer->gui_DrawRect(bottomBarRect, colorLightBackground, colorDarkishBorder, colorOtherBorder);

    // For now in draw function
    startButton->setEnabled(iSkirmishMap > -1);
    modifyButton->setEnabled(iSkirmishMap > -1);

    backButton->draw();
    startButton->draw();
    modifyButton->draw();
    nextMapButton->draw();
    previousMapButton->draw();

    // MOUSE
    m_mouse->draw();
}

void cSetupSkirmishState::drawTeams(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &teamsRect) const
{
    Color textColor = getTextColorForRect(sSkirmishPlayer, teamsRect);
    m_textDrawer->drawText(teamsRect.getX(), teamsRect.getY(), textColor, std::format("{}", sSkirmishPlayer.team));
}

void cSetupSkirmishState::drawStartingUnits(const s_SkirmishPlayer &sSkirmishPlayer,
        const cRectangle &startingUnitsRect) const
{

    Color textColor = getTextColorForRect(sSkirmishPlayer, startingUnitsRect);
    m_textDrawer->drawText(startingUnitsRect.getX(), startingUnitsRect.getY(), textColor,
                        std::format("{}", sSkirmishPlayer.startingUnits));
}

void cSetupSkirmishState::drawCredits(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &creditsRect) const
{
    Color textColor = getTextColorForRect(sSkirmishPlayer, creditsRect);
    m_textDrawer->drawText(creditsRect.getX(), creditsRect.getY(), textColor, std::format("{}", sSkirmishPlayer.iCredits));
}

Color cSetupSkirmishState::getTextColorForRect(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &rect) const
{
    if (rect.isPointWithin(m_mouse->getX(), m_mouse->getY())) {
        Color colorSelectedRedFade = m_interface->getColorFadeSelected(255, 0, 0);
        Color colorDisabledFade = m_interface->getColorFadeSelected(128, 128, 128);
        return sSkirmishPlayer.bPlaying ? colorSelectedRedFade : colorDisabledFade;
    }

    if (sSkirmishPlayer.bHuman) { // should be redundant when player is always m_playing?
        return Color::White;
    }

    return sSkirmishPlayer.bPlaying ? Color::White : colorDisabled;
}

void cSetupSkirmishState::drawHouse(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &houseRec) const
{
    Color textColor = getTextColorForRect(sSkirmishPlayer, houseRec);
    const std::string &cPlayerHouseString = cPlayer::getHouseNameForId(sSkirmishPlayer.iHouse);
    const char *cHouse = sSkirmishPlayer.iHouse > 0 ? cPlayerHouseString.c_str() : "Random";
    m_textDrawer->drawText(houseRec.getX(), houseRec.getY(), textColor, cHouse);
}

void cSetupSkirmishState::drawPlayerBrain(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &brainRect) const
{
    if (sSkirmishPlayer.bHuman) {
        m_textDrawer->drawText(brainRect.getX(), brainRect.getY(), "Human");
    }
    else {
        Color textColor = getTextColorForRect(sSkirmishPlayer, brainRect);
        m_textDrawer->drawText(brainRect.getX(), brainRect.getY(), textColor, "  CPU");
    }
}

void cSetupSkirmishState::drawStartPoints(int iStartingPoints, const cRectangle &startPoints) const
{
    Color textColor = Color::White;
    if (iSkirmishMap == 0) { // random map selected
        if (startPoints.isPointWithin(m_mouse->getX(), m_mouse->getY())) {
            textColor = Color::Red;
        }
    }
    else {
        textColor = colorDisabled;
    }

    m_textDrawer->drawText(startPoints.getX(), startPoints.getY(), textColor,std::format("Startpoints: {}",iStartingPoints));
}

void cSetupSkirmishState::drawPreviewMapAndMore(const cRectangle &previewMapRect) const
{
    if (iSkirmishMap > -1) {
        s_PreviewMap &selectedMap = m_previewMaps->getMap(iSkirmishMap);
        // Render skirmish map as-is (pre-loaded map)
        if (iSkirmishMap > 0) {
            if (selectedMap.name[0] != '\0') {
                if (selectedMap.terrain) {
                    cRectangle dst;
                    if (selectedMap.previewTex->w > selectedMap.previewTex->h) {
                        dst = cRectangle(previewMapRect.getX(), previewMapRect.getY(),
                                         previewMapRect.getWidth(), previewMapRect.getWidth() * selectedMap.previewTex->h / selectedMap.previewTex->w);
                    }
                    else {
                        dst = cRectangle(previewMapRect.getX(), previewMapRect.getY(),
                                         previewMapRect.getHeight() * selectedMap.previewTex->w / selectedMap.previewTex->h, previewMapRect.getHeight());

                    }
                    m_renderDrawer->renderStrechFullSprite(selectedMap.previewTex, dst);
                }
            }
        }
        else {
            // render the 'random generated skirmish map'
            cRectangle dst = cRectangle(previewMapRect.getX(), previewMapRect.getY(),previewMapRect.getWidth(), previewMapRect.getWidth());            // when mouse is hovering, draw it, else do not
            if (previewMapRect.isPointWithin(m_mouse->getX(), m_mouse->getY())) {
                    m_renderDrawer->renderStrechFullSprite(selectedMap.previewTex, dst);
            } else {
                m_renderDrawer->renderStrechFullSprite(m_gfxinter->getTexture(BMP_UNKNOWNMAP), dst);
            }
        }
        m_textDrawer->drawText(previewMapRect.getX() + 4, previewMapRect.getY() + previewMapRect.getHeight() + 16,
                            Color::Yellow, std::format("Name: {}", selectedMap.name));
        m_textDrawer->drawText(previewMapRect.getX() + 4, previewMapRect.getY() + previewMapRect.getHeight() + 32+8,
                            Color::White, std::format("Author: {}", selectedMap.author));
        m_textDrawer->drawText(previewMapRect.getEndX() -50, previewMapRect.getY() + previewMapRect.getHeight() + 16,
                            colorDarkerYellow, std::format("{}", m_previewMaps->getMapSize(iSkirmishMap)));
        m_textDrawer->drawText(previewMapRect.getX() + 4, previewMapRect.getY() + previewMapRect.getHeight() + 32+30,
                            Color::White, std::format("{}", selectedMap.description));
    }
}

void cSetupSkirmishState::drawDetonateBlooms(const cRectangle &detonateBloomsRect) const
{
    if (spawnBlooms) {
        Color textColor = detonateBloomsRect.isPointWithin(m_mouse->getX(), m_mouse->getY()) ? Color::Red : Color::White;
        m_textDrawer->drawText(detonateBloomsRect.getX(), detonateBloomsRect.getY(), textColor,
                            std::format("Auto-detonate : {}", detonateBlooms ? "YES" : "NO"));
    }
    else {
        m_textDrawer->drawText(detonateBloomsRect.getX(), detonateBloomsRect.getY(), colorDisabled, "Auto-detonate : -");
    }
}

void cSetupSkirmishState::drawBlooms(const cRectangle &bloomsRect) const
{
    Color textColor = bloomsRect.isPointWithin(m_mouse->getX(), m_mouse->getY()) ? Color::Red : Color::White;
    m_textDrawer->drawText(bloomsRect.getX(), bloomsRect.getY(), textColor,
                        std::format("Spice blooms : {}", spawnBlooms ? "YES" : "NO"));
}

void cSetupSkirmishState::drawWorms(const cRectangle &wormsRect) const
{
    Color textColor = wormsRect.isPointWithin(m_mouse->getX(), m_mouse->getY()) ? Color::Red : Color::White;
    m_textDrawer->drawText(wormsRect.getX(), wormsRect.getY(), textColor,
                        std::format("Worms? : {}", spawnWorms));
}

void cSetupSkirmishState::drawTechLevel(const cRectangle &techLevelRect) const
{
    Color textColor = techLevelRect.isPointWithin(m_mouse->getX(), m_mouse->getY()) ? Color::Red : Color::White;
    m_textDrawer->drawText(techLevelRect.getX(), techLevelRect.getY(), textColor,
                        std::format("TechLevel : {}", techLevel));
}

void cSetupSkirmishState::prepareSkirmishGameToPlayAndTransitionToCombatState(int iSkirmishMap)
{
    s_PreviewMap &selectedMap = m_previewMaps->getMap(iSkirmishMap);

    // this needs to be before setupPlayers :/
    m_dataCampaign->mission = techLevel;

    m_interface->setupPlayers();

    // Starting skirmish mode
    m_settings->setSkirmish(true);

    /* set up starting positions */
    std::vector<int> iStartPositions;
    int startCellsOnSkirmishMap = 0;
    for (int s = 0; s < 5; s++) {
        int startPosition = selectedMap.iStartCell[s];
        if (startPosition < 0) continue;
        iStartPositions.push_back(startPosition);
    }

    startCellsOnSkirmishMap = iStartPositions.size();

    // REGENERATE MAP DATA FROM INFO
    m_objects->getMap().init(selectedMap.width, selectedMap.height);

    auto mapEditor = cMapEditor(m_objects->getMap());
    for (int c = 0; c < m_objects->getMap().getMaxCells(); c++) {
        mapEditor.createCell(c, selectedMap.terrainType[c], 0);
    }
    mapEditor.smoothMap();

    if (m_settings->isDebugMode()) {
        logbook("Starting positions before shuffling:");
        for (int i = 0; i < startCellsOnSkirmishMap; i++) {
            logbook(std::format("iStartPositions[{}] = [{}]", i, iStartPositions[i]));
        }
    }

    logbook("Shuffling starting positions");
    std::shuffle(iStartPositions.begin(), iStartPositions.end(), RNG::getGenerator());

    if (m_settings->isDebugMode()) {
        logbook("Starting positions after shuffling:");
        for (int i = 0; i < startCellsOnSkirmishMap; i++) {
            logbook(std::format("iStartPositions[{}] = [{}]", i, iStartPositions[i]));
        }
    }

    int maxThinkingAIs = MAX_PLAYERS;
    if (m_settings->isOneAi()) {
        maxThinkingAIs = 1;
    }

    if (m_settings->isDisableAI()) {
        maxThinkingAIs = 0;
    }

    // set up players
    for (int p = 0; p < MAX_PLAYERS; p++) {
        s_SkirmishPlayer &sSkirmishPlayer = skirmishPlayer[p];

        int iHouse = sSkirmishPlayer.iHouse; // get house selected, which can be 0 for RANDOM

        // not playing.. do nothing (only for playable factions)
        bool playableFaction = p < AI_CPU5;

        if (playableFaction) {
            if (!sSkirmishPlayer.bPlaying) {
                // make sure it is a brain dead AI...
                cPlayer* cPlayer = m_objects->getPlayer(p);
                cPlayer->init(p, nullptr);
                continue;
            }

            // house = 0 means pick random house
            if (iHouse == 0) {
                bool bOk = false;

                while (bOk == false) {
                    iHouse = RNG::rnd(4) + 1; // hark = 1, atr = 2, ord = 3, sar = 4

                    bool houseInUse = false;
                    for (int pl = 0; pl < AI_WORM; pl++) {
                        // already in use by other skirmish set-up players
                        if (skirmishPlayer[pl].iHouse > 0 &&
                                skirmishPlayer[pl].iHouse == iHouse) {
                            houseInUse = true;
                        }

                        if (m_objects->getPlayer(pl)->getHouse() == iHouse) {
                            // already in use by a already-setup player
                            houseInUse = true;
                        }
                    }

                    if (!houseInUse) {
                        bOk = true;
                    }
                }
            }
        }
        else {
            if (p == AI_CPU5) {
                iHouse = FREMEN;
            }
            else {
                iHouse = GENERALHOUSE;
            }
        }

        cPlayer* pPlayer = m_objects->getPlayer(p);

        // TEAM Logic
        if (p == HUMAN) {
            pPlayer->init(p, nullptr);
        }
        else if (p == AI_CPU5) {
            pPlayer->init(p, std::make_unique<brains::cPlayerBrainFremenSuperWeapon>(pPlayer));
        }
        else if (p == AI_CPU6) {
            if (!m_settings->isDisableWormAi()) {
                pPlayer->init(p, std::make_unique<brains::cPlayerBrainSandworm>(pPlayer));
            }
            else {
                pPlayer->init(p, nullptr);
            }
        }
        else {
            if (maxThinkingAIs > 0) {
                pPlayer->init(p, std::make_unique<brains::cPlayerBrainSkirmish>(pPlayer));
                maxThinkingAIs--;
            }
            else {
                pPlayer->init(p, nullptr);
            }
        }

        pPlayer->setCredits(sSkirmishPlayer.iCredits);
        pPlayer->setHouse(iHouse);

        // from here, ignore non playable factions
        if (!playableFaction) continue;

        pPlayer->setFocusCell(iStartPositions[p]);

        // Set map position
        if (p == HUMAN) {
            m_interface->getMapCamera()->centerAndJumpViewPortToCell(pPlayer->getFocusCell());
        }

        // create constyard
        const s_PlaceResult &result = pPlayer->canPlaceStructureAt(pPlayer->getFocusCell(), CONSTYARD);
        if (!result.success) {
            // when failure, create mcv instead
            // cUnits::unitCreate(pPlayer->getFocusCell(), MCV, p, true);
            const s_GameEvent event {
                .eventType = eGameEventType::GAME_EVENT_CREATE_UNIT,
                .data = DeployUnitEvent {
                    .iCell = pPlayer->getFocusCell(),
                    .unitType = MCV,
                    .iPlayer = p,
                    .bOnStart = true,
                    .isReinforcement = false
                }
            };
            m_interface->onNotifyGameEvent(event);
        }
        else {
            pPlayer->placeStructure(pPlayer->getFocusCell(), CONSTYARD, 100);
        }
    }


    // amount of units
    int u = 0;
    int maxAmountOfStartingUnits = 0;

    for (int p = 0; p < MAX_PLAYERS; p++) {
        s_SkirmishPlayer &sSkirmishPlayer = skirmishPlayer[p];
        if (sSkirmishPlayer.startingUnits > maxAmountOfStartingUnits) {
            maxAmountOfStartingUnits = sSkirmishPlayer.startingUnits;
        }
    }

    // create units
    while (u < maxAmountOfStartingUnits) {
        // pick a random unit type
        int iType = RNG::rnd(12);

        for (int p = 0; p < MAX_PLAYERS; p++) {
            cPlayer* pPlayer = m_objects->getPlayer(p);
            s_SkirmishPlayer &pSkirmishPlayer = skirmishPlayer[p];

            if (!pSkirmishPlayer.bPlaying) continue; // skip non playing players

            if (u >= pSkirmishPlayer.startingUnits) {
                continue; // skip this player
            }

            int iPlayerUnitType = pPlayer->getSameOrSimilarUnitType(iType);

            int minRange = 3;
            int maxRange = 12;
            int cell = m_objects->getMap().getRandomCellFromWithRandomDistanceValidForUnitType(pPlayer->getFocusCell(),
                       minRange,
                       maxRange,
                       iPlayerUnitType);

            cUnits::unitCreate(cell, iPlayerUnitType, p, true);

            cLogger::getInstance()->log(LOG_TRACE, COMP_SKIRMISHSETUP, "Creating units",
                                        std::format("Wants {} amount of units; amount created {}", pSkirmishPlayer.startingUnits, u),
                                        OUTC_NONE, p, pPlayer->getHouse());
        }

        u++;
    }

    // TEAM LOGIC here, so we can decide which is Atreides and thus should be allied with Fremen...
    for (int p = 0; p < MAX_PLAYERS; p++) {
        cPlayer* player = m_objects->getPlayer(p);
        s_SkirmishPlayer &sSkirmishPlayer = skirmishPlayer[p];
        if (p == HUMAN) {
            player->setTeam(sSkirmishPlayer.team);
        }
        else if (p == AI_CPU5) {   // AI for fremen (super weapon)
            player->setTeam(AI_CPU5);
        }
        else if (p == AI_CPU6) {
            player->setTeam(AI_CPU6); // worm team is against everyone
        }
        else {
            // all other AI's are their own team (campaign == AI's are friends, here they are enemies)
            player->setTeam(sSkirmishPlayer.team);
        }
    }

    for (int p = 0; p < MAX_PLAYERS; p++) {
        cPlayer* player = m_objects->getPlayer(p);
        if (player->getHouse() == ATREIDES) {
            m_objects->getPlayer(AI_CPU5)->setTeam(player->getTeam());
        }
    }


    // default flags (destroy everyone but me/my team)
    m_interface->setWinFlags(3);
    m_interface->setLoseFlags(1);

    m_interface->playMusicByType(MUSIC_PEACE);

    m_objects->getMap().setAutoSpawnSpiceBlooms(spawnBlooms);
    m_objects->getMap().setAutoDetonateSpiceBlooms(detonateBlooms);
    m_objects->getMap().setDesiredAmountOfWorms(spawnWorms);

    // spawn requested amount of worms at start
    if (spawnWorms > 0) {
        int worms = spawnWorms;
        int minDistance = worms * 12; // so on 64x64 maps this still could work
        int maxDistance = worms * 32; // 128 / 4
        int wormCell = m_objects->getMapGeometry()->getRandomCell();
        int failures = 0;
        logbook(std::format("Skirmish game with {} sandworms, minDistance {}, maxDistance {}", worms, minDistance, maxDistance));
        while (worms > 0) {
            int cell = m_objects->getMap().getRandomCellFromWithRandomDistanceValidForUnitType(wormCell, minDistance, maxDistance,
                       SANDWORM);
            if (cell < 0) {
                // retry
                failures++;
                if (failures > 10) {
                    // too many failed attempts, just stop
                    logbook("Failed too many times to find spot for sandworm, aborting!");
                    break;
                }
                continue;
            }
            logbook(std::format("Spawning sandworm at {}", cell));
            // cUnits::unitCreate(cell, SANDWORM, AI_WORM, true);
            const s_GameEvent event {
                .eventType = eGameEventType::GAME_EVENT_CREATE_UNIT,
                .data = DeployUnitEvent {
                    .iCell = cell,
                    .unitType = SANDWORM,
                    .iPlayer = AI_WORM,
                    .bOnStart = true
                }
            };
            m_interface->onNotifyGameEvent(event);
            wormCell = cell; // start from here to spawn new worm
            worms--;
            failures = 0; // reset failures
        }
    }
    else {
        logbook("Skirmish game without sandworms");
    }

    auto drawManager = m_interface->getDrawManager();
    drawManager->missionInit();
    m_interface->setTransitionToWithFadingOut(GAME_PLAYING);
}

eGameStateType cSetupSkirmishState::getType()
{
    return GAMESTATE_SETUP_SKIRMISH;
}

void cSetupSkirmishState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    switch (event.eventType) {
        case MOUSE_LEFT_BUTTON_CLICKED:
            onMouseLeftButtonClicked(event);
            break;
        case MOUSE_RIGHT_BUTTON_CLICKED:
            onMouseRightButtonClicked(event);
            break;
        case MOUSE_MOVED_TO:
            onMouseMovedTo(event);
            break;
        default:
            break;
    }

    backButton->onNotifyMouseEvent(event);
    startButton->onNotifyMouseEvent(event);
    modifyButton->onNotifyMouseEvent(event);
    nextMapButton->onNotifyMouseEvent(event);
    previousMapButton->onNotifyMouseEvent(event);
}

void cSetupSkirmishState::onMouseRightButtonClicked(const s_MouseEvent &)
{
    onMouseRightButtonClickedAtStartPoints();
    onMouseRightButtonClickedAtWorms();
    onMouseRightButtonClickedAtPlayerList();
    onMouseRightButtonClickedAtTechLevel();
}

void cSetupSkirmishState::onMouseRightButtonClickedAtPlayerList()  // draw players who will be playing ;)
{
    for (int p = 0; p < (AI_WORM - 1); p++) {
        const int iDrawY = playerList.getY() + 4 + (p * 22);

        if (p < iStartingPoints) {
            s_SkirmishPlayer &sSkirmishPlayer = skirmishPlayer[p];
            if (!sSkirmishPlayer.bPlaying) continue;

            // HOUSE
            const int houseX = 74;
            const int houseY = iDrawY;
            cRectangle houseRec = cRectangle(houseX, houseY, 76, 16);
            // on click:
            if (houseRec.isPointWithin(m_mouse->getX(), m_mouse->getY())) {
                sSkirmishPlayer.iHouse--;
                if (sSkirmishPlayer.iHouse < 0) {
                    sSkirmishPlayer.iHouse = SARDAUKAR;
                }
            }

            // Credits
            const int creditsX = 174;
            const int creditsY = iDrawY;
            cRectangle creditsRect = cRectangle(creditsX, creditsY, 56, 16);

            // on click:
            if (creditsRect.isPointWithin(m_mouse->getX(), m_mouse->getY())) {
                sSkirmishPlayer.iCredits -= 500;
                if (sSkirmishPlayer.iCredits < 1000) {
                    sSkirmishPlayer.iCredits = 10000;
                }
            }

            // Units
            const int startingUnitsX = 272;
            const int startingUnitsY = iDrawY;
            cRectangle startingUnitsRect = cRectangle(startingUnitsX, startingUnitsY, 21, 16);
            // on click:
            if (startingUnitsRect.isPointWithin(m_mouse->getX(), m_mouse->getY())) {
                sSkirmishPlayer.startingUnits--;
                if (sSkirmishPlayer.startingUnits < 1) {
                    sSkirmishPlayer.startingUnits = 10;
                }
            }

            // Credits
            const int teamsX = 340;
            const int teamsY = iDrawY;
            cRectangle teamsRect = cRectangle(teamsX, teamsY, 21, 16);
            // on click:
            if (teamsRect.isPointWithin(m_mouse->getX(), m_mouse->getY())) {
                sSkirmishPlayer.team--;
                if (sSkirmishPlayer.team < 1) {
                    sSkirmishPlayer.team = iStartingPoints;
                }
            }
        }
    }
}

void cSetupSkirmishState::onMouseLeftButtonClicked(const s_MouseEvent &)
{
    onMouseLeftButtonClickedAtMapList(selectArea);
    onMouseLeftButtonClickedAtStartPoints();
    onMouseLeftButtonClickedAtWorms();
    onMouseLeftButtonClickedAtSpawnBlooms();
    onMouseLeftButtonClickedAtDetonateBlooms();
    onMouseLeftButtonClickedAtPlayerList();
    onMouseLeftButtonClickedAtTechLevel();
}

void cSetupSkirmishState::onMouseLeftButtonClickedAtPlayerList()
{
    for (int p = 0; p < (AI_WORM - 1); p++) {
        int iDrawY = playerList.getY() + 4 + (p * 22);
        int iDrawX = 4;

        if (p < iStartingPoints) {
            s_SkirmishPlayer &sSkirmishPlayer = skirmishPlayer[p];
            // player playing or not

            cRectangle brainRect = cRectangle(iDrawX, iDrawY, 73, 16);
            // on click:
            // only allow changing 'playing' state of CPU 2 or 3 (not 1, as there should always be one playing CPU)
            if (brainRect.isPointWithin(m_mouse->getX(),m_mouse->getY())) {
                if (p > 1) {
                    if (sSkirmishPlayer.bPlaying) {
                        sSkirmishPlayer.bPlaying = false;
                    }
                    else {
                        sSkirmishPlayer.bPlaying = true;
                    }
                }
            }

            if (!sSkirmishPlayer.bPlaying) {
                continue;
            }

            // HOUSE
            int houseX = 74;
            int houseY = iDrawY;
            cRectangle houseRec = cRectangle(houseX, houseY, 76, 16);
            // on click:
            if (houseRec.isPointWithin(m_mouse->getX(), m_mouse->getY())) {
                sSkirmishPlayer.iHouse++;
                if (sSkirmishPlayer.iHouse > SARDAUKAR) {
                    sSkirmishPlayer.iHouse = 0;
                }
            }

            // Credits
            int creditsX = 174;
            int creditsY = iDrawY;
            cRectangle creditsRect = cRectangle(creditsX, creditsY, 56, 16);
            // on click:
            if (creditsRect.isPointWithin(m_mouse->getX(), m_mouse->getY())) {
                sSkirmishPlayer.iCredits += 500;
                if (sSkirmishPlayer.iCredits > 10000) {
                    sSkirmishPlayer.iCredits = 1000;
                }
            }

            // Units
            int startingUnitsX = 272;
            int startingUnitsY = iDrawY;
            cRectangle startingUnitsRect = cRectangle(startingUnitsX, startingUnitsY, 21, 16);
            // on click:
            if (startingUnitsRect.isPointWithin(m_mouse->getX(), m_mouse->getY())) {
                sSkirmishPlayer.startingUnits++;
                if (sSkirmishPlayer.startingUnits > 10) {
                    sSkirmishPlayer.startingUnits = 1;
                }
            }

            // Credits
            int teamsX = 340;
            int teamsY = iDrawY;
            cRectangle teamsRect = cRectangle(teamsX, teamsY, 21, 16);
            //  on click:
            if (teamsRect.isPointWithin(m_mouse->getX(), m_mouse->getY())) {
                sSkirmishPlayer.team++;
                if (sSkirmishPlayer.team > iStartingPoints) {
                    sSkirmishPlayer.team = 1;
                }
            }
        }
    }
}

void cSetupSkirmishState::onMouseLeftButtonClickedAtDetonateBlooms()
{
    if (spawnBlooms) {
        if (detonateBloomsRect.isPointWithin(m_mouse->getX(), m_mouse->getY())) {
            detonateBlooms = !detonateBlooms;
        }
    }
}

void cSetupSkirmishState::onMouseLeftButtonClickedAtSpawnBlooms()
{
    if (bloomsRect.isPointWithin(m_mouse->getX(), m_mouse->getY())) {
        spawnBlooms = !spawnBlooms;
    }
}

void cSetupSkirmishState::onMouseLeftButtonClickedAtWorms()
{
    if (wormsRect.isPointWithin(m_mouse->getX(), m_mouse->getY())) {
        spawnWorms += 1;
        if (spawnWorms > 4) {
            spawnWorms = 0;
        }
    }
}

void cSetupSkirmishState::onMouseLeftButtonClickedAtStartPoints()
{
    if (iSkirmishMap == 0) { // random map selected
        if (startPointsRect.isPointWithin(m_mouse->getX(), m_mouse->getY())) {
            iStartingPoints++;

            if (iStartingPoints > 4) {
                iStartingPoints = 2;
            }

            generateRandomMap();
        }
    }
}

void cSetupSkirmishState::onMouseLeftButtonClickedAtMapList(const cRectangle &selectMapRect)
{
    int const margin = 5;
    int iDrawX = selectMapRect.getX() + margin;
    int iDrawY = selectMapRect.getY() + margin;

    int endIndex = mapStartingIndexToDisplay + maxMapsInSelectArea;

    // for every map that we read , draw here
    for (int j = mapStartingIndexToDisplay; j < endIndex; j++) {
        int mapIndexToConsiderClickedAt = j;

        if (m_previewMaps->getMapCount() < mapIndexToConsiderClickedAt) {
            continue;
        }

        // no title, safety measure
        auto &mapToConsiderClickedAt = m_previewMaps->getMap(mapIndexToConsiderClickedAt);
        if (mapToConsiderClickedAt.name.empty()) {
            continue;
        }

        cRectangle rect = cRectangle(iDrawX, iDrawY, mapItemButtonWidth, mapItemButtonHeight);
        const bool mouseHoversOverMapTile = rect.isPointWithin(m_mouse->getMouseCoords());

        if (mapToConsiderClickedAt.validMap && mouseHoversOverMapTile) {
            // Mark map as selected
            iSkirmishMap = mapIndexToConsiderClickedAt;

            // If Random map pressed, generate it
            if (mapIndexToConsiderClickedAt == 0) {
                generateRandomMap();
            } else {
                // Else, count the starting points from selected map, and set it
                iStartingPoints = 0;
                for (int s : mapToConsiderClickedAt.iStartCell) {
                    if (s > -1) {
                        iStartingPoints++;
                    }
                }
                // remove CPU players if new map has less starting points than currently set
                for (int p = iStartingPoints; p < (AI_WORM - 1); p++) {
                    skirmishPlayer[p].bPlaying = false;
                }
            }
        }

        // Determine next tile coordinates, and if needed wrap to next row
        iDrawX += mapItemButtonWidth + 15;

        if ((iDrawX + mapItemButtonWidth) > selectArea.getEndX()) {
            iDrawX = selectMapRect.getX() + margin;
            iDrawY += mapItemButtonHeight + 15;
        }
    }
}

void cSetupSkirmishState::generateRandomMap()
{
    auto &randomMap = m_previewMaps->getMap(0);
    int randomMapWidth = 128;
    int randomMapHeight = 128;
    int maxCells = randomMapWidth * randomMapHeight;
    randomMap.terrainType = std::vector<int>(maxCells, -1);

    if (randomMap.terrain == nullptr) {
        randomMap.terrain = SDL_CreateRGBSurface(0, randomMapWidth, randomMapHeight,32,0,0,0,255);
    }

    // delete any preview texture if it exists
    delete randomMap.previewTex;

    randomMapGenerator->generateRandomMap(randomMapWidth, randomMapHeight, iStartingPoints, randomMap);

    // @mira do better than (game.m_gameObjectsContext->getMap().getWidth() * game.m_gameObjectsContext->getMap().getHeight() > 64 * 64)
    spawnWorms = (randomMapWidth * randomMapHeight > 64 * 64) ? 4 : 2;

    randomMap.validMap = true;
    randomMap.author = "D2TM";

    SDL_Texture* out = SDL_CreateTextureFromSurface(m_renderDrawer->getRenderer(), randomMap.terrain);
    if (out == nullptr) {
        logbook(std::format("Error creating texture from surface: {}", SDL_GetError()));
        return;
    }
    randomMap.previewTex = new Texture(out, randomMap.terrain->w, randomMap.terrain->h);
}

void cSetupSkirmishState::drawMapList(const cRectangle &selectMapArea) const
{
    int const margin = 5;
    int iDrawX = selectMapArea.getX() + margin;
    int iDrawY = selectMapArea.getY() + margin;

    int endIndex = mapStartingIndexToDisplay + maxMapsInSelectArea;

    // for every map that we read , draw here
    for (int j = mapStartingIndexToDisplay; j < endIndex; j++) {
        int mapIndexToRender = j;

        if (m_previewMaps->getMapCount() < mapIndexToRender) {
            continue;
        }

        // no title, safety measure
        auto &mapToRender = m_previewMaps->getMap(mapIndexToRender);
        if (mapToRender.name.empty()) continue;

        // Start rendering
        bool isRenderingSelectedMap = mapIndexToRender == iSkirmishMap;

        // RENDERS ! & also get true/false if mouse hovers
        const bool bHover = guiDrawFrame(iDrawX, iDrawY, mapItemButtonWidth, mapItemButtonHeight);

        Color textColor = bHover ? Color::Red : Color::White;

        if (bHover && mapToRender.validMap && m_mouse->isLeftButtonClicked()) {
            // RENDERS (AGAIN)!
            guiDrawFramePressed(iDrawX, iDrawY, mapItemButtonWidth, mapItemButtonHeight);
        }

        // selected map, always render as pressed
        if (isRenderingSelectedMap) {
            textColor = bHover ? colorDarkerYellow : Color::Yellow;
            // RENDERS (AGAIN)!
            guiDrawFramePressed(iDrawX, iDrawY, mapItemButtonWidth, mapItemButtonHeight);
        }

        // In case invalid map, render as not-selectable
        if (!mapToRender.validMap) {
            textColor = colorDisabled;
        }

        m_textDrawer->drawText(iDrawX + 4, iDrawY + 4, textColor, mapToRender.name);

        Texture *tex = nullptr;
        if (mapIndexToRender == 0) {
            // random map, render the 'random map' texture
            tex = m_gfxinter->getTexture(BMP_UNKNOWNMAP);
        } else {
            tex = mapToRender.previewTex;
        }

        // Render preview map on tile
        cRectangle dest = cRectangle(iDrawX + 4, iDrawY + 20, mapItemButtonWidth - 8, mapItemButtonHeight - 24);
        m_renderDrawer->renderStrechFullSprite(tex, dest);

        // Determine next tile coordinates, and if needed wrap to next row
        iDrawX += mapItemButtonWidth + 15;

        if ((iDrawX + mapItemButtonWidth) > selectArea.getEndX()) {
            iDrawX = selectMapArea.getX() + margin;
            iDrawY += mapItemButtonHeight + 15;
        }
    }
}

void cSetupSkirmishState::onMouseRightButtonClickedAtStartPoints()
{
    if (iSkirmishMap == 0) { // random map selected
        if (startPointsRect.isPointWithin(m_mouse->getMouseCoords())) {
            iStartingPoints--;

            if (iStartingPoints < 2) { // < 2 startpoints is not allowed
                iStartingPoints = 4; // wrap around to max
            }

            generateRandomMap();
        }
    }
}

void cSetupSkirmishState::onMouseRightButtonClickedAtWorms()
{
    if (wormsRect.isPointWithin(m_mouse->getMouseCoords())) {
        spawnWorms -= 1;
        if (spawnWorms < 0) {
            spawnWorms = 4;
        }
    }
}


void cSetupSkirmishState::onMouseRightButtonClickedAtTechLevel()
{
    if (techLevelRect.isPointWithin(m_mouse->getMouseCoords())) {
        techLevel--;

        if (techLevel < 2) { // < 2 techlevel is not allowed
            techLevel = 9; // wrap around to max
        }
    }
}

void cSetupSkirmishState::onMouseLeftButtonClickedAtTechLevel()
{
    if (techLevelRect.isPointWithin(m_mouse->getX(), m_mouse->getY())) {
        techLevel++;
        if (techLevel > 9) {
            techLevel = 2;
        }
    }
}

void cSetupSkirmishState::onMouseMovedTo(const s_MouseEvent &)
{
}

void cSetupSkirmishState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (event.isType(eKeyEventType::PRESSED)) {
        if (event.isAction(eKeyAction::MENU_BACK)) {
            m_interface->setTransitionToWithFadingOut(GAME_MENU);
        }
        if (event.isAction(eKeyAction::SCROLL_LEFT)) {
            previousFunction();
        }
        if (event.isAction(eKeyAction::SCROLL_RIGHT)) {
            nextFunction();
        }
    }
}

