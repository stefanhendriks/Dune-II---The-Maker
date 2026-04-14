#include "cGameControlsContext.h"
#include "game/cGameSettings.h"
#include "gameobjects/structures/cAbstractStructure.h"
#include "gameobjects/structures/cStructures.h"
#include "gameobjects/units/cUnits.h"
#include "map/cMap.h"
#include "data/gfxdata.h"
#include "game/cGame.h"
#include "include/d2tmc.h"
#include "managers/cDrawManager.h"
#include "map/cMapCamera.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"

#include "controls/mousestates/cMouseNormalState.h"
#include "controls/mousestates/cMouseUnitsSelectedState.h"
#include "controls/mousestates/cMouseRepairState.h"
#include "controls/mousestates/cMousePlaceState.h"
#include "controls/mousestates/cMouseDeployState.h"

#include <cassert>

cGameControlsContext::cGameControlsContext(cPlayer *player, cMouse *mouse) :
    m_mouseHoveringOverStructureId(-1),
    m_mouseHoveringOverUnitId(-1),
    m_mouseOnBattleField(false),
    m_mouseCell(-99),
    m_player(player),
    m_state(MOUSESTATE_SELECT),
    m_prevState(MOUSESTATE_SELECT),
    m_prevStateBeforeRepair(MOUSESTATE_SELECT),
    m_mouse(mouse),
    m_mouseNormalState(std::make_unique<cMouseNormalState>(player, this, m_mouse)),
    m_mouseUnitsSelectedState(std::make_unique<cMouseUnitsSelectedState>(player, this, m_mouse)),
    m_mouseRepairState(std::make_unique<cMouseRepairState>(player, this, m_mouse)),
    m_mousePlaceState(std::make_unique<cMousePlaceState>(player, this, m_mouse)),
    m_mouseDeployState(std::make_unique<cMouseDeployState>(player, this, m_mouse)),
    m_prevTickMouseAtBattleField(false)
{
    assert(player!=nullptr && "Expected player in cGameControlsContext constructor");
    assert(mouse!=nullptr && "Expected mouse in cGameControlsContext constructor");
}

cGameControlsContext::~cGameControlsContext()
{
    // not owner
    m_player = nullptr;
    m_mouse = nullptr;

    // smart pointers handle cleanup
}

void cGameControlsContext::updateMouseCell(const cPoint &coords)
{
    if (coords.y < cSideBar::TopBarHeight) {
        m_mouseCell = MOUSE_CELL_HOVER_TOPBAR; // at the top bar or higher, so no mouse cell id.
        m_mouseOnBattleField = false;
        return;
    }

    if (game.m_drawManager->getMiniMapDrawer()->isMouseOver()) {
        m_mouseCell = MOUSE_CELL_HOVER_MINIMAP; // on minimap
        m_mouseOnBattleField = false;
        return;
    }

    if (coords.x > (game.m_gameSettings->getScreenW() - cSideBar::SidebarWidth)) {
        m_mouseCell = MOUSE_CELL_HOVER_SIDEBAR; // on sidebar
        m_mouseOnBattleField = false;
        return;
    }
    m_mouseOnBattleField = true;
    m_mouseCell = getMouseCellFromScreen(coords.x, coords.y);
}

int cGameControlsContext::getMouseCellFromScreen(int mouseX, int mouseY) const
{
    int absMapX = game.m_mapCamera->getAbsMapMouseX(mouseX);
    int absMapY = game.m_mapCamera->getAbsMapMouseY(mouseY);
    return game.m_mapCamera->getCellFromAbsolutePosition(absMapX, absMapY);
}

void cGameControlsContext::determineHoveringOverStructureId()
{
    m_mouseHoveringOverStructureId = -1;

    if (!game.m_gameObjectsContext->getMap().isVisible(m_mouseCell, this->m_player)) {
        return; // cell not visible
    }

    m_mouseHoveringOverStructureId = game.m_gameObjectsContext->getMap().getCellIdStructuresLayer(m_mouseCell);
}

void cGameControlsContext::determineHoveringOverUnitId()
{
    if (m_mouseHoveringOverUnitId > -1) {
        cUnit &aUnit = game.m_gameObjectsContext->getUnits()[m_mouseHoveringOverUnitId];
        if (aUnit.isValid()) {
            aUnit.rendering.bHovered = false;
        }
    }
    m_mouseHoveringOverUnitId = -1;
    int mc = getMouseCell();
    tCell *cellOfMouse = game.m_gameObjectsContext->getMap().getCell(mc);
    if (cellOfMouse == nullptr) return; // mouse is not on battlefield

    if (!game.m_gameObjectsContext->getMap().isVisible(mc, this->m_player)) {
        return; // cell not visible
    }

    if (cellOfMouse->id[MAPID_UNITS] > -1) {
        int iUnitId = cellOfMouse->id[MAPID_UNITS];

        if (!game.m_gameObjectsContext->getUnits()[iUnitId].isHidden()) {
            m_mouseHoveringOverUnitId = iUnitId;
        }

    }
    else if (cellOfMouse->id[MAPID_WORMS] > -1) {
        int iUnitId = cellOfMouse->id[MAPID_WORMS];
        m_mouseHoveringOverUnitId = iUnitId;
    }

    if (m_mouseHoveringOverUnitId > -1) {
        cUnit &aUnit = game.m_gameObjectsContext->getUnits()[m_mouseHoveringOverUnitId];
        if (aUnit.isValid()) {
            aUnit.rendering.bHovered = true;
        }
    }
}

cAbstractStructure *cGameControlsContext::getStructurePointerWhereMouseHovers() const
{
    if (m_mouseHoveringOverStructureId < 0) {
        return nullptr;
    }
    return game.m_gameObjectsContext->getStructures()[m_mouseHoveringOverStructureId];
}

void cGameControlsContext::onMouseMovedTo(const s_MouseEvent &event)
{
    updateMouseCell(event.coords);
    bool mouseOnBattleField = isMouseOnBattleField();
    if (mouseOnBattleField) {
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

        logbook(std::format("setMouseState() : changing state from [{}] to [{}] (prevState=[{}], prevStateBeforeRepair=[{}])", mouseStateString(m_state), mouseStateString(newState), mouseStateString(m_prevState), mouseStateString(m_prevStateBeforeRepair)));
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
