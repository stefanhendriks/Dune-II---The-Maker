#pragma once
#include <string>

struct GameSettings {
    int screenW = 800;
    int screenH = 600;
    float cameraDragMoveSpeed = 0.5f;
    float cameraBorderOrKeyMoveSpeed = 0.5f;
    bool cameraEdgeMove = true;
    bool windowed = false;
    bool allowRepeatingReinforcements = false;
    bool turretsDownOnLowPower = false;
    bool rocketTurretsDownOnLowPower = false;
    bool playMusic = true;
    bool playSound = true;
    bool debugMode = false;
    bool drawUnitDebug = false;
    bool disableAI = false;
    bool oneAi = false;
    bool disableWormAi = false;
    bool disableReinforcements = false;
    bool noAiRest = false;
    bool drawUsages = false;
    std::string gameFilename = "game.ini";
};