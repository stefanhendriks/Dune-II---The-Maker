/*
 * cMainMenuDialog.h
 *
 *  Created on: 1-okt-2011
 *      Author: Stefan
 */

#ifndef CMAINMENUDIALOG_H_
#define CMAINMENUDIALOG_H_

#include "../shapes/cGuiShape.h"

class cMainMenuDialog  : public cGuiShape {
	public:
		cMainMenuDialog(cRectangle * rect);
		~cMainMenuDialog();

		void setNewCampaignButton(cGuiButton * button ) { newCampaignButton = button; }
		void setSkirmishButton(cGuiButton * button ) { newSkirmishButton = button; }
		void setMultiplayerButton(cGuiButton * button ) { newMultiplayerButton = button; }
		void setLoadGameButton(cGuiButton * button ) { loadGameButton = button; }
		void setOptionsButton(cGuiButton * button ) { optionsButton = button; }
		void setHallOfFameButton(cGuiButton * button ) { hallfOfFameButton = button; }
		void setExitGameButton(cGuiButton * button ) { exitGameButton = button; }

	protected:

	private:
		cGuiButton * newCampaignButton;
		cGuiButton * newSkirmishButton;
		cGuiButton * newMultiplayerButton;
		cGuiButton * loadGameButton;
		cGuiButton * optionsButton;
		cGuiButton * hallfOfFameButton;
		cGuiButton * exitGameButton;
};

#endif /* CMAINMENUDIALOG_H_ */
