/**
 * @file BeneMentat.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "BeneMentat.h"

#include "controls/eKeyAction.h"
#include "include/gfxmentat.h"
#include "data/GfxNames.hpp"
#include "game/GameStateTransition.hpp"
#include "game/MissionFlow.hpp"
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
#include "controls/cKeyboardEvent.h"
#include <array>
#include <string_view>
#include <iostream>

namespace {

// sprite per eyes/mouth frame, the only place that lists the BEN sprites
constexpr std::array<std::string_view, 5> eyesNames = {gfxnames::BEN_EYES01, gfxnames::BEN_EYES02, gfxnames::BEN_EYES03, gfxnames::BEN_EYES04, gfxnames::BEN_EYES05};
constexpr std::array<std::string_view, 5> mouthNames = {gfxnames::BEN_MOUTH01, gfxnames::BEN_MOUTH02, gfxnames::BEN_MOUTH03, gfxnames::BEN_MOUTH04, gfxnames::BEN_MOUTH05};

}

BeneMentat::BeneMentat(GameContext* ctx, cGameSettings* settings, GameStateTransition* stateTransitions, MissionFlow* missionFlow, cPlayer* humanPlayer, s_DataCampaign* dataCampaign) : AbstractMentat(ctx, settings, stateTransitions, nullptr, false), m_humanPlayer(humanPlayer), m_missionFlow(missionFlow), m_dataCampaign(dataCampaign)
{
    d2tm_assert(ctx != nullptr);
    d2tm_assert(dataCampaign != nullptr);
    resolveFaceFrames(eyesNames, mouthNames);
    m_backgroundTexture = gfxmentat->textureId(gfxnames::MENTATM);
    Texture *leftButtonTexture = gfxmentat->getTexture(gfxmentat->textureId(gfxnames::BTN_NO));
    buildLeftButton(leftButtonTexture, 293, 423);
    buildRightButton(gfxmentat->getTexture(gfxmentat->textureId(gfxnames::BTN_YES)), 466, 423);

    leftGuiButton = GuiButtonBuilder()
            .withRect(*leftButton)
            .withLabel("No")
            .withTexture(leftButtonTexture)
            .withRenderer(m_sdlDrawer)
            .withKind(GuiRenderKind::WITH_TEXTURE)
            .onClick([this]() { this->onNoButtonPressed(); })
            .build();

    rightGuiButton = GuiButtonBuilder()
            .withRect(*rightButton)
            .withLabel("Yes")
            .withTexture(gfxmentat->getTexture(gfxmentat->textureId(gfxnames::BTN_YES)))
            .withRenderer(m_sdlDrawer)
            .withKind(GuiRenderKind::WITH_TEXTURE)
            .onClick([this]() { this->onYesButtonPressed(); })
            .build();
}

void BeneMentat::onYesButtonPressed()
{
    Logger::info(COMP_GAME, "BeneMentat::onYesButtonPressed", "Player chose to join house [{}]", cPlayer::getHouseNameForId(this->getHouse()));
    m_stateTransitions->setNextState(eGameState::BRIEFING);
    m_dataCampaign->mission = 1; // first mission
    m_dataCampaign->region  = 1; // and the first "region" so to speak
    m_missionFlow->init();
    m_humanPlayer->setHouse(this->getHouse());
    m_dataCampaign->housePlayer = this->getHouse();
    m_stateTransitions->fadeOut();
}

void BeneMentat::onNoButtonPressed()
{
    Logger::info(COMP_GAME, "BeneMentat::onNoButtonPressed", "Player chose NOT to join house [{}]", cPlayer::getHouseNameForId(this->getHouse()));
    m_humanPlayer->setHouse(GENERALHOUSE);
    m_dataCampaign->housePlayer = GENERALHOUSE;
    m_stateTransitions->setNextState(eGameState::SELECT_HOUSE);
    m_stateTransitions->fadeOut();
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
    m_sdlDrawer->renderSprite(gfxmentat->getTexture(m_eyesTextures[iMentatEyes]), offsetX + 128, offsetY + 240);
}

void BeneMentat::draw_mouth()
{
    m_sdlDrawer->renderSprite(gfxmentat->getTexture(m_mouthTextures[iMentatMouth]), offsetX + 112, offsetY + 272);
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