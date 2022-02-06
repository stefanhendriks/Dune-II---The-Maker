#include "cMouseDeployState.h"

#include <algorithm>
#include "d2tmh.h"

cMouseDeployState::cMouseDeployState(cPlayer *player, cGameControlsContext *context, cMouse *mouse) :
        cMouseState(player, context, mouse) {
}

void cMouseDeployState::onNotifyMouseEvent(const s_MouseEvent &event) {
    // these methods can have a side-effect which changes mouseTile...
    switch (event.eventType) {
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
    if (m_context->isState(MOUSESTATE_DEPLOY)) { // if , required in case we switched state
        m_mouse->setTile(mouseTile);
    }
}

void cMouseDeployState::onMouseLeftButtonClicked() {
    // this assumes the m_context has been updated beforehand...
    int mouseCell = m_context->getMouseCell();

    if (mouseCell < 0) {
        return;
    }

    cBuildingListItem *itemToDeploy = m_player->getSideBar()->getList(eListType::LIST_PALACE)->getItemToDeploy();
    if (itemToDeploy == nullptr) return;

    s_GameEvent event {
            .eventType = eGameEventType::GAME_EVENT_SPECIAL_LAUNCH,
            .entityType = eBuildType::SPECIAL,
            .entityID = -1,
            .player = m_player,
            .entitySpecificType = itemToDeploy->getBuildId(),
            .atCell = mouseCell,
            .isReinforce = false,
            .buildingListItem = itemToDeploy
    };
    game.onNotifyGameEvent(event);
}

void cMouseDeployState::onMouseRightButtonPressed() {
    m_mouse->dragViewportInteraction();
}

void cMouseDeployState::onMouseRightButtonClicked() {
    if (m_mouse->isMapScrolling()) {
        m_mouse->resetDragViewportInteraction();
    } else {
        m_context->toPreviousState();
    }
}

void cMouseDeployState::onMouseMovedTo() {
    // TODO: update state for determining place result so that renderer does not need to do
    // that logic every frame (see cPlaceItDrawer)
}

void cMouseDeployState::onStateSet() {
    mouseTile = MOUSE_ATTACK; // TODO: have other cursor for this? based on thing to deploy?
    m_mouse->setTile(mouseTile);
}


void cMouseDeployState::onNotifyKeyboardEvent(const cKeyboardEvent &) {
}

void cMouseDeployState::onFocus() {
    m_mouse->setTile(mouseTile);
}

void cMouseDeployState::onNotifyGameEvent(const s_GameEvent &) {
    m_context->toPreviousState();
}
