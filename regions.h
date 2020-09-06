/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

  */

class cRegion
{
public:
    int iHouse;         // who owns this?
    int x, y;           // x and y position of the region
    int iAlpha;         // Alpha (0 = not visible, > 0 goes up to 255)
    int iTile;          // tile refering to gfxworld.dat
    bool bSelectable;   // selectable for attacking (default = false)
};

void REGION_DRAW(cRegion &regionPiece); // draw this region
int  REGION_OVER(); // what region is the mouse on?
void REGION_SETUP(int iMission, int iHouse);
void REGION_NEW(int x, int y, int iAlpha, int iHouse, int iTile);
