#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "controls/eKeyAction.h"
#include "sMouseEvent.h"
#include <optional>
#include <vector>
#include <SDL2/SDL.h>

class Texture;

class cReinforcements;
class cTextDrawer;
class cMapCamera;
class cGameInterface;
class cStructureFactory;
class cDrawManager;
class cPlayer;
class cTextDrawer;

class cGamePlaying : public cGameState {
public:
    explicit cGamePlaying(sGameServices* services);
    ~cGamePlaying() override;

    void thinkFast() override;
    void thinkNormal() override;
    void thinkSlow() override;
    void draw() const override;
    void update() override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

    void missionInit();
private:
    cGameObjectContext* m_objects = nullptr;
    cGameSettings* m_settings = nullptr;
    cReinforcements* m_reinforcements = nullptr;
    cGameInterface* m_interface = nullptr;
    cMapCamera* m_mapCamera = nullptr;
    cStructureFactory* m_structureFactory = nullptr;
    cDrawManager *m_drawManager = nullptr;
    cPlayer *m_controlledPlayer = nullptr;
    cTextDrawer* m_textDrawer = nullptr;
    void evaluatePlayerStatus();
    void drawCombatMouse() const;
    int m_TIMER_evaluatePlayerStatus;
    

    void onKeyDownGamePlaying(const cKeyboardEvent &event);
    void onKeyPressedGamePlaying(const cKeyboardEvent &event);
    void onKeyDownDebugMode(const cKeyboardEvent &event);
    void tryDebugSwitchToPlayer(int playerIndex);

    void centerCameraOnUnits(const std::vector<int>& unitIds);
    void drawTrackingOverlays() const;

    std::vector<int> m_trackedUnitIds;
    int m_lastGroupKeyPressed = 0;
    Uint32 m_lastGroupKeyPressedTick = 0;
    std::optional<eKeyAction> m_lastDebugSwitchAction;
};
