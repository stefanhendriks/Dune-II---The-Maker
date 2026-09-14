/**
 * @file GuiButtonGroup.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

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
