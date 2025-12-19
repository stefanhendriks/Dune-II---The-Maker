#include "gui/GuiStateButton.h"
#include "d2tmc.h"

GuiStateButton::GuiStateButton(const cRectangle &rect)
    : GuiObject(rect)
{
    m_state = GuiState::UNCLICKED;
    rectState[GuiState::UNCLICKED] = std::make_unique<cRectangle>(0,0,rect.getWidth()/3, rect.getHeight());
    rectState[GuiState::DISABLED] = std::make_unique<cRectangle>(rect.getWidth()/3,0,rect.getWidth()/3, rect.getHeight());
    rectState[GuiState::CLICKED] = std::make_unique<cRectangle>(2*rect.getWidth()/3,0,rect.getWidth()/3, rect.getHeight());
    m_currentRectState = rectState[GuiState::DISABLED].get();
}

GuiStateButton::~GuiStateButton()
{}

void GuiStateButton::setTexture(Texture *tex)
{
    m_tex = tex;
}

void GuiStateButton::changeState()
{
    switch (m_state)
    {
    case GuiState::CLICKED:
        m_currentRectState = rectState[GuiState::CLICKED].get();
        break;
    case GuiState::DISABLED:
        m_currentRectState = rectState[GuiState::DISABLED].get();
        break;
    default:
        m_currentRectState = rectState[GuiState::UNCLICKED].get();
        break;
    }
}

void GuiStateButton::draw() const 
{
    renderDrawer->renderStrechSprite(m_tex, *m_currentRectState, m_rect);
}

void GuiStateButton::onNotifyMouseEvent(const s_MouseEvent &event)
{
}

void GuiStateButton::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
}

void GuiStateButton::setOnLeftMouseButtonClickedAction(std::function<void()> action)
{
    m_onLeftMouseButtonClickedAction = std::move(action);
}

void GuiStateButton::setOnRightMouseButtonClickedAction(std::function<void()> action)
{
    m_onRightMouseButtonClickedAction = std::move(action);
}

void GuiStateButton::setRenderKind(GuiRenderKind value)
{
    m_renderKind = value;
}