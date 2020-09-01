/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

  */

#ifndef CELL_CALCULATOR
#define CELL_CALCULATOR

class cCellCalculator {

public:
	cCellCalculator(cMap * theMap);
	cCellCalculator(int theHeight, int theWidth);
	~cCellCalculator();

	// get X / Y from cell
	int getX(int cell);
	int getY(int cell);

	int getAbsoluteX(int cell) {
	    return getX(cell) * 32;
	}

	int getAbsoluteY(int cell) {
	    return getY(cell) * 32;
	}

	int getCellWithMapBorders(int x, int y);

	// get cell from MAP X, Y coordinates
	int getCell(int x, int y);
	int getCellWithMapDimensions(int x, int y, int mapWidth, int mapHeight);

	// calculate distance between 2 coordinates:
	double distance(int x1, int y1, int x2, int y2);
	double distance(int cell1, int cell2);

	int findCloseMapBorderCellRelativelyToDestinationCel(int destinationCell);

	private:

		int height, width;
};

#endif
