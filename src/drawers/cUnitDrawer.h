/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2009 (c) code by Stefan Hendriks

  */

// This class is solely responsible for drawing a unit on the screen.

#ifndef CUNIT_DRAWER
#define CUNIT_DRAWER

class cUnitDrawer {

private:
	static cUnitDrawer *instance;

protected:
	cUnitDrawer();

public:

	static cUnitDrawer *getInstance();

	void draw(cAbstractUnit *unit);

	// get draw coordinates, taking the abstractUnit as param
	int getDrawX(cAbstractUnit *unit);
	int getDrawY(cAbstractUnit *unit);

};

#endif
