/**
 * The GuiDelegate is a class that holds the bare minimum for a delegate class to be. Concrete delegate classes
 * should be used within their respective gui shapes, dialogs and windows. Ie, a cGuiWindow will never call a cGuiDelegate, but
 * rather a cGui<ConcreteType>WindowDelegate.
 *
 * This is purely used as minimum interface so you are obliged to pass a delegate class at most abstract levels.
 *
 */
#ifndef GUIDELEGATE_H_
#define GUIDELEGATE_H_

#include "GuiElement.h"

class GuiDelegate {
	public:
		GuiDelegate();
		virtual ~GuiDelegate();

		// this function must be implemented by each concrete delegate class
		virtual void interact(GuiElement * guiElement) = 0;
};

#endif /* GUIDELEGATE_H_ */
