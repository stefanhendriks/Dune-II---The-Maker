/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2009 (c) code by Stefan Hendriks

  */


int iCellGiveX(int c);
int iCellGiveY(int c);
int iCellMake(int x, int y);


// Angle stuff for unit facing
float fDegrees(int x1, int y1, int x2, int y2);
float fRadians(int x1, int y1, int x2, int y2);
int face_angle(float angle);
int convert_angle(int face_angle);

// bullet only
int bullet_face_angle(float angle);
int bullet_correct_angle(int face_angle);

bool bCellValid(int c);


// randomizing timer(s)
int rnd(int max);

// lenght calculation
double length(int x1, int y1, int x2, int y2);
double ABS_length(int x1, int y1, int x2, int y2); // returns only value > -1

/* Functions related to cells */
int CELL_ABOVE(int c);        // return which cell is above this one
int CELL_UNDER(int c);        // return which cell is under this one
int CELL_LEFT(int c);         // return which cell is at the left of it
int CELL_RIGHT(int c);        // return which cell is at the right of it
      
// upper and lower right/lefts
int CELL_U_LEFT(int c);
int CELL_U_RIGHT(int c);
int CELL_L_LEFT(int c);
int CELL_L_RIGHT(int c);

bool CELL_BORDERS(int iOrigin, int iCell);

