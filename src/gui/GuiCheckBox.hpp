#pragma once

#include "GuiObject.h"
#include "utils/cRectangle.h"

#include <string>
#include <functional>

struct GuiCheckBoxParams {
    cRectangle rect;
    GuiRenderKind kind = GuiRenderKind::OPAQUE_WITH_BORDER;
    GuiTheme theme = GuiTheme::Light();
    std::function<void()> onCheckAction = nullptr;
    std::function<void()> onUnCheckAction = nullptr;
};


class GuiCheckBox : public GuiObject {
public:
    GuiCheckBox(const cRectangle &rect);
    ~GuiCheckBox();

    // From cInputObserver
    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    // From GuiObject
    void draw() const override;
    void setRenderKind(GuiRenderKind value);

    void nextRenderKind();
    void setCheckAction(std::function<void()> checkAction);
    void setUnCheckAction(std::function<void()> unCheckAction);

    // disable CheckBox
    void setEnabled(bool value);
    // tells the component what initial state it should be in
    void setChecked(bool value);

private:
    GuiRenderKind m_renderKind;
    std::function<void()> m_onCheckAction;
    std::function<void()> m_onUnCheckAction;

    bool m_focus;
    bool m_pressed;
    bool m_enabled;
    bool m_checked;

    // Functions
    void drawBox() const;
    void onMouseMovedTo(const s_MouseEvent &event);
    void onMouseRightButtonPressed(const s_MouseEvent &);
    void onMouseLeftButtonPressed(const s_MouseEvent &event);
    void onMouseLeftButtonClicked(const s_MouseEvent &event);
};


class GuiCheckBoxBuilder {
public:
    GuiCheckBoxBuilder& withRect(const cRectangle& rect) {
        params.rect = rect;
        return *this;
    }

    GuiCheckBoxBuilder& withKind(GuiRenderKind kind) {
        params.kind = kind;
        return *this;
    }

    GuiCheckBoxBuilder& withTheme(const GuiTheme& theme) {
        params.theme = theme;
        return *this;
    }

    GuiCheckBoxBuilder& onCheck(std::function<void()> callback) {
        params.onCheckAction = std::move(callback);
        return *this;
    }

    GuiCheckBoxBuilder& onUnCheck(std::function<void()> callback) {
        params.onUnCheckAction = std::move(callback);
        return *this;
    }

    GuiCheckBox* build() const {
        GuiCheckBox* btn = new GuiCheckBox(params.rect);
        btn->setRenderKind(params.kind);
        btn->setTheme(params.theme);
        if (params.onCheckAction)
            btn->setCheckAction(params.onCheckAction);
        if (params.onUnCheckAction)
            btn->setUnCheckAction(params.onUnCheckAction);
        return btn;
    }

private:
    GuiCheckBoxParams params;
};
