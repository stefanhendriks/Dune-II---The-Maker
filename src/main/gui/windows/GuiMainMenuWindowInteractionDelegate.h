/**
 * Concrete implementation of interactions with the main menu window. The GuiWIndow delegates all interactions to this class. This
 * class will act upon the events given.
 *
 */
#ifndef GUIMAINMENUWINDOWINTERACTIONDELEGATE_H_
#define GUIMAINMENUWINDOWINTERACTIONDELEGATE_H_

#include "../GuiInteractionDelegate.h"

class GuiMainMenuWindowInteractionDelegate : public GuiInteractionDelegate {
	public:
		GuiMainMenuWindowInteractionDelegate();
		~GuiMainMenuWindowInteractionDelegate();

		void interact(GuiElement * GuiElementCallingDelegate);
};

#endif /* GUIMAINMENUWINDOWINTERACTIONDELEGATE_H_ */
