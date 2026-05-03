#pragma once

#include "gameobjects/map/cPreviewMaps.h"

class cRandomMapGenerator {
public:
    cRandomMapGenerator();
    void generateRandomMap(int width, int height, int startingPoints, s_PreviewMap &randomMapEntry);

private:
    //void drawProgress(float progress) const;
};
