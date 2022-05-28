#include "cOrdosMentat.h"

#include "cProceedButtonCommand.h"
#include "cRepeatButtonCommand.h"
#include "d2tmc.h"
#include "data/gfxmentat.h"

#include <allegro.h>

cOrdosMentat::cOrdosMentat(bool allowMissionSelect) : cAbstractMentat(allowMissionSelect) {
    iBackgroundFrame = MENTATO;
    buildLeftButton((BITMAP *) gfxmentat[BTN_REPEAT].dat, 293, 423);
    buildRightButton((BITMAP *) gfxmentat[BTN_PROCEED].dat, 466, 423);
    leftButtonCommand = new cRepeatButtonCommand();
    rightButtonCommand = new cProceedButtonCommand();
}

void cOrdosMentat::think() {
    // think like base class
    cAbstractMentat::think();
}

void cOrdosMentat::draw() {
    cAbstractMentat::draw();
}

void cOrdosMentat::draw_other() {

}

void cOrdosMentat::draw_eyes() {
    draw_sprite(bmp_screen, (BITMAP *)gfxmentat[ORD_EYES01+ iMentatEyes].dat, offsetX + 32, offsetY + 240);
}

void cOrdosMentat::draw_mouth() {
    draw_sprite(bmp_screen, (BITMAP *)gfxmentat[ORD_MOUTH01+ iMentatMouth].dat, offsetX + 31, offsetY + 270);
}

void cOrdosMentat::interact() {
    cAbstractMentat::interact();
}