#pragma once

#include "cGuiAction.h"
#include "cGuiObject.h"
#include "utils/cRectangle.h"
#include "drawers/cTextDrawer.h"

#include <string>

enum eGuiButtonRenderKind {
    OPAQUE_WITH_BORDER = 0,
    OPAQUE_WITHOUT_BORDER = 1,
    TRANSPARENT_WITH_BORDER = 2,
    TRANSPARENT_WITHOUT_BORDER = 3
};

enum eGuiTextAlignHorizontal {
    LEFT,
    CENTER
};

class cGuiButton : public cGuiObject {
public:
    cGuiButton(const cTextDrawer &textDrawer, const cRectangle &rect, const std::string &btnText);

    cGuiButton(const cTextDrawer &textDrawer, const cRectangle &rect, const std::string &btnText,
               eGuiButtonRenderKind renderKind);

    cGuiButton(const cTextDrawer &textDrawer, const cRectangle &rect, const std::string &btnText, int gui_colorButton,
               int gui_colorBorderLight, int gui_colorBorderDark);

    ~cGuiButton();

    // From cInputObserver
    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    // From cGuiObject
    void draw() const override;

    bool hasFocus();

    void setTextAlignHorizontal(eGuiTextAlignHorizontal value);

    void setRenderKind(eGuiButtonRenderKind value);

    void nextRenderKind();

    void toggleTextAlignHorizontal();

    void setGui_ColorButton(int value);

    void setTextColor(int value);

    void setTextColorHover(int value);

    void setOnLeftMouseButtonClickedAction(cGuiAction *action);

    void setEnabled(bool value);

private:
    cTextDrawer m_textDrawer;
    std::string m_buttonText;
    eGuiButtonRenderKind m_renderKind;
    eGuiTextAlignHorizontal m_textAlignHorizontal;
    cGuiAction *m_onLeftMouseButtonClickedAction;

    bool m_focus;

    int m_guiColorButton;
    int m_guiColorBorderLight;
    int m_guiColorBorderDark;

    int m_textColor;
    int m_textColorHover;

    // pressed state
    bool m_pressed;

    bool m_enabled;

    // Functions
    void drawText() const;

    void onMouseMovedTo(const s_MouseEvent &event);

    void onMouseRightButtonClicked(const s_MouseEvent &event);

    void onMouseLeftButtonPressed(const s_MouseEvent &event);

    void onMouseLeftButtonClicked(const s_MouseEvent &event);
};
