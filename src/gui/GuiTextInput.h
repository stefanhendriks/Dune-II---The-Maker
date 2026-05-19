#pragma once

#include "GuiObject.h"
#include <string>
#include <functional>
#include <memory>

class cTextDrawer;
class SDLDrawer;

class GuiTextInput : public GuiObject {
public:
    GuiTextInput(SDLDrawer* drawer, const cRectangle& rect, cTextDrawer* textDrawer);

    void draw() const override;
    void onNotifyKeyboardEvent(const cKeyboardEvent& event) override;
    void onNotifyMouseEvent(const s_MouseEvent& event) override;
    bool hasKeyboardFocus() const override { return m_focused; }

    const std::string& getText() const { return m_text; }
    void setText(const std::string& text);
    void setFocused(bool focused) { m_focused = focused; }
    void setTextDrawer(cTextDrawer* drawer) { m_writer = drawer; }
    void setOnEnter(std::function<void(const std::string&)> callback) { m_onEnter = std::move(callback); }

private:
    int getMaxTextPixelWidth() const;
    bool canAppendText(const std::string& text) const;
    std::string getFittedDisplayText() const;

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
    GuiTextInputBuilder& withRenderer(SDLDrawer* render) {
        params.renderer = render;
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

    GuiTextInputBuilder& onEnter(std::function<void(const std::string&)> callback) {
        params.onEnter = std::move(callback);
        return *this;
    }

    std::unique_ptr<GuiTextInput> build() const {
        auto btn = std::make_unique<GuiTextInput>(params.renderer, params.rect, params.drawer);
        btn->setTextDrawer(params.drawer);
        btn->setTheme(params.theme);
        if (params.onEnter) {
            btn->setOnEnter(params.onEnter);
        }
        return btn;
    }

private:
    struct {
        cRectangle rect;
        cTextDrawer* drawer = nullptr;
        SDLDrawer* renderer = nullptr;
        GuiTheme theme = cGuiThemeBuilder().light().build();
        std::function<void(const std::string&)> onEnter = nullptr;
    } params;
};