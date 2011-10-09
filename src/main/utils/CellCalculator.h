/*

 Dune II - The Maker

 Author : Stefan Hendriks
 Contact: stefanhen83@gmail.com
 Website: http://dune2themaker.fundynamic.com

 2001 - 2011 (c) code by Stefan Hendriks

 */

#ifndef CELL_CALCULATOR
#define CELL_CALCULATOR

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <assert.h>

#include "../include/definitions.h"

#include "../map/cMap.h"

class CellCalculator {

	public:
		CellCalculator(cMap * map);
		~CellCalculator();

		// get X / Y from cell
		int getX(int cell);
		int getY(int cell);

		int getCellTakingMapBordersIntoAccount(int x, int y);

		// get cell from X, Y
		int getCell(int x, int y);

		// calculate distance between 2 coordinates:
		double distance(int x1, int y1, int x2, int y2);
		double distance(int cell1, int cell2);

		int findCloseMapBorderCellRelativelyToDestinationCell(int destinationCell);

	private:
		cMap * map;
};

#endif
