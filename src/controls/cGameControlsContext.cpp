#include "cGameControlsContext.h"

#include "data/gfxdata.h"
#include "d2tmc.h"
#include "managers/cDrawManager.h"

cGameControlsContext::cGameControlsContext(cPlayer *player, cMouse *mouse) :
    m_mouseHoveringOverStructureId(-1),
    m_mouseHoveringOverUnitId(-1),
    m_mouseOnBattleField(false),
    m_drawToolTip(false),
    m_mouseCell(-99),
    m_player(player),
    m_state(MOUSESTATE_SELECT),
    m_prevState(MOUSESTATE_SELECT),
    m_prevStateBeforeRepair(MOUSESTATE_SELECT),
    m_mouse(mouse),
    m_mouseNormalState(new cMouseNormalState(player, this, m_mouse)),
    m_mouseUnitsSelectedState(new cMouseUnitsSelectedState(player, this, m_mouse)),
    m_mouseRepairState(new cMouseRepairState(player, this, m_mouse)),
    m_mousePlaceState(new cMousePlaceState(player, this, m_mouse)),
    m_mouseDeployState(new cMouseDeployState(player, this, m_mouse)),
    m_prevTickMouseAtBattleField(false)
{
    assert(player && "Expected player in cGameControlsContext constructor");
    assert(mouse && "Expected mouse in cGameControlsContext constructor");
}

cGameControlsContext::~cGameControlsContext()
{
    // not owner
    m_player = nullptr;
    m_mouse = nullptr;

    // created these, so delete
    delete m_mouseNormalState;
    delete m_mouseUnitsSelectedState;
    delete m_mouseRepairState;
    delete m_mousePlaceState;
    delete m_mouseDeployState;
}


void cGameControlsContext::updateMouseCell(const cPoint &coords)
{
    if (coords.y < cSideBar::TopBarHeight) {
        m_mouseCell = MOUSECELL_TOPBAR; // at the top bar or higher, so no mouse cell id.
        m_mouseOnBattleField = false;
        return;
    }

    if (drawManager->getMiniMapDrawer()->isMouseOver()) {
        m_mouseCell = MOUSECELL_MINIMAP; // on minimap
        m_mouseOnBattleField = false;
        return;
    }

    if (coords.x > (game.m_screenW - cSideBar::SidebarWidth)) {
        m_mouseCell = MOUSECELL_SIDEBAR; // on sidebar
        m_mouseOnBattleField = false;
        return;
    }
    m_mouseOnBattleField = true;
    m_mouseCell = getMouseCellFromScreen(coords.x, coords.y);
}

int cGameControlsContext::getMouseCellFromScreen(int mouseX, int mouseY) const
{
    int absMapX = mapCamera->getAbsMapMouseX(mouseX);
    int absMapY = mapCamera->getAbsMapMouseY(mouseY);
    return mapCamera->getCellFromAbsolutePosition(absMapX, absMapY);
}

void cGameControlsContext::determineToolTip()
{
    // @Mira : magical call to key ... 
    // m_drawToolTip = false;
    // if (key[SDL_SCANCODE_T] && isMouseOnBattleField()) { // TODO: this gets removed later, when we redo tooltips anyway
        // m_drawToolTip = true;
    // }
}

void cGameControlsContext::determineHoveringOverStructureId()
{
    m_mouseHoveringOverStructureId = -1;

    if (!map.isVisible(m_mouseCell, this->m_player)) {
        return; // cell not visible
    }

    m_mouseHoveringOverStructureId = map.getCellIdStructuresLayer(m_mouseCell);
}

