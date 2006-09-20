/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@dune2k.com / stefanhen83@planet.nl
  Website: http://d2tm.duneii.com

  2001 - 2006 (c) code by Stefan Hendriks

  */

#include "../d2tmh.h"

cAtreidesMentat::cAtreidesMentat() {
	
}

cAtreidesMentat::~cAtreidesMentat() {

}


void cAtreidesMentat::draw_mouth() {	 
	MMEngine->drawSprite((BITMAP *)gfxmentat[ATR_MOUTH01+ getMentatMouth()].dat, bmp_screen, 80, 273);
}

void cAtreidesMentat::draw_eyes() {
  MMEngine->drawSprite((BITMAP *)gfxmentat[ATR_EYES01 + getMentatEyes()].dat, bmp_screen, 80, 241);
}

void cAtreidesMentat::draw_other() {
	/** nothing to draw here **/
}

void cAtreidesMentat::draw() {
	select_palette( general_palette  );
    game.draw_movie(ATREIDES);
	draw_sprite(bmp_screen, (BITMAP *)gfxmentat[MENTATA].dat, 0, 0);

	draw_mouth();
	draw_eyes();
	draw_other();
	drawSentences();
}

