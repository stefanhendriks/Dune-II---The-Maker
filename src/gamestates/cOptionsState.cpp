#include "cOptionsState.h"

#include "d2tmc.h"
#include "config.h"

#include "gui/GuiButton.h"
#include "gui/GuiWindow.h"
#include "gui/GuiLabel.hpp"
#include "gui/GuiCheckBox.hpp"
#include "gui/GuiSlider.hpp"
#include "context/GameContext.hpp"
#include "utils/cSoundPlayer.h"



cOptionsState::cOptionsState(cGame &theGame, GameContext *ctx, int prevState)
    : cGameState(theGame, ctx),
    m_textDrawer(ctx->getTextContext()->getBeneTextDrawer()),
    m_prevState(prevState),
    m_guiWindow(nullptr)
{
    refresh();
    m_backgroundTexture = m_game.getScreenTexture();
}

void cOptionsState::constructWindow(int prevState)
{
    int margin = m_game.m_screenH * 0.3;
    int mainMenuFrameX = margin;
    int mainMenuFrameY = margin;
    int mainMenuWidth = m_game.m_screenW - (margin * 2);
    int mainMenuHeight = m_game.m_screenH - (margin * 2);

    margin = 4;
    int buttonHeight = (m_textDrawer->getFontHeight() + margin);
    int buttonWidth = mainMenuWidth - 8;

    const cRectangle &window = cRectangle(mainMenuFrameX, mainMenuFrameY, mainMenuWidth, mainMenuHeight);
    m_guiWindow = new GuiWindow(window, m_textDrawer);
    m_guiWindow->setTheme(GuiTheme::Light());
    cSoundPlayer* soundPlayer = m_ctx->getSoundPlayer();

    // Title
    m_guiWindow->setTitle("Dune II - The Maker - version " + D2TM_VERSION);

    // EXIT
    int rows = 2;
    int toMainMenu = mainMenuHeight - ((buttonHeight*rows)+(margin*rows));// 424
    const cRectangle &toMainMenuRect = m_guiWindow->getRelativeRect(margin, toMainMenu, buttonWidth, buttonHeight);
    GuiButton *gui_btn_toMenu = GuiButtonBuilder()
            .withRect(toMainMenuRect)        
            .withLabel("Back to main menu")
            .withTextDrawer(m_textDrawer)
            .withTheme(GuiTheme::Light())
            .onClick([this]() {
                m_game.setNextStateToTransitionTo(GAME_MENU);
                m_game.initiateFadingOut();})
            .build();
    m_guiWindow->addGuiObject(gui_btn_toMenu);

    // QUIT game
    int quit = mainMenuHeight - (buttonHeight + margin);// 464
    int width = (buttonWidth / 2);
    const cRectangle &quitRect = m_guiWindow->getRelativeRect(margin, quit, width, buttonHeight);
    GuiButton *gui_btn_Quit = GuiButtonBuilder()
            .withRect(quitRect)        
            .withLabel("Quit game")
            .withTextDrawer(m_textDrawer)
            .withTheme(GuiTheme::Light())
            .onClick([this]() {
                m_game.m_playing = false;
                m_game.initiateFadingOut();})
            .build();
    m_guiWindow->addGuiObject(gui_btn_Quit);

    // BACK to where we came from
    int back = mainMenuHeight - (buttonHeight + margin);// 444
    const cRectangle &backRect = m_guiWindow->getRelativeRect(margin + width + margin, back, (width - margin), buttonHeight);
    
    GuiButton *gui_btn_Back = GuiButtonBuilder()
            .withRect(backRect)        
            .withLabel("Back")
            .withTextDrawer(m_textDrawer)
            .withTheme(GuiTheme::Light())
            .onClick([this,prevState](){
                m_game.setNextStateToTransitionTo(prevState);})
            .build();
    m_guiWindow->addGuiObject(gui_btn_Back);

    // Mission select from options menu, only when playing the game
    if (m_prevState == GAME_PLAYING) {
        rows = 3;
        int toMissionSelect = mainMenuHeight - ((buttonHeight * rows) + (margin * rows));// 424
        const cRectangle &toMissionSelectRect = m_guiWindow->getRelativeRect(margin, toMissionSelect, buttonWidth,
                                                buttonHeight);
    GuiButton *gui_btn_toMissionSelect = GuiButtonBuilder()
            .withRect(toMissionSelectRect)        
            .withLabel("Mission select")
            .withTextDrawer(m_textDrawer)
            .withTheme(GuiTheme::Light())
            .onClick([this]() {
                m_game.setNextStateToTransitionTo(GAME_MISSIONSELECT);})
            .build();
        m_guiWindow->addGuiObject(gui_btn_toMissionSelect);
    }
    const cRectangle &musicRect = m_guiWindow->getRelativeRect(5, 5+buttonHeight, 50, buttonHeight);
    GuiLabel *gui_MusicLabel = GuiLabelBuilder()
            .withLabel("Music")
            .withTextDrawer(m_textDrawer)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .withRect(musicRect)
            .build();
    m_guiWindow->addGuiObject(gui_MusicLabel);

    const cRectangle &musicCheckRect = m_guiWindow->getRelativeRect(5+75, 5+buttonHeight, buttonHeight, buttonHeight);
    GuiCheckBox *gui_MusicCheckLabel = GuiCheckBoxBuilder()
            .withRect(musicCheckRect)
            .onCheck([soundPlayer]() {
                soundPlayer->setMusicEnabled(true);
            })
            .onUnCheck([soundPlayer]() {
                soundPlayer->setMusicEnabled(false);
            })
            .build();
    gui_MusicCheckLabel->setChecked(soundPlayer->getMusicEnabled());
    m_guiWindow->addGuiObject(gui_MusicCheckLabel);

    const cRectangle &musicVolumeRect = m_guiWindow->getRelativeRect(5, 5+buttonHeight, 50+buttonWidth/2, buttonHeight);
    GuiLabel *gui_MusicVolumeLabel = GuiLabelBuilder()
            .withLabel("Volume")
            .withTextDrawer(m_textDrawer)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .withRect(musicVolumeRect)
            .build();
    m_guiWindow->addGuiObject(gui_MusicVolumeLabel);

    const cRectangle &btn_musicVolumeRect = m_guiWindow->getRelativeRect(5+buttonWidth/4 + 50*2, 5+buttonHeight, buttonHeight*5, buttonHeight);
    GuiSlider *gui_sld_musicVolumeRect = GuiSliderBuilder()
            .withRect(btn_musicVolumeRect)
            .withMinValue(0)
            .withMaxValue(10)
            .withInitialValue(soundPlayer->getMusicVolume())
            .onValueChanged([soundPlayer](int newValue) {
                soundPlayer->setMusicVolume(newValue);
            })
            .build();
    m_guiWindow->addGuiObject(gui_sld_musicVolumeRect);

    const cRectangle &soundRect = m_guiWindow->getRelativeRect(5, (5+buttonHeight)*2, 50, buttonHeight);
    GuiLabel *gui_SoundLabel = GuiLabelBuilder()
            .withLabel("Sound")
            .withTextDrawer(m_textDrawer)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .withRect(soundRect)
            .build();
    m_guiWindow->addGuiObject(gui_SoundLabel);

    const cRectangle &soundCheckRect = m_guiWindow->getRelativeRect(5+75, (5+buttonHeight)*2, buttonHeight, buttonHeight);
    GuiCheckBox *gui_SoundCheckLabel = GuiCheckBoxBuilder()
            .withRect(soundCheckRect)
            .onCheck([soundPlayer]() {
                soundPlayer->setSoundEnabled(true);
            })
            .onUnCheck([soundPlayer]() {
                soundPlayer->setSoundEnabled(false);
            })
            .build();
    gui_SoundCheckLabel->setChecked(soundPlayer->getSoundEnabled());
    m_guiWindow->addGuiObject(gui_SoundCheckLabel);

    const cRectangle &soundVolumeRect = m_guiWindow->getRelativeRect(5, (5+buttonHeight)*2, 50+buttonWidth/2, buttonHeight);
    GuiLabel *gui_SoundVolumeLabel = GuiLabelBuilder()
            .withLabel("Volume")
            .withTextDrawer(m_textDrawer)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .withRect(soundVolumeRect)
            .build();
    m_guiWindow->addGuiObject(gui_SoundVolumeLabel);

    const cRectangle &btn_soundVolumeRect = m_guiWindow->getRelativeRect(5+buttonWidth/4 + 50*2, (5+buttonHeight)*2, buttonHeight*5, buttonHeight);
    GuiSlider *gui_sld_soundVolumeRect = GuiSliderBuilder()
            .withRect(btn_soundVolumeRect)
            .withMinValue(0)
            .withMaxValue(10)
            .withInitialValue(soundPlayer->getSoundVolume())
            .onValueChanged([soundPlayer](int newValue) {
                soundPlayer->setSoundVolume(newValue);
            })
            .build();
    m_guiWindow->addGuiObject(gui_sld_soundVolumeRect);

    const cRectangle &speedRect = m_guiWindow->getRelativeRect(5, (5+buttonHeight)*3, 50, buttonHeight);
    GuiLabel *gui_DifficultyLabel = GuiLabelBuilder()
            .withLabel("Speed")
            .withTextDrawer(m_textDrawer)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .withRect(speedRect)
            .build();
    m_guiWindow->addGuiObject(gui_DifficultyLabel);

    cTimeManager* timeManager = m_ctx->getTimeManager();
    const cRectangle &sld_speedRect = m_guiWindow->getRelativeRect(5 + 75, (5+buttonHeight)*3, 100, buttonHeight);
    int convertedForSlider = 2 - timeManager->getGlobalSpeed() + 10;
    GuiSlider *gui_sld_speedRect = GuiSliderBuilder()
            .withRect(sld_speedRect)
            .withMinValue(2)
            .withMaxValue(10)
            .withInitialValue(convertedForSlider)
            .onValueChanged([timeManager](int newValue) {
                int globalSpeed = 10 - newValue + 2;
                // higher means slower, so convert into opposite
                timeManager->setGlobalSpeed(globalSpeed);
            })
            .build();
    m_guiWindow->addGuiObject(gui_sld_speedRect);
}

cOptionsState::~cOptionsState()
{
    delete m_guiWindow;
}

void cOptionsState::thinkFast()
{

}

void cOptionsState::draw() const
{
    if (m_backgroundTexture) {
        renderDrawer->renderSprite(m_backgroundTexture,0,0);
    }
    m_guiWindow->draw();

    // MOUSE
    m_game.getMouse()->draw();
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
    delete m_guiWindow;
    m_guiWindow = nullptr;
    constructWindow(m_prevState);
    m_backgroundTexture = nullptr;
}