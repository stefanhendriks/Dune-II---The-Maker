#include "GuiInteractionDelegate.h"

GuiInteractionDelegate::~GuiInteractionDelegate() {

}

void GuiInteractionDelegate::interact(GuiElement * guiElementCallingDelegate) {
	// derived classes need to implement this
}
