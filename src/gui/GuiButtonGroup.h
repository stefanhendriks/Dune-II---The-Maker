/**
 * @file GuiButtonGroup.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include <vector>

class GuiStateButton; 

class GuiButtonGroup {
public:
    GuiButtonGroup() = default;
    void add(GuiStateButton* button);
    void updateStates(GuiStateButton* clickedButton);
    void updateState(int index);
    bool isCurrentlySelected(const GuiStateButton* button) const;
private:
    std::vector<GuiStateButton*> members;
    GuiStateButton* m_currentlySelected = nullptr;
};