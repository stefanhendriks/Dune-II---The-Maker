/*
 * cPlayerStatistics.h
 *
 *  Created on: 1-aug-2010
 *      Author: Stefan
 */

#ifndef CPLAYERSTATISTICS_H_
#define CPLAYERSTATISTICS_H_

class cPlayerStatistics {
	public:
		cPlayerStatistics();
		~cPlayerStatistics();

		// set methods
		void setEnemyUnitsDestroyed(int theEnemyUnitsDestroyed) {
			enemyUnitsDestroyed = theEnemyUnitsDestroyed;
		}
		void setEnemyStructuresDestroyed(int theEnemyStructuresDestroyed) {
			enemyStructuresDestroyed = theEnemyStructuresDestroyed;
		}
		void setUnitsDestroyed(int theUnitsDestroyed) {
			unitsDestroyed = theUnitsDestroyed;
		}
		void setUnitsEaten(int theUnitsEaten) {
			unitsEaten = theUnitsEaten;
		}
		void setStructuresDestroyed(int theStructuresDestroyed) {
			structuresDestroyed = theStructuresDestroyed;
		}
		void setSpiceMined(int theSpiceMined) {
			spiceMined = theSpiceMined;
		}
		void setStructuresConstructed(int theStructuresConstructed) {
			structuresConstructed = theStructuresConstructed;
		}
		void setUnitsConstructed(int theUnitsConstructed) {
			unitsConstructed = theUnitsConstructed;
		}
		void setSuperWeaponsUsed(int theSuperWeaponsUsed) {
			superWeaponsUsed = theSuperWeaponsUsed;
		}

		// get methods
		int getEnemyUnitsDestroyed() {
			return enemyUnitsDestroyed;
		}
		int getEnemyStructuresDestroyed() {
			return enemyStructuresDestroyed;
		}
		int getUnitsDestroyed() {
			return unitsDestroyed;
		}
		int getUnitsEaten() {
			return unitsEaten;
		}
		int getStructuresDestroyed() {
			return structuresDestroyed;
		}
		int getSpiceMined() {
			return spiceMined;
		}
		int getStructuresConstructed() {
			return structuresConstructed;
		}
		int getUnitsConstructed() {
			return unitsConstructed;
		}
		int getSuperWeaponsUsed() {
			return superWeaponsUsed;
		}

	protected:

	private:
		// enemy units and structures destroyed
		int enemyUnitsDestroyed;
		int enemyStructuresDestroyed;

		// casualties
		int unitsDestroyed; // unit has been destroyed by (enemy) fire
		int unitsEaten; // unit has been eaten by sandworm
		int structuresDestroyed; // structure has been destroyed

		// misc
		int spiceMined; // total amount of spice mined
		int structuresConstructed; // total amount of structures constructed
		int unitsConstructed; // total amount of units constructed
		int superWeaponsUsed; // total amount of super weapons used
};

#endif /* CPLAYERSTATISTICS_H_ */
