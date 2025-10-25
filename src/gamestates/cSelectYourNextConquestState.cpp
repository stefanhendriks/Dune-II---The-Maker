#include "cSelectYourNextConquestState.h"

#include "d2tmc.h"
#include "data/gfxdata.h"
#include "data/gfxinter.h"
#include "data/gfxworld.h"
#include "drawers/SDLDrawer.hpp"
#include "drawers/cMessageDrawer.h"
#include "ini.h"
#include "managers/cDrawManager.h"
#include "player/cPlayer.h"
// #include "gui/actions/cGuiActionToGameState.h"
#include "gui/GuiButton.h"
#include "utils/Graphics.hpp"
#include "context/GameContext.hpp"

#include <SDL2/SDL.h>
#include <iostream>
#include <format>

// Return color index from pixel (x, y) in surface
static Uint8 getPixelColorIndexFromSurface(SDL_Surface *surface, int x, int y)
{
    if (surface == nullptr) {
        std::cerr << "Error : nullptr surface" << std::endl;
        return 0;
    }

    if (surface->format->format != SDL_PIXELFORMAT_INDEX8) {
        std::cerr << "Error : no indexed 8 bits surface (format actuel : "
                  << SDL_GetPixelFormatName(surface->format->format) << ")." << std::endl;
        return 0;
    }

    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h) {
        std::cerr << "Error : out of range pixel (" << x << ", " << y << ") in (" << surface->w << ", " << surface->h << ")." << std::endl;
        return 0;
    }
    Uint8 *pixelData = (Uint8 *)surface->pixels;
    Uint8 colorIndex = pixelData[y * (surface->pitch) + x];
    return colorIndex;
}

cSelectYourNextConquestState::cSelectYourNextConquestState(GameContext*ctx, cGame &theGame) : cGameState(theGame), textDrawer(bene_font)
{
    gfxworld = ctx->getGraphicsContext()->gfxworld.get();
    state = eRegionState::REGSTATE_INIT;
    regionSceneState = eRegionSceneState::SCENE_INIT;

    iRegionSceneAlpha = 0;  // alpha for scene in introduction state

    memset(iRegionConquer, -1, sizeof(iRegionConquer));
    memset(iRegionHouse, -1, sizeof(iRegionHouse));
    memset(cRegionText, 0, sizeof(cRegionText));

    calculateOffset();

    regionClickMapBmp = nullptr;

    regionMouseIsHoveringOver = -1;
    isFinishedConqueringRegions = true;

    // the quick-way to get to a mission select window
    //const eGuiButtonRenderKind buttonKind = TRANSPARENT_WITHOUT_BORDER;
    //const eGuiTextAlignHorizontal buttonTextAlignment = CENTER;

    int length = textDrawer.getTextLength("Mission select");
    const cRectangle &toMissionSelectRect = *textDrawer.getAsRectangle(game.m_screenW - length,
                                            game.m_screenH - textDrawer.getFontHeight(),
                                            "Mission select");
    GuiButton *gui_btn_toMissionSelect = GuiButtonBuilder()
            .withRect(toMissionSelectRect)        
            .withLabel("Mission select")
            .withTextDrawer(&textDrawer)    
            .withTheme(GuiTheme::Light())
            .onClick([this]() {
                game.setNextStateToTransitionTo(GAME_MISSIONSELECT);})
            .build();   
    // GuiButton *gui_btn_toMissionSelect = new GuiButton(textDrawer, toMissionSelectRect,
    //         "Mission select", buttonKind);
    // gui_btn_toMissionSelect->setTheme(GuiTheme::Light());
    // gui_btn_toMissionSelect->setTextAlignHorizontal(buttonTextAlignment);
    // // cGuiActionToGameState *action = new cGuiActionToGameState(GAME_MISSIONSELECT, false);
    // gui_btn_toMissionSelect->setOnLeftMouseButtonClickedAction([this]() {
    //     game.setNextStateToTransitionTo(GAME_MISSIONSELECT);});
    m_guiBtnToMissionSelect = gui_btn_toMissionSelect;
}

