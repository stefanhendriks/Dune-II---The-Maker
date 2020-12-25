/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

  */

#include "include/d2tmh.h"

void REGION_SETUP(int iMission, int iHouse) {
    // The first mission, nothing is 'ready', as the pieces gets placed and taken by the houses.
    // Later, after mission 2, the pieces are already taken. Thats what this function takes care off
    // making sure everything is 'there' to go on with. Hard-coded stuff.

    // First step:
    // remove all pieces (house properties)
    for (int i = 0; i < MAX_REGIONS; i++)
        world[i].bSelectable = false;

    // clear conquer stuff
    memset(game.iRegionConquer, -1, sizeof(game.iRegionConquer));
    memset(game.iRegionHouse, -1, sizeof(game.iRegionHouse));
    memset(game.cRegionText, 0, sizeof(game.cRegionText));

    // Per mission assign:
    // Every house has a different campaign, so...

    INI_Load_Regionfile(iHouse, iMission);

    // prepare players, so we know house index == player index (for colorizing region pieces)
    for (int i = 1; i < FREMEN; i++) {
        player[i].init(i);
        player[i].setHouse(i);
    }

    return;
}

// add a new region
void REGION_NEW(int x, int y, int iAlpha, int iHouse, int iTile)
{
    int iNew=-1;

    for (int i=0; i < MAX_REGIONS; i++)
        if (world[i].x < 0 || world[i].y < 0)
        {
            iNew=i;
            break;
        }

    // invalid index
    if (iNew < 0)
        return;

    world[iNew].x = x;
    world[iNew].y = y;
    world[iNew].iAlpha = iAlpha;
    world[iNew].iHouse = iHouse;
    world[iNew].iTile  = iTile;
}

// draw a region
void REGION_DRAW(cRegion &regionPiece) {
    if (regionPiece.iAlpha <= 0) {
        // no alpha, no use in drawing
        return;
    }

    int screenBitDepth = bitmap_color_depth(bmp_screen);

    // HACK HACK - Using a temp player variable, we do a trick to calculate the proper palette for this
    // highly not efficient.... but will do for now
    if (regionPiece.iHouse > -1) {
        cPlayer &temp = player[regionPiece.iHouse];
        select_palette(temp.pal);           // retrieve pal


        // alpha is lower then 255
        if (regionPiece.iAlpha < 255) {
            regionPiece.iAlpha += 7;

            if (game.iRegionState <= 2)
                if (MOUSE_BTN_LEFT())
                    regionPiece.iAlpha += 25;
        }

        // When alpha is 255 or higher, do not use trans_sprite, which is useless
        if (regionPiece.iAlpha >= 255) {
            draw_sprite(bmp_screen, (BITMAP *)gfxworld[regionPiece.iTile].dat, regionPiece.x, regionPiece.y);
        } else {
            // Draw region with trans_sprite, the trick is that we have to convert it to 16 bit first.
            set_trans_blender(0, 0, 0, regionPiece.iAlpha);				// set blender
            BITMAP *tempregion=create_bitmap_ex(screenBitDepth, 256, 256);				// 16 bit bmp
            clear_to_color(tempregion, makecol(255,0,255));			// clear
            draw_sprite(tempregion, (BITMAP *)gfxworld[regionPiece.iTile].dat, 0, 0); // copy
            draw_trans_sprite(bmp_screen, tempregion, regionPiece.x, regionPiece.y); //	draw trans
            destroy_bitmap(tempregion); // destroy temp
        }
    } // House > -1

    // select your next conquest... always draw them in the human playing house color
    if (regionPiece.bSelectable && game.iRegionState > 2) {
        cPlayer &temp = player[game.iHouse];
        select_palette(temp.pal);           // retrieve pal

        // House < 0
        if (regionPiece.iHouse < 0)
        {
            if (regionPiece.iAlpha < 255)
                regionPiece.iAlpha += 5;
        }

        // Alpha >= 255
        if (regionPiece.iAlpha >= 255)
        {
            draw_sprite(bmp_screen, (BITMAP *)gfxworld[regionPiece.iTile].dat, regionPiece.x, regionPiece.y);
            regionPiece.iAlpha = 1;
        }
        else
        {
            set_trans_blender(0, 0, 0, regionPiece.iAlpha);
            BITMAP *tempregion=create_bitmap_ex(screenBitDepth, 256, 256);
            clear_to_color(tempregion, makecol(255,0,255));
            draw_sprite(tempregion, (BITMAP *)gfxworld[regionPiece.iTile].dat, 0, 0);
            draw_trans_sprite(bmp_screen, tempregion, regionPiece.x, regionPiece.y);
            destroy_bitmap(tempregion);
        }
    }
} // End of function

int REGION_OVER()
{
   // what region is this mouse on?
   // 16, 73

   // when mouse is not even on the map, return -1
   if (mouse_y < 72 || mouse_y > 313 || mouse_x < 16 || mouse_x > 624)
       return -1;

   // from here, we are on a region
   int iRegion=-1;

   // temp bitmap to read from
   BITMAP *tempreg=create_bitmap_ex(8, 640,480); // 8 bit bitmap
   select_palette( general_palette); // default palette
   clear(tempreg); // clear bitmap

   draw_sprite(tempreg, (BITMAP *)gfxworld[WORLD_DUNE_CLICK].dat, 16, 73);

   int c = getpixel(tempreg, mouse_x, mouse_y);

   //alfont_textprintf(bmp_screen, bene_font, 17,17, makecol(0,0,0), "region %d", c-1);

   iRegion=c-1;
   destroy_bitmap(tempreg);
   return iRegion;
}

