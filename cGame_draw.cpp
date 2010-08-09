/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2010 (c) code by Stefan Hendriks

  -----------------------------------------------
  Game menu items
  -----------------------------------------------
*/

#include "d2tmh.h"

// Fading between menu items
void cGame::FADE_OUT()
{
    iFadeAction = 1; // fade out
    draw_sprite(bmp_fadeout, bmp_screen, 0, 0);
}

// Drawing of any movie/scene loaded
void cGame::draw_movie(int iType)
{
    if (gfxmovie != NULL && iMovieFrame > -1)
    {

        // drawing only, circulating is done in think function
        draw_sprite(bmp_screen, (BITMAP *)gfxmovie[iMovieFrame].dat, 256, 120);

    }
}

// draw the message
void cGame::draw_message()
{
	if (iMessageAlpha > -1)
	{
		set_trans_blender(0,0,0,iMessageAlpha);
		BITMAP *temp = create_bitmap(480,30);
		clear_bitmap(temp);
		rectfill(temp, 0,0,480,40, makecol(255,0,255));
		draw_sprite(temp, (BITMAP *)gfxinter[BMP_MESSAGEBAR].dat, 0,0);

		// draw message
//		alfont_textprintf(temp, game_font, 13,7, makecol(0,0,0), cMessage);
		alfont_textprintf(temp, game_font, 13,21, makecol(0,0,0), cMessage);

		// draw temp
		draw_trans_sprite(bmp_screen, temp, 1, 42);

		destroy_bitmap(temp);
	}


}

