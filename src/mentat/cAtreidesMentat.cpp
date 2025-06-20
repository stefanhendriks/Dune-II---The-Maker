#include "cAtreidesMentat.h"

#include "cProceedButtonCommand.h"
#include "cRepeatButtonCommand.h"
#include "d2tmc.h"
#include "data/gfxmentat.h"
#include "drawers/cAllegroDrawer.h"

#include <allegro/datafile.h>

cAtreidesMentat::cAtreidesMentat(bool allowMissionSelect) : cAbstractMentat(allowMissionSelect) {
    iBackgroundFrame = MENTATA;
    buildLeftButton((BITMAP *) gfxmentat[BTN_REPEAT].dat, 293, 423);
    buildRightButton((BITMAP *) gfxmentat[BTN_PROCEED].dat, 466, 423);
    leftButtonCommand = new cRepeatButtonCommand();
    rightButtonCommand = new cProceedButtonCommand();
}

void cAtreidesMentat::think() {
    // think like base class
    cAbstractMentat::think();
}

void cAtreidesMentat::draw() {
    cAbstractMentat::draw();
}

void cAtreidesMentat::draw_other() {

}

void cAtreidesMentat::draw_eyes() {
    renderDrawer->drawSprite(bmp_screen, (BITMAP *)gfxmentat[ATR_EYES01+ iMentatEyes].dat,  offsetX + 80, offsetY + 241);
}

void cAtreidesMentat::draw_mouth() {
    renderDrawer->drawSprite(bmp_screen, (BITMAP *)gfxmentat[ATR_MOUTH01+ iMentatMouth].dat,  offsetX + 80, offsetY + 273);
}

void cAtreidesMentat::interact() {
    cAbstractMentat::interact();
}