void cSelectYourNextConquestState::calculateOffset()
{
    offsetX = (game.m_screenW - 640) / 2;
    offsetY = (game.m_screenH - 480) / 2; // same goes for offsetY (but then for 480 height).
}

cSelectYourNextConquestState::~cSelectYourNextConquestState()
{
    delete m_guiBtnToMissionSelect;
}

void cSelectYourNextConquestState::thinkFast()
{

    // First time INIT
    if (state == eRegionState::REGSTATE_INIT) {
        // temp bitmap to read from
        regionClickMapBmp = gfxworld->getSurface(WORLD_DUNE_CLICK); // 8 bit bitmap
        // NOTE: No need to use Offset here, as it is on a tempreg and we pretend our mouse is on that BMP as well
        // we substract the offset from mouse coordinates to compensate
        state = fastForward ? eRegionState::REGSTATE_CONQUER_REGIONS : eRegionState::REGSTATE_INTRODUCTION;
        return;
    }

    if (state == eRegionState::REGSTATE_INTRODUCTION) {
        if (iRegionSceneAlpha < 255) {
            iRegionSceneAlpha += 1;
        }

        if (iRegionSceneAlpha > 255) {
            iRegionSceneAlpha = 255;
        }

        int iHouse = players[0].getHouse();
        int iMission = game.m_mission;

        bool hasMessage = drawManager->hasMessage();

        if (regionSceneState == SCENE_INIT) {
            REGION_SETUP_NEXT_MISSION(iMission, iHouse);
            drawManager->setMessage("3 Houses have come to Dune.");
            transitionToNextRegionSceneState(SCENE_THREE_HOUSES_COME_FOR_DUNE);
        }
        else if (regionSceneState == SCENE_THREE_HOUSES_COME_FOR_DUNE) {
            if (!hasMessage && iRegionSceneAlpha >= 255) {
                drawManager->setMessage("To take control of the land.");
                transitionToNextRegionSceneState(SCENE_TO_TAKE_CONTROL_OF_THE_LAND);
            }
        }
        else if (regionSceneState == SCENE_TO_TAKE_CONTROL_OF_THE_LAND) {
            if (!hasMessage && iRegionSceneAlpha >= 255) {
                drawManager->setMessage("That has become divided.");
                transitionToNextRegionSceneState(SCENE_THAT_HAS_BECOME_DIVIDED);
            }
        }
        else if (regionSceneState == SCENE_THAT_HAS_BECOME_DIVIDED) {
            if (iRegionSceneAlpha >= 255) {
                transitionToNextRegionSceneState(SCENE_SELECT_YOUR_NEXT_CONQUEST);
                conquerRegions(); // this will change the message bar and state
            }
        }
    }

    if (state == eRegionState::REGSTATE_CONQUER_REGIONS || state == eRegionState::REGSTATE_INTRODUCTION) {
        for (int i = 0; i < 27; i++) {
            cRegion &regionPiece = world[i];

            if (regionPiece.iHouse < 0) continue; // skip pieces that do not belong to any house

            if (regionPiece.iAlpha < 255) {
                regionPiece.iAlpha += 3;

                // speed up when holding mouse button
                if (game.getMouse()->isLeftButtonPressed()) {
                    regionPiece.iAlpha += 3;
                }
            }
        }

        isFinishedConqueringRegions = true;

        for (int i = 0; i < MAX_REGIONS; i++) {
            if (iRegionConquer[i] < 0) continue;
            int houseThatConquersTheRegion = iRegionHouse[i];
            if (houseThatConquersTheRegion < 0) continue;

            int iRegNr = iRegionConquer[i];

            cRegion &region = world[iRegNr];

            // when the region is NOT this house, turn it into this house
            if (region.iHouse != houseThatConquersTheRegion) {
                const char *regionTextString = cRegionText[i];
                bool isRegionTextGiven = regionTextString[0] != '\0';
                bool isRegionTextEmpty = regionTextString[0] == '\0';

                if ((isRegionTextGiven && !drawManager->hasMessage()) || isRegionTextEmpty) {
                    // set this up
                    region.iHouse = houseThatConquersTheRegion;
                    region.iAlpha = 1; // this makes it > 0 and thus it will become opaque over time (see THINK function)

                    if (isRegionTextGiven) {
                        drawManager->setMessage(regionTextString);
                    }
                    isFinishedConqueringRegions = false;
                    break;
                }
                else {
                    isFinishedConqueringRegions = false;
                    break;

                }
            }
            else {
                // house = the same
                if (region.iAlpha >= 255) {
                    // remove this from the 'regionToConquer' index
                    iRegionConquer[i] = -1;
                    houseThatConquersTheRegion = -1; //
                    isFinishedConqueringRegions = false;

                    break;
                }
                else if (region.iAlpha < 255) {
                    isFinishedConqueringRegions = false;
                    break; // not done yet, so wait before we do another region!
                }
            }
        }

        if (isFinishedConqueringRegions) {
            state = REGSTATE_SELECT_NEXT_CONQUEST;
        }
    }

    // select your next conquest... always draw them in the human playing house color
    if (state == REGSTATE_SELECT_NEXT_CONQUEST) {
        if (selectNextConquestAlpha < 255) {
            selectNextConquestAlpha += 1;
        }
        else {
            selectNextConquestAlpha = 1;
        }

        for (int i = 0; i < 27; i++) {
            cRegion &regionPiece = world[i];

            if (!regionPiece.bSelectable) continue; // only animate selectable pieces

            regionPiece.iAlpha = selectNextConquestAlpha;
        }
    }

}

