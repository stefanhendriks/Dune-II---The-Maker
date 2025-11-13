#pragma once

#include <memory>
#include "drawers/cTextDrawer.h"
class cTextDrawer;

class TextContext {

public:
    void resetCache() const;

    std::unique_ptr<cTextDrawer> smallTextDrawer;
    std::unique_ptr<cTextDrawer> beneTextDrawer;
    std::unique_ptr<cTextDrawer> gameTextDrawer;
};