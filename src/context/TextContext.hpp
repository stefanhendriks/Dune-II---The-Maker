/**
 * @file TextContext.hpp
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

#include <memory>
#include "drawers/cTextDrawer.h"
class cTextDrawer;

class TextContext {

public:
    TextContext(
        std::unique_ptr<cTextDrawer> smallTextDrawer,
        std::unique_ptr<cTextDrawer> beneTextDrawer,
        std::unique_ptr<cTextDrawer> gameTextDrawer
    );

    void resetCache() const;

    [[nodiscard]] cTextDrawer* getSmallTextDrawer() const;
    [[nodiscard]] cTextDrawer* getBeneTextDrawer() const;
    [[nodiscard]] cTextDrawer* getGameTextDrawer() const;

private:
    std::unique_ptr<cTextDrawer> m_smallTextDrawer;
    std::unique_ptr<cTextDrawer> m_beneTextDrawer;
    std::unique_ptr<cTextDrawer> m_gameTextDrawer;
};