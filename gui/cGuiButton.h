#ifndef D2TM_CGUIBUTTON_H
#define D2TM_CGUIBUTTON_H

#include <utils/cRectangle.h>
#include <observers/cMouseObserver.h>
#include "cGuiObject.h"

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

//    /**
//     * Copy constructor
//     * @param src
//    */
//    cGuiButton(const cGuiButton &src);
//
//    /**
//     * Assignment operator
//     * @param rhs
//     * @return
//     */
//    cGuiButton &operator=(const cGuiButton &rhs);

    void onNotifyMouseEvent(const s_MouseEvent &event) override;

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
    cRectangle rect;
    cTextDrawer textDrawer;
    std::string btnText;
    eGuiButtonRenderKind renderKind;
    eGuiTextAlignHorizontal textAlignHorizontal;
    cGuiAction *onLeftMouseButtonClicked_action;

    bool focus;

    int gui_colorButton;
    int gui_colorBorderLight;
    int gui_colorBorderDark;

    int text_color;
    int text_colorHover;

    // pressed state
    bool pressed;

    bool enabled;

    // Functions
    void drawText() const;

    void onMouseMovedTo(const s_MouseEvent &event);

    void onMouseRightButtonClicked(const s_MouseEvent &event);

    void onMouseLeftButtonPressed(const s_MouseEvent &event);

    void onMouseLeftButtonClicked(const s_MouseEvent &event);
};


#endif //D2TM_CGUIBUTTON_H
