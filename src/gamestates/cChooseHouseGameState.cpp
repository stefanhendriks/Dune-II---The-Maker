#include "cChooseHouseGameState.h"

#include "d2tmc.h"
#include "data/gfxinter.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/cSoundPlayer.h"
#include "utils/Graphics.hpp"
#include <SDL2/SDL.h>
#include "include/Texture.hpp"
#include <algorithm>

cChooseHouseGameState::cChooseHouseGameState(cGame &theGame) :
    cGameState(theGame),
    textDrawer(cTextDrawer(bene_font))
{
    backButtonRect = textDrawer.getAsRectangle(0, game.m_screenH - textDrawer.getFontHeight(), " BACK");

    bmp_Dune = gfxinter->getTexture(BMP_GAME_DUNE);

    int duneAtTheRight = game.m_screenW - bmp_Dune->w;
    int duneAlmostAtBottom = game.m_screenH - (bmp_Dune->h * 0.90);
    coords_Dune = cPoint(duneAtTheRight, duneAlmostAtBottom);

    bmp_SelectYourHouseTitle = gfxinter->getTexture(BMP_SELECT_YOUR_HOUSE);

    selectYourHouseXCentered = (game.m_screenW / 2) - bmp_SelectYourHouseTitle->w / 2;
    coords_SelectYourHouseTitle = cPoint(selectYourHouseXCentered, 0);

    bmp_HouseAtreides = gfxinter->getTexture(BMP_SELECT_HOUSE_ATREIDES);
    bmp_HouseOrdos = gfxinter->getTexture(BMP_SELECT_HOUSE_ORDOS);
    bmp_HouseHarkonnen = gfxinter->getTexture(BMP_SELECT_HOUSE_HARKONNEN);

    int selectYourHouseY = game.m_screenH * 0.25f;

    int columnWidth = game.m_screenW / 7; // empty, atr, empty, har, empty, ord, empty (7 columns)

    int offset = (columnWidth / 2) - (bmp_HouseAtreides->w / 2);

    houseAtreides = cRectangle((columnWidth * 1) + offset, selectYourHouseY, 90, 98);
    houseOrdos = cRectangle((columnWidth * 3) + offset, selectYourHouseY, 90, 98);
    houseHarkonnen = cRectangle((columnWidth * 5) + offset, selectYourHouseY, 90, 98);

    hoversOverBackButton = false;
}

cChooseHouseGameState::~cChooseHouseGameState()
{
    delete backButtonRect;

    // not owner of these
    bmp_Dune = nullptr;
    bmp_SelectYourHouseTitle = nullptr;
    bmp_HouseAtreides = nullptr;
    bmp_HouseOrdos = nullptr;
    bmp_HouseHarkonnen = nullptr;
}

void cChooseHouseGameState::thinkFast()
{

}

void cChooseHouseGameState::draw() const
{
    cMouse *mouse = game.getMouse();

    // Render the planet Dune a bit downward
    renderDrawer->renderSprite(bmp_Dune, coords_Dune.x, coords_Dune.y);

    // HOUSES
    renderDrawer->renderSprite(bmp_SelectYourHouseTitle, coords_SelectYourHouseTitle.x, coords_SelectYourHouseTitle.y);

    renderDrawer->renderSprite(bmp_HouseAtreides, houseAtreides.getX(), houseAtreides.getY());
    renderDrawer->renderSprite(bmp_HouseOrdos, houseOrdos.getX(), houseOrdos.getY());
    renderDrawer->renderSprite(bmp_HouseHarkonnen, houseHarkonnen.getX(),houseOrdos.getY());

    // BACK (bottom right
    Color color = hoversOverBackButton ? Color{255, 0, 0,255} :
                      Color{255, 255, 255,255};
    textDrawer.drawText(backButtonRect->getTopLeft(), color, " BACK");

    // MOUSE
    mouse->draw();
}

eGameStateType cChooseHouseGameState::getType()
{
    return GAMESTATE_SELECT_YOUR_NEXT_CONQUEST;
}

void cChooseHouseGameState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    switch (event.eventType) {
        case MOUSE_LEFT_BUTTON_CLICKED:
            onMouseLeftButtonClicked(event);
            break;
        case MOUSE_MOVED_TO:
            onMouseMoved(event);
            break;
        default:
            break;
    }
}

void cChooseHouseGameState::onMouseLeftButtonClicked(const s_MouseEvent &event) const
{

    if (event.coords.isWithinRectangle(&houseAtreides)) {
        game.prepareMentatToTellAboutHouse(ATREIDES);
        game.playSound(SOUND_ATREIDES);
        game.setNextStateToTransitionTo(GAME_TELLHOUSE);
        game.initiateFadingOut();
    }
    else if (event.coords.isWithinRectangle(&houseOrdos)) {
        game.prepareMentatToTellAboutHouse(ORDOS);
        game.playSound(SOUND_ORDOS);
        game.setNextStateToTransitionTo(GAME_TELLHOUSE);
        game.initiateFadingOut();
    }
    else if (event.coords.isWithinRectangle(&houseHarkonnen)) {
        game.prepareMentatToTellAboutHouse(HARKONNEN);
        game.playSound(SOUND_HARKONNEN);
        game.setNextStateToTransitionTo(GAME_TELLHOUSE);
        game.initiateFadingOut();
    }
    else if (event.coords.isWithinRectangle(backButtonRect)) {
        game.setNextStateToTransitionTo(GAME_MENU);
        game.initiateFadingOut();
    }
}

void cChooseHouseGameState::onMouseMoved(const s_MouseEvent &event)
{
    hoversOverBackButton = event.coords.isWithinRectangle(backButtonRect);
}

void cChooseHouseGameState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (event.isType(eKeyEventType::PRESSED)) {
        if (event.hasKey(SDL_SCANCODE_ESCAPE)) {
            game.setNextStateToTransitionTo(GAME_MENU);
            game.initiateFadingOut();
        }
    }
}