void cGameControlsContext::determineHoveringOverUnitId()
{
    if (m_mouseHoveringOverUnitId > -1) {
        cUnit &aUnit = unit[m_mouseHoveringOverUnitId];
        if (aUnit.isValid()) {
            aUnit.bHovered = false;
        }
    }
    m_mouseHoveringOverUnitId = -1;
    int mc = getMouseCell();
    tCell *cellOfMouse = map.getCell(mc);
    if (cellOfMouse == nullptr) return; // mouse is not on battlefield

    if (!map.isVisible(mc, this->m_player)) {
        return; // cell not visible
    }

    if (cellOfMouse->id[MAPID_UNITS] > -1) {
        int iUnitId = cellOfMouse->id[MAPID_UNITS];

        if (!unit[iUnitId].isHidden()) {
            m_mouseHoveringOverUnitId = iUnitId;
        }

    }
    else if (cellOfMouse->id[MAPID_WORMS] > -1) {
        int iUnitId = cellOfMouse->id[MAPID_WORMS];
        m_mouseHoveringOverUnitId = iUnitId;
    }

    if (m_mouseHoveringOverUnitId > -1) {
        cUnit &aUnit = unit[m_mouseHoveringOverUnitId];
        if (aUnit.isValid()) {
            aUnit.bHovered = true;
        }
    }
}

cAbstractStructure *cGameControlsContext::getStructurePointerWhereMouseHovers() const
{
    if (m_mouseHoveringOverStructureId < 0) {
        return nullptr;
    }
    return structure[m_mouseHoveringOverStructureId];
}

void cGameControlsContext::onMouseMovedTo(const s_MouseEvent &event)
{
    updateMouseCell(event.coords);
    bool mouseOnBattleField = isMouseOnBattleField();
    if (mouseOnBattleField) {
        determineToolTip();
        determineHoveringOverStructureId();
        determineHoveringOverUnitId();

        if (!m_prevTickMouseAtBattleField) {
            onFocusMouseStateEvent();
        }
    }
    else {
        m_mouseHoveringOverStructureId = -1;
        m_mouseHoveringOverUnitId = -1;

        if (m_prevTickMouseAtBattleField) {
            onBlurMouseStateEvent();
        }
    }

    // remember state
    m_prevTickMouseAtBattleField = mouseOnBattleField;
}

void cGameControlsContext::onNotifyMouseEvent(const s_MouseEvent &event)
{
    if (event.eventType == eMouseEventType::MOUSE_MOVED_TO) {
        onMouseMovedTo(event);
    }

    // mouse states only apply to battlefield (for now)
    onNotifyMouseStateEvent(event);

    if (!isMouseOnBattleField()) {
        m_mouse->setTile(MOUSE_NORMAL);
    }

}

bool cGameControlsContext::isMouseOnBattleField() const
{
    return m_mouseOnBattleField;
}

void cGameControlsContext::onNotifyMouseStateEvent(const s_MouseEvent &event)
{
    if (isMouseOnBattleField()) {
        switch (m_state) {
            case MOUSESTATE_SELECT:
                m_mouseNormalState->onNotifyMouseEvent(event);
                break;
            case MOUSESTATE_UNITS_SELECTED:
                m_mouseUnitsSelectedState->onNotifyMouseEvent(event);
                break;
            case MOUSESTATE_REPAIR:
                m_mouseRepairState->onNotifyMouseEvent(event);
                break;
            case MOUSESTATE_PLACE:
                m_mousePlaceState->onNotifyMouseEvent(event);
                break;
            case MOUSESTATE_DEPLOY:
                m_mouseDeployState->onNotifyMouseEvent(event);
                break;
        }
    }
    else {
        // if
    }
}

void cGameControlsContext::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    switch (m_state) {
        case MOUSESTATE_SELECT:
            m_mouseNormalState->onNotifyKeyboardEvent(event);
            break;
        case MOUSESTATE_UNITS_SELECTED:
            m_mouseUnitsSelectedState->onNotifyKeyboardEvent(event);
            break;
        case MOUSESTATE_REPAIR:
            m_mouseRepairState->onNotifyKeyboardEvent(event);
            break;
        case MOUSESTATE_PLACE:
            m_mousePlaceState->onNotifyKeyboardEvent(event);
            break;
        case MOUSESTATE_DEPLOY:
            m_mouseDeployState->onNotifyKeyboardEvent(event);
            break;
    }
}

