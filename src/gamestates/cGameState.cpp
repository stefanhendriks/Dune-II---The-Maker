#include "cGameState.h"

cGameState::cGameState(cGame &theGame, GameContext* _ctx) :
    m_game(theGame),
    m_ctx(_ctx)
{
}


std::string gameStateToString(GameState state)
{
    switch (state) {
        case GameState::INITIALIZE: return "INITIALIZE";
        case GameState::OVER: return "OVER";
        case GameState::MENU: return "MENU";
        case GameState::PLAYING: return "PLAYING";
        case GameState::BRIEFING: return "BRIEFING";
        case GameState::EDITING: return "EDITING";
        case GameState::OPTIONS: return "OPTIONS";
        case GameState::REGION: return "REGION";
        case GameState::SELECT_HOUSE: return "SELECT_HOUSE";
        case GameState::INTRO: return "INTRO";
        case GameState::TELLHOUSE: return "TELLHOUSE";
        case GameState::WINNING: return "WINNING";
        case GameState::WINBRIEF: return "WINBRIEF";
        case GameState::LOSEBRIEF: return "LOSEBRIEF";
        case GameState::LOSING: return "LOSING";
        case GameState::SKIRMISH: return "SKIRMISH";
        case GameState::SETUPSKIRMISH: return "SETUPSKIRMISH";
        case GameState::CREDITS: return "CREDITS";
        case GameState::MISSIONSELECT: return "MISSIONSELECT";
        // COUNT is not a valid state for the game, but it is for the iteration.
        // We return a special string. Maybe an out of range error is better ?
        case GameState::COUNT: return "[Error: GameState::COUNT]";
    }
    // Handling unrecognized values ​​(if the enum is cast from an arbitrary character)
    throw std::out_of_range("Unknown GameState value");
}