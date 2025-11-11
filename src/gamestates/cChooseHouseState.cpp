#include "gamestates/cChooseHouseState.h"

#include "d2tmc.h"
#include "data/gfxinter.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/cSoundPlayer.h"
#include "utils/Graphics.hpp"
#include <SDL2/SDL.h>
#include "include/Texture.hpp"
#include "context/GameContext.hpp"
#include "context/GraphicsContext.hpp"
#include <algorithm>

#include "data/gfxaudio.h"

cChooseHouseState::cChooseHouseState(cGame &theGame, GameContext* ctx) :
    cGameState(theGame, ctx),
    m_textDrawer(ctx->getTextContext()->beneTextDrawer.get()),
    m_gfxinter(ctx->getGraphicsContext()->gfxinter.get())
{
    backButtonRect = m_textDrawer->getAsRectangle(0, m_game.m_screenH - m_textDrawer->getFontHeight(), " BACK");
    bmp_Dune = m_gfxinter->getTexture(BMP_GAME_DUNE);

    int duneAtTheRight = m_game.m_screenW - bmp_Dune->w;
    int duneAlmostAtBottom = m_game.m_screenH - (bmp_Dune->h * 0.90);
    coords_Dune = cPoint(duneAtTheRight, duneAlmostAtBottom);

    bmp_SelectYourHouseTitle = m_gfxinter->getTexture(BMP_SELECT_YOUR_HOUSE);

    selectYourHouseXCentered = (m_game.m_screenW / 2) - bmp_SelectYourHouseTitle->w / 2;
    coords_SelectYourHouseTitle = cPoint(selectYourHouseXCentered, 0);

    bmp_HouseAtreides = m_gfxinter->getTexture(BMP_SELECT_HOUSE_ATREIDES);
    bmp_HouseOrdos = m_gfxinter->getTexture(BMP_SELECT_HOUSE_ORDOS);
    bmp_HouseHarkonnen = m_gfxinter->getTexture(BMP_SELECT_HOUSE_HARKONNEN);

    int selectYourHouseY = m_game.m_screenH * 0.25f;

    int columnWidth = m_game.m_screenW / 7; // empty, atr, empty, har, empty, ord, empty (7 columns)

    int offset = (columnWidth / 2) - (bmp_HouseAtreides->w / 2);

    houseAtreides = cRectangle((columnWidth * 1) + offset, selectYourHouseY, 90, 98);
    houseOrdos = cRectangle((columnWidth * 3) + offset, selectYourHouseY, 90, 98);
    houseHarkonnen = cRectangle((columnWidth * 5) + offset, selectYourHouseY, 90, 98);

    hoversOverBackButton = false;
}

cChooseHouseState::~cChooseHouseState()
{
    delete backButtonRect;

    // not owner of these
    bmp_Dune = nullptr;
    bmp_SelectYourHouseTitle = nullptr;
    bmp_HouseAtreides = nullptr;
    bmp_HouseOrdos = nullptr;
    bmp_HouseHarkonnen = nullptr;
}

void cChooseHouseState::thinkFast()
{

}

void cChooseHouseState::draw() const
{
    cMouse *mouse = m_game.getMouse();

    // Render the planet Dune a bit downward
    renderDrawer->renderSprite(bmp_Dune, coords_Dune.x, coords_Dune.y);

    // HOUSES
    renderDrawer->renderSprite(bmp_SelectYourHouseTitle, coords_SelectYourHouseTitle.x, coords_SelectYourHouseTitle.y);

    renderDrawer->renderSprite(bmp_HouseAtreides, houseAtreides.getX(), houseAtreides.getY());
    renderDrawer->renderSprite(bmp_HouseOrdos, houseOrdos.getX(), houseOrdos.getY());
    renderDrawer->renderSprite(bmp_HouseHarkonnen, houseHarkonnen.getX(),houseOrdos.getY());

    // BACK (bottom right
    // TODO: Use actual cGuiButton
    Color color = hoversOverBackButton ? Color{255, 0, 0,255} : Color{255, 255, 255,255};
    m_textDrawer->drawText(backButtonRect->getTopLeft(), color, " BACK");

    // MOUSE
    mouse->draw();
}

eGameStateType cChooseHouseState::getType()
{
    return GAMESTATE_SELECT_YOUR_NEXT_CONQUEST;
}

void cChooseHouseState::onNotifyMouseEvent(const s_MouseEvent &event)
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

void cChooseHouseState::onMouseLeftButtonClicked(const s_MouseEvent &event) const
{

    if (event.coords.isWithinRectangle(&houseAtreides)) {
        m_game.prepareMentatToTellAboutHouse(ATREIDES);
        m_game.playSound(SOUND_ATREIDES);
        m_game.setNextStateToTransitionTo(GAME_TELLHOUSE);
        m_game.initiateFadingOut();
    }
    else if (event.coords.isWithinRectangle(&houseOrdos)) {
        m_game.prepareMentatToTellAboutHouse(ORDOS);
        m_game.playSound(SOUND_ORDOS);
        m_game.setNextStateToTransitionTo(GAME_TELLHOUSE);
        m_game.initiateFadingOut();
    }
    else if (event.coords.isWithinRectangle(&houseHarkonnen)) {
        m_game.prepareMentatToTellAboutHouse(HARKONNEN);
        m_game.playSound(SOUND_HARKONNEN);
        m_game.setNextStateToTransitionTo(GAME_TELLHOUSE);
        m_game.initiateFadingOut();
    }
    else if (event.coords.isWithinRectangle(backButtonRect)) {
        m_game.setNextStateToTransitionTo(GAME_MENU);
        m_game.initiateFadingOut();
    }
}

void cChooseHouseState::onMouseMoved(const s_MouseEvent &event)
{
    hoversOverBackButton = event.coords.isWithinRectangle(backButtonRect);
}

void cChooseHouseState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (event.isType(eKeyEventType::PRESSED)) {
        if (event.hasKey(SDL_SCANCODE_ESCAPE)) {
            m_game.setNextStateToTransitionTo(GAME_MENU);
            m_game.initiateFadingOut();
        }
    }
}
