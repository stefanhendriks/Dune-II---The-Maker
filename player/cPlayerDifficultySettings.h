/*
 * cPlayerDifficultySettings.h
 *
 *  Created on: 1-aug-2010
 *      Author: Stefan
 */

#pragma once

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
 * Each cPlayer class holds a reference to a cPlayerDifficultySettings
 *
 */
class cPlayerDifficultySettings {
	public:
        cPlayerDifficultySettings();
		cPlayerDifficultySettings(float moveSpeedFactor, float buildSpeedFactor, float harvestSpeedFactor, float dumpSpeedFactor, float inflictDamageFactor);

        ~cPlayerDifficultySettings();

        // get move speed of a unit
		float getMoveSpeed(int iUnitType, int slowDown);

        // get build speed
		float getBuildSpeed(int iSpeed);

        // get harvesting speed (or rather, the delay before harvesting one unit of spice)
		float getHarvestSpeed(int iSpeed);

        // get harvester 'dumping spice in refinery' speed (or rather, the delay before dumping credits)
		float getDumpSpeed(int iSpeed);

		// get damage inflicted by this house
		float getInflictDamage(int iDamageInflicted);

		/**
		 * Factory method for constructing this based on House type
		 * @param house
		 * @return
		 */
		static cPlayerDifficultySettings * createFromHouse(int house);

	protected:

	private:
      float m_moveSpeedFactor;
	    float m_buildSpeedFactor;
	    float m_harvestSpeedFactor;
	    float m_dumpSpeedFactor;
	    float m_inflictDamageFactor;

};
