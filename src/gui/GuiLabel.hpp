#pragma once

#include "GuiObject.h"
#include "utils/cRectangle.h"
#include "drawers/cTextDrawer.h"

#include <string>
#include <functional>

struct GuiLabelParams {
    cTextDrawer* drawer = nullptr;
    cRectangle rect;
    std::string label = "";
    GuiRenderKind kind = GuiRenderKind::OPAQUE_WITH_BORDER;
    GuiTheme theme = GuiTheme::Light();
    GuiTextAlignHorizontal align = GuiTextAlignHorizontal::CENTER;
    Texture *tex = nullptr;
};


class GuiLabel : public GuiObject {
public:
    GuiLabel(const cRectangle &rect, const std::string &btnText);
    ~GuiLabel();

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

    void setEnabled(bool value);

private:
    cTextDrawer *m_textDrawer;
    std::string m_buttonText;
    GuiRenderKind m_renderKind;
    GuiTextAlignHorizontal m_textAlignHorizontal;
    Texture *m_tex;

    bool m_focus;
    bool m_enabled;

    // Functions
    void drawText() const;
    void onMouseMovedTo(const s_MouseEvent &event);
};


class GuiLabelBuilder {
public:
    GuiLabelBuilder& withTextDrawer(cTextDrawer* drawer);
    GuiLabelBuilder& withRect(const cRectangle& rect);
    GuiLabelBuilder& withLabel(const std::string& label);
    GuiLabelBuilder& withKind(GuiRenderKind kind);
    GuiLabelBuilder& withTheme(const GuiTheme& theme);
    GuiLabelBuilder& withTextAlign(GuiTextAlignHorizontal align);
    GuiLabelBuilder& withTexture(Texture* tex);
    std::unique_ptr<GuiLabel> build() const;

private:
    GuiLabelParams params;
};