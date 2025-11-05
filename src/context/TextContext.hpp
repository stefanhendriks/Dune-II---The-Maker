#pragma once

#include <memory>
class cTextDrawer;

class TextContext {
public:
    std::shared_ptr<cTextDrawer> smallTextDrawer;
    std::shared_ptr<cTextDrawer> beneTextDrawer;
    std::shared_ptr<cTextDrawer> gameTextDrawer;
};