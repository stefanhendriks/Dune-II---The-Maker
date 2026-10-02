/**
 * @file GuiValueButton.cpp
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

#include "gui/GuiValueButton.h"

#include "drawers/SDLDrawer.hpp"
#include "drawers/cTextDrawer.h"

#include <algorithm>
#include "include/cAssert.h"

GuiValueButton::GuiValueButton(SDLDrawer* drawer, const cRectangle& rect, int initialValue, int stepValue, int minValue, int maxValue)
    : GuiObject(drawer, rect)
    , m_value(initialValue)
    , m_stepValue(std::max(1, stepValue))
    , m_minValue(std::min(minValue, maxValue))
    , m_maxValue(std::max(minValue, maxValue))
{
    d2tm_assert(drawer != nullptr);
    updateValue(initialValue);
}

void GuiValueButton::onNotifyMouseEvent(const s_MouseEvent &event)
{
    if (!m_enabled) {
        return;
    }

    switch (event.eventType) {
        case MOUSE_MOVED_TO:
            m_focus = m_rect.isPointWithin(event.coords);
            if (!m_focus) {
                m_pressed = false;
            }
            if (!m_enabled) {
                m_visualState = VisualState::Disabled;
            } else if (m_pressed) {
                m_visualState = VisualState::Pressed;
            } else if (m_focus) {
                m_visualState = VisualState::Hover;
            } else {
                m_visualState = VisualState::Normal;
            }
            break;
        case MOUSE_LEFT_BUTTON_PRESSED:
        case MOUSE_RIGHT_BUTTON_PRESSED:
            m_pressed = m_rect.isPointWithin(event.coords);
            if (m_pressed) {
                m_visualState = VisualState::Pressed;
            }
            break;
        case MOUSE_LEFT_BUTTON_CLICKED:
            if (m_rect.isPointWithin(event.coords)) {
                increaseValue();
                if (m_onLeftMouseButtonClickedAction) {
                    m_onLeftMouseButtonClickedAction();
                }
            }
            m_pressed = false;
            m_visualState = m_focus ? VisualState::Hover : VisualState::Normal;
            break;
        case MOUSE_RIGHT_BUTTON_CLICKED:
            if (m_rect.isPointWithin(event.coords)) {
                decreaseValue();
                if (m_onRightMouseButtonClickedAction) {
                    m_onRightMouseButtonClickedAction();
                }
            }
            m_pressed = false;
            m_visualState = m_focus ? VisualState::Hover : VisualState::Normal;
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
    m_sdlDrawer->renderRectFillColor(m_rect, m_theme.fillColor);
    if (m_pressed || m_visualState == VisualState::Pressed) {
        m_sdlDrawer->gui_DrawRectBorder(m_rect, m_theme.borderDark, m_theme.borderLight);
    } else {
        m_sdlDrawer->gui_DrawRectBorder(m_rect, m_theme.borderLight, m_theme.borderDark);
    }

    if (m_textDrawer != nullptr) {
        Color textColor = m_focus ? m_theme.textColorHover : m_theme.textColor;
        if (!m_enabled) {
            textColor = m_theme.disabledTextColor;
        }
        if (!m_label.empty() || m_texture != nullptr) {
            const int topHeight = m_rect.getHeight() / 2;
            const int bottomHeight = m_rect.getHeight() - topHeight;
            const cRectangle topRect(m_rect.getX(), m_rect.getY(), m_rect.getWidth(), topHeight);
            const cRectangle bottomRect(m_rect.getX(), m_rect.getY() + topHeight, m_rect.getWidth(), bottomHeight);

            if (m_texture != nullptr) {
                m_sdlDrawer->renderStrechFullSprite(m_texture, topRect);
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

void GuiValueButton::setEnabled(bool enabled)
{
    m_enabled = enabled;
    if (!m_enabled) {
        m_visualState = VisualState::Disabled;
        m_pressed = false;
        m_focus = false;
    } else if (m_visualState == VisualState::Disabled) {
        m_visualState = VisualState::Normal;
    }
}

bool GuiValueButton::isEnabled() const
{
    return m_enabled;
}

void GuiValueButton::setOnLeftMouseButtonClickedAction(std::function<void()> action)
{
    m_onLeftMouseButtonClickedAction = std::move(action);
}

void GuiValueButton::setOnRightMouseButtonClickedAction(std::function<void()> action)
{
    m_onRightMouseButtonClickedAction = std::move(action);
}

void GuiValueButton::setVisualState(VisualState state)
{
    m_visualState = state;
    switch (state) {
        case VisualState::Disabled:
            m_enabled = false;
            m_pressed = false;
            m_focus = false;
            break;
        case VisualState::Pressed:
            m_pressed = true;
            m_focus = true;
            break;
        case VisualState::Hover:
            m_pressed = false;
            m_focus = true;
            break;
        case VisualState::Normal:
            m_pressed = false;
            m_focus = false;
            break;
    }
}

GuiValueButton::VisualState GuiValueButton::visualState() const
{
    return m_visualState;
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