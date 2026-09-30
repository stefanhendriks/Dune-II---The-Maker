/**
 * @file cUnitDrawer.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

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
