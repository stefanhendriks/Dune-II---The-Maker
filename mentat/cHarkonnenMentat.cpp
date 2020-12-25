#include "d2tmh.h"

cHarkonnenMentat::cHarkonnenMentat() {
    iBackgroundFrame = MENTATH;
}

cHarkonnenMentat::~cHarkonnenMentat() = default;

void cHarkonnenMentat::think() {
    // think like base class
    cAbstractMentat::think();
}

void cHarkonnenMentat::draw() {
    cAbstractMentat::draw();
}

void cHarkonnenMentat::draw_other() {

}

void cHarkonnenMentat::draw_eyes() {
    draw_sprite(bmp_screen, (BITMAP *)gfxmentat[HAR_EYES01+ iMentatEyes].dat, 64, 256);
}

void cHarkonnenMentat::draw_mouth() {
    draw_sprite(bmp_screen, (BITMAP *)gfxmentat[HAR_MOUTH01+ iMentatMouth].dat, 64, 288);
}

void cHarkonnenMentat::interact() {
    cAbstractMentat::interact();
}