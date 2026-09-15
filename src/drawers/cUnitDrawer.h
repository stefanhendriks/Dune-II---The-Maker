#pragma once

class cUnit;
class SDLDrawer;
class Graphics;
class cTextDrawer;

class cUnitDrawer {
public:
    cUnitDrawer(cUnit &unit, SDLDrawer *renderer, Graphics *gfxdata);
    ~cUnitDrawer() = default;

    void draw();
    void draw_health();
    void draw_experience();
    void draw_spice();
    void draw_group(cTextDrawer *textDrawer);
    void draw_path() const;
    void draw_debug(cTextDrawer *textDrawer);

private:
    cUnit &m_unit;
    SDLDrawer *m_renderer;
    Graphics *m_gfxdata;
};
