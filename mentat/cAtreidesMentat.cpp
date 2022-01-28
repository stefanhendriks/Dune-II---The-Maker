#include "d2tmh.h"

#include <allegro.h>

cAtreidesMentat::cAtreidesMentat() {
    iBackgroundFrame = MENTATA;
    buildLeftButton((BITMAP *) gfxmentat[BTN_REPEAT].dat, 293, 423);
    buildRightButton((BITMAP *) gfxmentat[BTN_PROCEED].dat, 466, 423);
    leftButtonCommand = new cRepeatButtonCommand();
    rightButtonCommand = new cProceedButtonCommand();
}

cAtreidesMentat::~cAtreidesMentat() = default;

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
    draw_sprite(bmp_screen, (BITMAP *)gfxmentat[ATR_EYES01+ iMentatEyes].dat,  offsetX + 80, offsetY + 241);
}

void cAtreidesMentat::draw_mouth() {
    draw_sprite(bmp_screen, (BITMAP *)gfxmentat[ATR_MOUTH01+ iMentatMouth].dat,  offsetX + 80, offsetY + 273);
}

void cAtreidesMentat::interact() {
    cAbstractMentat::interact();
}