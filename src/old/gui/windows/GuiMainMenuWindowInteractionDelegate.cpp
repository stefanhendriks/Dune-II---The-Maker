#include "GuiMainMenuWindowInteractionDelegate.h"
#include "GuiMainMenuWindow.h"

#include "../../utils/Logger.h"

GuiMainMenuWindowInteractionDelegate::GuiMainMenuWindowInteractionDelegate(){

}

GuiMainMenuWindowInteractionDelegate::~GuiMainMenuWindowInteractionDelegate() {
	Logger::getInstance()->debug("destructor GuiMainMenuWindowInteractionDelegate");
}

void GuiMainMenuWindowInteractionDelegate::interact(GuiElement * guiElementCallingDelegate) {
	GuiMainMenuWindow * guiMainMenuWindow = dynamic_cast<GuiMainMenuWindow*>(guiElementCallingDelegate);

	// do something, check the elements, etc.
	// 1. Cast calling element to GuiMainMenuWindow
	// 2. read information from its objects (read state)
	// 3. Interact upon that information (update state)

}