void cGameControlsContext::setMouseState(eMouseState newState)
{
    if (newState != m_state) {
        // Remember previous state as long as it is not the PLACE state, since we can't go back to that state
        if (m_state != eMouseState::MOUSESTATE_PLACE) {
            m_prevState = m_state;
            if (newState == eMouseState::MOUSESTATE_REPAIR) {
                m_prevStateBeforeRepair = m_state;
            }
        }

        // we might get into a situation where our previous state was repair state, and we get into
        // repair state again (ie, Repair->Place->Repair state). In that case, we override the 'prevState'
        // with the 'prevStateBeforeRepair'.
        // since we don't have a real 'history' or 'stack' of mouse states, this is for now good enough.
        if (newState == m_prevState && newState == eMouseState::MOUSESTATE_REPAIR) {
            m_prevState = m_prevStateBeforeRepair;
        }

        logbook(fmt::format("setMouseState() : changing state from [{}] to [{}] (prevState=[{}], prevStateBeforeRepair=[{}])", mouseStateString(m_state), mouseStateString(newState), mouseStateString(m_prevState), mouseStateString(m_prevStateBeforeRepair)));
        m_state = newState;
        switch (m_state) {
            case MOUSESTATE_SELECT:
                m_mouseNormalState->onStateSet();
                break;
            case MOUSESTATE_UNITS_SELECTED:
                m_mouseUnitsSelectedState->onStateSet();
                break;
            case MOUSESTATE_REPAIR:
                m_mouseRepairState->onStateSet();
                break;
            case MOUSESTATE_PLACE:
                m_mousePlaceState->onStateSet();
                break;
            case MOUSESTATE_DEPLOY:
                m_mouseDeployState->onStateSet();
                break;
        }
    }
}

void cGameControlsContext::toPreviousState()
{
    setMouseState(m_prevState);
}

bool cGameControlsContext::isState(eMouseState other) const
{
    return this->m_state == other;
}

void cGameControlsContext::onBlurMouseStateEvent()
{
    switch (m_state) {
        case MOUSESTATE_SELECT:
            m_mouseNormalState->onBlur();
            break;
        case MOUSESTATE_UNITS_SELECTED:
            m_mouseUnitsSelectedState->onBlur();
            break;
        case MOUSESTATE_REPAIR:
            m_mouseRepairState->onBlur();
            break;
        case MOUSESTATE_PLACE:
            m_mousePlaceState->onBlur();
            break;
        case MOUSESTATE_DEPLOY:
            m_mouseDeployState->onBlur();
            break;
    }
}

void cGameControlsContext::onFocusMouseStateEvent()
{
    switch (m_state) {
        case MOUSESTATE_SELECT:
            m_mouseNormalState->onFocus();
            break;
        case MOUSESTATE_UNITS_SELECTED:
            m_mouseUnitsSelectedState->onFocus();
            break;
        case MOUSESTATE_REPAIR:
            m_mouseRepairState->onFocus();
            break;
        case MOUSESTATE_PLACE:
            m_mousePlaceState->onFocus();
            break;
        case MOUSESTATE_DEPLOY:
            m_mouseDeployState->onFocus();
            break;
    }
}

void cGameControlsContext::onNotifyGameEvent(const s_GameEvent &event)
{
    switch (m_state) {
        case MOUSESTATE_SELECT:
            m_mouseNormalState->onNotifyGameEvent(event);
            break;
        case MOUSESTATE_UNITS_SELECTED:
            m_mouseUnitsSelectedState->onNotifyGameEvent(event);
            break;
        case MOUSESTATE_REPAIR:
            m_mouseRepairState->onNotifyGameEvent(event);
            break;
        case MOUSESTATE_PLACE:
            m_mousePlaceState->onNotifyGameEvent(event);
            break;
        case MOUSESTATE_DEPLOY:
            m_mouseDeployState->onNotifyGameEvent(event);
            break;
    }
}
