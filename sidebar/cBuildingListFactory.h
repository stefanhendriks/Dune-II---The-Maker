#pragma once

#include "sidebar/cBuildingList.h"

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

		void initializeList(cBuildingList *list, eListType listType);
		cBuildingList *createList(eListType listType);

};
