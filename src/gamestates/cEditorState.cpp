#include "gamestates/cEditorState.h"
#include "gui/GuiBar.h"
#include "d2tmc.h"
#include "config.h"
#include "data/gfxinter.h"
#include "data/gfxdata.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "context/GameContext.hpp"
#include "context/GraphicsContext.hpp"
#include "map/cPreviewMaps.h"
#include "utils/Graphics.hpp"
#include "data/gfxeditor.h"
#include "data/gfxinter.h"

#include <SDL2/SDL.h>
#include <format>
#include <iostream>

const int heightSize = 34;

cEditorState::cEditorState(cGame &theGame, GameContext* ctx) 
    : cGameState(theGame, ctx),
    m_gfxinter(ctx->getGraphicsContext()->gfxinter.get()),
    m_gfxeditor(ctx->getGraphicsContext()->gfxeditor.get())
{
    const cRectangle &selectRect = cRectangle(0, 0, m_game.m_screenW, heightSize);
    const cRectangle &modifRect = cRectangle(m_game.m_screenW-heightSize, heightSize, heightSize, m_game.m_screenH-heightSize);
    m_selectBar = std::make_unique<GuiBar>(selectRect,GuiBarPlacement::HORIZONTAL);
    m_modifBar = std::make_unique<GuiBar>(modifRect,GuiBarPlacement::VERTICAL);
    m_selectBar->setTheme(GuiTheme::Light());
    m_modifBar->setTheme(GuiTheme::Light());
}

cEditorState::~cEditorState()
{

}

void cEditorState::thinkFast()
{
}

void cEditorState::draw() const
{
    m_selectBar->draw();
    m_modifBar->draw();
    m_game.getMouse()->draw();
}

void cEditorState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    m_selectBar->onNotifyMouseEvent(event);
    m_modifBar->onNotifyMouseEvent(event);
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
    m_mapData = std::make_unique<Matrix<int>>(map->terrainType, map->width, map->height);
}