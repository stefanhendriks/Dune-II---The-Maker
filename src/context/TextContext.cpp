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
