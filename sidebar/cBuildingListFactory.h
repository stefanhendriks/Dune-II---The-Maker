/*
 * cBuildingListFactory.h
 *
 *  Created on: Aug 2, 2009
 *      Author: Stefan
 */

#pragma once

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

		void initializeList(cBuildingList *list, eListType listType);
		cBuildingList *createList(eListType listType);

};
