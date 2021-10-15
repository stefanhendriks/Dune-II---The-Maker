#include <algorithm>
#include "d2tmh.h"


cChooseHouseGameState::cChooseHouseGameState(cGame &theGame) : cGameState(theGame) {
    int screen_x = game.screen_x;
    int screen_y = game.screen_y;
    textDrawer = cTextDrawer(bene_font);
    backButtonRect = textDrawer.getAsRectangle(0, screen_y - textDrawer.getFontHeight(), " BACK");

    bmp_Dune = (BITMAP *) gfxinter[BMP_GAME_DUNE].dat;

    int duneAtTheRight = game.screen_x - bmp_Dune->w;
    int duneAlmostAtBottom = game.screen_y - (bmp_Dune->h * 0.90);
    coords_Dune = cPoint(duneAtTheRight, duneAlmostAtBottom);

    bmp_SelectYourHouseTitle = (BITMAP *) gfxinter[BMP_SELECT_YOUR_HOUSE].dat;

    selectYourHouseXCentered = (game.screen_x / 2) - bmp_SelectYourHouseTitle->w / 2;
    coords_SelectYourHouseTitle = cPoint(selectYourHouseXCentered, 0);

    bmp_HouseAtreides = (BITMAP *) gfxinter[BMP_SELECT_HOUSE_ATREIDES].dat;
    bmp_HouseOrdos = (BITMAP *) gfxinter[BMP_SELECT_HOUSE_ORDOS].dat;
    bmp_HouseHarkonnen = (BITMAP *) gfxinter[BMP_SELECT_HOUSE_HARKONNEN].dat;

    int selectYourHouseY = game.screen_y * 0.25f;

    int columnWidth = game.screen_x / 7; // empty, atr, empty, har, empty, ord, empty (7 columns)

    int offset = (columnWidth / 2) - (bmp_HouseAtreides->w / 2);

    houseAtreides = cRectangle((columnWidth * 1) + offset, selectYourHouseY, 90, 98);
    houseOrdos = cRectangle((columnWidth * 3) + offset, selectYourHouseY, 90, 98);
    houseHarkonnen = cRectangle((columnWidth * 5) + offset, selectYourHouseY, 90, 98);

    hoversOverBackButton = false;
}

cChooseHouseGameState::~cChooseHouseGameState() {
    delete backButtonRect;

    // not owner of these
    bmp_Dune = nullptr;
    bmp_SelectYourHouseTitle = nullptr;
    bmp_HouseAtreides = nullptr;
    bmp_HouseOrdos = nullptr;
    bmp_HouseHarkonnen = nullptr;
}

void cChooseHouseGameState::thinkFast() {

}

void cChooseHouseGameState::draw() const {
    cMouse *mouse = game.getMouse();

    // Render the planet Dune a bit downward
    draw_sprite(bmp_screen, bmp_Dune, coords_Dune.x, coords_Dune.y);
    
    // HOUSES
    draw_sprite(bmp_screen, bmp_SelectYourHouseTitle, coords_SelectYourHouseTitle.x, coords_SelectYourHouseTitle.y);

    allegroDrawer->blitSprite(bmp_HouseAtreides, bmp_screen, &houseAtreides);
    allegroDrawer->blitSprite(bmp_HouseOrdos, bmp_screen, &houseOrdos);
    allegroDrawer->blitSprite(bmp_HouseHarkonnen, bmp_screen, &houseHarkonnen);

    // BACK (bottom right
    int color = hoversOverBackButton ? makecol(255, 0, 0) : makecol(255, 255, 255);
    textDrawer.drawText(backButtonRect->getTopLeft(), color, " BACK");
    
    // MOUSE
    mouse->draw();
}

eGameStateType cChooseHouseGameState::getType() {
    return GAMESTATE_SELECT_YOUR_NEXT_CONQUEST;
}

void cChooseHouseGameState::onNotifyMouseEvent(const s_MouseEvent &event) {
    switch (event.eventType) {
        case MOUSE_LEFT_BUTTON_CLICKED:
            onMouseLeftButtonClicked(event);
            break;
        case MOUSE_MOVED_TO:
            onMouseMoved(event);
            break;
    }
}

void cChooseHouseGameState::onMouseLeftButtonClicked(const s_MouseEvent &event) const {

    if (event.coords.isWithinRectangle(&houseAtreides)) {
        game.prepareMentatToTellAboutHouse(ATREIDES);
        play_sound_id(SOUND_ATREIDES);
        game.setState(GAME_TELLHOUSE);
        game.START_FADING_OUT();
    } else if (event.coords.isWithinRectangle(&houseOrdos)) {
        game.prepareMentatToTellAboutHouse(ORDOS);
        play_sound_id(SOUND_ORDOS);
        game.setState(GAME_TELLHOUSE);
        game.START_FADING_OUT();
    } else if (event.coords.isWithinRectangle(&houseHarkonnen)) {
        game.prepareMentatToTellAboutHouse(HARKONNEN);
        play_sound_id(SOUND_HARKONNEN);
        game.setState(GAME_TELLHOUSE);
        game.START_FADING_OUT();
    } else if (event.coords.isWithinRectangle(backButtonRect)) {
        game.setState(GAME_MENU);
        game.START_FADING_OUT();
    }
}

void cChooseHouseGameState::onMouseMoved(const s_MouseEvent &event) {
    hoversOverBackButton = event.coords.isWithinRectangle(backButtonRect);
}
