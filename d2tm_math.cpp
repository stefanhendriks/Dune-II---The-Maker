/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks

  Purpose of this file:
  
    math.cpp should deliver all mathematical functions needed in the game.


  */
#include <time.h> 
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "d2tmh.h"

// Is the cell valid?
bool bCellValid(int c)
{
  if (c < 0 || c >= MAX_CELLS) 
	  return false;

  return true;
}

// return the X value from a cell
int iCellGiveX(int c) 
{ 
  //int t = c / MAP_W_MAX;  
  if (c < 0 || c >= MAX_CELLS) 
  {
	  if (DEBUGGING)
		  logbook("ERROR: Encountered invalid cell");

    return -1;
  }
  return (c - ((c/MAP_W_MAX) * MAP_W_MAX)); 
} 

// return the Y value from a cell
int iCellGiveY(int c)
{
    if (c < 0 || c >= MAX_CELLS) 
    {
		if (DEBUGGING)
			logbook("ERROR: Encountered invalid cell");

        return -1;
    }

  return (c / MAP_W_MAX);
}

// make cell number out of X and Y value
int iCellMake(int x, int y)
{
  if (x < 0) x=0;
  if (y < 0) y=0;
  
  int c = (y * MAP_W_MAX) + x;
  
  // FIXED: Do never give a cell number higher then the max!
  if (c >= MAX_CELLS)
      c = (MAX_CELLS-1);

  return c;
}

// returns in fRadians
float fRadians(int x1, int y1, int x2, int y2)
{
  float delta_x = (x2-x1);
  float delta_y = (y2-y1);  
  
  // calculate fRadians
  float r = (atan2(delta_y, delta_x));
  
  // return in fRadians
  return r;
}


// return fDegrees between 2 given x,y positions
float fDegrees(int x1, int y1, int x2, int y2)
{  
  /***
   calculation between two 2D positions, returning the angle.

   Method used:

   x1,y1 is always center of our 'fictial triangle'. 
   Example:

  
    A           B
   x1,y1--------
   \           |
    ----       |
        \------+x2,y2
                C

  
   Using tanges to calculate the angle.

  tan = AB/BC

  
  the distances between A and B , B and C are calculated by delta_x and delta_y. Those will always
  be > 0 due the ABS command.
  ***/

  /*** original - working , old code
  double angle    = -1;         // the angle we will return later on

  int delta_x = (x2-x1);
  int delta_y = (y2-y1);
  
  // tofix: delta_x and delta_y should be swapped, and some other stuff with the angle
  //  note: it works now, so dont touch it yet (dont fix anything that aint broken)
  angle = (atan2(delta_x, delta_y));
  
  double x = angle / PI * 180.0f;

  angle = x;

  // a little bit more modification to make it right
  
  // TODO: Read this entire thing over and write a better and cleaner function!!!!!

  angle += 180;
  
  return angle;
  ***/
  float angle    = -1;         // the angle we will return later on

  float delta_x = (x2-x1);
  float delta_y = (y2-y1);
  
  // TOFIX: Somehow delta_x and delta_y need to be twisted here (should be y , x in the atan2 method)
  // should be fixed some day, it works but I feel itchy about this.
  angle = (atan2(delta_x, delta_y));
  
  // convert to fDegrees
  angle =  angle * (180 / PI);

  angle += 180;
  return angle;
}

// for bullets; bullets have twice as many facing's. (16).
int bullet_face_angle(float angle)
{
  int a = (int)angle;
  int chop = (45/2);        // 45/2 fDegrees is one chop now
  a = (abs)(a-360);
  return (a/chop);
}

// Return the correct face angle (for drawing that is)
int bullet_correct_angle(int face_angle)
{
  int fa=face_angle;  
  return fa;  
}

// Give correct facing angle, used in MAIN.H
// note: Not to be used with drawing yet, use convert_angle() with this result first then
// - face angle is for units only
int face_angle(float angle)
{
  int a = (int)angle;
  
  // since the original results are:
  // 180 = UP           (-90)
  // 90  = RIGHT        (-90)
  // 0   = DOWN         (-90)
  // -90 = LEFT         (-90 (-180 becomes 180 again))
  
  // we have to make it a scale of 0 to 360 in order to chop it correctly.
  // the new results should be:
  // 
  
  // chop 360 fDegrees in 8 pieces
  int chop = 45;        // 45 fDegrees is one chop

  // 360 fDegrees is UP, 270 = RIGHT, 180 = DOWN, 90 = LEFT, 0 is also UP  
  // when deviding 360 / 45 you will get results like:
  // 360 = down
  // 0 = up, etc

  // Because 360 is UP, we need to reverse it now
  //a += 180; // make scale to 360 fDegrees  
  a = (abs)(a-360);
  
  // we now can return the correct facing, but do note that this value is not how the
  // picture is stored in our 8bit pictures. The drawing routine will fix this up for us
  // its a pain in the ass but i hate the unlogical order which Westwood used to put their
  // units in and i dont want to recode stuff here now.

  return (a/chop);
}

