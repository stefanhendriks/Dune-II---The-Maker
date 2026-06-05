#include "BeneMentat.h"

#include "controls/eKeyAction.h"
#include "data/gfxmentat.h"
#include "game/cGameInterface.h"
#include "include/eGameState.h"
#include "include/sDataCampaign.h"
#include "gameobjects/players/cPlayer.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "utils/Log.h"
#include "drawers/cTextDrawer.h"
#include <SDL3_ttf/SDL_ttf.h>
#include "gui/GuiButton.h"
#include "context/GameContext.hpp"
#include "context/cInfoContext.h"
#include <iostream>

BeneMentat::BeneMentat(GameContext* ctx, s_DataCampaign* dataCampaign) : AbstractMentat(ctx, false), m_dataCampaign(dataCampaign)
{
    d2tm_assert(ctx != nullptr);
    d2tm_assert(dataCampaign != nullptr);
    iBackgroundFrame = MENTATM;
    buildLeftButton(gfxmentat->getTexture(BTN_NO), 293, 423);
    buildRightButton(gfxmentat->getTexture(BTN_YES), 466, 423);

    leftGuiButton = GuiButtonBuilder()
            .withRect(*leftButton)
            .withLabel("No")
            .withTexture(gfxmentat->getTexture(BTN_NO))
            .withRenderer(m_renderDrawer)
            .withKind(GuiRenderKind::WITH_TEXTURE)
            .onClick([this]() { this->onNoButtonPressed(); })
            .build();

    rightGuiButton = GuiButtonBuilder()
            .withRect(*rightButton)
            .withLabel("Yes")
            .withTexture(gfxmentat->getTexture(BTN_YES))
            .withRenderer(m_renderDrawer)
            .withKind(GuiRenderKind::WITH_TEXTURE)
            .onClick([this]() { this->onYesButtonPressed(); })
            .build();
}

void BeneMentat::onYesButtonPressed()
{
    Logger::info(COMP_GAME, "BeneMentat::onYesButtonPressed", "Player chose to join house [{}]", cPlayer::getHouseNameForId(this->getHouse()));
    m_gameInterface->setNextStateToTransitionTo(GAME_BRIEFING);
    m_dataCampaign->mission = 1; // first mission
    m_dataCampaign->region  = 1; // and the first "region" so to speak
    m_gameInterface->missionInit();
    m_gameInterface->getPlayer(HUMAN)->setHouse(this->getHouse());
    m_dataCampaign->housePlayer = this->getHouse();
    m_gameInterface->initiateFadingOut();
}

void BeneMentat::onNoButtonPressed()
{
    Logger::info(COMP_GAME, "BeneMentat::onNoButtonPressed", "Player chose NOT to join house [{}]", cPlayer::getHouseNameForId(this->getHouse()));
    m_gameInterface->getPlayer(HUMAN)->setHouse(GENERALHOUSE);
    m_dataCampaign->housePlayer = GENERALHOUSE;
    m_gameInterface->setNextStateToTransitionTo(GAME_SELECT_HOUSE);
    m_gameInterface->initiateFadingOut();
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
    m_renderDrawer->renderSprite(gfxmentat->getTexture(BEN_EYES01+ iMentatEyes), offsetX + 128, offsetY + 240);
}

void BeneMentat::draw_mouth()
{
    m_renderDrawer->renderSprite(gfxmentat->getTexture(BEN_MOUTH01+ iMentatMouth), offsetX + 112, offsetY + 272);
}

void BeneMentat::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (event.isAction(eKeyAction::MENU_CONFIRM)) {
        this->onYesButtonPressed();
    }
    else
    if (event.isAction(eKeyAction::MENU_CANCEL)) {
        this->onNoButtonPressed();
    }
}