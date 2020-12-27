#include "d2tmh.h"

cBeneMentat::cBeneMentat() {
    iBackgroundFrame = MENTATM;
    buildLeftButton((BITMAP *) gfxmentat[BTN_NO].dat, 293, 423);
    buildRightButton((BITMAP *) gfxmentat[BTN_YES].dat, 466, 423);
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
        std::string houseName = "Unknown";
        if (house == HARKONNEN) {
            houseName = "Harkonnen";
        } else if (house == ATREIDES) {
            houseName = "Atreides";
        } else if (house == ORDOS) {
            houseName = "Ordos";
        }
        alfont_textprintf_aa_ex(bmp_screen, font, 17, 17, makecol(0, 0, 0), makecol(0, 0, 0),
                                "Do you wish to join house %s ?", houseName.c_str());
        alfont_textprintf_aa_ex(bmp_screen, font, 16, 16, makecol(255, 214, 137), makecol(0, 0, 0),
                                "Do you wish to join house %s ?", houseName.c_str());
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