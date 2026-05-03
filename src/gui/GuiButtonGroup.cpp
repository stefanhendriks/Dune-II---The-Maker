#include "gui/GuiButtonGroup.h"
#include "gui/GuiStateButton.h"

void GuiButtonGroup::updateStates(GuiStateButton* clickedButton)
{
    if (clickedButton == nullptr || m_currentlySelected == clickedButton) {
        return;
    }

    if (m_currentlySelected != nullptr) {
        m_currentlySelected->setPressed(false);
    }

    m_currentlySelected = clickedButton;
    m_currentlySelected->setPressed(true);
}

void GuiButtonGroup::updateState(int index)
{
    if (index < 0 || index >= static_cast<int>(members.size())) {
        return;
    }

    updateStates(members[index]);
}

void GuiButtonGroup::add(GuiStateButton* button)
{
    members.push_back(button);
}

bool GuiButtonGroup::isCurrentlySelected(const GuiStateButton* button) const
{
    return m_currentlySelected == button;
}
