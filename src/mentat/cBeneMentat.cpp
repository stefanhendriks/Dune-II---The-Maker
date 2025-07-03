#include "cBeneMentat.h"

#include "cNoButtonCommand.h"
#include "cYesButtonCommand.h"
#include "data/gfxmentat.h"
#include "d2tmc.h"
#include "player/cPlayer.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "drawers/cTextDrawer.h"
#include <SDL2/SDL_ttf.h>

cBeneMentat::cBeneMentat() : cAbstractMentat(false)
{
    iBackgroundFrame = MENTATM;
    buildLeftButton(gfxmentat->getTexture(BTN_NO), 293, 423);
    buildRightButton(gfxmentat->getTexture(BTN_YES), 466, 423);
    leftButtonCommand = new cNoButtonCommand();
    rightButtonCommand = new cYesButtonCommand();
    textDrawer = new cTextDrawer(gr_bene_font);
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
        std::string msg = fmt::format("Do you wish to join house {} ?", houseName);
        //Mira TEXT alfont_textprintf_aa_ex(bmp_screen, font, offsetX + 17, offsetY + 17, Color{0, 0, 0), Color{0, 0, 0), "Do you wish to join house %s ?", houseName.c_str());
        textDrawer->drawText(offsetX + 17, offsetY + 17, Color{0, 0, 0,255}, msg);
        //Mira TEXT alfont_textprintf_aa_ex(bmp_screen, font, offsetX + 16, offsetY + 16, Color{255, 214, 137), Color{0, 0, 0),  "Do you wish to join house %s ?", houseName.c_str());
        textDrawer->drawText(offsetX + 16, offsetY + 16, Color{255, 214, 137,255}, msg);
    }
}

void cBeneMentat::draw_other()
{

}

void cBeneMentat::draw_eyes()
{
    renderDrawer->renderSprite(gfxmentat->getTexture(BEN_EYES01+ iMentatEyes), offsetX + 128, offsetY + 240);
}

void cBeneMentat::draw_mouth()
{
    renderDrawer->renderSprite(gfxmentat->getTexture(BEN_MOUTH01+ iMentatMouth), offsetX + 112, offsetY + 272);
}

void cBeneMentat::interact()
{
    cAbstractMentat::interact();
}