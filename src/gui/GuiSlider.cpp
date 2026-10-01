/**
 * @file GuiSlider.cpp
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

#include "gui/GuiSlider.hpp"
#include "drawers/SDLDrawer.hpp"
#include <algorithm>
#include <iostream>
#include "include/cAssert.h"
static constexpr auto GUI_KNOB_WIDTH = 12;

GuiSlider::GuiSlider(SDLDrawer* drawer, const cRectangle &rect, int minValue, int maxValue, int initialValue)
    : GuiObject(drawer, rect)
    , m_minValue(minValue)
    , m_maxValue(maxValue)
    , m_value(std::clamp(initialValue, minValue, maxValue))
    , m_dragging(false)
{
    d2tm_assert(drawer != nullptr);
}

void GuiSlider::setEnabled(bool enabled)
{
    m_enabled = enabled;
    if (!m_enabled) {
        m_visualState = VisualState::Disabled;
    } else {
        m_visualState = VisualState::Normal;
    }
}

bool GuiSlider::isEnabled() const
{
    return m_enabled;
}

void GuiSlider::setOnLeftMouseButtonClickedAction(std::function<void()> action)
{
    m_onLeftMouseButtonClickedAction = std::move(action);
}

void GuiSlider::setOnRightMouseButtonClickedAction(std::function<void()> action)
{
    m_onRightMouseButtonClickedAction = std::move(action);
}

void GuiSlider::setVisualState(VisualState state)
{
    m_visualState = state;
    if (state == VisualState::Disabled) {
        m_enabled = false;
    } else {
        m_enabled = true;
    }
}

GuiSlider::VisualState GuiSlider::visualState() const
{
    return m_visualState;
}

void GuiSlider::draw() const {
    drawTrack();
    drawKnob();
}

void GuiSlider::drawTrack() const {
    m_sdlDrawer->renderRectFillColor(m_rect, m_theme.fillColor);
    drawRectBorder(m_theme.borderLight, m_theme.borderDark);
}

void GuiSlider::drawKnob() const {
    float t = float(m_value - m_minValue) / float(m_maxValue - m_minValue);

    int knobX = m_rect.getX() + int(t * (m_rect.getWidth() - GUI_KNOB_WIDTH));
    int knobY = m_rect.getY();

    cRectangle knobRect(knobX, knobY, GUI_KNOB_WIDTH, m_rect.getHeight());
    m_sdlDrawer->renderRectFillColor(knobRect, m_theme.borderDark);
    m_sdlDrawer->gui_DrawRectBorder(knobRect, m_theme.borderLight, m_theme.borderDark);
}

void GuiSlider::setValue(int value) {
    int clamped = std::clamp(value, m_minValue, m_maxValue);
    if (clamped != m_value) {
        m_value = clamped;
        if (m_onValueChanged) {
            m_onValueChanged(m_value);
        }
    }
}

int GuiSlider::getValue() const {
    return m_value;
}

void GuiSlider::setOnValueChanged(std::function<void(int)> callback) {
    m_onValueChanged = std::move(callback);
}

void GuiSlider::onNotifyMouseEvent(const s_MouseEvent &event) {
    if (!m_enabled) {
        return;
    }

    switch (event.eventType) {
        case MOUSE_LEFT_BUTTON_PRESSED:
            if (m_rect.isPointWithin(event.coords)) {
                m_dragging = true;
                m_visualState = VisualState::Pressed;
                updateValueFromMouse(event.coords.x);
                if (m_onLeftMouseButtonClickedAction) {
                    m_onLeftMouseButtonClickedAction();
                }
            }
            break;
        case MOUSE_LEFT_BUTTON_CLICKED:
            m_dragging = false;
            m_visualState = VisualState::Normal;
            break;
        case MOUSE_MOVED_TO:
            if (m_dragging) {
                updateValueFromMouse(event.coords.x);
            }
            break;
        default:
            break;
    }
}

void GuiSlider::updateValueFromMouse(int mouseX) {
    int positionInTrack = mouseX - m_rect.getX();
    float normalizedValueInTrack = float(positionInTrack) / float(m_rect.getWidth());
    normalizedValueInTrack = std::clamp(normalizedValueInTrack, 0.0f, 1.0f);

    int maxAmountOfValues = (m_maxValue - m_minValue);

    float newValueNormalized = normalizedValueInTrack * maxAmountOfValues;
    setValue(int(m_minValue + newValueNormalized));
}


void GuiSlider::onNotifyKeyboardEvent(const cKeyboardEvent &)
{}
