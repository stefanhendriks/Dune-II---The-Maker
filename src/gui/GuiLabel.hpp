#pragma once

#include "GuiObject.h"
#include "utils/cRectangle.h"

#include <string>
#include <functional>

class SDLDrawer;
class cTextDrawer;

struct GuiLabelParams {
    cTextDrawer* drawer = nullptr;
    cRectangle rect;
    SDLDrawer* renderer;
    std::string label = "";
    eGuiRenderKind kind = eGuiRenderKind::OPAQUE_WITH_BORDER;
    GuiTheme theme = cGuiThemeBuilder().light().build();
    eGuiTextAlignHorizontal align = eGuiTextAlignHorizontal::CENTER;
    Texture *tex = nullptr;
};


class GuiLabel : public GuiObject {
public:
    GuiLabel(SDLDrawer* drawer, const cRectangle &rect, const std::string &btnText);
    ~GuiLabel();

    // From cInputObserver
    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    // From GuiObject
    void draw() const override;
    void setTextAlignHorizontal(eGuiTextAlignHorizontal value);
    void setTextDrawer(cTextDrawer *cTextDrawer);
    void setRenderKind(eGuiRenderKind value);
    void setTexture(Texture *tex);

    void nextRenderKind();

    void toggleTextAlignHorizontal();

    void setEnabled(bool value);

private:
    cTextDrawer *m_textDrawer;
    std::string m_buttonText;
    eGuiRenderKind m_renderKind;
    eGuiTextAlignHorizontal m_textAlignHorizontal;
    Texture *m_tex;

    bool m_focus;
    bool m_enabled;

    // Functions
    void drawText() const;
    void onMouseMovedTo(const s_MouseEvent &event);
};


class GuiLabelBuilder {
public:
    GuiLabelBuilder& withTextDrawer(cTextDrawer* drawer) {
        params.drawer = drawer;
        return *this;
    }

    GuiLabelBuilder& withRect(const cRectangle& rect) {
        params.rect = rect;
        return *this;
    }

    GuiLabelBuilder& withRenderer(SDLDrawer* render) {
        params.renderer = render;
        return *this;
    }

    GuiLabelBuilder& withLabel(const std::string& label) {
        params.label = label;
        return *this;
    }

    GuiLabelBuilder& withKind(eGuiRenderKind kind) {
        params.kind = kind;
        return *this;
    }

    GuiLabelBuilder& withTheme(const GuiTheme& theme) {
        params.theme = theme;
        return *this;
    }

    GuiLabelBuilder& withTextAlign(eGuiTextAlignHorizontal align) {
        params.align = align;
        return *this;
    }

    GuiLabelBuilder& withTexture(Texture* tex) {
        params.tex = tex;
        return *this;
    }

    GuiLabel* build() const {
        GuiLabel* btn = new GuiLabel(params.renderer, params.rect, params.label);
        btn->setTextDrawer(params.drawer);
        btn->setRenderKind(params.kind);
        btn->setTheme(params.theme);
        btn->setTextAlignHorizontal(params.align);
        btn->setTexture(params.tex);
        return btn;
    }

private:
    GuiLabelParams params;
};
