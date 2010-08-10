#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../d2tmh.h"

cCellCalculator::cCellCalculator() {
	// do nothing
}

int cCellCalculator::findCloseMapBorderCellRelativelyToDestinationCel(int destinationCell) {
	assert(destinationCell > -1);
	// Cell x and y coordinates
	int iCllX = getX(destinationCell);
	int iCllY = getY(destinationCell);

	// STEP 1: determine starting
	int iStartCell=-1;
	int lDistance=9999;

	int tDistance=9999;
	int cll=-1;

	// HORIZONTAL cells
	for (int iX=0; iX < game.map_width; iX++) {
		// check when Y = 0 (top)
		tDistance = distance(iX, 0, iCllX, iCllY);

		if (tDistance < lDistance) {
			lDistance = tDistance;

			cll = getCell(iX, 0);

			if (map.occupied(cll) == false) {
				iStartCell = cll;
			}
		}

		// check when Y = map_height (bottom)
		tDistance = distance(iX, game.map_height-1, iCllX, iCllY);

		if (tDistance < lDistance) {
			lDistance = tDistance;

			cll = getCell(iX, game.map_height-1);

			if (map.occupied(cll) == false) {
				iStartCell = cll;
			}
		}
	}

	// VERTICAL cells
	for (int iY=0; iY < game.map_height; iY++)
	{
		// check when X = 0 (left)
		tDistance = distance(0, iY, iCllX, iCllY);

		if (tDistance < lDistance) {
			lDistance = tDistance;

			cll = getCell(0, iY);

			if (map.occupied(cll) == false) {
				iStartCell = cll;
			}
		}

		// check when XY = map_width (bottom)
		tDistance = distance(game.map_width-1, iY, iCllX, iCllY);

		if (tDistance < lDistance) {
			lDistance = tDistance;
			cll = getCell(game.map_width-1, iY);

			if (map.occupied(cll) == false) {
				iStartCell = cll;
			}
		}
	}

	return iStartCell;
}

/**
	The X coordinate is found by finding out how many 'rows' (the Y) are there, then
	the remaining of that value is the X.
**/
int cCellCalculator::getX(int cell) {
	assert(cell > -1);
	assert(cell < MAX_CELLS);
	int org = (cell - ((cell/MAP_W_MAX) * MAP_W_MAX));
	int nw = cell % MAP_W_MAX; // return remainder
	char msg[255];
	sprintf(msg, "Original X is = %d and the new one is %d", org, nw);
	return org;
}

/**
	The Y coordinate is found by finding as many MAP_W_MAX can fit in the given cell
**/
int cCellCalculator::getY(int cell) {
	return (cell / MAP_W_MAX);
}

// get X / Y from structure
int cCellCalculator::getX(cAbstractStructure *structure) {
	return getX(structure->getCell());
}

int cCellCalculator::getY(cAbstractStructure *structure) {
	return getY(structure->getCell());
}

/**
	Return a cell from an X,Y coordinate.

	Remember that coordinates are 1-64 based. While the array in Map (tCell) is 0-till 4096.

	This means that the coordinate 1,1 is NOT the first row, but it is : 0,0. This also means the
	MAX at the right is MAP_W_MAX, but it is MAP_W_MAX - 1. (in this case not 64, but 63 as it is 0-63 instead
	of 1-64).

	This method will not do any fancy tricks to fix the boundaries, instead it will assert its input and output.
**/
int cCellCalculator::getCell(int x, int y) {
	assert(x > -1);
	assert(x < MAP_W_MAX); // should never be higher!
	assert(y > -1);
	assert(y < MAP_W_MAX);

	// create cell
	int cell = (y * MAP_W_MAX) + x;

	assert(cell < MAX_CELLS); // may never be => (will since MAX_CELLS-1 is max in array!)
	assert(cell > -1); // may never be < 0

	return cell;
}

double cCellCalculator::distance(int x1, int y1, int x2, int y2) {
  if (x1 == x2 && y1 == y2) return 1; // when all the same, distance is 1 ...

  int A = abs(x2-x1) * abs(x2-x1);
  int B = abs(y2-y1) * abs(y2-y1);
  return sqrt((double)(A+B)); // get C from A and B
}


/**
	Shortcut method, which takes cells as arguments, creates x1, y1 and x2, y2 out of them
	and runs the normal distance method to get the distance.

**/
double cCellCalculator::distance(int cell1, int cell2) {
	int x1 = getX(cell1);
	int y1 = getY(cell1);

	int x2 = getX(cell2);
	int y2 = getY(cell2);
	return ABS_length(x1, y1, x2, y2);
}

/**
 * Return map cell; taking the map borders into account.
 *
 * @param x
 * @param y
 * @return
 */
int cCellCalculator::getCellWithMapBorders(int x, int y) {
	if (x < 1) x = 1;
	if (y < 1) y = 1;
	if (x > 63) x = 63;
	if (y > 63) y = 63;
	return getCell(x, y);
}

