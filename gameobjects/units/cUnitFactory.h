/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#pragma once

#include "cAbstractUnit.h"

class cUnitFactory {
  protected:
	  cUnitFactory();

  public:
    static cUnitFactory& getInstance();

	  void deleteUnit(cAbstractUnit *unit);
	  cAbstractUnit *createUnit(int type);
};

