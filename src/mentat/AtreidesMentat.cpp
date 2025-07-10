#include "AtreidesMentat.h"

#include "d2tmc.h"
#include "data/gfxmentat.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "gui/GuiButton.h"
#include <iostream>


AtreidesMentat::AtreidesMentat(GameContext* ctx, bool allowMissionSelect) : AbstractMentat(ctx,allowMissionSelect)
{
    iBackgroundFrame = MENTATA;
    buildLeftButton(gfxmentat->getTexture(BTN_REPEAT), 293, 423);
    buildRightButton(gfxmentat->getTexture(BTN_PROCEED), 466, 423);


    leftGuiButton = GuiButtonBuilder()
            .withRect(*leftButton)        
            .withLabel("Repeat")
            .withTexture(gfxmentat->getTexture(BTN_REPEAT))
            .withKind(GuiRenderKind::WITH_TEXTURE)
            .onClick([this]() {this->resetSpeak();})
            .build();

    rightGuiButton = GuiButtonBuilder()
            .withRect(*rightButton)        
            .withLabel("Yes")
            .withTexture(gfxmentat->getTexture(BTN_YES))
            .withKind(GuiRenderKind::WITH_TEXTURE)
            .onClick([this]() {
                logbook("cYesButtonCommand::execute()");
                game.execute(*this);})
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
    renderDrawer->renderSprite(gfxmentat->getTexture(ATR_EYES01+ iMentatEyes),  offsetX + 80, offsetY + 241);
}

void AtreidesMentat::draw_mouth()
{
    renderDrawer->renderSprite(gfxmentat->getTexture(ATR_MOUTH01+ iMentatMouth),  offsetX + 80, offsetY + 273);
}
