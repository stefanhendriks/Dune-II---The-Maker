#pragma once

class GameContext;
class cGameObjectContext;
class cInfoContext;
class cGameSettings;
class cLog;
class cStructureUtils;
class cEventEmitter;

struct sGameServices
{
    GameContext *ctx = nullptr;
    cGameObjectContext *objects = nullptr;
    cInfoContext *info = nullptr;
    cGameSettings *settings = nullptr;
    cStructureUtils *structureUtils = nullptr;
    cLog *m_log = nullptr;
    cEventEmitter *eventEmitter = nullptr;
};