void cSelectYourNextConquestState::draw() const
{
    game.getMouse()->setTile(MOUSE_NORMAL); // global state of mouse
    // STEPS:
    // 1. Show current conquered regions
    // 2. Show next progress + story (in message bar)
    // 3. Click next region
    // 4. Set up region and go to GAME_BRIEFING, which will do the rest...-> fade out

    int iHouse = players[0].getHouse();
    if (state == eRegionState::REGSTATE_INTRODUCTION) {
        drawStateIntroduction();
    }
    else if (state == eRegionState::REGSTATE_CONQUER_REGIONS) {
        drawStateConquerRegions();
    }
    else if (state == eRegionState::REGSTATE_SELECT_NEXT_CONQUEST) {
        drawStateSelectYourNextConquest();
    }
    else if (state == eRegionState::REGSTATE_FADEOUT) {
        drawStateConquerRegions();
    }


    // Draw this last
    renderDrawer->renderSprite(gfxworld->getTexture(BMP_NEXTCONQ), offsetX, offsetY); // title "Select your next Conquest"
    drawLogoInFourCorners(iHouse);
    drawManager->drawMessageBar();

    m_guiBtnToMissionSelect->draw();

    drawManager->drawMouse();
}

void cSelectYourNextConquestState::drawLogoInFourCorners(int iHouse) const
{
    int iLogo = -1;

    // Draw your logo
    if (iHouse == ATREIDES)
        iLogo = BMP_NCATR;

    if (iHouse == ORDOS)
        iLogo = BMP_NCORD;

    if (iHouse == HARKONNEN)
        iLogo = BMP_NCHAR;

    // Draw 4 times the logo (in each corner)
    if (iLogo > -1) {
        renderDrawer->renderSprite(gfxworld->getTexture(iLogo), offsetX, offsetY);
        renderDrawer->renderSprite(gfxworld->getTexture(iLogo), offsetX + (640) - 64, offsetY);
        renderDrawer->renderSprite(gfxworld->getTexture(iLogo), offsetX, offsetY + (480) - 64);
        renderDrawer->renderSprite(gfxworld->getTexture(iLogo), offsetX + (640) - 64, offsetY + (480) - 64);
    }
}

