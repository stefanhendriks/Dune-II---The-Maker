#include "cMouseUnitsSelectedState.h"

#include "d2tmc.h"
#include "data/gfxdata.h"
#include "controls/cGameControlsContext.h"
#include "gameobjects/particles/cParticle.h"
#include "map/cMapCamera.h"
#include "player/cPlayer.h"
#include "utils/cSoundPlayer.h"

#include <fmt/core.h>
#include <allegro/mouse.h>

#include <algorithm>
#include <string>

namespace {

std::string mouseUnitsSelectedStateString(eMouseUnitsSelectedState state) {
    switch (state) {
        case SELECTED_STATE_MOVE: return "SELECTED_STATE_MOVE";
        case SELECTED_STATE_SELECT: return "SELECTED_STATE_SELECT";
        case SELECTED_STATE_ADD_TO_SELECTION: return "SELECTED_STATE_ADD_TO_SELECTION";
        case SELECTED_STATE_ATTACK: return "SELECTED_STATE_ATTACK";
        case SELECTED_STATE_FORCE_ATTACK: return "SELECTED_STATE_FORCE_ATTACK";
        case SELECTED_STATE_REPAIR: return "SELECTED_STATE_REPAIR";
        case SELECTED_STATE_CAPTURE: return "SELECTED_STATE_CAPTURE";
        case SELECTED_STATE_REFINERY: return "SELECTED_STATE_REFINERY";
        default:
            assert(false);
            break;
    }
    return {};
}

}

cMouseUnitsSelectedState::cMouseUnitsSelectedState(cPlayer *player, cGameControlsContext *context, cMouse *mouse) :
        cMouseState(player, context, mouse),
        m_selectedUnits(),
        m_harvestersSelected(false),
        m_infantrySelected(false),
        m_repairableUnitsSelected(false),
        m_state(SELECTED_STATE_MOVE),
        m_prevState(SELECTED_STATE_MOVE) {

}

void cMouseUnitsSelectedState::onNotifyMouseEvent(const s_MouseEvent &event) {

    // these methods can have a side-effect which changes mouseTile...
    switch (event.eventType) {
        case MOUSE_LEFT_BUTTON_PRESSED:
            m_mouse->boxSelectLogic(m_context->getMouseCell());
            break;
        case MOUSE_LEFT_BUTTON_CLICKED:
            onMouseLeftButtonClicked();
            break;
        case MOUSE_RIGHT_BUTTON_PRESSED:
            onMouseRightButtonPressed();
            break;
        case MOUSE_RIGHT_BUTTON_CLICKED:
            onMouseRightButtonClicked();
            break;
        case MOUSE_MOVED_TO:
            onMouseMovedTo();
            break;
        default:
            break;
    }

    // ... so set it here
    if (m_context->isState(MOUSESTATE_UNITS_SELECTED)) { // if , required in case we switched state
        m_mouse->setTile(mouseTile);
    }
}

void cMouseUnitsSelectedState::onMouseLeftButtonClicked() {
    if (m_mouse->isBoxSelecting()) {
        // clear only when we don't want to add to selection
        if (m_state != SELECTED_STATE_ADD_TO_SELECTION) {
            m_player->deselectAllUnits();
            m_selectedUnits.clear();
        }

        cRectangle boxSelectRectangle = m_mouse->getBoxSelectRectangle();
        const std::vector<int> &ids = m_player->getAllMyUnitsWithinViewportRect(boxSelectRectangle);
        m_player->selectUnits(ids);
    } else {
        // single click, no box select
        evaluateSelectedUnits();

        int mouseCell = m_context->getMouseCell();

        bool infantryAcknowledged = false;
        bool unitAcknowledged = false;
        if (m_state == SELECTED_STATE_SELECT) {
            int hoverStructureId = m_context->getIdOfStructureWhereMouseHovers();
            if (hoverStructureId > -1) {
                m_player->selected_structure = hoverStructureId;
                cAbstractStructure *pStructure = m_player->getSelectedStructure();
                if (pStructure && pStructure->isValid() && pStructure->belongsTo(m_player)) {
                    m_player->getSideBar()->setSelectedListId(pStructure->getAssociatedListID());
                } else {
                    m_player->selected_structure = -1;
                }
            }
        } else  if (m_state == SELECTED_STATE_REPAIR ||
                    m_state == SELECTED_STATE_REFINERY ||
                    m_state == SELECTED_STATE_MOVE) {

            for (auto id: m_selectedUnits) {
                cUnit &pUnit = unit[id];
                if (m_state == SELECTED_STATE_REPAIR) {
                    // only send units that are eligible for repair to facility
                    if (pUnit.isEligibleForRepair()) {
                        pUnit.move_to(mouseCell);
                    }
                    unitAcknowledged = true;
                } else if (m_state == SELECTED_STATE_REFINERY) {
                    // only send harvesters in group
                    if (pUnit.isHarvester()) {
                        pUnit.move_to(mouseCell);
                    }
                    unitAcknowledged = true;
                } else {
                    if (pUnit.isInfantryUnit()) {
                        infantryAcknowledged = true;
                    }
                    unitAcknowledged = true;
                    pUnit.move_to(mouseCell);
                }
            }
            spawnParticle(D2TM_PARTICLE_MOVE);
        } else if (m_state == SELECTED_STATE_ATTACK || m_state == SELECTED_STATE_FORCE_ATTACK) {
            for (auto id: m_selectedUnits) {
                cUnit &pUnit = unit[id];
                if (pUnit.isHarvester()) {
                    continue;
                }
                if (pUnit.isInfantryUnit()) {
                    infantryAcknowledged = true;
                } else {
                    unitAcknowledged = true;
                }
                pUnit.attackAt(mouseCell);
            }

            spawnParticle(D2TM_PARTICLE_ATTACK);
        }

        if (infantryAcknowledged) {
            game.playSound(SOUND_MOVINGOUT + rnd(2));
        }

        if (unitAcknowledged) {
            game.playSound(SOUND_ACKNOWLEDGED + rnd(3));
        }
    }

    m_mouse->resetBoxSelect();

    m_selectedUnits = m_player->getSelectedUnits();
    if (m_selectedUnits.empty()) {
        m_context->setMouseState(MOUSESTATE_SELECT);
    }

}

