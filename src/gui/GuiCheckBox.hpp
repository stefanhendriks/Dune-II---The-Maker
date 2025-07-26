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

    void setEnabled(bool value);

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
    GuiCheckBoxBuilder() = default;
    GuiCheckBoxBuilder& withRect(const cRectangle& rect);
    GuiCheckBoxBuilder& withKind(GuiRenderKind kind);
    GuiCheckBoxBuilder& withTheme(const GuiTheme& theme);
    GuiCheckBoxBuilder& onCheck(std::function<void()> callback);
    GuiCheckBoxBuilder& onUnCheck(std::function<void()> callback);
    std::unique_ptr<GuiCheckBox> build() const;

private:
    GuiCheckBoxParams params;
};