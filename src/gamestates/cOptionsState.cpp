#include "cOptionsState.h"
#include "config.h"

#include "gui/GuiButton.h"
#include "gui/GuiWindow.h"
#include "gui/GuiLabel.hpp"
#include "gui/GuiCheckBox.hpp"
#include "gui/GuiSlider.hpp"
#include "game/cTimeManager.h"
#include "context/GameContext.hpp"
#include "utils/cSoundPlayer.h"
#include "drawers/cTextDrawer.h"
#include "game/cGameInterface.h"
#include "game/cGameSettings.h"

#include <cassert>

cOptionsState::cOptionsState(sGameServices* services, int prevState)
    : cGameState(services),
    m_textDrawer(m_ctx->getTextContext()->getBeneTextDrawer()),
    m_settings(services->settings),
    m_interface(m_ctx->getGameInterface()),
    m_prevState(prevState),
    m_guiWindow(nullptr)
{
    assert(m_textDrawer != nullptr);
    assert(m_settings != nullptr);
    assert(m_interface != nullptr);

    refresh();
    m_backgroundTexture = m_interface->getScreenTexture();
}

void cOptionsState::constructWindow(int prevState)
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
    m_guiWindow = std::make_unique<GuiWindow>(m_renderDrawer, window, m_textDrawer);
    m_guiWindow->setTheme(cGuiThemeBuilder().light().build());
    cSoundPlayer* soundPlayer = m_ctx->getSoundPlayer();

    // Title
    m_guiWindow->setTitle("Dune II - The Maker - version " + D2TM_VERSION);

    // EXIT
    int rows = 2;
    int toMainMenu = mainMenuHeight - ((buttonHeight*rows)+(margin*rows));// 424
    const cRectangle &toMainMenuRect = m_guiWindow->getRelativeRect(margin, toMainMenu, buttonWidth, buttonHeight);
    auto gui_btn_toMenu = GuiButtonBuilder()
            .withRect(toMainMenuRect)        
            .withLabel("Back to main menu")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withTheme(cGuiThemeBuilder().light().build())
            .onClick([this]() {
                m_interface->setTransitionToWithFadingOut(GAME_MENU);
            })
            .build();
    m_guiWindow->addGuiObject(std::move(gui_btn_toMenu));

    if (m_settings->isCheatMode()) {
        const cRectangle &cheatRect = m_guiWindow->getRelativeRect(margin, toMainMenu - 2*(buttonHeight + margin), buttonWidth, buttonHeight);
        auto gui_cheatLabel = GuiLabelBuilder()
                .withLabel("Cheat mode enabled")
                .withTextDrawer(m_textDrawer)
                .withRenderer(m_renderDrawer)
                .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
                .withTheme(cGuiThemeBuilder().light().withTextColor(Color::Yellow).build())
                .withRect(cheatRect)
                .build();
        m_guiWindow->addGuiObject(std::move(gui_cheatLabel));
    }

    // QUIT game
    int quit = mainMenuHeight - (buttonHeight + margin);// 464
    int width = (buttonWidth / 2);
    const cRectangle &quitRect = m_guiWindow->getRelativeRect(margin, quit, width, buttonHeight);
    auto gui_btn_Quit = GuiButtonBuilder()
            .withRect(quitRect)        
            .withLabel("Quit game")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withTheme(cGuiThemeBuilder().light().build())
            .onClick([this]() {
                m_settings->setPlaying(false);
                m_interface->initiateFadingOut();
            })
            .build();
    m_guiWindow->addGuiObject(std::move(gui_btn_Quit));

    // BACK to where we came from
    int back = mainMenuHeight - (buttonHeight + margin);// 444
    const cRectangle &backRect = m_guiWindow->getRelativeRect(margin + width + margin, back, (width - margin), buttonHeight);
    
    auto gui_btn_Back = GuiButtonBuilder()
            .withRect(backRect)        
            .withLabel("Back")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withTheme(cGuiThemeBuilder().light().build())
            .onClick([this,prevState](){
                m_interface->setNextStateToTransitionTo(prevState);
            })
                .build();
    m_guiWindow->addGuiObject(std::move(gui_btn_Back));

    // Mission select from options menu, only when playing the game
    if (m_prevState == GAME_PLAYING) {
        rows = 3;
        int toMissionSelect = mainMenuHeight - ((buttonHeight * rows) + (margin * rows));// 424
        const cRectangle &toMissionSelectRect = m_guiWindow->getRelativeRect(margin, toMissionSelect, buttonWidth,
                                                buttonHeight);
    auto gui_btn_toMissionSelect = GuiButtonBuilder()
            .withRect(toMissionSelectRect)        
            .withLabel("Mission select")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withTheme(cGuiThemeBuilder().light().build())
            .onClick([this]() {
                m_interface->setNextStateToTransitionTo(GAME_MISSIONSELECT);
            })
            .build();
        m_guiWindow->addGuiObject(std::move(gui_btn_toMissionSelect));
    }
    const cRectangle &musicRect = m_guiWindow->getRelativeRect(5, 5+buttonHeight, 50, buttonHeight);
    auto gui_MusicLabel = GuiLabelBuilder()
            .withLabel("Music")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .withRect(musicRect)
            .build();
    m_guiWindow->addGuiObject(std::move(gui_MusicLabel));

    const cRectangle &musicCheckRect = m_guiWindow->getRelativeRect(5+75, 5+buttonHeight, buttonHeight, buttonHeight);
    auto gui_MusicCheckLabel = GuiCheckBoxBuilder()
            .withRect(musicCheckRect)
            .withRenderer(m_renderDrawer)
            .onCheck([soundPlayer]() {
                soundPlayer->setMusicEnabled(true);
            })
            .onUnCheck([soundPlayer]() {
                soundPlayer->setMusicEnabled(false);
            })
            .build();
    gui_MusicCheckLabel->setChecked(soundPlayer->getMusicEnabled());
    m_guiWindow->addGuiObject(std::move(gui_MusicCheckLabel));

    const cRectangle &musicVolumeRect = m_guiWindow->getRelativeRect(5, 5+buttonHeight, 50+buttonWidth/2, buttonHeight);
    auto gui_MusicVolumeLabel = GuiLabelBuilder()
            .withLabel("Volume")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .withRect(musicVolumeRect)
            .build();
    m_guiWindow->addGuiObject(std::move(gui_MusicVolumeLabel));

    const cRectangle &btn_musicVolumeRect = m_guiWindow->getRelativeRect(5+buttonWidth/4 + 50*2, 5+buttonHeight, buttonHeight*5, buttonHeight);
    auto gui_sld_musicVolumeRect = GuiSliderBuilder()
            .withRect(btn_musicVolumeRect)
            .withRenderer(m_renderDrawer)
            .withMinValue(0)
            .withMaxValue(10)
            .withInitialValue(soundPlayer->getMusicVolume())
            .onValueChanged([soundPlayer](int newValue) {
                soundPlayer->setMusicVolume(newValue);
            })
            .build();
    m_guiWindow->addGuiObject(std::move(gui_sld_musicVolumeRect));

    const cRectangle &soundRect = m_guiWindow->getRelativeRect(5, (5+buttonHeight)*2, 50, buttonHeight);
    auto gui_SoundLabel = GuiLabelBuilder()
            .withLabel("Sound")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .withRect(soundRect)
            .build();
    m_guiWindow->addGuiObject(std::move(gui_SoundLabel));

    const cRectangle &soundCheckRect = m_guiWindow->getRelativeRect(5+75, (5+buttonHeight)*2, buttonHeight, buttonHeight);
    auto gui_SoundCheckLabel = GuiCheckBoxBuilder()
            .withRect(soundCheckRect)
            .withRenderer(m_renderDrawer)
            .onCheck([soundPlayer]() {
                soundPlayer->setSoundEnabled(true);
            })
            .onUnCheck([soundPlayer]() {
                soundPlayer->setSoundEnabled(false);
            })
            .build();
    gui_SoundCheckLabel->setChecked(soundPlayer->getSoundEnabled());
    m_guiWindow->addGuiObject(std::move(gui_SoundCheckLabel));

    const cRectangle &soundVolumeRect = m_guiWindow->getRelativeRect(5, (5+buttonHeight)*2, 50+buttonWidth/2, buttonHeight);
    auto gui_SoundVolumeLabel = GuiLabelBuilder()
            .withLabel("Volume")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .withRect(soundVolumeRect)
            .build();
    m_guiWindow->addGuiObject(std::move(gui_SoundVolumeLabel));

    const cRectangle &btn_soundVolumeRect = m_guiWindow->getRelativeRect(5+buttonWidth/4 + 50*2, (5+buttonHeight)*2, buttonHeight*5, buttonHeight);
    auto gui_sld_soundVolumeRect = GuiSliderBuilder()
            .withRect(btn_soundVolumeRect)
            .withRenderer(m_renderDrawer)
            .withMinValue(0)
            .withMaxValue(10)
            .withInitialValue(soundPlayer->getSoundVolume())
            .onValueChanged([soundPlayer](int newValue) {
                soundPlayer->setSoundVolume(newValue);
            })
            .build();
    m_guiWindow->addGuiObject(std::move(gui_sld_soundVolumeRect));

    const cRectangle &speedRect = m_guiWindow->getRelativeRect(5, (5+buttonHeight)*3, 50, buttonHeight);
    auto gui_DifficultyLabel = GuiLabelBuilder()
            .withLabel("Speed")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .withRect(speedRect)
            .build();
    m_guiWindow->addGuiObject(std::move(gui_DifficultyLabel));

    cTimeManager* timeManager = m_ctx->getTimeManager();
    const cRectangle &sld_speedRect = m_guiWindow->getRelativeRect(5 + 75, (5+buttonHeight)*3, 100, buttonHeight);
    int convertedForSlider = 2 - timeManager->getGlobalSpeed() + 10;
    auto gui_sld_speedRect = GuiSliderBuilder()
            .withRect(sld_speedRect)
            .withRenderer(m_renderDrawer)
            .withMinValue(2)
            .withMaxValue(10)
            .withInitialValue(convertedForSlider)
            .onValueChanged([timeManager](int newValue) {
                int globalSpeed = 10 - newValue + 2;
                // higher means slower, so convert into opposite
                timeManager->setGlobalSpeed(globalSpeed);
            })
            .build();
    m_guiWindow->addGuiObject(std::move(gui_sld_speedRect));
}

cOptionsState::~cOptionsState()
{
    // Smart pointers handle cleanup
}

void cOptionsState::thinkFast()
{
}

void cOptionsState::draw() const
{
    if (m_backgroundTexture) {
        m_renderDrawer->renderSprite(m_backgroundTexture,0,0);
    }
    m_guiWindow->draw();

    // MOUSE
    m_interface->drawCursor();
}

void cOptionsState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    m_guiWindow->onNotifyMouseEvent(event);
}

eGameStateType cOptionsState::getType()
{
    return GAMESTATE_OPTIONS;
}

void cOptionsState::onNotifyKeyboardEvent(const cKeyboardEvent &)
{
}

/**
 * Reconstructs window so that it knows to which previous state it should go 'back to'
 * @param prevState
 */
void cOptionsState::setPrevState(int prevState)
{
    m_prevState = prevState;
    refresh();
}

void cOptionsState::refresh()
{
    m_guiWindow.reset();
    constructWindow(m_prevState);
    m_backgroundTexture = nullptr;
}