#include "gui/GuiSlider.hpp"
#include "d2tmc.h"
#include "drawers/SDLDrawer.hpp"
#include <algorithm>
#include <iostream>

GuiSlider::GuiSlider(const cRectangle &rect, int minValue, int maxValue, int initialValue)
    : GuiObject(rect)
    , m_minValue(minValue)
    , m_maxValue(maxValue)
    , m_value(std::clamp(initialValue, minValue, maxValue))
    , m_dragging(false)
{
}

void GuiSlider::draw() const {
    drawTrack();
    drawKnob();
}

void GuiSlider::drawTrack() const {
    // Dessine la piste (track)
    renderDrawer->renderRectFillColor(m_rect, m_theme.fillColor);
    drawRectBorder(m_theme.borderLight, m_theme.borderDark);
}

void GuiSlider::drawKnob() const {
    // Position horizontale du knob en fonction de la valeur
    float t = float(m_value - m_minValue) / float(m_maxValue - m_minValue);
    int knobX = m_rect.getX() + int(t * (m_rect.getWidth() - 10)); // knob = 10px large
    int knobY = m_rect.getY();

    cRectangle knobRect(knobX, knobY, 10, m_rect.getHeight());
    renderDrawer->renderRectFillColor(knobRect, m_theme.borderDark);
    renderDrawer->gui_DrawRectBorder(knobRect, m_theme.borderLight, m_theme.borderDark);
}

void GuiSlider::setValue(int value) {
    int clamped = std::clamp(value, m_minValue, m_maxValue);
    if (clamped != m_value) {
        m_value = clamped;
        if (m_onValueChanged) {
            std::cout << "change with value " << clamped << "\n";
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
    switch (event.eventType) {
        case MOUSE_LEFT_BUTTON_PRESSED:
            if (m_rect.isPointWithin(event.coords)) {
                m_dragging = true;
                updateValueFromMouse(event.coords.x);
            }
            break;
        case MOUSE_LEFT_BUTTON_CLICKED:
            m_dragging = false;
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
    float t = float(mouseX - m_rect.getX()) / float(m_rect.getWidth());
    t = std::clamp(t, 0.0f, 1.0f);
    setValue(int(m_minValue + t * (m_maxValue - m_minValue)));
}


void GuiSlider::onNotifyKeyboardEvent(const cKeyboardEvent &)
{}
