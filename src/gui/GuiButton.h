#pragma once

// #include "GuiAction.h"
#include "GuiObject.h"
#include "utils/cRectangle.h"
#include "drawers/cTextDrawer.h"

#include <string>
#include <functional>

struct GuiButtonParams {
    cTextDrawer* drawer = nullptr;
    cRectangle rect;
    std::string label = "";
    GuiRenderKind kind = GuiRenderKind::OPAQUE_WITH_BORDER;
    GuiTheme theme = GuiTheme::Light();
    GuiTextAlignHorizontal align = GuiTextAlignHorizontal::CENTER;
    std::function<void()> onLeftClick = nullptr;
    std::function<void()> onRightClick = nullptr;
    Texture *tex = nullptr;
};


class GuiButton : public GuiObject {
public:
    GuiButton(/*const cTextDrawer &textDrawer,*/ const cRectangle &rect, const std::string &btnText);

    //GuiButton(const cTextDrawer &textDrawer, const cRectangle &rect, const std::string &btnText, eGuiButtonRenderKind renderKind);

    // cGuiButton(const cTextDrawer &textDrawer, const cRectangle &rect, const std::string &btnText, Color gui_colorButton,
    //            Color gui_colorBorderLight, Color gui_colorBorderDark);

    ~GuiButton();

    // From cInputObserver
    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    // From GuiObject
    void draw() const override;

    // bool hasFocus();

    void setTextAlignHorizontal(GuiTextAlignHorizontal value);
    void setTextDrawer(cTextDrawer *cTextDrawer);
    void setRenderKind(GuiRenderKind value);
    void setTexture(Texture *tex);

    void nextRenderKind();

    void toggleTextAlignHorizontal();

    // void setGui_ColorButton(Color value);

    // void setTextColor(Color value);

    // void setTextColorHover(Color value);

    // [[deprecated]] void setOnLeftMouseButtonClickedAction(cGuiAction *action);
    void setOnLeftMouseButtonClickedAction(std::function<void()> action);
    void setOnRightMouseButtonClickedAction(std::function<void()> action);

    void setEnabled(bool value);

private:
    cTextDrawer *m_textDrawer;
    std::string m_buttonText;
    GuiRenderKind m_renderKind;
    GuiTextAlignHorizontal m_textAlignHorizontal;
    // cGuiAction *m_onLeftMouseButtonClickedAction;
    std::function<void()> m_onLeftMouseButtonClickedAction;
    std::function<void()> m_onRightMouseButtonClickedAction;
    Texture *m_tex;

    bool m_focus;

    // Color m_guiColorButton;
    // Color m_guiColorBorderLight;
    // Color m_guiColorBorderDark;

    // Color m_textColor;
    // Color m_textColorHover;

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
        params.drawer = drawer;
        return *this;
    }

    GuiButtonBuilder& withRect(const cRectangle& rect) {
        params.rect = rect;
        return *this;
    }

    GuiButtonBuilder& withLabel(const std::string& label) {
        params.label = label;
        return *this;
    }

    GuiButtonBuilder& withKind(GuiRenderKind kind) {
        params.kind = kind;
        return *this;
    }

    GuiButtonBuilder& withTheme(const GuiTheme& theme) {
        params.theme = theme;
        return *this;
    }

    GuiButtonBuilder& withTextAlign(GuiTextAlignHorizontal align) {
        params.align = align;
        return *this;
    }

    GuiButtonBuilder& onClick(std::function<void()> callback) {
        params.onLeftClick = std::move(callback);
        return *this;
    }

    GuiButtonBuilder& onRightClick(std::function<void()> callback) {
        params.onRightClick = std::move(callback);
        return *this;
    }

    GuiButtonBuilder& withTexture(Texture* tex) {
        params.tex = tex;
        return *this;
    }

    GuiButton* build() const {
        GuiButton* btn = new GuiButton(params.rect, params.label);
        btn->setTextDrawer(params.drawer);
        btn->setRenderKind(params.kind);
        btn->setTheme(params.theme);
        btn->setTextAlignHorizontal(params.align);
        btn->setTexture(params.tex);
        if (params.onLeftClick)
            btn->setOnLeftMouseButtonClickedAction(params.onLeftClick);
        if (params.onRightClick)
            btn->setOnRightMouseButtonClickedAction(params.onRightClick);
        
        return btn;
    }

private:
    GuiButtonParams params;
};
