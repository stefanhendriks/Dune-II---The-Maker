#include "d2tmh.h"
#include "cGuiButton.h"

#include <allegro/color.h>

cGuiButton::cGuiButton(const cTextDrawer &textDrawer, const cRectangle &rect, const std::string & btnText)
  : rect(rect)
  , textDrawer(textDrawer)
  , btnText(btnText)
  , renderKind(eGuiButtonRenderKind::OPAQUE_WITHOUT_BORDER)
  , textAlignHorizontal(eGuiTextAlignHorizontal::CENTER)
  , onLeftMouseButtonClicked_action(nullptr)
  , focus(false)
  , gui_colorButton(makecol(176, 176, 196))
  , gui_colorBorderLight(makecol(252, 252, 252))
  , gui_colorBorderDark(makecol(84, 84, 120))
  , text_color(makecol(255, 255, 255)) // default white color
  , text_colorHover(makecol(255, 0, 0))
  , pressed(false)
  , enabled(true) { // by default always enabled
 }

cGuiButton::cGuiButton(const cTextDrawer &textDrawer, const cRectangle &rect, const std::string &btnText,
                       eGuiButtonRenderKind renderKind)
  : cGuiButton(textDrawer, rect, btnText) {
    this->renderKind = renderKind;
}

cGuiButton::cGuiButton(const cTextDrawer &textDrawer, const cRectangle &rect, const std::string &btnText, int gui_colorButton,
                       int gui_colorBorderLight, int gui_colorBorderDark)
  : cGuiButton(textDrawer, rect, btnText) {
    this->gui_colorButton = gui_colorButton;
    this->gui_colorBorderLight = gui_colorBorderLight;
    this->gui_colorBorderDark = gui_colorBorderDark;
}

cGuiButton::~cGuiButton() {
    delete onLeftMouseButtonClicked_action;
}

void cGuiButton::draw() const {
    switch (renderKind) {
        case OPAQUE_WITHOUT_BORDER:
            allegroDrawer->drawRectangleFilled(bmp_screen, rect, gui_colorButton);
            drawText();
            break;
        case TRANSPARENT_WITHOUT_BORDER:
            drawText();
            break;
        case OPAQUE_WITH_BORDER:
            allegroDrawer->drawRectangleFilled(bmp_screen, rect, gui_colorButton);
            if (pressed) {
                allegroDrawer->gui_DrawRectBorder(bmp_screen, rect, gui_colorBorderDark, gui_colorBorderLight);
            } else {
                allegroDrawer->gui_DrawRectBorder(bmp_screen, rect, gui_colorBorderLight, gui_colorBorderDark);
            }
            drawText();
            break;
        case TRANSPARENT_WITH_BORDER:
            if (pressed) {
                allegroDrawer->gui_DrawRectBorder(bmp_screen, rect, gui_colorBorderDark, gui_colorBorderLight);
            } else {
                allegroDrawer->gui_DrawRectBorder(bmp_screen, rect, gui_colorBorderLight, gui_colorBorderDark);
            }
            drawText();
            break;
    }
}

bool cGuiButton::hasFocus() {
    return focus;
}

void cGuiButton::setTextAlignHorizontal(eGuiTextAlignHorizontal value) {
    textAlignHorizontal = value;
}

void cGuiButton::setRenderKind(eGuiButtonRenderKind value) {
    renderKind = value;
}

void cGuiButton::drawText() const {
    int textColor = focus ? text_colorHover : text_color;
    if (!enabled) {
        textColor = focus ? gui_colorBorderDark : gui_colorBorderLight;
    }

    switch (textAlignHorizontal) {
        case eGuiTextAlignHorizontal::CENTER:
            if (pressed) {
                textDrawer.drawTextCenteredInBox(btnText.c_str(), rect, textColor, 1, 1);
            } else {
                textDrawer.drawTextCenteredInBox(btnText.c_str(), rect, textColor);
            }
            break;
        case eGuiTextAlignHorizontal::LEFT:
            if (pressed) {
                textDrawer.drawText(rect.getX() + 1, rect.getY() + 1, textColor, btnText.c_str());
            } else {
                textDrawer.drawText(rect.getX(), rect.getY(), textColor, btnText.c_str());
            }
            break;
    }
}

void cGuiButton::nextRenderKind() {
    if (renderKind == eGuiButtonRenderKind::OPAQUE_WITH_BORDER) {
        renderKind = eGuiButtonRenderKind::OPAQUE_WITHOUT_BORDER;
    } else if (renderKind == eGuiButtonRenderKind::OPAQUE_WITHOUT_BORDER) {
        renderKind = eGuiButtonRenderKind::TRANSPARENT_WITH_BORDER;
    } else if (renderKind == eGuiButtonRenderKind::TRANSPARENT_WITH_BORDER) {
        renderKind = eGuiButtonRenderKind::TRANSPARENT_WITHOUT_BORDER;
    } else if (renderKind == eGuiButtonRenderKind::TRANSPARENT_WITHOUT_BORDER) {
        renderKind = eGuiButtonRenderKind::OPAQUE_WITH_BORDER;
    }
}

