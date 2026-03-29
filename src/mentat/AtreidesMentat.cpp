#include "AtreidesMentat.h"

#include "d2tmc.h"
#include "game/cGame.h"
#include "data/gfxmentat.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "gui/GuiButton.h"
#include <iostream>
#include <cassert>

AtreidesMentat::AtreidesMentat(GameContext* ctx, bool allowMissionSelect) : AbstractMentat(ctx,allowMissionSelect)
{
    assert(ctx != nullptr);
    iBackgroundFrame = MENTATA;
    buildLeftButton(gfxmentat->getTexture(BTN_REPEAT), 293, 423);
    buildRightButton(gfxmentat->getTexture(BTN_PROCEED), 466, 423);


    leftGuiButton = GuiButtonBuilder()
            .withRect(*leftButton)        
            .withLabel("Repeat")
            .withTexture(gfxmentat->getTexture(BTN_REPEAT))
            .withRenderer(m_renderDrawer)
            .withKind(GuiRenderKind::WITH_TEXTURE)
            .onClick([this]() {this->resetSpeak();})
            .build();

    rightGuiButton = GuiButtonBuilder()
            .withRect(*rightButton)        
            .withLabel("Yes")
            .withTexture(gfxmentat->getTexture(BTN_YES))
            .withRenderer(m_renderDrawer)
            .withKind(GuiRenderKind::WITH_TEXTURE)
            .onClick([this]() {
                logbook("cYesButtonCommand::changeStateFromMentat()");
                game.changeStateFromMentat();})
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
    m_renderDrawer->renderSprite(gfxmentat->getTexture(ATR_EYES01+ iMentatEyes),  offsetX + 80, offsetY + 241);
}

void AtreidesMentat::draw_mouth()
{
    m_renderDrawer->renderSprite(gfxmentat->getTexture(ATR_MOUTH01+ iMentatMouth),  offsetX + 80, offsetY + 273);
}
