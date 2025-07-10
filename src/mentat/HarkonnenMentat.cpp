#include "HarkonnenMentat.h"
#include "d2tmc.h"
#include "data/gfxmentat.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "gui/GuiButton.h"
#include "context/GameContext.hpp"

HarkonnenMentat::HarkonnenMentat(GameContext* ctx, bool allowMissionSelect) : AbstractMentat(ctx, allowMissionSelect)
{
    iBackgroundFrame = MENTATH;
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