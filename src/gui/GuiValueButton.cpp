#include "gui/GuiValueButton.h"

#include "drawers/SDLDrawer.hpp"
#include "drawers/cTextDrawer.h"

#include <algorithm>
#include <cassert>

GuiValueButton::GuiValueButton(SDLDrawer* drawer, const cRectangle& rect, int initialValue, int stepValue, int minValue, int maxValue)
    : GuiObject(drawer, rect)
    , m_value(initialValue)
    , m_stepValue(std::max(1, stepValue))
    , m_minValue(std::min(minValue, maxValue))
    , m_maxValue(std::max(minValue, maxValue))
{
    assert(drawer != nullptr);
    updateValue(initialValue);
}

void GuiValueButton::onNotifyMouseEvent(const s_MouseEvent &event)
{
    switch (event.eventType) {
        case MOUSE_MOVED_TO:
            m_focus = m_rect.isPointWithin(event.coords);
            if (!m_focus) {
                m_pressed = false;
            }
            break;
        case MOUSE_LEFT_BUTTON_PRESSED:
        case MOUSE_RIGHT_BUTTON_PRESSED:
            m_pressed = m_rect.isPointWithin(event.coords);
            break;
        case MOUSE_LEFT_BUTTON_CLICKED:
            if (m_rect.isPointWithin(event.coords)) {
                decreaseValue();
            }
            m_pressed = false;
            break;
        case MOUSE_RIGHT_BUTTON_CLICKED:
            if (m_rect.isPointWithin(event.coords)) {
                increaseValue();
            }
            m_pressed = false;
            break;
        default:
            break;
    }
}

void GuiValueButton::onNotifyKeyboardEvent(const cKeyboardEvent &)
{
}

void GuiValueButton::draw() const
{
    m_renderDrawer->renderRectFillColor(m_rect, m_theme.fillColor);
    if (m_pressed) {
        m_renderDrawer->gui_DrawRectBorder(m_rect, m_theme.borderDark, m_theme.borderLight);
    } else {
        m_renderDrawer->gui_DrawRectBorder(m_rect, m_theme.borderLight, m_theme.borderDark);
    }

    if (m_textDrawer != nullptr) {
        Color textColor = m_focus ? m_theme.textColorHover : m_theme.textColor;
        if (!m_label.empty() || m_texture != nullptr) {
            const int topHeight = m_rect.getHeight() / 2;
            const int bottomHeight = m_rect.getHeight() - topHeight;
            const cRectangle topRect(m_rect.getX(), m_rect.getY(), m_rect.getWidth(), topHeight);
            const cRectangle bottomRect(m_rect.getX(), m_rect.getY() + topHeight, m_rect.getWidth(), bottomHeight);

            if (m_texture != nullptr) {
                m_renderDrawer->renderStrechFullSprite(m_texture, topRect);
            } else {
                m_textDrawer->drawTextCenteredInBox(m_label, topRect, textColor);
            }
            m_textDrawer->drawTextCenteredInBox(std::to_string(m_value), bottomRect, textColor);
            return;
        }

        m_textDrawer->drawTextCenteredInBox(std::to_string(m_value), m_rect, textColor);
    }
}

void GuiValueButton::setTextDrawer(cTextDrawer *drawer)
{
    m_textDrawer = drawer;
}

void GuiValueButton::setOnChanged(std::function<void(int)> callback)
{
    m_onChanged = std::move(callback);
}

void GuiValueButton::setLabel(const std::string& label)
{
    m_label = label;
}

void GuiValueButton::setTexture(Texture *texture)
{
    m_texture = texture;
}

int GuiValueButton::getValue() const
{
    return m_value;
}

void GuiValueButton::setValue(int value)
{
    updateValue(value);
}

void GuiValueButton::increaseValue()
{
    updateValue(m_value + m_stepValue);
}

void GuiValueButton::decreaseValue()
{
    updateValue(m_value - m_stepValue);
}

void GuiValueButton::updateValue(int nextValue)
{
    const int clampedValue = std::clamp(nextValue, m_minValue, m_maxValue);
    if (clampedValue == m_value) {
        return;
    }

    m_value = clampedValue;
    if (m_onChanged) {
        m_onChanged(m_value);
    }
}