#include <algorithm>
#include "d2tmh.h"


cChooseHouseGameState::cChooseHouseGameState(cGame &theGame) : cGameState(theGame) {
    int screen_x = game.screen_x;
    int screen_y = game.screen_y;
    textDrawer = cTextDrawer(bene_font);
    backButtonRect = textDrawer.getAsRectangle(0, screen_y - textDrawer.getFontHeight(), " BACK");
}

cChooseHouseGameState::~cChooseHouseGameState() {
    delete backButtonRect;
}

void cChooseHouseGameState::thinkFast() {

}

void cChooseHouseGameState::draw() {
    cMouse *mouse = game.getMouse();

    // Render the planet Dune a bit downward
    BITMAP *duneBitmap = (BITMAP *) gfxinter[BMP_GAME_DUNE].dat;
    draw_sprite(bmp_screen, duneBitmap, ((game.screen_x - duneBitmap->w)), ((game.screen_y - (duneBitmap->h * 0.90))));

    // HOUSES
    BITMAP *sprite = (BITMAP *) gfxinter[BMP_SELECT_YOUR_HOUSE].dat;
    int selectYourHouseXCentered = (game.screen_x / 2) - sprite->w / 2;
    draw_sprite(bmp_screen, sprite, selectYourHouseXCentered, 0);

    int selectYourHouseY = game.screen_y * 0.25f;

    int columnWidth = game.screen_x / 7; // empty, atr, empty, har, empty, ord, empty (7 columns)
    int offset = (columnWidth / 2) - (((BITMAP *)gfxinter[BMP_SELECT_HOUSE_ATREIDES].dat)->w / 2);
    cRectangle houseAtreides = cRectangle((columnWidth * 1) + offset, selectYourHouseY, 90, 98);
    cRectangle houseOrdos = cRectangle((columnWidth * 3) + offset, selectYourHouseY, 90, 98);
    cRectangle houseHarkonnen = cRectangle((columnWidth * 5) + offset, selectYourHouseY, 90, 98);
    allegroDrawer->blitSprite((BITMAP *)gfxinter[BMP_SELECT_HOUSE_ATREIDES].dat, bmp_screen, &houseAtreides);
    allegroDrawer->blitSprite((BITMAP *)gfxinter[BMP_SELECT_HOUSE_ORDOS].dat, bmp_screen, &houseOrdos);
    allegroDrawer->blitSprite((BITMAP *)gfxinter[BMP_SELECT_HOUSE_HARKONNEN].dat, bmp_screen, &houseHarkonnen);

    // back
    if (backButtonRect) {
        textDrawer.drawText(backButtonRect->getX(), backButtonRect->getY(), makecol(255, 255, 255), " BACK");

        if (mouse->isOverRectangle(backButtonRect)) {
            textDrawer.drawText(backButtonRect->getX(), backButtonRect->getY(), makecol(255, 0, 0), " BACK");
        }
    }

    int transitionToState = -1;

    if (mouse->isLeftButtonClicked()) {
        if (mouse->isOverRectangle(&houseAtreides)) {
            game.prepareMentatToTellAboutHouse(ATREIDES);
            play_sound_id(SOUND_ATREIDES);
            transitionToState = GAME_TELLHOUSE;
        } else if (mouse->isOverRectangle(&houseOrdos)) {
            game.prepareMentatToTellAboutHouse(ORDOS);
            play_sound_id(SOUND_ORDOS);
            transitionToState = GAME_TELLHOUSE;
        } else if (mouse->isOverRectangle(&houseHarkonnen)) {
            game.prepareMentatToTellAboutHouse(HARKONNEN);
            play_sound_id(SOUND_HARKONNEN);
            transitionToState = GAME_TELLHOUSE;
        } else if (mouse->isOverRectangle(backButtonRect)) {
            transitionToState = GAME_MENU;
        }
    }

    // MOUSE
    mouse->draw();

    if (transitionToState > -1) {
        game.setState(transitionToState);
        game.START_FADING_OUT();
    }
}

eGameStateType cChooseHouseGameState::getType() {
    return GAMESTATE_SELECT_YOUR_NEXT_CONQUEST;
}

void cChooseHouseGameState::onNotifyMouseEvent(const s_MouseEvent &event) {
    switch (event.eventType) {
        case MOUSE_LEFT_BUTTON_CLICKED:
            onMouseLeftButtonClicked(event);
            break;
    }
}

void cChooseHouseGameState::onMouseLeftButtonClicked(const s_MouseEvent &event) {

}
