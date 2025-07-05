#include "HarkonnenMentat.h"

// #include "cProceedButtonCommand.h"
// #include "cRepeatButtonCommand.h"
#include "d2tmc.h"
#include "data/gfxmentat.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "gui/GuiButton.h"

HarkonnenMentat::HarkonnenMentat(bool allowMissionSelect) : AbstractMentat(allowMissionSelect)
{
    iBackgroundFrame = MENTATH;
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

void HarkonnenMentat::think()
{
    // think like base class
    AbstractMentat::think();
}

void HarkonnenMentat::draw()
{
    AbstractMentat::draw();
}

void HarkonnenMentat::draw_other()
{

}

void HarkonnenMentat::draw_eyes()
{
    renderDrawer->renderSprite(gfxmentat->getTexture(HAR_EYES01+ iMentatEyes), offsetX + 64, offsetY + 256);
}

void HarkonnenMentat::draw_mouth()
{
    renderDrawer->renderSprite(gfxmentat->getTexture(HAR_MOUTH01+ iMentatMouth), offsetX + 64, offsetY + 288);
}

// void cHarkonnenMentat::interact()
// {
//     cAbstractMentat::interact();
// }