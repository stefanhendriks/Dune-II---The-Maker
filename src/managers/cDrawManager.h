#pragma once

#include "controls/cKeyboardEvent.h"
#include "drawers/cBuildingListDrawer.h"
#include "drawers/cMapDrawer.h"
#include "drawers/cMessageDrawer.h"
#include "drawers/cMiniMapDrawer.h"
#include "drawers/cMouseDrawer.h"
#include "drawers/cOrderDrawer.h"
#include "drawers/cParticleDrawer.h"
#include "drawers/cPlaceItDrawer.h"
#include "drawers/CreditsDrawer.h"
//#include "drawers/cSideBarDrawer.h"
#include "drawers/cStructureDrawer.h"

#include "observers/cInputObserver.h"
#include "sMouseEvent.h"

#include <memory>

class cPlayer;
struct SDL_Surface;
class Texture;
class GameContext;
class Graphics;
class SDLDrawer;
class cSideBarDrawer;
class GameContext;

/**
 * Candidate class for PlayingMissionState (draw function?), but also for SelectYourNextConquestState (drawing
 * message bar is re-used there)
 */
class cDrawManager : cInputObserver {

public:
    explicit cDrawManager(GameContext *ctx, cPlayer *thePlayer);
    ~cDrawManager();

    void drawCombatState();

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    cMiniMapDrawer *getMiniMapDrawer() {
        return m_miniMapDrawer.get();
    }

    cOrderDrawer *getOrderDrawer() {
        return m_orderDrawer.get();
    }

    cMouseDrawer *getMouseDrawer() {
        return m_mouseDrawer;
    }

    cPlaceItDrawer *getPlaceItDrawer() {
        return m_placeitDrawer.get();
    }

    cBuildingListDrawer *getBuildingListDrawer();

    void drawMouse();

    void drawCombatMouse();

    void setPlayerToDraw(cPlayer *playerToDraw);

    void think();

    void init();

    void missionInit();

    void thinkFast_statePlaying();

    void thinkFast();

    void drawMessageBar();

    void setMessage(std::string msg, bool keepMessage = false);

    void setKeepMessage(bool value);

    void regionInit(int offsetX, int offsetY);

    bool hasMessage();

    void reset();
protected:
    void drawSidebar();

    void drawCredits();

    void drawStructurePlacing();

    void drawMessage();

    void drawRallyPoint();

    void drawTopBarBackground();

private:
    void drawOptionBar();

    void drawDebugInfoUsages() const;

    void drawNotifications();

    // Properties:
    std::unique_ptr<cSideBarDrawer> m_sidebarDrawer;
    std::unique_ptr<CreditsDrawer> m_creditsDrawer;
    std::unique_ptr<cOrderDrawer> m_orderDrawer;
    std::unique_ptr<cMapDrawer> m_mapDrawer;
    std::unique_ptr<cMiniMapDrawer> m_miniMapDrawer;
    std::unique_ptr<cParticleDrawer> m_particleDrawer;
    std::unique_ptr<cMessageDrawer> m_messageDrawer;
    std::unique_ptr<cPlaceItDrawer> m_placeitDrawer;
    std::unique_ptr<cStructureDrawer> m_structureDrawer;
    Color m_sidebarColor;
    Texture *m_btnOptions;
    bool m_drawToolTip = false;
    // TODO: bullet/projectile drawer
    cPlayer *m_player = nullptr;
    GameContext *m_ctx = nullptr;
    cTextDrawer *m_textDrawer = nullptr;
    SDLDrawer* m_renderDrawer = nullptr;
    Graphics* m_gfxinter = nullptr;
    Graphics* m_gfxdata = nullptr;
    cMouseDrawer* m_mouseDrawer = nullptr;

    void onKeyDown(const cKeyboardEvent &event);

    void onKeyPressed(const cKeyboardEvent &event);
};
