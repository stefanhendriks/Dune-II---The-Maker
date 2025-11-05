#pragma once

#include <memory>
class cTextDrawer;

class TextContext {
public:
    std::unique_ptr<cTextDrawer> smallTextDrawer;
    std::unique_ptr<cTextDrawer> beneTextDrawer;
    std::unique_ptr<cTextDrawer> gameTextDrawer;
};