void cSelectYourNextConquestState::drawStateIntroduction() const
{
    if (regionSceneState == SCENE_THREE_HOUSES_COME_FOR_DUNE) {
        // draw dune planet (being faded in)
        renderDrawer->renderSprite(gfxinter->getTexture(BMP_GAME_DUNE),offsetX, offsetY + 12,iRegionSceneAlpha);
    }
    else if (regionSceneState == SCENE_TO_TAKE_CONTROL_OF_THE_LAND) {
        renderDrawer->renderSprite(gfxinter->getTexture(BMP_GAME_DUNE), offsetX, offsetY + 12); // dune is opaque
        renderDrawer->renderSprite(gfxworld->getTexture(WORLD_DUNE), offsetX + 16, offsetY + 73, iRegionSceneAlpha);
    }
    else if (regionSceneState == SCENE_THAT_HAS_BECOME_DIVIDED) {
        // now the world map is opaque
        renderDrawer->renderSprite(gfxworld->getTexture(WORLD_DUNE), offsetX + 16, offsetY + 73);
        renderDrawer->renderSprite(gfxworld->getTexture(WORLD_DUNE_REGIONS), offsetX + 16, offsetY + 73,iRegionSceneAlpha);
    }
    else if (regionSceneState == SCENE_SELECT_YOUR_NEXT_CONQUEST) {
        renderDrawer->renderSprite(gfxworld->getTexture(WORLD_DUNE_REGIONS), offsetX + 16, offsetY + 73);
    }
}

void cSelectYourNextConquestState::drawStateConquerRegions() const   // draw dune first
{
    renderDrawer->renderSprite(gfxworld->getTexture(WORLD_DUNE), offsetX + 16, offsetY + 73);
    renderDrawer->renderSprite(gfxworld->getTexture(WORLD_DUNE_REGIONS), offsetX + 16, offsetY + 73);

    // draw here stuff
    for (int i = 0; i < 27; i++) {
        REGION_DRAW(world[i]);
    }

    // Animate here (so add regions that are conquered)
    if (isFinishedConqueringRegions && !drawManager->hasMessage()) {
        drawManager->setMessage("Select your next region.", true);
    }
}

void cSelectYourNextConquestState::drawStateSelectYourNextConquest() const
{
    renderDrawer->renderSprite(gfxworld->getTexture(WORLD_DUNE), offsetX + 16, offsetY + 73);
    renderDrawer->renderSprite(gfxworld->getTexture(WORLD_DUNE_REGIONS), offsetX + 16, offsetY + 73);

    cRegion *pRegion = getRegionMouseIsOver();
    if (pRegion && pRegion->bSelectable) {
        pRegion->iAlpha = 256;
        game.getMouse()->setTile(MOUSE_ATTACK);
    }

    // draw here stuff
    for (int i = 0; i < 27; i++) {
        REGION_DRAW(world[i]);
    }

    // Animate here (so add regions that are conquered)
    if (isFinishedConqueringRegions && !drawManager->hasMessage()) {
        drawManager->setMessage("Select your next region.", true);
    }
}

void cSelectYourNextConquestState::loadScenarioAndTransitionToNextState(int iMission)
{
    // Calculate mission from region:
    // region 1 = mission 1
    // region 2, 3, 4 = mission 2
    // region 5, 6, 7 = mission 3
    // region 8, 9, 10 = mission 4
    // region 11,12,13 = mission 5
    // region 14,15,16 = mission 6
    // region 17,18,19 = mission 7
    // region 20,21    = mission 8
    // region 22 = mission 9

    // calculate region stuff, and add it up
    int iNewReg = 0;
    if (iMission == 0) iNewReg = 1;
    if (iMission == 1) iNewReg = 2;
    if (iMission == 2) iNewReg = 5;
    if (iMission == 3) iNewReg = 8;
    if (iMission == 4) iNewReg = 11;
    if (iMission == 5) iNewReg = 14;
    if (iMission == 6) iNewReg = 17;
    if (iMission == 7) iNewReg = 20;
    if (iMission == 8) iNewReg = 22;

    // selected....
    int iReg = 0;
    for (int ir = 0; ir < MAX_REGIONS; ir++) {
        if (world[ir].bSelectable) {
            if (ir != regionMouseIsHoveringOver) {
                iReg++;
            }
            else {
                break;
            }
        }
    }

    iNewReg += iReg;

    game.missionInit();
    game.setNextStateToTransitionTo(GAME_BRIEFING);
    game.m_region = iNewReg;
    game.m_mission++;                        // FINALLY ADD MISSION NUMBER...

    // set up drawStateMentat
    game.createAndPrepareMentatForHumanPlayer();

    // load map
    game.loadScenario();

    //sprintf(msg, "Mission = %d", game.m_mission);

    game.playMusicByType(MUSIC_BRIEFING);

    state = REGSTATE_FADEOUT;
    game.initiateFadingOut();
}

