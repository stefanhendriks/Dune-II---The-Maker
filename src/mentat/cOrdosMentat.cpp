#include "cOrdosMentat.h"

#include "cProceedButtonCommand.h"
#include "cRepeatButtonCommand.h"
#include "d2tmc.h"
#include "data/gfxmentat.h"
#include "drawers/SDLDrawer.hpp"


cOrdosMentat::cOrdosMentat(bool allowMissionSelect) : cAbstractMentat(allowMissionSelect)
{
    iBackgroundFrame = MENTATO;
    buildLeftButton(gfxmentat->getSurface(BTN_REPEAT), 293, 423);
    buildRightButton(gfxmentat->getSurface(BTN_PROCEED), 466, 423);
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
    renderDrawer->drawSprite(bmp_screen, gfxmentat->getSurface(ORD_EYES01+ iMentatEyes), offsetX + 32, offsetY + 240);
}

void cOrdosMentat::draw_mouth()
{
    renderDrawer->drawSprite(bmp_screen, gfxmentat->getSurface(ORD_MOUTH01+ iMentatMouth), offsetX + 31, offsetY + 270);
}

void cOrdosMentat::interact()
{
    cAbstractMentat::interact();
}