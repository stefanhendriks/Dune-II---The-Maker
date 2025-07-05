#include "cAtreidesMentat.h"

// #include "cProceedButtonCommand.h"
// #include "cRepeatButtonCommand.h"
#include "d2tmc.h"
#include "data/gfxmentat.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "gui/GuiButton.h"
#include <iostream>


cAtreidesMentat::cAtreidesMentat(bool allowMissionSelect) : cAbstractMentat(allowMissionSelect)
{
    iBackgroundFrame = MENTATA;
    buildLeftButton(gfxmentat->getTexture(BTN_REPEAT), 293, 423);
    buildRightButton(gfxmentat->getTexture(BTN_PROCEED), 466, 423);
    // leftButtonCommand = new cRepeatButtonCommand();
    // rightButtonCommand = new cProceedButtonCommand();

    leftGuiButton = GuiButtonBuilder()
            .withRect(*leftButton)        
            .withLabel("Repeat")
            .withTexture(gfxmentat->getTexture(BTN_REPEAT))
            .withKind(eGuiButtonRenderKind::WITH_TEXTURE)
            .onClick([this]() {this->resetSpeak();})
            .build();

    rightGuiButton = GuiButtonBuilder()
            .withRect(*rightButton)        
            .withLabel("Yes")
            .withTexture(gfxmentat->getTexture(BTN_YES))
            .withKind(eGuiButtonRenderKind::WITH_TEXTURE)
            .onClick([this]() {
                logbook("cYesButtonCommand::execute()");
                game.execute(*this);})
            .build();
}

void cAtreidesMentat::think()
{
    // think like base class
    cAbstractMentat::think();
}

void cAtreidesMentat::draw()
{
    cAbstractMentat::draw();
}

void cAtreidesMentat::draw_other()
{

}

void cAtreidesMentat::draw_eyes()
{
    renderDrawer->renderSprite(gfxmentat->getTexture(ATR_EYES01+ iMentatEyes),  offsetX + 80, offsetY + 241);
}

void cAtreidesMentat::draw_mouth()
{
    renderDrawer->renderSprite(gfxmentat->getTexture(ATR_MOUTH01+ iMentatMouth),  offsetX + 80, offsetY + 273);
}

// void cAtreidesMentat::interact()
// {
//     cAbstractMentat::interact();
// }