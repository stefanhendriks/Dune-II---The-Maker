#include "cBeneMentat.h"

#include "cNoButtonCommand.h"
#include "cYesButtonCommand.h"
#include "data/gfxmentat.h"
#include "d2tmc.h"
#include "player/cPlayer.h"
#include "drawers/cAllegroDrawer.h"

// #include <alfont.h>
#include <allegro/datafile.h>

cBeneMentat::cBeneMentat() : cAbstractMentat(false)
{
    iBackgroundFrame = MENTATM;
    buildLeftButton(gfxmentat->getSurface(BTN_NO), 293, 423);
    buildRightButton(gfxmentat->getSurface(BTN_YES), 466, 423);
    leftButtonCommand = new cNoButtonCommand();
    rightButtonCommand = new cYesButtonCommand();
}

void cBeneMentat::think()
{
    // think like base class
    cAbstractMentat::think();
}

void cBeneMentat::draw()
{
    cAbstractMentat::draw();

    // when not speaking, draw 'do you wish to join house x'
    if (state == AWAITING_RESPONSE) {
        //Mira TEXT alfont_set_font_size(font, 35); // set size
        std::string houseName = cPlayer::getHouseNameForId(house);
        //Mira TEXT alfont_textprintf_aa_ex(bmp_screen, font, offsetX + 17, offsetY + 17, SDL_Color{0, 0, 0), SDL_Color{0, 0, 0), "Do you wish to join house %s ?", houseName.c_str());
        //Mira TEXT alfont_textprintf_aa_ex(bmp_screen, font, offsetX + 16, offsetY + 16, SDL_Color{255, 214, 137), SDL_Color{0, 0, 0),  "Do you wish to join house %s ?", houseName.c_str());
    }
}

void cBeneMentat::draw_other()
{

}

void cBeneMentat::draw_eyes()
{
    renderDrawer->drawSprite(bmp_screen, gfxmentat->getSurface(BEN_EYES01+ iMentatEyes), offsetX + 128, offsetY + 240);
}

void cBeneMentat::draw_mouth()
{
    renderDrawer->drawSprite(bmp_screen, gfxmentat->getSurface(BEN_MOUTH01+ iMentatMouth), offsetX + 112, offsetY + 272);
}

void cBeneMentat::interact()
{
    cAbstractMentat::interact();
}