#include "context/TextContext.hpp"

void TextContext::resetCache() const {
    if (smallTextDrawer) {
        smallTextDrawer->resetCache();
    }
    if (beneTextDrawer) {
        beneTextDrawer->resetCache();
    }
    if (gameTextDrawer) {
        gameTextDrawer->resetCache();
    }
}