cRegion *cSelectYourNextConquestState::getRegionMouseIsOver() const
{
    if (regionMouseIsHoveringOver < 0) {
        return nullptr;
    }
    return &world[regionMouseIsHoveringOver];
}

void cSelectYourNextConquestState::REGION_SETUP_LOST_MISSION()
{
    drawManager->regionInit(offsetX, offsetY);

    selectNextConquestAlpha = 1;

    // prepare players, so we know house index == player index (for colorizing region pieces)
    for (int i = 1; i < FREMEN; i++) {
        players[i].init(i, nullptr);
        players[i].setHouse(i);
    }

    return;
}

void cSelectYourNextConquestState::REGION_SETUP_NEXT_MISSION(int iMission, int iHouse)
{
    // The first mission, nothing is 'ready', as the pieces gets placed and taken by the houses.
    // Later, after mission 2, the pieces are already taken. That's what this function takes care off
    // making sure everything is 'there' to go on with. Hard-coded stuff.
    drawManager->regionInit(offsetX, offsetY);

    // make world pieces not selectable
    for (int i = 0; i < MAX_REGIONS; i++) {
        world[i].bSelectable = false;
    }

    // clear conquer stuff
    memset(iRegionConquer, -1, sizeof(iRegionConquer));
    memset(iRegionHouse, -1, sizeof(iRegionHouse));
    memset(cRegionText, 0, sizeof(cRegionText));

    // Per mission assign:
    // Every house has a different campaign, so...
    INI_Load_Regionfile(iHouse, iMission, this);

    selectNextConquestAlpha = 1;

    // prepare players, so we know house index == player index (for colorizing region pieces)
    for (int i = 1; i < FREMEN; i++) {
        players[i].init(i, nullptr);
        players[i].setHouse(i);
    }

    return;
}

void cSelectYourNextConquestState::REGION_DRAW(cRegion &regionPiece) const
{
    if (regionPiece.iAlpha <= 0) {
        // no alpha, no use in drawing
        return;
    }

    // HACK HACK - Using a temp player variable, we do a trick to calculate the proper palette for this
    // highly not efficient.... but will do for now
    if (regionPiece.iHouse > -1) {
        // single player campaign has house ID == player ID, so we can do this hack and assume player with iHouse
        // is the player we want to get the correct house collor for this piece...
        cPlayer &temp = players[regionPiece.iHouse];
        // select_palette(temp.pal);
        if (regionPiece.iHouse!=regionPiece.oldHouse) {
            regionPiece.bmpColor = temp.createTextureFromIndexedSurfaceWithPalette(regionPiece.bmp, TransparentColorIndex);
            regionPiece.oldHouse=regionPiece.iHouse;
        }
        drawRegion(regionPiece);
    } // House > -1

    // select your next conquest... always draw them in the human playing house color
    if (regionPiece.bSelectable && state == eRegionState::REGSTATE_SELECT_NEXT_CONQUEST) {
        int iHouse = players[HUMAN].getHouse();
        cPlayer &temp = players[iHouse];
        if (regionPiece.iHouse!=regionPiece.oldHouse) {
            regionPiece.bmpColor = temp.createTextureFromIndexedSurfaceWithPalette(regionPiece.bmp, TransparentColorIndex);
        }
        drawRegion(regionPiece);
    }

}

