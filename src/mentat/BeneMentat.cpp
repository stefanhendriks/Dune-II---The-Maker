#include "BeneMentat.h"

#include "data/gfxmentat.h"
#include "d2tmc.h"
#include "include/sDataCampaign.h"
#include "player/cPlayer.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "drawers/cTextDrawer.h"
#include <SDL2/SDL_ttf.h>
#include "gui/GuiButton.h"
#include "context/GameContext.hpp"
#include <iostream>

BeneMentat::BeneMentat(GameContext* ctx, s_DataCampaign* dataCampaign) : AbstractMentat(ctx, false), m_dataCampaign(dataCampaign)
{
    iBackgroundFrame = MENTATM;
    buildLeftButton(gfxmentat->getTexture(BTN_NO), 293, 423);
    buildRightButton(gfxmentat->getTexture(BTN_YES), 466, 423);

    leftGuiButton = GuiButtonBuilder()
            .withRect(*leftButton)        
            .withLabel("No")
            .withTexture(gfxmentat->getTexture(BTN_NO))
            .withKind(GuiRenderKind::WITH_TEXTURE)
            .onClick([this]() { this->onNoButtonPressed(); })
            .build();

    rightGuiButton = GuiButtonBuilder()
            .withRect(*rightButton)        
            .withLabel("Yes")
            .withTexture(gfxmentat->getTexture(BTN_YES))
            .withKind(GuiRenderKind::WITH_TEXTURE)
            .onClick([this]() { this->onYesButtonPressed(); })
            .build();
}

void BeneMentat::onYesButtonPressed()
{
    logbook("cYesButtonCommand::execute()");
    game.setNextStateToTransitionTo(GAME_BRIEFING);
    m_dataCampaign->mission = 1; // first mission
    m_dataCampaign->region  = 1; // and the first "region" so to speak
    game.missionInit();
    players[HUMAN].setHouse(this->getHouse());
    game.initiateFadingOut();
}

void BeneMentat::onNoButtonPressed()
{
    logbook("cNoButtonCommand::execute()");
    // head back to choose house
    players[HUMAN].setHouse(GENERALHOUSE);
    game.setNextStateToTransitionTo(GAME_SELECT_HOUSE);
    game.initiateFadingOut();
}

void BeneMentat::think()
{
    // think like base class
    AbstractMentat::think();
}

void BeneMentat::draw()
{
    AbstractMentat::draw();

    // when not speaking, draw 'do you wish to join house x'
    if (state == AWAITING_RESPONSE) {
        std::string houseName = cPlayer::getHouseNameForId(house);
        std::string msg = std::format("Do you wish to join house {} ?", houseName);
        m_textDrawer->drawText(offsetX + 17, offsetY + 17, Color{0, 0, 0,255}, msg);
        m_textDrawer->drawText(offsetX + 16, offsetY + 16, Color{255, 214, 137,255}, msg);
    }
}

void BeneMentat::draw_other()
{
}

void BeneMentat::draw_eyes()
{
    renderDrawer->renderSprite(gfxmentat->getTexture(BEN_EYES01+ iMentatEyes), offsetX + 128, offsetY + 240);
}

void BeneMentat::draw_mouth()
{
    renderDrawer->renderSprite(gfxmentat->getTexture(BEN_MOUTH01+ iMentatMouth), offsetX + 112, offsetY + 272);
}

void BeneMentat::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (event.hasKey(SDL_SCANCODE_RETURN)) {
        this->onYesButtonPressed();
    }
    else
    if (event.hasKey(SDL_SCANCODE_BACKSPACE)) {
        this->onNoButtonPressed();
    }
}