void cMouseUnitsSelectedState::onMouseRightButtonPressed() {
    m_mouse->dragViewportInteraction();
}

void cMouseUnitsSelectedState::onMouseRightButtonClicked() {
    // if we were dragging the viewport, keep the units and this state.
    if (!m_mouse->isMapScrolling()) {
        m_player->deselectAllUnits();
        // back to "select" state
        m_context->setMouseState(MOUSESTATE_SELECT);
    }

    m_mouse->resetDragViewportInteraction();
}

void cMouseUnitsSelectedState::onMouseMovedTo() {
    if (m_state == SELECTED_STATE_FORCE_ATTACK) {
        mouseTile = MOUSE_ATTACK;
    } if (m_state == SELECTED_STATE_ADD_TO_SELECTION) {
        mouseTile = MOUSE_NORMAL;
    } else {
        evaluateMouseMoveState();
    }
}

void cMouseUnitsSelectedState::evaluateMouseMoveState() {
    mouseTile = MOUSE_MOVE;
    setState(SELECTED_STATE_MOVE);

    cAbstractStructure *hoverStructure = m_context->getStructurePointerWhereMouseHovers();

    // this feels a little awkward, but having an extra 'bool' for this if statement
    // is probably a bit too much at this point.
    bool unitsWhichCanAttackSelected = m_infantrySelected || m_repairableUnitsSelected; // don't try to attack with harvesters

    if (hoverStructure) {
        if (!hoverStructure->getPlayer()->isSameTeamAs(m_player)) {
            if (unitsWhichCanAttackSelected) {
                mouseTile = MOUSE_ATTACK;
                setState(SELECTED_STATE_ATTACK);
            }
        } else if (hoverStructure->belongsTo(m_player)) {
            if (hoverStructure->getType() == REFINERY) {
                if (m_harvestersSelected) {
                    setState(SELECTED_STATE_REFINERY);
                } else {
                    setState(SELECTED_STATE_SELECT);
                    mouseTile = MOUSE_NORMAL; // allow "selecting" of structure, event though we have units selected
                }
            } else if (hoverStructure->getType() == REPAIR) {
                if (m_repairableUnitsSelected) {
                    setState(SELECTED_STATE_REPAIR);
                } else {
                    setState(SELECTED_STATE_SELECT);
                    mouseTile = MOUSE_NORMAL; // allow "selecting" of structure, event though we have units selected
                }
            } else {
                setState(SELECTED_STATE_SELECT);
                mouseTile = MOUSE_NORMAL; // allow "selecting" of structure, event though we have units selected
            }
        }
    }

    if (unitsWhichCanAttackSelected) {
        int hoverUnitId = m_context->getIdOfUnitWhereMouseHovers();
        if (hoverUnitId > -1) {
            if (unit[hoverUnitId].isValid() && !unit[hoverUnitId].getPlayer()->isSameTeamAs(m_player)) {
                mouseTile = MOUSE_ATTACK;
                setState(SELECTED_STATE_ATTACK);
            }
        }
    }
}

void cMouseUnitsSelectedState::onStateSet() {
    // only on state change we refresh the entire vector
    m_selectedUnits = m_player->getSelectedUnits();

    evaluateSelectedUnits();
    updateSelectedUnitsState();

    mouseTile = MOUSE_MOVE; // TODO: check if unit at cell
    m_mouse->setTile(mouseTile);
}

void cMouseUnitsSelectedState::updateSelectedUnitsState() {
    m_harvestersSelected = false;
    m_infantrySelected = false;
    m_repairableUnitsSelected = false;
    for (auto id: m_selectedUnits) {
        cUnit &pUnit = unit[id];
        if (pUnit.isHarvester()) {
            m_harvestersSelected = true;
            m_repairableUnitsSelected = true;
        } else if (pUnit.isInfantryUnit()) {
            m_infantrySelected = true;
        } else {
            m_repairableUnitsSelected = true;
        }

        if (m_harvestersSelected && m_infantrySelected && m_repairableUnitsSelected) {
            // no need to evaluate further
            break;
        }
    }
}

