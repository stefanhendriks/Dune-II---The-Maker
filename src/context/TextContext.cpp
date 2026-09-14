/**
 * @file TextContext.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "context/TextContext.hpp"

TextContext::TextContext(
    std::unique_ptr<cTextDrawer> smallTextDrawer,
    std::unique_ptr<cTextDrawer> beneTextDrawer,
    std::unique_ptr<cTextDrawer> gameTextDrawer) :
m_smallTextDrawer(std::move(smallTextDrawer)),
m_beneTextDrawer(std::move(beneTextDrawer)),
m_gameTextDrawer(std::move(gameTextDrawer))
{
}

void TextContext::resetCache() const {
    m_smallTextDrawer->resetCache();
    m_beneTextDrawer->resetCache();
    m_gameTextDrawer->resetCache();
}

cTextDrawer * TextContext::getSmallTextDrawer() const {
    return m_smallTextDrawer.get();
}

cTextDrawer * TextContext::getBeneTextDrawer() const {
    return m_beneTextDrawer.get();
}

cTextDrawer * TextContext::getGameTextDrawer() const {
    return m_gameTextDrawer.get();
}
