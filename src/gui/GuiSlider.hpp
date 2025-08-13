#pragma once

#include "gui/GuiObject.h"
#include <functional>


struct GuiSliderParams {
    cRectangle rect;
    int minValue = 0;
    int maxValue = 100;
    int initialValue = 0;
    GuiTheme theme = GuiTheme::Light();
    std::function<void(int)> onValueChanged = nullptr;
};

class GuiSlider : public GuiObject {
public:
    GuiSlider(const cRectangle &rect, int minValue = 0, int maxValue = 100, int initialValue = 0);

    void draw() const override;

    void setValue(int value);
    int getValue() const;

    void setOnValueChanged(std::function<void(int)> callback);

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

private:
    int m_minValue;
    int m_maxValue;
    int m_value;
    bool m_dragging;
    std::function<void(int)> m_onValueChanged;

    void drawTrack() const;
    void drawKnob() const;

    void updateValueFromMouse(int mouseX);
};

class GuiSliderBuilder {
public:
    GuiSliderBuilder& withRect(const cRectangle& rect) {
        params.rect = rect;
        return *this;
    }

    GuiSliderBuilder& withMinValue(int minVal) {
        params.minValue = minVal;
        return *this;
    }

    GuiSliderBuilder& withMaxValue(int maxVal) {
        params.maxValue = maxVal;
        return *this;
    }

    GuiSliderBuilder& withInitialValue(int initialVal) {
        params.initialValue = initialVal;
        return *this;
    }

    GuiSliderBuilder& withTheme(const GuiTheme& theme) {
        params.theme = theme;
        return *this;
    }

    GuiSliderBuilder& onValueChanged(std::function<void(int)> callback) {
        params.onValueChanged = std::move(callback);
        return *this;
    }

    GuiSlider* build() const {
        GuiSlider* slider = new GuiSlider(params.rect, params.minValue, params.maxValue, params.initialValue);
        slider->setTheme(params.theme);
        if (params.onValueChanged)
            slider->setOnValueChanged(params.onValueChanged);
        return slider;
    }

private:
    GuiSliderParams params;
};
