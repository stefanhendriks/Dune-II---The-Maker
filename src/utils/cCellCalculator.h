/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2009 (c) code by Stefan Hendriks

  */

#ifndef CELL_CALCULATOR
#define CELL_CALCULATOR

class cCellCalculator {

public:
	cCellCalculator(cMap * theMap);
	~cCellCalculator();

	// get X / Y from cell
	int getX(int cell);
	int getY(int cell);

	int getCellWithMapBorders(int x, int y);

	// get cell from X, Y
	int getCell(int x, int y);

	// calculate distance between 2 coordinates:
	double distance(int x1, int y1, int x2, int y2);
	double distance(int cell1, int cell2);

	int findCloseMapBorderCellRelativelyToDestinationCel(int destinationCell);

	private:

		int height, width;
};

#endif
