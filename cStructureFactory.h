#ifndef STRUCTURE_FACTORY
#define STRUCTURE_FACTORY

class cStructureFactory {

private:
	static cStructureFactory *instance;

protected:
	cStructureFactory();

public:

	static cStructureFactory *getInstance();
	
	void deleteStructure(cStructure *structure);
	cStructure *createStructure(int type);

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