void cSelectYourNextConquestState::drawRegion(cRegion &regionPiece) const
{
    if (regionPiece.bmpColor ==nullptr) {
        return;
    }
    int regionX = offsetX + regionPiece.x;
    int regionY = offsetY + regionPiece.y;

    if (regionPiece.iAlpha >= 255) {
        renderDrawer->renderSprite(regionPiece.bmpColor, regionX, regionY);
    }
    else {
        renderDrawer->renderSprite(regionPiece.bmpColor, regionX, regionY,regionPiece.iAlpha);
    }
}
// End of function

int cSelectYourNextConquestState::REGION_OVER(int mouseX, int mouseY)
{
    // when mouse is not even on the map, return -1
    cRectangle mapRect(offsetX + 16, offsetY + 72, 608, 241);
    if (!mapRect.isPointWithin(mouseX, mouseY)) return -1;

    // from here, we are on a region
    int wCalc = round((mouseX-offsetX-16)*608.0/regionClickMapBmp->w);
    int hCalc = round((mouseY-offsetY-73)*240.0/regionClickMapBmp->h);
    int c = getPixelColorIndexFromSurface(regionClickMapBmp, wCalc, hCalc);
    //std::cout << "REGION_OVER " << mouseX-offsetX << " " << mouseY-offsetY << " " << c << std::endl;
    // @Mira display text
    //alfont_textprintf(bmp_screen, bene_font, 17,17, Color{0,0,0), "region %d", c-1);
    drawManager->setMessage(std::format("region {}",c-1));
    return c - 1;
}

void cSelectYourNextConquestState::REGION_NEW(int x, int y, int iAlpha, int iHouse, int iTile)
{
    int iNew = -1;

    for (int i = 0; i < MAX_REGIONS; i++) {
        if (world[i].x < 0 || world[i].y < 0) {
            iNew = i;
            break;
        }
    }

    // invalid index
    if (iNew < 0)
        return;

    cRegion &region = world[iNew];
    region.x = x;
    region.y = y;
    region.iAlpha = iAlpha;
    region.iHouse = iHouse;
    region.iTile = iTile;
    region.bmp = gfxworld->getSurface(iTile);
}

void cSelectYourNextConquestState::INSTALL_WORLD()
{
    // create regions
    for (int i = 0; i < MAX_REGIONS; i++) {
        world[i].bSelectable = false;
        world[i].iAlpha = 0;
        world[i].iHouse = -1;
        world[i].oldHouse = -2; // all <0 except iHouse = -1 (for cache)
        world[i].iTile = -1;
        world[i].x = -1;
        world[i].y = -1;
        world[i].bmp = nullptr;
    }

    // Now create the regions (x,y wise)

    // FIRST ROW (EASY, SAME Y AXIS)
    REGION_NEW(16, 73, 1, -1, PIECE_DUNE_001);
    REGION_NEW(126, 73, 1, -1, PIECE_DUNE_002);
    REGION_NEW(210, 73, 1, -1, PIECE_DUNE_003);
    REGION_NEW(306, 73, 1, -1, PIECE_DUNE_004);
    REGION_NEW(438, 73, 1, -1, PIECE_DUNE_005);
    REGION_NEW(510, 73, 1, -1, PIECE_DUNE_006);

    // SECOND ROW, HARDER FROM NOW ON (DIFFERENT Y AXIS ALL THE TIME)
    REGION_NEW(16, 91, 1, -1, PIECE_DUNE_007);
    REGION_NEW(158, 149, 1, -1, PIECE_DUNE_008);
    REGION_NEW(282, 135, 1, -1, PIECE_DUNE_009);
    REGION_NEW(362, 105, 1, -1, PIECE_DUNE_010);
    REGION_NEW(456, 117, 1, -1, PIECE_DUNE_011);
    REGION_NEW(544, 105, 1, -1, PIECE_DUNE_012);

    // THIRD ROW
    REGION_NEW(16, 155, 1, -1, PIECE_DUNE_013);
    REGION_NEW(58, 165, 1, -1, PIECE_DUNE_014);
    REGION_NEW(190, 213, 1, -1, PIECE_DUNE_015);
    REGION_NEW(312, 163, 1, -1, PIECE_DUNE_016);
    REGION_NEW(388, 163, 1, -1, PIECE_DUNE_017);
    REGION_NEW(502, 167, 1, -1, PIECE_DUNE_018);
    REGION_NEW(576, 167, 1, -1, PIECE_DUNE_019);

    // FOURTH ROW
    REGION_NEW(16, 237, 1, -1, PIECE_DUNE_020);
    REGION_NEW(62, 255, 1, -1, PIECE_DUNE_021);
    REGION_NEW(134, 245, 1, -1, PIECE_DUNE_022);
    REGION_NEW(282, 257, 1, -1, PIECE_DUNE_023);
    REGION_NEW(360, 253, 1, -1, PIECE_DUNE_024);
    REGION_NEW(406, 213, 1, -1, PIECE_DUNE_025);
    REGION_NEW(448, 269, 1, -1, PIECE_DUNE_026);
    REGION_NEW(514, 227, 1, -1, PIECE_DUNE_027);
}

