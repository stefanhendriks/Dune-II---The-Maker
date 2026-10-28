/**
 * @file cNewmapEditorState.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "gamestates/cNewMapEditorState.h"
#include "game/GameStateTransition.hpp"
#include "game/ScenarioFlow.hpp"
#include "controls/cMouse.h"

#include "gui/GuiButton.h"
#include "gui/GuiWindow.h"
#include "gui/GuiLabel.hpp"
#include "gui/GuiTextInput.h"
#include "gui/GuiCycleButton.h"
#include "gui/GuiCheckBox.hpp"
#include "context/GameContext.hpp"
#include "context/cGameObjectContext.h"
#include "drawers/cTextDrawer.h"
#include "gameobjects/map/cPreviewMaps.h"
#include "gameobjects/map/cRandomMapGenerator.h"
#include "gameobjects/map/SeedMapLoader.hpp"
#include "utils/ini.h"
#include "utils/Log.h"

#include "include/cAssert.h"
#include <charconv>
#include <chrono>
#include <optional>

static const int borderSize = 2;

cNewMapEditorState::cNewMapEditorState(sGameServices* services, cIni* ini)
    : cGameState(services),
    m_textDrawer(m_ctx->getTextContext()->getBeneTextDrawer()),
    m_settings(services->settings),
    m_scenarioFlow(services->scenarioFlow),
    m_stateTransitions(services->stateTransitions),
    m_mouse(services->mouse),
    m_guiWindow(nullptr),
    m_objects(services->objects),
    m_previewMap(services->objects->getPreviewMaps()),
    m_cIni(ini)
{
    d2tm_assert(m_textDrawer != nullptr);
    d2tm_assert(m_settings != nullptr);
    d2tm_assert(m_scenarioFlow != nullptr);
    d2tm_assert(m_stateTransitions != nullptr);
    d2tm_assert(m_cIni != nullptr);
    constructWindow();
}

void cNewMapEditorState::constructWindow()
{
    int margin = m_settings->getScreenH() * 0.3;
    int mainMenuFrameX = margin;
    int mainMenuFrameY = margin;
    int mainMenuWidth = m_settings->getScreenW() - (margin * 2);
    int mainMenuHeight = m_settings->getScreenH() - (margin * 2);

    margin = 4;
    int buttonHeight = (m_textDrawer->getFontHeight() + margin);
    int buttonWidth = mainMenuWidth - 8;

    const cRectangle &window = cRectangle(mainMenuFrameX, mainMenuFrameY, mainMenuWidth, mainMenuHeight);
    m_guiWindow = std::make_unique<GuiWindow>(m_sdlDrawer, window, m_textDrawer);
    m_guiWindow->setTheme(cGuiThemeBuilder().light().build());

    // Title
    m_guiWindow->setTitle("Map editor configuration");

    int labelX = 55;
    int labelY = 45;
    int betweenY =30;

    // name
    const cRectangle &nameRect = m_guiWindow->getRelativeRect(labelX, labelY, 75, buttonHeight);
    auto gui_NameLabel = GuiLabelBuilder()
        .withLabel("Name your map:")
        .withTextDrawer(m_textDrawer)
        .withRenderer(m_sdlDrawer)
        .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
        .withTextAlign(GuiTextAlignHorizontal::LEFT)
        .withRect(nameRect)
        .build();
    m_guiWindow->addGuiObject(std::move(gui_NameLabel));

    auto inputName = std::make_unique<GuiTextInput>( m_sdlDrawer,
        m_guiWindow->getRelativeRect(labelX+200, labelY-2, 200, m_textDrawer->getFontHeight() + 4),
        m_textDrawer);
    inputName->setTheme(cGuiThemeBuilder().light().build()); 
    m_inputName = inputName.get();
    m_guiWindow->addGuiObject(std::move(inputName));

    // author
    const cRectangle &authorRect = m_guiWindow->getRelativeRect(labelX, labelY+betweenY, 75, buttonHeight);
    auto gui_authorLabel = GuiLabelBuilder()
        .withLabel("Name of author:")
        .withTextDrawer(m_textDrawer)
        .withRenderer(m_sdlDrawer)
        .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
        .withTextAlign(GuiTextAlignHorizontal::LEFT)
        .withRect(authorRect)
        .build();
    m_guiWindow->addGuiObject(std::move(gui_authorLabel));
    
    auto inputAuthor = std::make_unique<GuiTextInput>(m_sdlDrawer, 
        m_guiWindow->getRelativeRect(labelX+200, labelY+betweenY-2, 200, m_textDrawer->getFontHeight() + 4),
        m_textDrawer);
    inputAuthor->setTheme(cGuiThemeBuilder().light().build());
    m_inputAuthor = inputAuthor.get();
    m_guiWindow->addGuiObject(std::move(inputAuthor));

    // Description
    const cRectangle &descriptionRect = m_guiWindow->getRelativeRect(labelX, labelY+betweenY*2, 75, buttonHeight);
    auto gui_descriptionLabel = GuiLabelBuilder()
        .withLabel("Description:")
        .withTextDrawer(m_textDrawer)
        .withRenderer(m_sdlDrawer)
        .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
        .withTextAlign(GuiTextAlignHorizontal::LEFT)
        .withRect(descriptionRect)
        .build();
    m_guiWindow->addGuiObject(std::move(gui_descriptionLabel));

    auto inputDescription = std::make_unique<GuiTextInput>( m_sdlDrawer,
        m_guiWindow->getRelativeRect(labelX+200, labelY+betweenY*2-2, 200, m_textDrawer->getFontHeight() + 4),
        m_textDrawer);
    inputDescription->setTheme(cGuiThemeBuilder().light().build());
    m_inputDescription = inputDescription.get();
    m_guiWindow->addGuiObject(std::move(inputDescription));

    // width
    const cRectangle &widthRect = m_guiWindow->getRelativeRect(labelX, labelY+betweenY*3, 75, buttonHeight);
    auto gui_widthLabel = GuiLabelBuilder()
        .withLabel("Width size:")
        .withTextDrawer(m_textDrawer)
        .withRenderer(m_sdlDrawer)
        .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
        .withTextAlign(GuiTextAlignHorizontal::LEFT)
        .withRect(widthRect)
        .build();
    m_guiWindow->addGuiObject(std::move(gui_widthLabel));

    auto cycleWidth = GuiCycleButtonBuilder()
        .withRect( m_guiWindow->getRelativeRect(labelX+200, labelY+betweenY*3, 50, buttonHeight) )
        .withValues( m_sizesMap )
        .withTextDrawer( m_textDrawer )
        .withRenderer(m_sdlDrawer)
        .withTheme(cGuiThemeBuilder().light().build())
        .onChanged([this](int) {
            updateRandomCheckBox();
        })
        .build();
    m_cycleWidth = cycleWidth.get();
    m_guiWindow->addGuiObject(std::move(cycleWidth));

    // height
    const cRectangle &heightRect = m_guiWindow->getRelativeRect(labelX, labelY+betweenY*4, 75, buttonHeight);
    auto gui_heightLabel = GuiLabelBuilder()
        .withLabel("Height size:")
        .withTextDrawer(m_textDrawer)
        .withRenderer(m_sdlDrawer)
        .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
        .withTextAlign(GuiTextAlignHorizontal::LEFT)
        .withRect(heightRect)
        .build();
    m_guiWindow->addGuiObject(std::move(gui_heightLabel));

    auto cycleHeight = GuiCycleButtonBuilder()
        .withRect( m_guiWindow->getRelativeRect(labelX+200, labelY+betweenY*4, 50, buttonHeight) )
        .withValues( m_sizesMap )
        .withTextDrawer( m_textDrawer )
        .withRenderer(m_sdlDrawer)
        .withTheme( cGuiThemeBuilder().light().build() )
        .onChanged([this](int) {
            updateRandomCheckBox();
        })
        .build();
    m_cycleHeight = cycleHeight.get();
    m_guiWindow->addGuiObject(std::move(cycleHeight));

    // random map
    const cRectangle &randomRect = m_guiWindow->getRelativeRect(labelX, labelY+betweenY*5, 75, buttonHeight);
    auto gui_randomLabel = GuiLabelBuilder()
        .withLabel("Random:")
        .withTextDrawer(m_textDrawer)
        .withRenderer(m_sdlDrawer)
        .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
        .withTextAlign(GuiTextAlignHorizontal::LEFT)
        .withRect(randomRect)
        .build();
    m_guiWindow->addGuiObject(std::move(gui_randomLabel));

    auto checkRandom = GuiCheckBoxBuilder()
        .withRect(m_guiWindow->getRelativeRect(labelX+200, labelY+betweenY*5, buttonHeight, buttonHeight))
        .withRenderer(m_sdlDrawer)
        .withTheme(cGuiThemeBuilder().light().build())
        .onCheck([this]() {
            m_randomChecked = true;
        })
        .onUnCheck([this]() {
            m_randomChecked = false;
        })
        .build();
    checkRandom->setChecked(m_randomChecked); // GuiCheckBox starts checked
    m_checkRandom = checkRandom.get();
    m_guiWindow->addGuiObject(std::move(checkRandom));
    updateRandomCheckBox();

    // Dune 2 seed
    const cRectangle &seedRect = m_guiWindow->getRelativeRect(labelX, labelY+betweenY*6, 75, buttonHeight);
    auto gui_seedLabel = GuiLabelBuilder()
        .withLabel("Dune 2 seed: ")
        .withTextDrawer(m_textDrawer)
        .withRenderer(m_sdlDrawer)
        .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
        .withTextAlign(GuiTextAlignHorizontal::LEFT)
        .withRect(seedRect)
        .build();
    m_guiWindow->addGuiObject(std::move(gui_seedLabel));

    auto inputSeed = std::make_unique<GuiTextInput>(m_sdlDrawer,
        m_guiWindow->getRelativeRect(labelX+200, labelY+betweenY*6-2, 200, m_textDrawer->getFontHeight() + 4),
        m_textDrawer);
    inputSeed->setTheme(cGuiThemeBuilder().light().build());
    m_inputSeed = inputSeed.get();
    m_guiWindow->addGuiObject(std::move(inputSeed));

    // QUIT game
    int quit = mainMenuHeight - (buttonHeight + margin);// 464
    int width = (buttonWidth / 2);
    const cRectangle &quitRect = m_guiWindow->getRelativeRect(margin, quit, width, buttonHeight);
    auto gui_btn_Quit = GuiButtonBuilder()
        .withRect(quitRect)        
        .withLabel("Return to menu")
        .withTextDrawer(m_textDrawer)
        .withRenderer(m_sdlDrawer)
        .withTextAlign(GuiTextAlignHorizontal::CENTER)
        .withTheme(cGuiThemeBuilder().light().build())
        .onClick([this]() {
            m_stateTransitions->setNextStateWithFade(GAME_MENU);
        })
        .build();
    m_guiWindow->addGuiObject(std::move(gui_btn_Quit));

    // BACK to where we came from
    int back = mainMenuHeight - (buttonHeight + margin);// 444
    const cRectangle &backRect = m_guiWindow->getRelativeRect(margin + width + margin, back, (width - margin), buttonHeight);
    
    auto gui_btn_Back = GuiButtonBuilder()
        .withRect(backRect)        
        .withLabel("Create map")
        .withTextDrawer(m_textDrawer)
        .withRenderer(m_sdlDrawer)
        .withTheme(cGuiThemeBuilder().light().build())
        .onClick([this](){
            this->constructEmptyMap();
            m_scenarioFlow->loadMapFromEditor(-1); // -1 means load the map from the editor, instead of loading a skirmish map
            m_stateTransitions->setNextStateWithFade(GAME_EDITOR);
        })
        .build();
    m_guiWindow->addGuiObject(std::move(gui_btn_Back));
}

cNewMapEditorState::~cNewMapEditorState()
{
    // Smart pointers handle cleanup
}

void cNewMapEditorState::thinkFast()
{

}

void cNewMapEditorState::draw() const
{
    m_guiWindow->draw();
    m_mouse->draw();
}

void cNewMapEditorState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    m_guiWindow->onNotifyMouseEvent(event);
}

eGameStateType cNewMapEditorState::getType()
{
    return GAMESTATE_NEW_MAP_EDITOR;
}

void cNewMapEditorState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    m_guiWindow->onNotifyKeyboardEvent(event);
}

bool cNewMapEditorState::isKeyboardCapturedByUi() const
{
    return m_guiWindow && m_guiWindow->hasFocusedInput();
}


// random maps need room: only allowed for sizes of minRandomMapSize or bigger
void cNewMapEditorState::updateRandomCheckBox()
{
    bool allowed = m_cycleWidth->getSelectedValue() >= minRandomMapSize &&
                   m_cycleHeight->getSelectedValue() >= minRandomMapSize;
    m_checkRandom->setEnabled(allowed);
    if (!allowed) {
        m_checkRandom->setChecked(false);
        m_randomChecked = false;
    }
}

void cNewMapEditorState::constructEmptyMap()
{
    // On récupère les valeurs localement pour être sûr
    std::string name = m_inputName->getText();
    if (name.empty()) {
        auto now = std::chrono::system_clock::now();
        name = std::format("New unnamed map {:%Y-%m-%d %H:%M:%S}", now);
    }
    std::string author = m_inputAuthor->getText();
    if (author.empty()) {
        author = "Unknown user";
    }
    std::string desc = m_inputDescription->getText();
    if (desc.empty()) {
        desc = "Made with D2TM: Internal Editor";
    }

    // a seed overrides the width and height settings
    const std::string &seedText = m_inputSeed->getText();
    if (!seedText.empty()) {
        int seed = 0;
        auto [ptr, ec] = std::from_chars(seedText.data(), seedText.data() + seedText.size(), seed);
        if (ec == std::errc() && ptr == seedText.data() + seedText.size()) {
            s_PreviewMap seedMap = SeedMapLoader::toPreviewMap(seed);
            seedMap.name = name;
            seedMap.author = author;
            seedMap.description = desc;
            m_previewMap->setEditorMap(std::move(seedMap));
            return;
        }
        Logger::warn(COMP_MAP, "cNewMapEditorState", "Seed [{}] is not a number, using width and height instead", seedText);
    }

    int width = m_cycleWidth->getSelectedValue()+borderSize;
    int height = m_cycleHeight->getSelectedValue()+borderSize;

    // a valid seed wins over Random, because it is the more specific request
    if (m_randomChecked) {
        s_PreviewMap randomMap;
        cRandomMapGenerator().generateRandomMap(width, height, 4, randomMap, m_objects);
        randomMap.validMap = true;
        randomMap.name = name;
        randomMap.author = author;
        randomMap.description = desc;
        m_previewMap->setEditorMap(std::move(randomMap));
        return;
    }
    m_previewMap->createEmptyMap(name, author, desc, width, height);
}