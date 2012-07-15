#ifndef GUIWINDOWBUILDER_H_
#define GUIWINDOWBUILDER_H_

#include "GuiMainMenuWindow.h"
#include "../dialogs/DialogBuilder.h"

class GuiWindowBuilder {
	public:
		GuiWindowBuilder(ScreenResolution * screenResolution);
		~GuiWindowBuilder();

		GuiMainMenuWindow * buildMainMenuWindow();

	protected:

	private:
		ScreenResolution * screenResolution;

		DialogBuilder * dialogBuilder;

};

#endif /* GUIWINDOWBUILDER_H_ */
