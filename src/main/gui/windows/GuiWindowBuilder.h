#ifndef GUIWINDOWBUILDER_H_
#define GUIWINDOWBUILDER_H_

#include "GuiMainMenuWindow.h"

class GuiWindowBuilder {
	public:
		GuiWindowBuilder();
		~GuiWindowBuilder();

		GuiMainMenuWindow * buildMainMenuWindow();

	protected:

	private:

};

#endif /* GUIWINDOWBUILDER_H_ */
