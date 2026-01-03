#include "gui/GuiTextInput.h"
#include "drawers/cTextDrawer.h"
#include <iostream>

GuiTextInput::GuiTextInput(const cRectangle& rect, cTextDrawer* textDrawer)
    : GuiObject(rect), m_writer(textDrawer)
{
}

void GuiTextInput::draw() const
{
    drawRectFillBorder(m_theme);
    // Affiche un curseur si focus
    m_writer->drawText(m_rect.getX() + 4, m_rect.getY() + 4, m_text + (m_focused ? "|" : "")); 
}

void GuiTextInput::onNotifyKeyboardEvent(const cKeyboardEvent& event)
{
    //std::cout << "GuiTextInput received keyboard event: " << event.toString() << std::endl;
    if (!m_focused) return;
    //std::cout << "GuiTextInput focused received keyboard event: " << event.toString() << std::endl;
    if (event.isPrintable()) {
        m_text += event.getChar();
    } else if (event.isBackspace()) {
        if (!m_text.empty()) m_text.pop_back();
    } else if (event.isEnter()) {
        if (m_onEnter) m_onEnter(m_text);
    }
}

void GuiTextInput::onNotifyMouseEvent(const s_MouseEvent& event)
{
    //std::cout << "GuiTextInput received mouse event: " << s_MouseEvent::toString(event) << std::endl;
    // Focus si clic dans la zone
    m_focused = m_rect.isPointWithin(event.coords);
}