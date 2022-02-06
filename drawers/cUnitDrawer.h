/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

// This class is solely responsible for drawing a unit on the screen.

#pragma once

#include "gameobjects/units/cAbstractUnit.h"

class cUnitDrawer {
  private:
	cUnitDrawer();

  public:
	static const cUnitDrawer& getInstance();

	void draw(cAbstractUnit *unit) const;

	// get draw coordinates, taking the abstractUnit as param
	int getDrawX(cAbstractUnit *unit) const;
	int getDrawY(cAbstractUnit *unit) const;
};
