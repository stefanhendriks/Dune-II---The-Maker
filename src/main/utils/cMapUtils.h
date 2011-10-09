#ifndef CMAPUTILS_H_
#define CMAPUTILS_H_

#include "CellCalculator.h"

class cMapUtils {
	public:
		cMapUtils(cMap *theMap);
		~cMapUtils();

		bool isCellVisible(cPlayer *thePlayer, int iCell);
		bool isCellVisibleForPlayerId(int playerId, int iCell);
		bool isCellWithinMapBorders(int cell);
		bool isWithinMapBorders(int x, int y);

		int checkAndFixXCoordinate(int originalX);
		int checkAndFixYCoordinate(int originalY);

		int createCellWithoutTakingMapBordersIntoAccount(int x, int y);
		int createCell(int x, int y);

		void clearAllCells();

	protected:

	private:
		cMap *map;
		CellCalculator * cellCalculator;
};

#endif /* CMAPUTILS_H_ */
