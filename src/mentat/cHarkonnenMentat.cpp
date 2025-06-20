#include "cHarkonnenMentat.h"

#include "cProceedButtonCommand.h"
#include "cRepeatButtonCommand.h"
#include "d2tmc.h"
#include "data/gfxmentat.h"
#include "drawers/cAllegroDrawer.h"

#include <allegro/datafile.h>


cHarkonnenMentat::cHarkonnenMentat(bool allowMissionSelect) : cAbstractMentat(allowMissionSelect)
{
    iBackgroundFrame = MENTATH;
    buildLeftButton((BITMAP *) gfxmentat[BTN_REPEAT].dat, 293, 423);
    buildRightButton((BITMAP *) gfxmentat[BTN_PROCEED].dat, 466, 423);
    leftButtonCommand = new cRepeatButtonCommand();
    rightButtonCommand = new cProceedButtonCommand();
}

void cHarkonnenMentat::think()
{
    // think like base class
    cAbstractMentat::think();
}

void cHarkonnenMentat::draw()
{
    cAbstractMentat::draw();
}

void cHarkonnenMentat::draw_other()
{

}

void cHarkonnenMentat::draw_eyes()
{
    renderDrawer->drawSprite(bmp_screen, (BITMAP *)gfxmentat[HAR_EYES01+ iMentatEyes].dat, offsetX + 64, offsetY + 256);
}

void cHarkonnenMentat::draw_mouth()
{
    renderDrawer->drawSprite(bmp_screen, (BITMAP *)gfxmentat[HAR_MOUTH01+ iMentatMouth].dat, offsetX + 64, offsetY + 288);
}

void cHarkonnenMentat::interact()
{
    cAbstractMentat::interact();
}