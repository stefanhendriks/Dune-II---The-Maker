#pragma once

#include "map/cPreviewMaps.h"

class cRandomMapGenerator {
public:
    cRandomMapGenerator();

    void generateRandomMap(int startingPoints, s_PreviewMap &randomMapEntry);

private:

    void drawProgress(float progress) const;
};
