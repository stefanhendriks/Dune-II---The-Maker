#ifndef DIALOGBUILDER_H_
#define DIALOGBUILDER_H_

#include "../../gameobjects/ScreenResolution.h"

#include "../GuiColors.h"
#include "MainMenuDialog.h"

class DialogBuilder {
	public:
		DialogBuilder(ScreenResolution * screenResolution);
		~DialogBuilder();

		MainMenuDialog * buildMainMenuDialog();

	protected:

	private:
		GuiColors guiColors;
		ScreenResolution * screenResolution;

};

#endif /* DIALOGBUILDER_H_ */
