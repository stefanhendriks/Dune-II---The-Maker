#pragma once

#include "GuiObject.h"
#include "utils/cRectangle.h"

#include <string>
#include <functional>
#include <memory>

class SDLDrawer;
class cTextDrawer;


class GuiButton : public GuiObject {
public:
    GuiButton(SDLDrawer* drawer, const cRectangle &rect, const std::string &btnText);
    ~GuiButton();

    // From cInputObserver
    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    // From GuiObject
    void draw() const override;

    void setTextAlignHorizontal(GuiTextAlignHorizontal value);
    void setTextDrawer(cTextDrawer *cTextDrawer);
    void setRenderKind(GuiRenderKind value);
    void setTexture(Texture *tex);

    void nextRenderKind();

    void toggleTextAlignHorizontal();

    void setOnLeftMouseButtonClickedAction(std::function<void()> action);
    void setOnRightMouseButtonClickedAction(std::function<void()> action);

    void setEnabled(bool value);

private:
    cTextDrawer *m_textDrawer;
    std::string m_buttonText;
    GuiRenderKind m_renderKind;
    GuiTextAlignHorizontal m_textAlignHorizontal;
    std::function<void()> m_onLeftMouseButtonClickedAction;
    std::function<void()> m_onRightMouseButtonClickedAction;
    Texture *m_tex;

    bool m_focus;

    // pressed state
    bool m_pressed;
    bool m_enabled;

    // Functions
    void drawText() const;

    void onMouseMovedTo(const s_MouseEvent &event);

    void onMouseRightButtonPressed(const s_MouseEvent &event);
    void onMouseRightButtonClicked(const s_MouseEvent &event);
    void onMouseLeftButtonPressed(const s_MouseEvent &event);
    void onMouseLeftButtonClicked(const s_MouseEvent &event);
};


class GuiButtonBuilder {
public:
    GuiButtonBuilder& withTextDrawer(cTextDrawer* drawer) {
        this->m_drawer = drawer;
        return *this;
    }

    GuiButtonBuilder& withRect(const cRectangle& rect) {
        this->m_rect = rect;
        return *this;
    }

    GuiButtonBuilder& withRenderer(SDLDrawer* render) {
        this->m_renderer = render;
        return *this;
    }

    GuiButtonBuilder& withLabel(const std::string& label) {
        this->m_label = label;
        return *this;
    }

    GuiButtonBuilder& withKind(GuiRenderKind kind) {
        this->m_kind = kind;
        return *this;
    }

    GuiButtonBuilder& withTheme(const GuiTheme& theme) {
        this->m_theme = theme;
        return *this;
    }

    GuiButtonBuilder& withTextAlign(GuiTextAlignHorizontal align) {
        this->m_align = align;
        return *this;
    }

    GuiButtonBuilder& onClick(std::function<void()> callback) {
        this->m_onLeftClick = std::move(callback);
        return *this;
    }

    GuiButtonBuilder& onRightClick(std::function<void()> callback) {
        this->m_onRightClick = std::move(callback);
        return *this;
    }

    GuiButtonBuilder& withTexture(Texture* tex) {
        this->m_tex = tex;
        return *this;
    }

    std::unique_ptr<GuiButton> build() const {
        auto btn = std::make_unique<GuiButton>(m_renderer, m_rect, m_label);
        btn->setTextDrawer(m_drawer);
        btn->setRenderKind(m_kind);
        btn->setTheme(m_theme);
        btn->setTextAlignHorizontal(m_align);
        btn->setTexture(m_tex);
        if (m_onLeftClick) {
            btn->setOnLeftMouseButtonClickedAction(m_onLeftClick);
        }
        if (m_onRightClick) {
            btn->setOnRightMouseButtonClickedAction(m_onRightClick);
        }
        
        return btn;
    }

private:
    cTextDrawer* m_drawer = nullptr;
    cRectangle m_rect;
    SDLDrawer* m_renderer;
    std::string m_label = "";
    GuiRenderKind m_kind = GuiRenderKind::OPAQUE_WITH_BORDER;
    GuiTheme m_theme = cGuiThemeBuilder().light().build();
    GuiTextAlignHorizontal m_align = GuiTextAlignHorizontal::CENTER;
    std::function<void()> m_onLeftClick = nullptr;
    std::function<void()> m_onRightClick = nullptr;
    Texture *m_tex = nullptr;
};
