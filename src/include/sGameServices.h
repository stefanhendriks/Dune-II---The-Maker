#pragma once

class GameContext;
class cGameObjectContext;
class cInfoContext;
class cGameSettings;

struct sGameServices
{
    GameContext *ctx = nullptr;
    cGameObjectContext *objects = nullptr;
    cInfoContext *info = nullptr;
    cGameSettings *settings = nullptr;
};
