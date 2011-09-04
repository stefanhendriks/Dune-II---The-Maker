/*
 * cMapUtils.h
 *
 *  Created on: 19-okt-2010
 *      Author: Stefan
 */

#ifndef CMAPUTILS_H_
#define CMAPUTILS_H_

class cMapUtils {
	public:
		cMapUtils(cMap *theMap);
		~cMapUtils();

		bool isCellVisible(cPlayer *thePlayer, int iCell);
		bool isCellVisibleForPlayerId(int playerId, int iCell);

		void clearAllCells();

	protected:

	private:
		cMap *map;
};

#endif /* CMAPUTILS_H_ */
