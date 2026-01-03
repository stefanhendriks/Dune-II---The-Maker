#pragma once

#include "gui/GuiObject.h"
#include "drawers/cTextDrawer.h"
#include <vector>
#include <string>
#include <functional>


class GuiCycleButton : public GuiObject {
public:
    GuiCycleButton(const cRectangle& rect, const std::vector<int>& values);

    void onNotifyMouseEvent(const s_MouseEvent& event) override;

    void onNotifyKeyboardEvent(const cKeyboardEvent& event) override;

    void draw() const override;

    void nextValue();
    void previousValue();

    int getSelectedValue() const { return m_values[m_currentIndex]; }
    void setTextDrawer(cTextDrawer* drawer) { m_textDrawer = drawer; }
    void setOnChanged(std::function<void(int)> callback) { m_onChanged = callback; }

private:
    std::vector<int> m_values;
    size_t m_currentIndex;
    cTextDrawer* m_textDrawer;
    std::function<void(int)> m_onChanged; // Optionnal callback
};


class GuiCycleButtonBuilder {
public:
    GuiCycleButtonBuilder& withRect(const cRectangle& rect) {
        params.rect = rect;
        return *this;
    }

    GuiCycleButtonBuilder& withValues(const std::vector<int>& values) {
        params.values = values;
        return *this;
    }

    GuiCycleButtonBuilder& withTextDrawer(cTextDrawer* drawer) {
        params.drawer = drawer;
        return *this;
    }

    GuiCycleButtonBuilder& withTheme(const GuiTheme& theme) {
        params.theme = theme;
        return *this;
    }

    GuiCycleButtonBuilder& onChanged(std::function<void(int)> callback) {
        params.onChanged = std::move(callback);
        return *this;
    }

    GuiCycleButton* build() const {
        GuiCycleButton* btn = new GuiCycleButton(params.rect, params.values);
        btn->setTextDrawer(params.drawer);
        btn->setTheme(params.theme);
        if (params.onChanged) {
            btn->setOnChanged(params.onChanged);
        }
        return btn;
    }

private:
    struct {
        cRectangle rect;
        std::vector<int> values;
        cTextDrawer* drawer = nullptr;
        GuiTheme theme = GuiTheme::Light();
        std::function<void(int)> onChanged = nullptr;
    } params;
};