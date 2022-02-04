#ifndef CDRAWMANAGER_H_
#define CDRAWMANAGER_H_

class cDrawManager : cInputObserver {

public:
    cDrawManager(cPlayer *thePlayer);
    ~cDrawManager();

    void drawCombatState();

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    CreditsDrawer *getCreditsDrawer() { return creditsDrawer; }

    cMessageDrawer *getMessageDrawer() { return messageDrawer; }

    cMiniMapDrawer *getMiniMapDrawer() { return miniMapDrawer; }

    cOrderDrawer *getOrderDrawer() { return orderDrawer; }

    cMouseDrawer *getMouseDrawer() { return mouseDrawer; }

    cPlaceItDrawer *getPlaceItDrawer() { return placeitDrawer; }

    cBuildingListDrawer *getBuildingListDrawer() { return sidebarDrawer->getBuildingListDrawer(); }

    void drawMouse();

    void drawCombatMouse();

    void destroy();

    void setPlayerToDraw(cPlayer *playerToDraw);

    void think();

    void init();

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
    cSideBarDrawer *sidebarDrawer;
    CreditsDrawer *creditsDrawer;
    cOrderDrawer *orderDrawer;
    cMapDrawer *mapDrawer;
    cMiniMapDrawer *miniMapDrawer;
    cParticleDrawer *particleDrawer;
    cMessageDrawer *messageDrawer;
    cPlaceItDrawer *placeitDrawer;
    cStructureDrawer *structureDrawer;
    cMouseDrawer *mouseDrawer;

    BITMAP *optionsBar;

    int sidebarColor;

    // TODO: unitDrawer

    // TODO: bullet/projectile drawer

    cPlayer *player;

    BITMAP *topBarBmp;

    cTextDrawer *textDrawer;

    void onKeyDown(const cKeyboardEvent &event);

    void onKeyPressed(const cKeyboardEvent &event);
};

#endif
