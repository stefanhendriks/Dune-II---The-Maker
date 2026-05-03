#include "gui/GuiStateButton.h"
#include "gui/GuiButtonGroup.h"

GuiStateButton::GuiStateButton(SDLDrawer* drawer, const cRectangle &rect)
    : GuiObject(drawer, rect)
{
    assert(drawer != nullptr);
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
    m_renderDrawer->renderStrechSprite(m_tex, *m_currentRectState, m_rect);
}

void GuiStateButton::onNotifyMouseEvent(const s_MouseEvent &event)
{
    if (!event.coords.isWithinRectangle(&m_rect))
        return;
    if (event.eventType == eMouseEventType::MOUSE_LEFT_BUTTON_CLICKED) {
        if (m_group.has_value()) {
            if (m_group.value()->isCurrentlySelected(this) && m_onLeftMouseButtonClickedAction) {
                m_onLeftMouseButtonClickedAction();
                return;
            }
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
        // Only execute the click action if the state actually changes from UNCLICKED to CLICKED
        if (m_state != GuiState::CLICKED) {
            changeState(GuiState::CLICKED);
            m_onLeftMouseButtonClickedAction();
        }
    } else {
        changeState(GuiState::UNCLICKED);
    }
}

void GuiStateButton::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (!event.isType(eKeyEventType::PRESSED) || !m_group.has_value()) {
        return;
    }

    const int groupNumber = event.getGroupNumber();
    if (groupNumber > 0) {
        m_group.value()->updateState(groupNumber - 1);
    }
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