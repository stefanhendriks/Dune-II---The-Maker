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

public:
	static cSideBarFactory *getInstance();

	cSideBar *createSideBar(cPlayer * player, int techlevel, int house);

private:

};
#endif /* CSIDEBARFACTORY_H_ */
