#include "gamestates/cEditorState.h"
#include "gui/GuiBar.h"
#include "gui/GuiStateButton.h"
#include "gui/GuiButtonGroup.h"
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

const int heightSize = 48;

cEditorState::cEditorState(cGame &theGame, GameContext* ctx) 
    : cGameState(theGame, ctx),
    m_gfxinter(ctx->getGraphicsContext()->gfxinter.get()),
    m_gfxeditor(ctx->getGraphicsContext()->gfxeditor.get())
{
    const cRectangle &selectRect = cRectangle(0, 0, m_game.m_screenW, heightSize+1);
    const cRectangle &modifRect = cRectangle(m_game.m_screenW-heightSize-1, heightSize-1, heightSize, m_game.m_screenH-heightSize);
    m_selectBar = std::make_unique<GuiBar>(selectRect,GuiBarPlacement::HORIZONTAL);
    m_modifBar = std::make_unique<GuiBar>(modifRect,GuiBarPlacement::VERTICAL);
    m_selectBar->setTheme(GuiTheme::Light());
    m_modifBar->setTheme(GuiTheme::Light());

    populateSelectBar();
    populateModifBar();
}

cEditorState::~cEditorState()
{

}

void cEditorState::populateSelectBar()
{
    m_selectGroup = std::make_unique<GuiButtonGroup>();
    auto rectGui = cRectangle(96,1,heightSize,heightSize);
    auto guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(STONELAYER))
            .onClick([this]() {
                std::cout << "STONELAYER" << std::endl;
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addGuiObject(guiButton);

    rectGui = cRectangle(96+1*(heightSize+5),1,heightSize,heightSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(SPICELAYER))
            .onClick([this]() {
                std::cout << "SPICELAYER" << std::endl;
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addGuiObject(guiButton);

    rectGui = cRectangle(96+2*(heightSize+5),1,heightSize,heightSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(BUILDING))
            .onClick([this]() {
                std::cout << "BUILDING" << std::endl;
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addGuiObject(guiButton);

    rectGui = cRectangle(96+3*(heightSize+5),1,heightSize,heightSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(UNITS))
            .onClick([this]() {
                std::cout << "UNITS" << std::endl;
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addGuiObject(guiButton);
}

void cEditorState::populateModifBar()
{
    m_topologyGroup = std::make_unique<GuiButtonGroup>();
    auto rectGui = cRectangle(m_game.m_screenW-heightSize-1,96,heightSize,heightSize);
    auto guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_HILL))
            .onClick([this]() {
                std::cout << "TERRAN_HILL" << std::endl;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_modifBar->addGuiObject(guiButton);
    
    rectGui = cRectangle(m_game.m_screenW-heightSize-1, 96+heightSize+5,heightSize,heightSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_MOUNTAIN))
            .onClick([this]() {
                std::cout << "TERRAN_MOUNTAIN" << std::endl;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_modifBar->addGuiObject(guiButton);

    rectGui = cRectangle(m_game.m_screenW-heightSize-1,96+2*(heightSize+5),heightSize,heightSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_ROCK))
            .onClick([this]() {
                std::cout << "TERRAN_ROCK" << std::endl;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_modifBar->addGuiObject(guiButton);
    
    rectGui = cRectangle(m_game.m_screenW-heightSize-1, 96+3*(heightSize+5),heightSize,heightSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_SAND))
            .onClick([this]() {
                std::cout << "TERRAN_SAND" << std::endl;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_modifBar->addGuiObject(guiButton);
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