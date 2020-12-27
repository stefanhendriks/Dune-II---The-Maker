#include "d2tmh.h"

cAtreidesMentat::cAtreidesMentat() {
    iBackgroundFrame = MENTATA;
}

cAtreidesMentat::~cAtreidesMentat() = default;

void cAtreidesMentat::think() {
    logbook("cAtreidesMentat::think");
    // think like base class
    cAbstractMentat::think();
}

void cAtreidesMentat::draw() {
    cAbstractMentat::draw();
}

void cAtreidesMentat::draw_other() {

}

void cAtreidesMentat::draw_eyes() {
    draw_sprite(bmp_screen, (BITMAP *)gfxmentat[ATR_EYES01+ iMentatEyes].dat, 80, 241);
}

void cAtreidesMentat::draw_mouth() {
    draw_sprite(bmp_screen, (BITMAP *)gfxmentat[ATR_MOUTH01+ iMentatMouth].dat, 80, 273);
}

void cAtreidesMentat::interact() {
    cAbstractMentat::interact();
}