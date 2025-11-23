#include "cGameState.h"

cGameState::cGameState(cGame &theGame, GameContext* _ctx) :
    m_game(theGame),
    m_ctx(_ctx)
{
}


std::string gameStateToString(GameState state)
{
    switch (state) {
        case GameState::INITIALIZE: return "initialize game";
        case GameState::OVER: return "game over";
        case GameState::MENU: return "in a menu";
        case GameState::PLAYING: return "playing the actual game";
        case GameState::BRIEFING: return "doing some briefing";
        case GameState::EDITING: return "running the editor";
        case GameState::OPTIONS: return "options menu";
        case GameState::REGION: return "select a region / select your next conquest";
        case GameState::SELECT_HOUSE: return "pick a house";
        case GameState::INTRO: return "intro";
        case GameState::TELLHOUSE: return "tell about the house";
        case GameState::WINNING: return "when winning, do some cool animation";
        case GameState::WINBRIEF: return "mentat chatter when won the mission";
        case GameState::LOSEBRIEF: return "briefing when losing";
        case GameState::LOSING: return "when losing, do something cool";
        case GameState::SKIRMISH: return "playing a skirmish mission!";
        case GameState::SETUPSKIRMISH: return "set up a skirmish game";
        case GameState::CREDITS: return "credits";
        case GameState::MISSIONSELECT: return "mission select";
        // COUNT n'est pas un état valide pour le jeu, mais pour l'itération.
        // On peut lever une erreur ou retourner une chaîne spéciale.
        case GameState::COUNT: return "[Error: GameState::COUNT]";
    }
    // Gestion des valeurs non reconnues (si l'enum est casté à partir d'un char arbitraire)
    throw std::out_of_range("Unknown GameState value.");
}