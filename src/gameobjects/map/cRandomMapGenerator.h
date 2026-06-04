#pragma once

#include "gameobjects/map/cPreviewMaps.h"

class cGameObjectContext;

class cRandomMapGenerator {
public:
    cRandomMapGenerator();
    void generateRandomMap(int width, int height, int startingPoints, s_PreviewMap &randomMapEntry, cGameObjectContext* objects);

private:
    //void drawProgress(float progress) const;
};
