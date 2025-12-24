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

const int heightBarSize = 48;

cEditorState::cEditorState(cGame &theGame, GameContext* ctx) 
    : cGameState(theGame, ctx),
    m_gfxdata(ctx->getGraphicsContext()->gfxdata.get()),
    m_gfxeditor(ctx->getGraphicsContext()->gfxeditor.get())
{
    const cRectangle &selectRect = cRectangle(0, 0, m_game.m_screenW, heightBarSize+1);
    const cRectangle &modifRect = cRectangle(m_game.m_screenW-heightBarSize-1, heightBarSize-1, heightBarSize, m_game.m_screenH-heightBarSize);
    mapSizeArea = cRectangle(0,heightBarSize,m_game.m_screenW-heightBarSize-1,m_game.m_screenH-heightBarSize-1);
    m_selectBar = std::make_unique<GuiBar>(selectRect,GuiBarPlacement::HORIZONTAL);
    m_modifBar = std::make_unique<GuiBar>(modifRect,GuiBarPlacement::VERTICAL);
    m_selectBar->setTheme(GuiTheme::Light());
    m_modifBar->setTheme(GuiTheme::Light());

    populateSelectBar();
    populateModifBar();
    std::cout << "Entered Editor State" << std::endl;
}

cEditorState::~cEditorState()
{

}

void cEditorState::populateSelectBar()
{
    m_selectGroup = std::make_unique<GuiButtonGroup>();
    auto rectGui = cRectangle(96,1,heightBarSize,heightBarSize);
    auto guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(STONELAYER))
            .onClick([this]() {
                std::cout << "STONELAYER" << std::endl;
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addGuiObject(guiButton);

    rectGui = cRectangle(96+1*(heightBarSize+5),1,heightBarSize,heightBarSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(SPICELAYER))
            .onClick([this]() {
                std::cout << "SPICELAYER" << std::endl;
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addGuiObject(guiButton);

    rectGui = cRectangle(96+2*(heightBarSize+5),1,heightBarSize,heightBarSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(BUILDING))
            .onClick([this]() {
                std::cout << "BUILDING" << std::endl;
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addGuiObject(guiButton);

    rectGui = cRectangle(96+3*(heightBarSize+5),1,heightBarSize,heightBarSize);
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
    auto rectGui = cRectangle(m_game.m_screenW-heightBarSize-1,96,heightBarSize,heightBarSize);
    auto guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_HILL))
            .onClick([this]() {
                std::cout << "TERRAN_HILL" << std::endl;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_modifBar->addGuiObject(guiButton);
    
    rectGui = cRectangle(m_game.m_screenW-heightBarSize-1, 96+heightBarSize+5,heightBarSize,heightBarSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_MOUNTAIN))
            .onClick([this]() {
                std::cout << "TERRAN_MOUNTAIN" << std::endl;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_modifBar->addGuiObject(guiButton);

    rectGui = cRectangle(m_game.m_screenW-heightBarSize-1,96+2*(heightBarSize+5),heightBarSize,heightBarSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_ROCK))
            .onClick([this]() {
                std::cout << "TERRAN_ROCK" << std::endl;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_modifBar->addGuiObject(guiButton);
    
    rectGui = cRectangle(m_game.m_screenW-heightBarSize-1, 96+3*(heightBarSize+5),heightBarSize,heightBarSize);
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
    drawMap();
    m_selectBar->draw();
    m_modifBar->draw();
    m_game.getMouse()->draw();
}

void cEditorState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    if (event.coords.isWithinRectangle(&mapSizeArea)) {
        if (event.eventType== MOUSE_SCROLLED_DOWN) {
            tileLenSize -=4;
            tileLenSize = std::max(tileLenSize, 4);
        } else if (event.eventType== MOUSE_SCROLLED_UP) {
            tileLenSize +=4;
            tileLenSize = std::min(tileLenSize, 64);
        }
        return;
    } else {
        m_selectBar->onNotifyMouseEvent(event);
        m_modifBar->onNotifyMouseEvent(event);
    }
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

void cEditorState::drawMap() const
{
    if (m_mapData == nullptr) {
        return;
    }
    int tileID;
    cRectangle destRect;
    cRectangle srcRect{0,0,32,32}; // we take the first full textured sprite
    for (size_t j = 1; j < m_mapData->getRows()-1; ++j) { // Lignes
        for (size_t i = 1; i < m_mapData->getCols()-1; ++i) { // Colonnes
            destRect= cRectangle((i-1)*tileLenSize,heightBarSize+(j-1)*tileLenSize,tileLenSize,tileLenSize);
            tileID = (*m_mapData)[i][j];
            switch (tileID)
            {
            case TERRAIN_SPICE:
                renderDrawer->renderStrechSprite(m_gfxdata->getTexture(TERRAIN_SPICE), srcRect, destRect);
                break;
            case TERRAIN_SAND:
                renderDrawer->renderStrechSprite(m_gfxdata->getTexture(TERRAIN_SAND), srcRect, destRect);
                break;
            case TERRAIN_MOUNTAIN:
                renderDrawer->renderStrechSprite(m_gfxdata->getTexture(TERRAIN_MOUNTAIN), srcRect, destRect);
                break;
            case TERRAIN_ROCK:
                renderDrawer->renderStrechSprite(m_gfxdata->getTexture(TERRAIN_ROCK), srcRect, destRect);
                break;
            case TERRAIN_SPICEHILL:
                renderDrawer->renderStrechSprite(m_gfxdata->getTexture(TERRAIN_SPICEHILL), srcRect, destRect);
                break;  
            case TERRAIN_HILL:
                renderDrawer->renderStrechSprite(m_gfxdata->getTexture(TERRAIN_HILL), srcRect, destRect);
                break;            
            default:
                break;
            }
        }
    }
}