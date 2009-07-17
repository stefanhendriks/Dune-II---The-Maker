/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks

  */

#ifndef CELL_CALCULATOR
#define CELL_CALCULATOR

class cCellCalculator {

public:
	cCellCalculator();

	// get X / Y from cell
	int getX(int cell);
	int getY(int cell);

	// get X / Y from structure
	int getX(cAbstractStructure *structure);
	int getY(cAbstractStructure *structure);

	// get cell from X, Y
	int getCell(int x, int y);

	// calculate distance between 2 coordinates:
	double distance(int x1, int y1, int x2, int y2);
	double distance(int cell1, int cell2);

};

#endif