void cGuiButton::toggleTextAlignHorizontal() {
    if (textAlignHorizontal == eGuiTextAlignHorizontal::CENTER) {
        textAlignHorizontal = eGuiTextAlignHorizontal::LEFT;
    } else {
        textAlignHorizontal = eGuiTextAlignHorizontal::CENTER;
    }
}

void cGuiButton::setGui_ColorButton(int value) {
    gui_colorButton = value;
}

void cGuiButton::setTextColor(int value) {
    text_color = value;
}

void cGuiButton::setTextColorHover(int value) {
    text_colorHover = value;
}

void cGuiButton::onNotifyMouseEvent(const s_MouseEvent &event) {
    switch (event.eventType) {
        case MOUSE_MOVED_TO:
            onMouseMovedTo(event);
            break;
        case MOUSE_RIGHT_BUTTON_CLICKED:
            onMouseRightButtonClicked(event);
            break;
        case MOUSE_LEFT_BUTTON_CLICKED:
            onMouseLeftButtonClicked(event);
            break;
        case MOUSE_LEFT_BUTTON_PRESSED:
            onMouseLeftButtonPressed(event);
            break;
        default:
            break;
    }
}

void cGuiButton::onMouseMovedTo(const s_MouseEvent &event) {
    focus = rect.isPointWithin(event.coords);
}

void cGuiButton::onMouseRightButtonClicked(const s_MouseEvent &) {
    if (game.isDebugMode()) {
        if (key[KEY_LSHIFT]) { // TODO: replace with code in onNotifyKeyboardEvent
            if (focus) toggleTextAlignHorizontal();
        } else {
            if (focus) nextRenderKind();
        }
    }
}

void cGuiButton::onMouseLeftButtonPressed(const s_MouseEvent &) {
    if (enabled) {
        pressed = focus;
    }
}

void cGuiButton::onMouseLeftButtonClicked(const s_MouseEvent &) {
    if (focus) {
        if (enabled && onLeftMouseButtonClicked_action) {
            onLeftMouseButtonClicked_action->execute();
        }
    }
}

void cGuiButton::setOnLeftMouseButtonClickedAction(cGuiAction *action) {
    this->onLeftMouseButtonClicked_action = action;
}

void cGuiButton::setEnabled(bool value) {
    enabled = value;
}

void cGuiButton::onNotifyKeyboardEvent(const cKeyboardEvent &) {

}

//
//cGuiButton::cGuiButton(const cGuiButton &src) :
//        rect(src.rect),
//        textDrawer(src.textDrawer),
//        btnText(src.btnText),
//        renderKind(src.renderKind),
//        textAlignHorizontal(src.textAlignHorizontal),
//        onLeftMouseButtonClicked_action(src.onLeftMouseButtonClicked_action),
//        focus(src.focus),
//        gui_colorButton(src.gui_colorButton),
//        gui_colorBorderLight(src.gui_colorBorderLight),
//        gui_colorBorderDark(src.gui_colorBorderDark),
//        text_color(src.text_color),
//        text_colorHover(src.text_colorHover),
//        pressed(src.pressed) {
//
//}

//cGuiButton &cGuiButton::operator=(const cGuiButton &rhs) {
//    // Guard self assignment
//    if (this == &rhs)
//        return *this;
//
//    this->rect = rhs.rect;
//    this->textDrawer = rhs.textDrawer;
//    this->btnText = rhs.btnText;
//    this->renderKind = rhs.renderKind;
//    this->textAlignHorizontal = rhs.textAlignHorizontal;
////    if (rhs.onLeftMouseButtonClicked_action) {
////        this->missionKind = rhs.missionKind->clone(player, this);
////    } else {
////        this->missionKind = nullptr;
////    }
//    // onLeftMouseButtonClicked_action... want, delete wordt uitgevoerd bij resizen vector ofzo meen ik
//    this->focus = rhs.focus;
//    this->gui_colorButton = rhs.gui_colorButton;
//    this->gui_colorBorderLight = rhs.gui_colorBorderLight;
//    this->gui_colorBorderDark = rhs.gui_colorBorderDark;
//    this->text_color = rhs.text_color;
//    this->text_colorHover = rhs.text_colorHover;
//    this->pressed = rhs.pressed;
//
//    return *this;
//}
