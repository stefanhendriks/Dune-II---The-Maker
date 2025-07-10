#include "OrdosMentat.h"
#include "d2tmc.h"
#include "data/gfxmentat.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "gui/GuiButton.h"
#include "context/GameContext.hpp"

OrdosMentat::OrdosMentat(GameContext* ctx, bool allowMissionSelect) : AbstractMentat(ctx, allowMissionSelect)
{
    iBackgroundFrame = MENTATO;
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

void OrdosMentat::think()
{
    // think like base class
    AbstractMentat::think();
}

void OrdosMentat::draw()
{
    AbstractMentat::draw();
}

void OrdosMentat::draw_other()
{

}

void OrdosMentat::draw_eyes()
{
    renderDrawer->renderSprite(gfxmentat->getTexture(ORD_EYES01+ iMentatEyes), offsetX + 32, offsetY + 240);
}

void OrdosMentat::draw_mouth()
{
    renderDrawer->renderSprite(gfxmentat->getTexture(ORD_MOUTH01+ iMentatMouth), offsetX + 31, offsetY + 270);
}