#include "d2tmh.h"

cBeneMentat::cBeneMentat() {
    iBackgroundFrame = MENTATM;
    buildLeftButton((BITMAP *) gfxmentat[BTN_REPEAT].dat, 293, 423);
    buildRightButton((BITMAP *) gfxmentat[BTN_PROCEED].dat, 466, 423);
    leftButtonCommand = new cNoButtonCommand();
    rightButtonCommand = new cYesButtonCommand();
}

cBeneMentat::~cBeneMentat() = default;

void cBeneMentat::think() {
    // think like base class
    cAbstractMentat::think();
}

void cBeneMentat::draw() {
    cAbstractMentat::draw();

    // when not speaking, draw 'do you wish to join house x'
    if (state == AWAITING_RESPONSE) {
        alfont_set_font_size(font, 35); // set size
        if (game.iHouse == HARKONNEN) {
            alfont_textprintf_aa_ex(bmp_screen, font, 17, 17, makecol(0, 0, 0), makecol(0, 0, 0),
                                    "Do you wish to join house %s ?", "Harkonnen");
            alfont_textprintf_aa_ex(bmp_screen, font, 16, 16, makecol(255, 214, 137), makecol(0, 0, 0),
                                    "Do you wish to join house %s ?", "Harkonnen");
        } else if (game.iHouse == ATREIDES) {
            alfont_textprintf_aa_ex(bmp_screen, font, 17, 17, makecol(0, 0, 0), makecol(0, 0, 0),
                                    "Do you wish to join house %s ?", "Atreides");
            alfont_textprintf_aa_ex(bmp_screen, font, 16, 16, makecol(255, 214, 137), makecol(0, 0, 0),
                                    "Do you wish to join house %s ?", "Atreides");

        } else if (game.iHouse == ORDOS) {
            alfont_textprintf_aa_ex(bmp_screen, font, 17, 17, makecol(0, 0, 0), makecol(0, 0, 0),
                                    "Do you wish to join house %s ?", "Ordos");
            alfont_textprintf_aa_ex(bmp_screen, font, 16, 16, makecol(255, 214, 137), makecol(0, 0, 0),
                                    "Do you wish to join house %s ?", "Ordos");

        }
    }
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