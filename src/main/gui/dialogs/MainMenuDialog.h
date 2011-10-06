/*
 * cMainMenuDialog.h
 *
 *  Created on: 1-okt-2011
 *      Author: Stefan
 */

#ifndef CMAINMENUDIALOG_H_
#define CMAINMENUDIALOG_H_

#include "../shapes/GuiShape.h"
#include "../shapes/GuiButton.h"

class MainMenuDialog  : public GuiShape {
	public:
		MainMenuDialog(Rectangle * rect);
		~MainMenuDialog();

		void setNewCampaignButton(GuiButton * button ) { newCampaignButton = button; }
		void setSkirmishButton(GuiButton * button ) { newSkirmishButton = button; }
		void setMultiplayerButton(GuiButton * button ) { newMultiplayerButton = button; }
		void setLoadGameButton(GuiButton * button ) { loadGameButton = button; }
		void setOptionsButton(GuiButton * button ) { optionsButton = button; }
		void setHallOfFameButton(GuiButton * button ) { hallfOfFameButton = button; }
		void setExitGameButton(GuiButton * button ) { exitGameButton = button; }

	protected:

	private:
		GuiButton * newCampaignButton;
		GuiButton * newSkirmishButton;
		GuiButton * newMultiplayerButton;
		GuiButton * loadGameButton;
		GuiButton * optionsButton;
		GuiButton * hallfOfFameButton;
		GuiButton * exitGameButton;
};

#endif /* CMAINMENUDIALOG_H_ */
