/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@dune2k.com / stefanhen83@planet.nl
  Website: http://d2tm.duneii.com

  2001 - 2006 (c) code by Stefan Hendriks

  */

#include "../d2tmh.h"

cOrdosMentat::cOrdosMentat() {

}

cOrdosMentat::~cOrdosMentat() {

}


void cOrdosMentat::draw_mouth() {	
	MMEngine->drawSprite((BITMAP *)gfxmentat[ORD_MOUTH01+ getMentatMouth()].dat, bmp_screen, 31, 270);
}

void cOrdosMentat::draw_eyes() {
	MMEngine->drawSprite((BITMAP *)gfxmentat[ORD_EYES01+ getMentatEyes()].dat, bmp_screen, 32, 240);
}

void cOrdosMentat::draw_other() {
	/** the ordos ring **/
}

void cOrdosMentat::draw() {
	select_palette( general_palette  );
    game.draw_movie(ORDOS);
	draw_sprite(bmp_screen, (BITMAP *)gfxmentat[MENTATO].dat, 0, 0);
	draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x, mouse_y);
    if (isWaitingForAnAnswer()) {
		Mentat->buttonPress(false);
    }
	draw_mouth();
	draw_eyes();
	draw_other();
	drawSentences();
}
