/**
 * @file GuiCycleButton.cpp
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

#include "gui/GuiCycleButton.h"
#include "drawers/cTextDrawer.h"
#include "include/cAssert.h"

GuiCycleButton::GuiCycleButton(SDLDrawer* drawer,const cRectangle& rect, const std::vector<int>& values) 
        : GuiObject(drawer, rect), m_values(values), m_currentIndex(0), m_textDrawer(nullptr)
{
    d2tm_assert(drawer != nullptr);
}

void GuiCycleButton::onNotifyMouseEvent(const s_MouseEvent& event) 
{
    if (!m_rect.isPointWithin(event.coords.x, event.coords.y)) {
        return;
    }
    if (event.eventType == MOUSE_LEFT_BUTTON_CLICKED) {
        nextValue();
    }
    else if (event.eventType == MOUSE_RIGHT_BUTTON_CLICKED) {
        previousValue();
    }
}


void GuiCycleButton::onNotifyKeyboardEvent(const cKeyboardEvent& ) 
{}

void GuiCycleButton::draw() const
{
    // 1. Dessiner le fond/bordure via GuiObject
    drawRectFillBorder(m_theme);

    // 2. Dessiner le texte de la valeur actuelle
    if (m_textDrawer && !m_values.empty()) {
        std::string label = std::to_string(m_values[m_currentIndex]);
        // Utilise tes fonctions de dessin habituelles
        m_textDrawer->drawText(m_rect.getX()+5, m_rect.getY()+3, m_theme.textColor, label);
    }
}

// Navigation dans la liste
void GuiCycleButton::nextValue()
{
    if (m_values.empty()) return;
    m_currentIndex = (m_currentIndex + 1) % m_values.size();
    if (m_onChanged) m_onChanged(m_values[m_currentIndex]);
}

void GuiCycleButton::previousValue()
{
    if (m_values.empty()) return;
    m_currentIndex = (m_currentIndex == 0) ? m_values.size() - 1 : m_currentIndex - 1;
    if (m_onChanged) m_onChanged(m_values[m_currentIndex]);
}