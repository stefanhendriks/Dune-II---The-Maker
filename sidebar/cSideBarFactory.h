/*
 * cSideBarFactory.h
 *
 *  Created on: Aug 2, 2009
 *      Author: Stefan
 */

#ifndef CSIDEBARFACTORY_H_
#define CSIDEBARFACTORY_H_

class cSideBarFactory {

private:
	static cSideBarFactory *instance;

protected:
	cSideBarFactory();
	~cSideBarFactory();

public:
	static cSideBarFactory *getInstance();
	static void destroy();


	cSideBar *createSideBar(cPlayer& thePlayer, int house);

private:

};
#endif /* CSIDEBARFACTORY_H_ */
