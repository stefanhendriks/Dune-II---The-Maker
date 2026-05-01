#pragma once

#include "gui/GuiObject.h"

#include <functional>
#include <memory>
#include <string>

class SDLDrawer;
class cTextDrawer;
class Texture;

class GuiValueButton : public GuiObject {
public:
    GuiValueButton(SDLDrawer* drawer, const cRectangle& rect, int initialValue, int stepValue, int minValue, int maxValue);

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;
    void draw() const override;

    void setTextDrawer(cTextDrawer *drawer);
    void setOnChanged(std::function<void(int)> callback);
    void setLabel(const std::string& label);
    void setTexture(Texture *texture);

    int getValue() const;
    void setValue(int value);

private:
    void increaseValue();
    void decreaseValue();
    void updateValue(int nextValue);

    cTextDrawer *m_textDrawer = nullptr;
    std::function<void(int)> m_onChanged = nullptr;
    std::string m_label;
    Texture *m_texture = nullptr;
    int m_value = 0;
    int m_stepValue = 1;
    int m_minValue = 0;
    int m_maxValue = 0;
    bool m_focus = false;
    bool m_pressed = false;
};

class GuiValueButtonBuilder {
public:
    GuiValueButtonBuilder& withRect(const cRectangle& rect) {
        params.rect = rect;
        return *this;
    }

    GuiValueButtonBuilder& withRenderer(SDLDrawer* render) {
        params.renderer = render;
        return *this;
    }

    GuiValueButtonBuilder& withTextDrawer(cTextDrawer* drawer) {
        params.drawer = drawer;
        return *this;
    }

    GuiValueButtonBuilder& withTheme(const GuiTheme& theme) {
        params.theme = theme;
        return *this;
    }

    GuiValueButtonBuilder& withInitialValue(int value) {
        params.initialValue = value;
        return *this;
    }

    GuiValueButtonBuilder& withStepValue(int value) {
        params.stepValue = value;
        return *this;
    }

    GuiValueButtonBuilder& withMinValue(int value) {
        params.minValue = value;
        return *this;
    }

    GuiValueButtonBuilder& withMaxValue(int value) {
        params.maxValue = value;
        return *this;
    }

    GuiValueButtonBuilder& onChanged(std::function<void(int)> callback) {
        params.onChanged = std::move(callback);
        return *this;
    }

    GuiValueButtonBuilder& withLabel(const std::string& label) {
        params.label = label;
        return *this;
    }

    GuiValueButtonBuilder& withTexture(Texture* texture) {
        params.texture = texture;
        return *this;
    }

    std::unique_ptr<GuiValueButton> build() const {
        auto button = std::make_unique<GuiValueButton>(params.renderer, params.rect, params.initialValue, params.stepValue, params.minValue, params.maxValue);
        button->setTextDrawer(params.drawer);
        button->setTheme(params.theme);
        button->setLabel(params.label);
        button->setTexture(params.texture);
        if (params.onChanged) {
            button->setOnChanged(params.onChanged);
        }
        return button;
    }

private:
    struct {
        cRectangle rect;
        SDLDrawer* renderer = nullptr;
        cTextDrawer* drawer = nullptr;
        GuiTheme theme = cGuiThemeBuilder().light().build();
        int initialValue = 0;
        int stepValue = 1;
        int minValue = 0;
        int maxValue = 0;
        std::string label;
        Texture* texture = nullptr;
        std::function<void(int)> onChanged = nullptr;
    } params;
};