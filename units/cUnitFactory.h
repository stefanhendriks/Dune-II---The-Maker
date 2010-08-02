/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks

  */

#ifndef UNIT_FACTORY
#define UNIT_FACTORY
class cUnitFactory {

private:
	static cUnitFactory *instance;

protected:
	cUnitFactory();

public:

	static cUnitFactory *getInstance();

	void deleteUnit(cAbstractUnit *unit);
	cAbstractUnit *createUnit(int type);

};

#endif
