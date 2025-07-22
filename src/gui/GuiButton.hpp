#pragma once

#include "GuiObject.h"
#include "utils/cRectangle.h"
#include "drawers/cTextDrawer.h"

#include <string>
#include <functional>
#include <memory>

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
    GuiButton(const cRectangle &rect, const std::string &btnText);
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
    void setEnabled(bool value);

    void nextRenderKind();
    void toggleTextAlignHorizontal();

    void setOnLeftMouseButtonClickedAction(std::function<void()> action);
    void setOnRightMouseButtonClickedAction(std::function<void()> action);

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
    GuiButtonBuilder() = default;
    GuiButtonBuilder& withTextDrawer(cTextDrawer* drawer);
    GuiButtonBuilder& withRect(const cRectangle& rect);
    GuiButtonBuilder& withLabel(const std::string& label);
    GuiButtonBuilder& withKind(GuiRenderKind kind);
    GuiButtonBuilder& withTheme(const GuiTheme& theme);
    GuiButtonBuilder& withTextAlign(GuiTextAlignHorizontal align);
    GuiButtonBuilder& onClick(std::function<void()> callback);
    GuiButtonBuilder& onRightClick(std::function<void()> callback);
    GuiButtonBuilder& withTexture(Texture* tex);

    std::unique_ptr<GuiButton> build() const;

private:
    GuiButtonParams params;
};