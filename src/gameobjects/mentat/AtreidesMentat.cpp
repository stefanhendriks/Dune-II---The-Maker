/**
 * @file AtreidesMentat.cpp
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

#include "AtreidesMentat.h"
#include "data/gfxmentat.h"
#include "game/cGameInterface.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "utils/Log.h"
#include "gui/GuiButton.h"
#include <iostream>
#include "include/cAssert.h"

AtreidesMentat::AtreidesMentat(GameContext* ctx, bool allowMissionSelect) : AbstractMentat(ctx,allowMissionSelect)
{
    d2tm_assert(ctx != nullptr);
    iBackgroundFrame = MENTATA;
    buildLeftButton(gfxmentat->getTexture(BTN_REPEAT), 293, 423);
    buildRightButton(gfxmentat->getTexture(BTN_PROCEED), 466, 423);


    leftGuiButton = GuiButtonBuilder()
            .withRect(*leftButton)        
            .withLabel("Repeat")
            .withTexture(gfxmentat->getTexture(BTN_REPEAT))
            .withRenderer(m_sdlDrawer)
            .withKind(GuiRenderKind::WITH_TEXTURE)
            .onClick([this]() {this->resetSpeak();})
            .build();

    rightGuiButton = GuiButtonBuilder()
            .withRect(*rightButton)        
            .withLabel("Yes")
            .withTexture(gfxmentat->getTexture(BTN_YES))
            .withRenderer(m_sdlDrawer)
            .withKind(GuiRenderKind::WITH_TEXTURE)
            .onClick([this]() {
                Logger::info(COMP_GAME, "AtreidesMentat", "changing state from mentat");
                m_gameInterface->changeStateFromMentat();})
            .build();
}

void AtreidesMentat::think()
{
    // think like base class
    AbstractMentat::think();
}

void AtreidesMentat::draw()
{
    AbstractMentat::draw();
}

void AtreidesMentat::draw_other()
{

}

void AtreidesMentat::draw_eyes()
{
    m_sdlDrawer->renderSprite(gfxmentat->getTexture(ATR_EYES01+ iMentatEyes),  offsetX + 80, offsetY + 241);
}

void AtreidesMentat::draw_mouth()
{
    m_sdlDrawer->renderSprite(gfxmentat->getTexture(ATR_MOUTH01+ iMentatMouth),  offsetX + 80, offsetY + 273);
}
