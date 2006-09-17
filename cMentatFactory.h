#ifndef CMENTATFACTORY_H

class cMentatFactory {

private:
	int type;

	void deleteCurrent();
public:
	cMentatFactory();
	
	void create(int mentatType);
	
};

#endif

