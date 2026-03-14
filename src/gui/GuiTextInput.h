#pragma once

#include "GuiObject.h"
#include <string>
#include <functional>

class cTextDrawer;

class GuiTextInput : public GuiObject {
public:
    GuiTextInput(const cRectangle& rect, cTextDrawer* textDrawer);

    void draw() const override;
    void onNotifyKeyboardEvent(const cKeyboardEvent& event) override;
    void onNotifyMouseEvent(const s_MouseEvent& event) override;

    const std::string& getText() const { return m_text; }
    void setText(const std::string& text) { m_text = text; }
    void setTextDrawer(cTextDrawer* drawer) { m_writer = drawer; }
    void setOnEnter(std::function<void(const std::string&)> callback) { m_onEnter = std::move(callback); }

private:
    std::string m_text;
    cTextDrawer *m_writer;
    bool m_focused = false;
    std::function<void(const std::string&)> m_onEnter;
};



class GuiTextInputBuilder {
public:
    GuiTextInputBuilder& withRect(const cRectangle& rect) {
        params.rect = rect;
        return *this;
    }

    GuiTextInputBuilder& withTextDrawer(cTextDrawer* drawer) {
        params.drawer = drawer;
        return *this;
    }

    GuiTextInputBuilder& withTheme(const GuiTheme& theme) {
        params.theme = theme;
        return *this;
    }

    GuiTextInputBuilder& onChanged(std::function<void(const std::string&)> callback) {
        params.onChanged = std::move(callback);
        return *this;
    }

    GuiTextInput* build() const {
        GuiTextInput* btn = new GuiTextInput(params.rect, params.drawer);
        btn->setTextDrawer(params.drawer);
        btn->setTheme(params.theme);
        if (params.onChanged) {
            btn->setOnEnter(params.onChanged);
        }
        return btn;
    }

private:
    struct {
        cRectangle rect;
        cTextDrawer* drawer = nullptr;
        GuiTheme theme = GuiTheme::Light();
        std::function<void(const std::string&)> onChanged = nullptr;
    } params;
};