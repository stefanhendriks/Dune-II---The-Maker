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

void GuiButtonGroup::updateState(int index)
{
    if (index < 0 || index >= static_cast<int>(members.size())) {
        return;
    }

    updateStates(members[index]);
}