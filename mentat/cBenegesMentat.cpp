/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@dune2k.com / stefanhen83@planet.nl
  Website: http://d2tm.duneii.com

  2001 - 2006 (c) code by Stefan Hendriks

  */

#include "../d2tmh.h"

cBenegesMentat::cBenegesMentat() {
	
}

cBenegesMentat::~cBenegesMentat() {

}


void cBenegesMentat::draw_mouth() {
	MMEngine->drawSprite((BITMAP *)gfxmentat[BEN_MOUTH01+ getMentatMouth()].dat, bmp_screen, 112, 272);
}
    
void cBenegesMentat::draw_eyes() {
	MMEngine->drawSprite((BITMAP *)gfxmentat[BEN_EYES01+ getMentatEyes()].dat, bmp_screen, 128, 240);
}

void cBenegesMentat::draw_other() {
	/** nothing to draw here **/
}

void cBenegesMentat::draw() {
	select_palette( general_palette  );
    game.draw_movie(BENEGES);
	draw_sprite(bmp_screen, (BITMAP *)gfxmentat[MENTATM].dat, 0, 0);
	draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x, mouse_y);
    if (isWaitingForAnAnswer()) {
		Mentat->buttonPress(true);
    }
	draw_mouth();
	draw_eyes();
	draw_other();
	drawSentences();
}