eGameStateType cSelectYourNextConquestState::getType()
{
    return GAMESTATE_SELECT_YOUR_NEXT_CONQUEST;
}

void cSelectYourNextConquestState::conquerRegions()
{
    fastForward = false;
    state = REGSTATE_CONQUER_REGIONS;
}

void cSelectYourNextConquestState::transitionToNextRegionSceneState(eRegionSceneState newSceneState)
{
    regionSceneState = newSceneState;
    iRegionSceneAlpha = 0;
}

void cSelectYourNextConquestState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    switch (event.eventType) {
        case MOUSE_MOVED_TO:
            onMouseMove(event);
            break;
        case MOUSE_LEFT_BUTTON_CLICKED:
            onMouseLeftButtonClicked(event);
            break;
        default:
            break;
    }
    // WARNING: m_guiBtnToMissionSelect can trigger fast-forward. That will destroy this state object,
    // so it will crash if you do anything beyond here with this object!!
    m_guiBtnToMissionSelect->onNotifyMouseEvent(event);
}

void cSelectYourNextConquestState::onMouseMove(const s_MouseEvent &event)
{
    // no interaction unless we select next conquest
    if (state != eRegionState::REGSTATE_SELECT_NEXT_CONQUEST) return;

    this->regionMouseIsHoveringOver = REGION_OVER(event.coords.x, event.coords.y);

    cRegion *region = getRegionMouseIsOver();
    if (region && region->bSelectable) {
        region->iAlpha = 255;
        game.getMouse()->setTile(MOUSE_ATTACK);
    }
}

void cSelectYourNextConquestState::onMouseLeftButtonClicked(const s_MouseEvent &)
{
    // no interaction unless we select next conquest
    if (state != eRegionState::REGSTATE_SELECT_NEXT_CONQUEST) return;

    cRegion *pRegion = getRegionMouseIsOver();
    if (pRegion && pRegion->bSelectable) {
        loadScenarioAndTransitionToNextState(game.m_mission);
    }
}

void cSelectYourNextConquestState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (event.eventType == eKeyEventType::PRESSED) {
        if (event.hasKey(SDL_SCANCODE_ESCAPE)) {
            game.setNextStateToTransitionTo(GAME_OPTIONS);
        }
    }
}

void cSelectYourNextConquestState::fastForwardUntilMission(int missionNr, int house)
{
    fastForward = true;
    for (int m = 1; m < missionNr; m++) {
        REGION_SETUP_NEXT_MISSION(m, house);

        for (int i = 0; i < MAX_REGIONS; i++) {
            if (iRegionConquer[i] < 0) continue;
            int houseThatConquersTheRegion = iRegionHouse[i];
            if (houseThatConquersTheRegion < 0) continue;

            int iRegNr = iRegionConquer[i];

            cRegion &region = world[iRegNr];

            region.iAlpha = 255;
            region.iHouse = houseThatConquersTheRegion;

            iRegionConquer[i] = -1;
        }
    }
}
