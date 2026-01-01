#include "gui/GuiButtonGroup.h"
#include "gui/GuiStateButton.h"

void GuiButtonGroup::updateStates(GuiStateButton* clickedButton)
{
    for (GuiStateButton* btn : members) {
        if (btn == clickedButton) {
            btn->setPressed(true);
        } else {
            btn->setPressed(false);
        }
    }
}