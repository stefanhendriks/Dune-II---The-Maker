/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@dune2k.com / stefanhen83@planet.nl
  Website: http://d2tm.duneii.com

  2001 - 2006 (c) code by Stefan Hendriks

  */

#include "../d2tmh.h"

cHarkonnenMentat::cHarkonnenMentat() {

}

cHarkonnenMentat::~cHarkonnenMentat() {

}

void cHarkonnenMentat::draw_mouth() {	  
	MMEngine->drawSprite((BITMAP *)gfxmentat[HAR_MOUTH01+ getMentatMouth()].dat, bmp_screen, 64, 288);
}

void cHarkonnenMentat::draw_eyes() {	 
	MMEngine->drawSprite((BITMAP *)gfxmentat[HAR_EYES01+ getMentatEyes()].dat, bmp_screen, 64, 256);
}

void cHarkonnenMentat::draw_other() {

}

void cHarkonnenMentat::draw() {
	select_palette( general_palette  );
    game.draw_movie(HARKONNEN);
	draw_sprite(bmp_screen, (BITMAP *)gfxmentat[MENTATH].dat, 0, 0);
	draw_mouth();
	draw_eyes();
	draw_other();
	drawSentences();
}
