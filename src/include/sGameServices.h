#pragma once

class GameContext;
class cGameObjectContext;
class cInfoContext;
class cGameSettings;
class cLog;

struct sGameServices
{
    GameContext *ctx = nullptr;
    cGameObjectContext *objects = nullptr;
    cInfoContext *info = nullptr;
    cGameSettings *settings = nullptr;
    cLog *m_log = nullptr;
};
