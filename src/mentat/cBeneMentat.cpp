#include "cBeneMentat.h"

// #include "cNoButtonCommand.h"
// #include "cYesButtonCommand.h"
#include "data/gfxmentat.h"
#include "d2tmc.h"
#include "player/cPlayer.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "drawers/cTextDrawer.h"
#include <SDL2/SDL_ttf.h>
#include "gui/GuiButton.h"
#include <iostream>

cBeneMentat::cBeneMentat() : cAbstractMentat(false)
{
    iBackgroundFrame = MENTATM;
    buildLeftButton(gfxmentat->getTexture(BTN_NO), 293, 423);
    buildRightButton(gfxmentat->getTexture(BTN_YES), 466, 423);

    leftGuiButton = GuiButtonBuilder()
            .withRect(*leftButton)        
            .withLabel("No")
            .withTexture(gfxmentat->getTexture(BTN_NO))
            .withKind(eGuiButtonRenderKind::WITH_TEXTURE)
            .onClick([this]() {
                logbook("cNoButtonCommand::execute()");
                // head back to choose house
                players[HUMAN].setHouse(GENERALHOUSE);
                game.setNextStateToTransitionTo(GAME_SELECT_HOUSE);
                game.initiateFadingOut();})
            .build();

    rightGuiButton = GuiButtonBuilder()
            .withRect(*rightButton)        
            .withLabel("Yes")
            .withTexture(gfxmentat->getTexture(BTN_YES))
            .withKind(eGuiButtonRenderKind::WITH_TEXTURE)
            .onClick([this]() {
                logbook("cYesButtonCommand::execute()");
                game.setNextStateToTransitionTo(GAME_BRIEFING);
                game.m_mission = 1; // first mission
                game.m_region  = 1; // and the first "region" so to speak
                game.missionInit();
                players[HUMAN].setHouse(this->getHouse());
               game.initiateFadingOut();})
            .build();
    // leftButtonCommand = new cNoButtonCommand();
    // rightButtonCommand = new cYesButtonCommand();
    textDrawer = new cTextDrawer(gr_bene_font);

}

void cBeneMentat::think()
{
    // think like base class
    cAbstractMentat::think();
}

void cBeneMentat::draw()
{
    cAbstractMentat::draw();

    // when not speaking, draw 'do you wish to join house x'
    if (state == AWAITING_RESPONSE) {
        std::string houseName = cPlayer::getHouseNameForId(house);
        std::string msg = std::format("Do you wish to join house {} ?", houseName);
        textDrawer->drawText(offsetX + 17, offsetY + 17, Color{0, 0, 0,255}, msg);
        textDrawer->drawText(offsetX + 16, offsetY + 16, Color{255, 214, 137,255}, msg);
    }
}

void cBeneMentat::draw_other()
{

}

void cBeneMentat::draw_eyes()
{
    renderDrawer->renderSprite(gfxmentat->getTexture(BEN_EYES01+ iMentatEyes), offsetX + 128, offsetY + 240);
}

void cBeneMentat::draw_mouth()
{
    renderDrawer->renderSprite(gfxmentat->getTexture(BEN_MOUTH01+ iMentatMouth), offsetX + 112, offsetY + 272);
}

// void cBeneMentat::interact()
// {
//     cAbstractMentat::interact();
// }


// void cBeneMentat::onNotifyMouseEvent(const s_MouseEvent &event)
// {
//     if (leftGuiButton) {
//         leftGuiButton->onNotifyMouseEvent(event);
//     }
//     if (rightGuiButton) {
//         rightGuiButton->onNotifyMouseEvent(event);
//     }
// }