void cMouseUnitsSelectedState::evaluateSelectedUnits() {
    // remove all invalid and units that (no longer) belong to us (ie, deviated?)
    // TODO: let this (also?) react to game events, so we remove them when it happens.
    m_selectedUnits.erase(
            std::remove_if(
                    m_selectedUnits.begin(),
                    m_selectedUnits.end(),
                    [this](int id) {
                        return !unit[id].isValid() || // no (longer) valid
                               !unit[id].belongsTo(m_player) || // no longer belongs to player
                               unit[id].isHidden(); // hidden (entered structure, etc). Forget it then.
                    }),
            m_selectedUnits.end()
    );
}

void cMouseUnitsSelectedState::setState(eMouseUnitsSelectedState newState) {
    if (newState != m_state) {
        logbook(fmt::format("cMouseUnitsSelectedState: Changed state from [{}] to [{}]", mouseUnitsSelectedStateString(m_state),
                mouseUnitsSelectedStateString(newState)));
        this->m_prevState = this->m_state;
        this->m_state = newState;
    }
}

void cMouseUnitsSelectedState::onNotifyKeyboardEvent(const cKeyboardEvent &event) {
    switch (event.eventType) {
        case eKeyEventType::HOLD:
            onKeyDown(event);
            break;
        case eKeyEventType::PRESSED:
            onKeyPressed(event);
            break;
        default:
            break;

    }

    if (m_state == SELECTED_STATE_MOVE) {

    }

    if (m_context->isState(MOUSESTATE_UNITS_SELECTED)) { // if , required in case we switched state
        m_mouse->setTile(mouseTile);
    }
}

void cMouseUnitsSelectedState::onKeyDown(const cKeyboardEvent &event) {
    if (event.hasKey(KEY_LCONTROL) || event.hasKey(KEY_RCONTROL)) {
        setState(SELECTED_STATE_ATTACK);
        mouseTile = MOUSE_ATTACK;
    }

    bool appendingSelectionToGroup = event.hasKey(KEY_LSHIFT) || event.hasKey(KEY_RSHIFT);
    if (appendingSelectionToGroup) {
        setState(SELECTED_STATE_ADD_TO_SELECTION);
        mouseTile = MOUSE_NORMAL;

        int iGroup = event.getGroupNumber();

        // holding shift & group number, so add group to the selected units
        if (iGroup > 0) {
            this->m_player->selectUnitsFromGroup(iGroup);
        }

        m_selectedUnits = m_player->getSelectedUnits();
    } else {
        bool createGroup = event.hasKey(KEY_RCONTROL) || event.hasKey(KEY_LCONTROL);
        // Do this within the "HOLD" event, because if we do it at Pressed event
        // we miss the fact that we hold SHIFT as well (see cKeyboard for reason).
        if (!createGroup) {
            int iGroup = event.getGroupNumber();

            if (iGroup > 0) {
                // select all units for group
                m_player->deselectAllUnits();
                bool anyUnitSelected = m_player->selectUnitsFromGroup(iGroup);
                if (anyUnitSelected) {
                    m_selectedUnits = m_player->getSelectedUnits();
                } else {
                    // we get in a state where no units are selected,
                    // so get back into "select" state.
                    m_player->setContextMouseState(MOUSESTATE_SELECT);
                }
            }
        }
    }
    // force move?
}

void cMouseUnitsSelectedState::onKeyPressed(const cKeyboardEvent &event) {
    if (event.hasKey(KEY_LCONTROL) || event.hasKey(KEY_RCONTROL)) {
        setState(SELECTED_STATE_ATTACK);
        evaluateMouseMoveState();
    }

    if (event.hasKey(KEY_LSHIFT) || event.hasKey(KEY_RSHIFT)) {
        toPreviousState();
        evaluateMouseMoveState();
    }

    // go to repair state
    if (event.hasKey(KEY_R)) {
        m_context->setMouseState(MOUSESTATE_REPAIR);
    }
    
    // order any selected harvester to return to refinery
    if (event.hasKey(KEY_D)) {
        const std::vector<int> &selectedUnits = m_player->getSelectedUnits();
        for (auto & id : selectedUnits) {
            cUnit &pUnit = unit[id];
            if (pUnit.isHarvester() && pUnit.canUnload()) {
                pUnit.findBestStructureCandidateAndHeadTowardsItOrWait(REFINERY, true, INTENT_UNLOAD_SPICE);
            }
        }
    }

    // force move?
}

void cMouseUnitsSelectedState::toPreviousState() {
    m_state = m_prevState;
}

void cMouseUnitsSelectedState::spawnParticle(const int type) {
    int absoluteXCoordinate = mapCamera->getAbsMapMouseX(mouse_x);
    int absoluteYCoordinate = mapCamera->getAbsMapMouseY(mouse_y);
    cParticle::create(absoluteXCoordinate, absoluteYCoordinate, type, -1, -1);
}

void cMouseUnitsSelectedState::onFocus() {
    m_mouse->setTile(mouseTile);
}
