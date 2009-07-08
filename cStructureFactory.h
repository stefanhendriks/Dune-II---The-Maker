/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks

  */

#ifndef STRUCTURE_FACTORY
#define STRUCTURE_FACTORY

class cStructureFactory {

private:
	static cStructureFactory *instance;

protected:
	cStructureFactory();

public:

	static cStructureFactory *getInstance();
	
	void deleteStructureInstance(cStructure *structure);
	
	cStructure *createStructureInstance(int type);

	cStructure *createStructure(int iCell, int iStructureType, int iPlayer, int iPercent);
	cStructure *createStructure(int iCell, int iStructureType, int iPlayer);

	int getFreeSlot();

	void clearFogForStructureType(int iCell, cStructure *str);
	void clearFogForStructureType(int iCell, int iStructureType, int iSight, int iPlayer);

	void placeStructure(int iCell, int iStructureType, int iPlayer);

	int getSlabStatus(int iCell, int iStructureType, int iUnitIDToIgnore);

	void powerUp(int iStructureType, int iPlayer);
	void powerDown(int iStructureType, int iPlayer);

};
/*
class Singleton 
  {
  public:
      static Singleton* Instance();
  protected:
      Singleton();
      Singleton(const Singleton&);
      Singleton& operator= (const Singleton&);
  private:
      static Singleton* pinstance;
  };*/

#endif