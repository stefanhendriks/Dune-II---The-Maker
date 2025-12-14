#include "gamestates/cEditorState.h"

#include "d2tmc.h"
#include "config.h"
#include "data/gfxinter.h"
#include "data/gfxdata.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "context/GameContext.hpp"
#include "context/GraphicsContext.hpp"
#include "map/cPreviewMaps.h"
#include <SDL2/SDL.h>
#include <format>
#include <iostream>

cEditorState::cEditorState(cGame &theGame, GameContext* ctx) : cGameState(theGame, ctx)
{

}

cEditorState::~cEditorState()
{

}

void cEditorState::thinkFast()
{
}

void cEditorState::draw() const
{
}

void cEditorState::onNotifyMouseEvent(const s_MouseEvent &event)
{
}

eGameStateType cEditorState::getType()
{
    return GAMESTATE_EDITOR;
}

void cEditorState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (event.isType(eKeyEventType::PRESSED) && event.hasKey(SDL_SCANCODE_ESCAPE)) {
        m_game.setNextStateToTransitionTo(GAME_MENU);
        m_game.initiateFadingOut();
    }
}

void cEditorState::loadMap(s_PreviewMap* map)
{
    std::cout << "open |"<< map->name << "|" << std::endl;
}