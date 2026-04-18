#include "include/definitions.h"
#include "gamestates/cChooseHouseState.h"
#include "game/cGameSettings.h"
#include "controls/eKeyAction.h"
#include "data/gfxinter.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "include/Texture.hpp"
#include "context/GameContext.hpp"
#include "context/GraphicsContext.hpp"
#include "drawers/cTextDrawer.h"
#include "game/cGameInterface.h"
#include <cassert>

cChooseHouseState::cChooseHouseState(cGame &theGame, sGameServices* services) :
    cGameState(theGame, services),
    m_textDrawer(m_ctx->getTextContext()->getBeneTextDrawer()),
    m_gfxinter(m_ctx->getGraphicsContext()->gfxinter.get()),
    m_interface(m_ctx->getGameInterface())
{
    assert(m_textDrawer != nullptr);
    assert(m_gfxinter != nullptr);
    assert(m_interface != nullptr);
    auto settings =services->settings;
    assert(settings != nullptr);

    backButtonRect = m_textDrawer->getAsRectangle(0, settings->getScreenH() - m_textDrawer->getFontHeight(), " BACK");
    bmp_Dune = m_gfxinter->getTexture(BMP_GAME_DUNE);

    int duneAtTheRight = settings->getScreenW() - bmp_Dune->w;
    int duneAlmostAtBottom = settings->getScreenH() - (bmp_Dune->h * 0.90);
    coords_Dune = cPoint(duneAtTheRight, duneAlmostAtBottom);

    bmp_SelectYourHouseTitle = m_gfxinter->getTexture(BMP_SELECT_YOUR_HOUSE);

    selectYourHouseXCentered = (settings->getScreenW() / 2) - bmp_SelectYourHouseTitle->w / 2;
    coords_SelectYourHouseTitle = cPoint(selectYourHouseXCentered, 0);

    bmp_HouseAtreides = m_gfxinter->getTexture(BMP_SELECT_HOUSE_ATREIDES);
    bmp_HouseOrdos = m_gfxinter->getTexture(BMP_SELECT_HOUSE_ORDOS);
    bmp_HouseHarkonnen = m_gfxinter->getTexture(BMP_SELECT_HOUSE_HARKONNEN);

    int selectYourHouseY = settings->getScreenH() * 0.25f;

    int columnWidth = settings->getScreenW() / 7; // empty, atr, empty, har, empty, ord, empty (7 columns)

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
    // Render the planet Dune a bit downward
    m_renderDrawer->renderSprite(bmp_Dune, coords_Dune.x, coords_Dune.y);

    // HOUSES
    m_renderDrawer->renderSprite(bmp_SelectYourHouseTitle, coords_SelectYourHouseTitle.x, coords_SelectYourHouseTitle.y);

    m_renderDrawer->renderSprite(bmp_HouseAtreides, houseAtreides.getX(), houseAtreides.getY());
    m_renderDrawer->renderSprite(bmp_HouseOrdos, houseOrdos.getX(), houseOrdos.getY());
    m_renderDrawer->renderSprite(bmp_HouseHarkonnen, houseHarkonnen.getX(),houseOrdos.getY());

    // BACK (bottom right
    // TODO: Use actual cGuiButton
    Color color = hoversOverBackButton ? Color{255, 0, 0,255} : Color{255, 255, 255,255};
    m_textDrawer->drawText(backButtonRect->getTopLeft(), color, " BACK");

    // MOUSE
    m_interface->drawCursor();
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
        m_interface->prepareMentatToTellAboutHouse(ATREIDES);
    }
    else if (event.coords.isWithinRectangle(&houseOrdos)) {
        m_interface->prepareMentatToTellAboutHouse(ORDOS);
    }
    else if (event.coords.isWithinRectangle(&houseHarkonnen)) {
        m_interface->prepareMentatToTellAboutHouse(HARKONNEN);
    }
    else if (event.coords.isWithinRectangle(backButtonRect)) {
        m_interface->setTransitionToWithFadingOut(GAME_MENU);
    }
}

void cChooseHouseState::onMouseMoved(const s_MouseEvent &event)
{
    hoversOverBackButton = event.coords.isWithinRectangle(backButtonRect);
}

void cChooseHouseState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (event.isType(eKeyEventType::PRESSED)) {
        if (event.isAction(eKeyAction::MENU_BACK)) {
            m_interface->setTransitionToWithFadingOut(GAME_MENU);
        }
    }
}
