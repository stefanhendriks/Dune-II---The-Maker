#include "gui/GuiCycleButton.h"

GuiCycleButton::GuiCycleButton(const cRectangle& rect, const std::vector<int>& values) 
        : GuiObject(rect), m_values(values), m_currentIndex(0), m_textDrawer(nullptr)
{}

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
        m_textDrawer->drawText(m_rect.getX()+5, m_rect.getY()+5, m_theme.textColor, label);
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