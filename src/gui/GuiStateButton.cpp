#include "gui/GuiStateButton.h"
#include "gui/GuiButtonGroup.h"
#include "d2tmc.h"

GuiStateButton::GuiStateButton(const cRectangle &rect)
    : GuiObject(rect)
{
    m_state = GuiState::UNCLICKED;
    rectState[GuiState::UNCLICKED] = std::make_unique<cRectangle>(0,0,32, 32);
    rectState[GuiState::DISABLED] = std::make_unique<cRectangle>(32,0,32, 32);
    rectState[GuiState::CLICKED] = std::make_unique<cRectangle>(64,0,32, 32);
    m_currentRectState = rectState[GuiState::UNCLICKED].get();
    m_group = std::nullopt;
}

GuiStateButton::~GuiStateButton()
{}

void GuiStateButton::setGroup(GuiButtonGroup* g)
{
    m_group = g;
    m_group.value()->add(this);
}

void GuiStateButton::setTexture(Texture *tex)
{
    m_tex = tex;
}

void GuiStateButton::changeState(GuiState newState)
{
    if (m_state == newState) {
        return;
    }

    m_state = newState;
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
    if (!event.coords.isWithinRectangle(&m_rect))
        return;
    if (event.eventType == eMouseEventType::MOUSE_LEFT_BUTTON_CLICKED) {
        if (m_group.has_value()) {
            m_group.value()->updateStates(this);
        } else {
            setPressed(true);
            //m_onLeftMouseButtonClickedAction();
        }
    }
}

void GuiStateButton::setPressed(bool value)
{
    if (value) {
        changeState(GuiState::CLICKED);
        m_onLeftMouseButtonClickedAction();
    } else {
        changeState(GuiState::UNCLICKED);
    }
}

void GuiStateButton::onNotifyKeyboardEvent(const cKeyboardEvent &)
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