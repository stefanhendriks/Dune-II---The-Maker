#pragma once

#include "gameobjects/map/cPreviewMaps.h"

class cGameObjectContext;
class SDLDrawer;

class cRandomMapGenerator {
public:
    cRandomMapGenerator();
    void generateRandomMap(int width, int height, int startingPoints, s_PreviewMap &randomMapEntry, cGameObjectContext* objects, SDLDrawer* renderer);

private:
    //void drawProgress(float progress) const;
};
