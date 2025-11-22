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