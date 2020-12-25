#include "d2tmh.h"

cBeneMentat::cBeneMentat() {
    iBackgroundFrame = MENTATM;
}

cBeneMentat::~cBeneMentat() = default;

void cBeneMentat::think() {
    // think like base class
    cAbstractMentat::think();

    // when not speaking, draw 'do you wish to join house x'
    if (TIMER_Speaking < 0) {
        iBackgroundFrame = MEN_WISH;
    }
}

void cBeneMentat::draw() {
    cAbstractMentat::draw();

}

void cBeneMentat::draw_other() {

}

void cBeneMentat::draw_eyes() {
    draw_sprite(bmp_screen, (BITMAP *)gfxmentat[BEN_EYES01+ iMentatEyes].dat, 128, 240);
}

void cBeneMentat::draw_mouth() {
    draw_sprite(bmp_screen, (BITMAP *)gfxmentat[BEN_MOUTH01+ iMentatMouth].dat, 112, 272);
}

void cBeneMentat::interact() {
    cAbstractMentat::interact();
}