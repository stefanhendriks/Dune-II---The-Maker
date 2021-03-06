/*
 * cBuildingListFactory.h
 *
 *  Created on: Aug 2, 2009
 *      Author: Stefan
 */

#ifndef CBUILDINGLISTFACTORY_H_
#define CBUILDINGLISTFACTORY_H_

class cBuildingListFactory {

	private:
		static cBuildingListFactory *instance;

	protected:
		cBuildingListFactory();
		int getButtonDrawY();
		int getButtonDrawXStart();

	public:
		static cBuildingListFactory *getInstance();
		static void destroy();

		~cBuildingListFactory();

		void initializeList(cBuildingList *list, eListType listType, int house);
		cBuildingList *createList(eListType listType, int house);

};

#endif /* CBUILDINGLISTFACTORY_H_ */
