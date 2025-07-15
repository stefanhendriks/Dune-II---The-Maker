#include "cOrdosMentat.h"

#include "cProceedButtonCommand.h"
#include "cRepeatButtonCommand.h"
#include "d2tmc.h"
#include "data/gfxmentat.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"

cOrdosMentat::cOrdosMentat(bool allowMissionSelect) : cAbstractMentat(allowMissionSelect)
{
    iBackgroundFrame = MENTATO;
    buildLeftButton(gfxmentat->getTexture(BTN_REPEAT), 293, 423);
    buildRightButton(gfxmentat->getTexture(BTN_PROCEED), 466, 423);
    leftButtonCommand = new cRepeatButtonCommand();
    rightButtonCommand = new cProceedButtonCommand();
}

void cOrdosMentat::think()
{
    // think like base class
    cAbstractMentat::think();
}

void cOrdosMentat::draw()
{
    cAbstractMentat::draw();
}

void cOrdosMentat::draw_other()
{

}

void cOrdosMentat::draw_eyes()
{
    renderDrawer->renderSprite(gfxmentat->getTexture(ORD_EYES01+ iMentatEyes), offsetX + 32, offsetY + 240);
}

void cOrdosMentat::draw_mouth()
{
    renderDrawer->renderSprite(gfxmentat->getTexture(ORD_MOUTH01+ iMentatMouth), offsetX + 31, offsetY + 270);
}

void cOrdosMentat::interact()
{
    cAbstractMentat::interact();
}