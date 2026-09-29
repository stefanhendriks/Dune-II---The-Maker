#pragma once

class GameContext;
class cGameObjectContext;
class cInfoContext;
class cGameSettings;
class cLog;
class cMapCamera;
class cStructureUtils;
class cEventEmitter;
class cDrawManager;
class cRectangle;
class cReinforcements;

struct sGameServices
{
    GameContext *ctx = nullptr;
    cGameObjectContext *objects = nullptr;
    cInfoContext *info = nullptr;
    cGameSettings *settings = nullptr;
    cMapCamera *mapCamera = nullptr;
    cStructureUtils *structureUtils = nullptr;
    cEventEmitter *eventEmitter = nullptr;
    cDrawManager *drawManager = nullptr;
    cRectangle *mapViewport = nullptr;
    cReinforcements *reinforcements = nullptr;
};
