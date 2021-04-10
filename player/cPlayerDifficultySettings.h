/*
 * cPlayerDifficultySettings.h
 *
 *  Created on: 1-aug-2010
 *      Author: Stefan
 */

#ifndef CPLAYERDIFFICULTYSETTINGS_H_
#define CPLAYERDIFFICULTYSETTINGS_H_

/**
 * This class holds properties for a player which have overall influence on the
 * player performance. Typically these are characterized as house properties. For instance:
 * - overall harvesting speed
 * - overall move speed of units
 * - overall damage inflicted
 * - overall damage taken
 * etc.
 *
 * Example: A 'default' house takes 10 HP damage. But an easier house will take 80% of that.
 *
 * Each cPlayer class holds a reference to a cPlayerDifficultySettings implementation which
 * will give the player the given properties.
 *
 */
class cPlayerDifficultySettings {
	public:
		cPlayerDifficultySettings();
		virtual ~cPlayerDifficultySettings() = 0;

		virtual float getMoveSpeed(int iUnitType, int terrainSlowDown) = 0;	// get move speed of a unit

		virtual float getBuildSpeed(int iSpeed) = 0;		// get build speed

		virtual float getHarvestSpeed(int iSpeed) = 0;	// get harvesting speed

		virtual float getDumpSpeed(int iSpeed) = 0;		// get harvester 'dumping spice in refinery' speed

		virtual float getInflictDamage(int iDamageInflicted) = 0;	// get damage inflicted by this house

	protected:

	private:

};

#endif /* CPLAYERDIFFICULTYSETTINGS_H_ */
