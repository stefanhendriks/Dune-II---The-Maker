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

struct sGameServices
{
    GameContext *ctx = nullptr;
    cGameObjectContext *objects = nullptr;
    cInfoContext *info = nullptr;
    cGameSettings *settings = nullptr;
    cMapCamera *mapCamera = nullptr;
    cStructureUtils *structureUtils = nullptr;
    cLog *m_log = nullptr;
    cEventEmitter *eventEmitter = nullptr;
    cDrawManager *drawManager = nullptr;
    cRectangle *mapViewport = nullptr;
};
