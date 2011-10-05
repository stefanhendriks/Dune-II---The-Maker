/**
 * The GuiInteractionDelegate is responsible for interaction with the user. GuiWindow's pass down the interaction
 * to the concrete GuiInteractionDelegate classes.
 *
 * This is purely used as minimum interface so you are obliged to pass a delegate class at most abstract levels.
 *
 */
#ifndef GUIDELEGATE_H_
#define GUIDELEGATE_H_

#include "GuiElement.h"

class GuiInteractionDelegate {
	public:
		GuiInteractionDelegate();
		virtual ~GuiInteractionDelegate();

		// this function must be implemented by each concrete delegate class
		virtual void interact(GuiElement * guiElement) = 0;
};

#endif /* GUIDELEGATE_H_ */
