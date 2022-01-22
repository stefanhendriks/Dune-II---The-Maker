/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

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
