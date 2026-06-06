#pragma once

#include <memory>

// this class holds the game controls context. This means, the updateMouseAndKeyboardStateAndGamePlaying() method
// will check how the mouse is positioned in this particular frame. It then updates any ids, or other relevant data.
//
// For instance, this class knows if a mouse is hovering above a structure, or a unit. The sidebar
// or above the minimap. It also knows if an order has been issued (attack, move, repair, etc).
//
// a context belongs to a player

#include "controls/mousestates/eMouseStates.h"
#include "controls/cMouse.h"
#include "observers/cInputObserver.h"
#include "observers/cScenarioObserver.h"

// Forward declarations for mouse states
class cMouseNormalState;
class cMouseUnitsSelectedState;
class cMouseRepairState;
class cMousePlaceState;
class cMouseDeployState;

class cAbstractStructure;
class cGameObjectContext;
class cMapCamera;
class cGameSettings;
class cDrawManager;
class cRectangle;
class cGameInterface;
class cStructureUtils;
class cInfoContext;
struct sGameServices;

class cGameControlsContext : public cInputObserver, cScenarioObserver {
public:
    explicit cGameControlsContext(cPlayer *player, cMouse *mouse);
    ~cGameControlsContext() override;

    void serviceInit(sGameServices *services);

    cGameObjectContext *getObjects() const { return m_objects; }
    cMapCamera *getMapCamera() const { return m_mapCamera; }
    cGameSettings *getSettings() const { return m_settings; }
    cDrawManager *getDrawManager() const { return m_drawManager; }
    cGameInterface *getInterface() const { return m_interface; }
    cStructureUtils *getStructureUtils() const { return m_structureUtils; }
    cInfoContext *getInfos() const { return m_infos; }
    cRectangle *getMapViewport() const { return m_mapViewport; }

    int getIdOfStructureWhereMouseHovers() const {
        return m_mouseHoveringOverStructureId;
    }
    int getIdOfUnitWhereMouseHovers() const {
        return m_mouseHoveringOverUnitId;
    }

    int getMouseCell() const {
        return m_mouseCell;
    }
    int getMouseCellClamped() const {
        return m_mouseCellClamped;
    }

    bool isMouseOverStructure() const {
        return m_mouseHoveringOverStructureId > -1;
    }
    bool isMouseOverUnit() const {
        return m_mouseHoveringOverUnitId > -1;
    }

    bool isMouseOnSidebar() const {
        return m_mouseCell == MOUSE_CELL_HOVER_SIDEBAR;
    }
    bool isMouseOnTopBar() const {
        return m_mouseCell == MOUSE_CELL_HOVER_TOPBAR;
    }
    bool isMouseOnMiniMap() const {
        return m_mouseCell == MOUSE_CELL_HOVER_MINIMAP;
    }
    bool isMouseOnSidebarOrMinimap() const {
        return isMouseOnSidebar() || isMouseOnMiniMap();
    }

    bool isMouseOnBattleField() const;

    bool isMouseRightButtonPressed() const{
        return m_mouse->isRightButtonPressed();
    }

    cAbstractStructure *getStructurePointerWhereMouseHovers() const;

    int getMouseCellFromScreen(int mouseX, int mouseY) const;
    int getMouseCellFromScreenClamped(int mouseX, int mouseY) const;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;
    void onNotifyGameEvent(const s_GameEvent &event) override;

    void setMouseState(eMouseState newState);

    void toPreviousState();

    bool isState(eMouseState other) const;

    void onFocusMouseStateEvent();

protected:
    void determineHoveringOverStructureId();
    void determineHoveringOverUnitId();

private:
    int m_mouseHoveringOverStructureId;
    int m_mouseHoveringOverUnitId;
    bool m_mouseOnBattleField;

    // on what cell is the mouse hovering
    int m_mouseCell;
    int m_mouseCellClamped;

    // context belongs to specific player
    cPlayer *m_player;

    // the state to direct events to
    eMouseState m_state;
    eMouseState m_prevState;             // used to switch back previous state (ie from Place, or Repair mode)
    eMouseState m_prevStateBeforeRepair; // the state before repair mode only, used when Place/Repair mode is used
    // in conjunction

    cMouse *m_mouse;

    cGameObjectContext *m_objects = nullptr;
    cMapCamera *m_mapCamera = nullptr;
    cGameSettings *m_settings = nullptr;
    cDrawManager *m_drawManager = nullptr;
    cGameInterface *m_interface = nullptr;
    cStructureUtils *m_structureUtils = nullptr;
    cInfoContext *m_infos = nullptr;
    cRectangle *m_mapViewport = nullptr;

    // the states, initialized once to save a lot of construct/destructs (and make it possible
    // to switch between states without needing to restore 'state' within the state object)
    std::unique_ptr<cMouseNormalState> m_mouseNormalState;
    std::unique_ptr<cMouseUnitsSelectedState> m_mouseUnitsSelectedState;
    std::unique_ptr<cMouseRepairState> m_mouseRepairState;
    std::unique_ptr<cMousePlaceState> m_mousePlaceState;
    std::unique_ptr<cMouseDeployState> m_mouseDeployState;

    //
    bool m_prevTickMouseAtBattleField;

    // mouse state
    void onNotifyMouseStateEvent(const s_MouseEvent &event);
    void onBlurMouseStateEvent();
    void onMouseMovedTo(const s_MouseEvent &event);
    void updateMouseCell(const cPoint &coords);

    static constexpr int MOUSE_CELL_HOVER_TOPBAR = -1;
    static constexpr int MOUSE_CELL_HOVER_MINIMAP = -2;
    static constexpr int MOUSE_CELL_HOVER_SIDEBAR = -3;
};
