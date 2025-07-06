#include "gui/GuiLabel.hpp"

#include "d2tmc.h"
#include "drawers/SDLDrawer.hpp"

GuiLabel::GuiLabel(const cRectangle &rect, const std::string &btnText)
    : GuiObject(rect)
    , m_textDrawer(nullptr)
    , m_buttonText(btnText)
    , m_renderKind(GuiRenderKind::OPAQUE_WITHOUT_BORDER)
    , m_textAlignHorizontal(GuiTextAlignHorizontal::CENTER)
    , m_tex(nullptr)
    , m_focus(false)
    , m_enabled(true)
{}

GuiLabel::~GuiLabel()
{}

void GuiLabel::draw() const
{
    if (!m_enabled)
        return;
    switch (m_renderKind) {
        case OPAQUE_WITHOUT_BORDER:
            renderDrawer->renderRectFillColor(m_rect.getX(), m_rect.getY(), m_rect.getWidth(), m_rect.getHeight(), m_theme.fillColor);
            drawText();
            break;
        case TRANSPARENT_WITHOUT_BORDER:
            drawText();
            break;
        case OPAQUE_WITH_BORDER:
            renderDrawer->renderRectFillColor(m_rect.getX(), m_rect.getY(), m_rect.getWidth(), m_rect.getHeight(), m_theme.fillColor);
            drawRectBorder(m_theme.borderLight, m_theme.borderDark);
            drawText();
            break;
        case TRANSPARENT_WITH_BORDER:
            drawRectBorder(m_theme.borderLight, m_theme.borderDark);
            drawText();
            break;
        case WITH_TEXTURE:
            renderDrawer->renderSprite(m_tex, m_rect.getX(),m_rect.getY());
            break;
    }
}

void GuiLabel::setTextAlignHorizontal(GuiTextAlignHorizontal value)
{
    m_textAlignHorizontal = value;
}

void GuiLabel::setRenderKind(GuiRenderKind value)
{
    m_renderKind = value;
}

void GuiLabel::setTextDrawer(cTextDrawer *cTextDrawer)
{
    m_textDrawer = cTextDrawer;
}

void GuiLabel::setTexture(Texture *tex)
{
    m_tex = tex;
}

void GuiLabel::drawText() const
{
    Color textColor = m_theme.textColor;

    switch (m_textAlignHorizontal) {
        case GuiTextAlignHorizontal::CENTER:
            m_textDrawer->drawTextCenteredInBox(m_buttonText.c_str(), m_rect, textColor);
            break;
        case GuiTextAlignHorizontal::LEFT:
            m_textDrawer->drawText(m_rect.getX(), m_rect.getY(), textColor, m_buttonText.c_str());
            break;
    }
}

void GuiLabel::nextRenderKind()
{
    if (m_renderKind == GuiRenderKind::OPAQUE_WITH_BORDER) {
        m_renderKind = GuiRenderKind::OPAQUE_WITHOUT_BORDER;
    }
    else if (m_renderKind == GuiRenderKind::OPAQUE_WITHOUT_BORDER) {
        m_renderKind = GuiRenderKind::TRANSPARENT_WITH_BORDER;
    }
    else if (m_renderKind == GuiRenderKind::TRANSPARENT_WITH_BORDER) {
        m_renderKind = GuiRenderKind::TRANSPARENT_WITHOUT_BORDER;
    }
    else if (m_renderKind == GuiRenderKind::TRANSPARENT_WITHOUT_BORDER) {
        m_renderKind = GuiRenderKind::OPAQUE_WITH_BORDER;
    }
}

void GuiLabel::toggleTextAlignHorizontal()
{
    if (m_textAlignHorizontal == GuiTextAlignHorizontal::CENTER) {
        m_textAlignHorizontal = GuiTextAlignHorizontal::LEFT;
    }
    else {
        m_textAlignHorizontal = GuiTextAlignHorizontal::CENTER;
    }
}

void GuiLabel::onNotifyMouseEvent(const s_MouseEvent &event)
{
    switch (event.eventType) {
        case MOUSE_MOVED_TO:
            onMouseMovedTo(event);
            break;
        default:
            break;
    }
}

void GuiLabel::onMouseMovedTo(const s_MouseEvent &event)
{
    m_focus = m_rect.isPointWithin(event.coords);
}

void GuiLabel::setEnabled(bool value)
{
    m_enabled = value;
}

void GuiLabel::onNotifyKeyboardEvent(const cKeyboardEvent &)
{}
