/*
 * cPlayerUtils.h
 *
 *  Created on: 20-okt-2010
 *      Author: Stefan
 */

#ifndef CPLAYERUTILS_H_
#define CPLAYERUTILS_H_

// this class can be used within a player context (given at construction), or the methods can
// be accessed without the context (the player context is given as param for each method).

class cPlayerUtils {
	public:
		cPlayerUtils();
		cPlayerUtils(cPlayer *thePlayer);
		~cPlayerUtils();

		int getAmountOfStructuresForStructureTypeForPlayer(cPlayer * player, int structureType);
		int getAmountOfStructuresForStructureTypeForPlayer(int structureType);

	protected:

	private:
		cPlayer *player; // the player context, optional

};

#endif /* CPLAYERUTILS_H_ */
