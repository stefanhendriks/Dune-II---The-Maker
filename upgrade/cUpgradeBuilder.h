/*
 * cUpgradeBuilder.h
 *
 *  Created on: 3-aug-2010
 *      Author: Stefan
 *
 *
 * Processes upgrades
 */

#ifndef CUPGRADEBUILDER_H_
#define CUPGRADEBUILDER_H_

class cUpgradeBuilder {
	public:
		cUpgradeBuilder(cPlayer * thePlayer);
		~cUpgradeBuilder();

		bool isUpgrading(int listId);

		void addUpgrade(int listId, cListUpgrade * listUpgrade);

		void processUpgrades(); // timer based method that processes upgrades

	protected:

	private:
		// each list has a corresponding upgrade item that is being processed
		cListUpgrade * upgrades[LIST_MAX];

		cPlayer * player;

};

#endif /* CUPGRADEBUILDER_H_ */
