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
		virtual ~cBuildingListFactory();

		void initializeList(cBuildingList *list, int listId, int house);
		cBuildingList *createList(int listId, int house);

};

#endif /* CBUILDINGLISTFACTORY_H_ */
