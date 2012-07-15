/*
 * cStructurePlacer.h
 *
 *  Created on: 2-aug-2010
 *      Author: Stefan
 *
 * This class is responsible for placing a structure on the map and call
 * necessary other methods of the player that is placing the structure.
 *
 * Ie:
 * - put structure on map
 * - call corresponding update method
 *
 */

#ifndef CSTRUCTUREPLACER_H_
#define CSTRUCTUREPLACER_H_

class cStructurePlacer {

	public:
		cStructurePlacer(cPlayer *thePlayer);
		~cStructurePlacer();

		void placeStructure(int destinationCell, int iStructureTypeId, int healthPercentage);

	protected:

	private:
		cPlayer *player;
};

#endif /* CSTRUCTUREPLACER_H_ */