// Converts the face_angle produced with the function above, into a correct number for drawing
// correctly.
int convert_angle(int face_angle)
{
  // Drawing works like this:
  // The unit looks at the RIGHT at the 1st picture. So you start at position 0,0 and copy the
  // correct part of that unit (using the properties given by the unit structure database). When
  // a unit looks a different way, you use this number to multiply by the width of a frame in
  // order to start at the correct positions.
  
  // Due the fact that we use 0 as UP , and since WW uses 0 as RIGHT, we have to convert.
  // another problem is that we go clockwise and WW does NOT. 
  
  // How the real facing is................ and what it should be for drawing...
  if (face_angle == FACE_RIGHT)       return 0;
  if (face_angle == FACE_UPRIGHT)     return 1;
  if (face_angle == FACE_UP)          return 2;
  if (face_angle == FACE_UPLEFT)      return 3;
  if (face_angle == FACE_LEFT)        return 4;
  if (face_angle == FACE_DOWNLEFT)    return 5;
  if (face_angle == FACE_DOWN)        return 6;
  if (face_angle == FACE_DOWNRIGHT)   return 7;
   
  assert(false);
  return 0; // theoretically cannot reach here, return 0 in all other cases
}

// return random number between 0 and 'max'
int rnd(int max)
{
  if (max < 1) return 0;
  return (rand() % max);
}

// returns length between 2 points
double length(int x1, int y1, int x2, int y2)
{

  int A = x2-x1;
  int B = y2-y1;

  A *= A;
  B *= B;

  return sqrt((double)(A+B)); // A2 + B2 = C2 :)
}


// ABS
// returns length between 2 points
double ABS_length(int x1, int y1, int x2, int y2)
{
  if (x1 == x2 && y1 == y2) return 1; // when all the same, distance is 1 ... 

  int A = abs(x2-x1) * abs(x2-x1);
  int B = abs(y2-y1) * abs(y2-y1);  
  return sqrt((double)(A+B)); // A2 + B2 = C2 :)
}


// Above
int CELL_ABOVE(int c)
{
  if ((c-MAP_W_MAX) < 0)
    return -1;
  else
    return (c-MAP_W_MAX);
}

// Under
int CELL_UNDER(int c)
{
  if ((c+MAP_W_MAX) >= MAX_CELLS)
    return -1;
  else
    return (c+MAP_W_MAX);
}

// Left
int CELL_LEFT(int c)
{
  int x = iCellGiveX(c);
//  int y = iCellGiveY(c);

  if (x-1 >=0)
  {
	  if (c-1 >= 0)
		  return c-1;

	  return -1;
  }

	  else
    return -1;
}

// Right
int CELL_RIGHT(int c)
{
  int x = iCellGiveX(c);
  //int y = iCellGiveY(c);

  if (x+1 < MAP_W_MAX)
  {
	    if (c+1 < MAX_CELLS) // make sure we don't overdo it
			return c+1;

		return -1;
  }
  else
	  return -1;
}


// Up / LEFT
int CELL_U_LEFT(int c)
{
  if ((CELL_ABOVE(c)-1) > -1)
    return (CELL_ABOVE(c) - 1);      // upper left is ONE row - 1 cell
  else
    return -1;
}

// Up / RIGHT
int CELL_U_RIGHT(int c)
{
  if ((CELL_ABOVE(c)+1) > -1)
    return (CELL_ABOVE(c) + 1);      // upper right is ONE row + 1 cell (to the right)
  else
    return -1;
}

// Low / LEFT
int CELL_L_LEFT(int c)
{
  if ((CELL_UNDER(c)-1) <= MAX_CELLS)
    return (CELL_UNDER(c)-1);
  else
    return -1;
}

// Low / RIGHT
int CELL_L_RIGHT(int c)
{
  if ((CELL_UNDER(c)+1) <= MAX_CELLS)
    return (CELL_UNDER(c)+1);
  else
    return -1;
}


bool CELL_BORDERS(int iOrigin, int iCell)
{
    // return true or false if the iCell borders to iOrigin
    if (CELL_ABOVE(iOrigin) == iCell) return true;
    if (CELL_UNDER(iOrigin) == iCell) return true;
    if (CELL_LEFT(iOrigin) == iCell) return true;
    if (CELL_RIGHT(iOrigin) == iCell) return true;
    
    // 
    if (CELL_U_LEFT(iOrigin) == iCell) return true;
    if (CELL_U_RIGHT(iOrigin) == iCell) return true;
    if (CELL_L_LEFT(iOrigin) == iCell) return true;
    if (CELL_L_RIGHT(iOrigin) == iCell) return true;

    return false;
}

