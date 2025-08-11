#include "cOptionsState.h"

#include "d2tmc.h"
#include "config.h"
#include "drawers/SDLDrawer.hpp"
// #include "gui/actions/cGuiActionExitGame.h"
// #include "gui/actions/cGuiActionToGameState.h"
#include "gui/GuiButton.h"
#include "gui/GuiWindow.h"
#include "gui/GuiLabel.hpp"
#include "gui/GuiCheckBox.hpp"
//#include "gui/GuiSlider.hpp"
#include "context/GameContext.hpp"



cOptionsState::cOptionsState(cGame &theGame, GameContext *context, int prevState)
    : cGameState(theGame)
    , m_textDrawer(cTextDrawer(bene_font))
    , m_prevState(prevState)
    , m_guiWindow(nullptr)
    , m_context(context)
{
    refresh();
}

void cOptionsState::constructWindow(int prevState)
{
    int margin = game.m_screenH * 0.3;
    int mainMenuFrameX = margin;
    int mainMenuFrameY = margin;
    int mainMenuWidth = game.m_screenW - (margin * 2);
    int mainMenuHeight = game.m_screenH - (margin * 2);

    margin = 4;
    int buttonHeight = (m_textDrawer.getFontHeight() + margin);
    int buttonWidth = mainMenuWidth - 8;

    const cRectangle &window = cRectangle(mainMenuFrameX, mainMenuFrameY, mainMenuWidth, mainMenuHeight);
    m_guiWindow = new GuiWindow(window);
    m_guiWindow->setTheme(GuiTheme::Light());

    //const GuiRenderKind buttonKinds = OPAQUE_WITH_BORDER;
    //const GuiTextAlignHorizontal buttonTextAlignment = CENTER;

    // Title
    m_guiWindow->setTitle("Dune II - The Maker - version " + D2TM_VERSION);

    // EXIT
    int rows = 2;
    int toMainMenu = mainMenuHeight - ((buttonHeight*rows)+(margin*rows));// 424
    const cRectangle &toMainMenuRect = m_guiWindow->getRelativeRect(margin, toMainMenu, buttonWidth, buttonHeight);
    GuiButton *gui_btn_toMenu = GuiButtonBuilder()
            .withRect(toMainMenuRect)        
            .withLabel("Back to main menu")
            .withTextDrawer(&m_textDrawer)    
            .withTheme(GuiTheme::Light())
            .onClick([this]() {
                game.setNextStateToTransitionTo(GAME_MENU);
                game.initiateFadingOut();})
            .build();   
    // GuiButton *gui_btn_toMenu = new GuiButton(m_textDrawer, toMainMenuRect, "Back to main menu", buttonKinds);
    // gui_btn_toMenu->setTheme(GuiTheme::Light());
    // gui_btn_toMenu->setTextAlignHorizontal(buttonTextAlignment);
    // // cGuiActionToGameState *action2 = new cGuiActionToGameState(GAME_MENU, true);
    // gui_btn_toMenu->setOnLeftMouseButtonClickedAction([this]() 
    //     {game.setNextStateToTransitionTo(GAME_MENU);
    //     game.initiateFadingOut();});
    m_guiWindow->addGuiObject(gui_btn_toMenu);

    // QUIT game
    int quit = mainMenuHeight - (buttonHeight + margin);// 464
    int width = (buttonWidth / 2);
    const cRectangle &quitRect = m_guiWindow->getRelativeRect(margin, quit, width, buttonHeight);
    GuiButton *gui_btn_Quit = GuiButtonBuilder()
            .withRect(quitRect)        
            .withLabel("Quit game")
            .withTextDrawer(&m_textDrawer)    
            .withTheme(GuiTheme::Light())
            .onClick([this]() {
                game.m_playing = false;
                game.initiateFadingOut();})
            .build();   
    // GuiButton *gui_btn_Quit = new GuiButton(m_textDrawer, quitRect, "Quit game", buttonKinds);
    // gui_btn_Quit->setTheme(GuiTheme::Light());
    // gui_btn_Quit->setTextAlignHorizontal(buttonTextAlignment);
    // gui_btn_Quit->setOnLeftMouseButtonClickedAction([this]() {
        // game.m_playing = false;
        // game.initiateFadingOut();});
    m_guiWindow->addGuiObject(gui_btn_Quit);

    // BACK to where we came from
    int back = mainMenuHeight - (buttonHeight + margin);// 444
    const cRectangle &backRect = m_guiWindow->getRelativeRect(margin + width + margin, back, (width - margin), buttonHeight);
    
    GuiButton *gui_btn_Back = GuiButtonBuilder()
            .withRect(backRect)        
            .withLabel("Back")
            .withTextDrawer(&m_textDrawer)    
            .withTheme(GuiTheme::Light())
            .onClick([this,prevState](){
                game.setNextStateToTransitionTo(prevState);})
            .build();   
    // GuiButton *gui_btn_Back = new GuiButton(m_textDrawer, backRect, "Back", buttonKinds);
    // gui_btn_Back->setTheme(GuiTheme::Light());
    // gui_btn_Back->setTextAlignHorizontal(buttonTextAlignment);
    // // cGuiActionToGameState *action = new cGuiActionToGameState(prevState, false);
    // gui_btn_Back->setOnLeftMouseButtonClickedAction([this,prevState]() 
    //     {game.setNextStateToTransitionTo(prevState);});
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
            .withTextDrawer(&m_textDrawer)    
            .withTheme(GuiTheme::Light())
            .onClick([this]() {
                game.setNextStateToTransitionTo(GAME_MISSIONSELECT);})
            .build();   
        // GuiButton *gui_btn_toMissionSelect = new GuiButton(m_textDrawer, toMissionSelectRect, "Mission select",
        //         buttonKinds);
        // gui_btn_toMissionSelect->setTheme(GuiTheme::Light());
        // gui_btn_toMissionSelect->setTextAlignHorizontal(buttonTextAlignment);
        // // cGuiActionToGameState *action3 = new cGuiActionToGameState(GAME_MISSIONSELECT, false);
        // gui_btn_toMissionSelect->setOnLeftMouseButtonClickedAction([this]() 
        //     {game.setNextStateToTransitionTo(GAME_MISSIONSELECT);});
        m_guiWindow->addGuiObject(gui_btn_toMissionSelect);
    }
    const cRectangle &musicRect = m_guiWindow->getRelativeRect(5, 5+buttonHeight, 50, buttonHeight);
    GuiLabel *gui_MusicLabel = GuiLabelBuilder()
            .withLabel("Music")
            .withTextDrawer(&m_textDrawer)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .withRect(musicRect)
            .build();
    m_guiWindow->addGuiObject(gui_MusicLabel);

    const cRectangle &musicCheckRect = m_guiWindow->getRelativeRect(5+75, 5+buttonHeight, buttonHeight, buttonHeight);
    GuiCheckBox *gui_MusicCheckLabel = GuiCheckBoxBuilder()
            .withRect(musicCheckRect)
            .build();
    m_guiWindow->addGuiObject(gui_MusicCheckLabel);

    const cRectangle &musicVolumeRect = m_guiWindow->getRelativeRect(5, 5+buttonHeight, 50+buttonWidth/2, buttonHeight);
    GuiLabel *gui_MusicVolumeLabel = GuiLabelBuilder()
            .withLabel("Volume")
            .withTextDrawer(&m_textDrawer)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .withRect(musicVolumeRect)
            .build();
    m_guiWindow->addGuiObject(gui_MusicVolumeLabel);

    const cRectangle &soundRect = m_guiWindow->getRelativeRect(5, (5+buttonHeight)*2, 50, buttonHeight);
    GuiLabel *gui_SoundLabel = GuiLabelBuilder()
            .withLabel("Sound")
            .withTextDrawer(&m_textDrawer)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .withRect(soundRect)
            .build();
    m_guiWindow->addGuiObject(gui_SoundLabel);

    const cRectangle &soundCheckRect = m_guiWindow->getRelativeRect(5+75, (5+buttonHeight)*2, buttonHeight, buttonHeight);
    GuiCheckBox *gui_SoundCheckLabel = GuiCheckBoxBuilder()
            .withRect(soundCheckRect)
            .build();
    m_guiWindow->addGuiObject(gui_SoundCheckLabel);

    const cRectangle &soundVolumeRect = m_guiWindow->getRelativeRect(5, (5+buttonHeight)*2, 50+buttonWidth/2, buttonHeight);
    GuiLabel *gui_SoundVolumeLabel = GuiLabelBuilder()
            .withLabel("Volume")
            .withTextDrawer(&m_textDrawer)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .withRect(soundVolumeRect)
            .build();
    m_guiWindow->addGuiObject(gui_SoundVolumeLabel);

    const cRectangle &difficultyRect = m_guiWindow->getRelativeRect(5, (5+buttonHeight)*3, 50, buttonHeight);
    GuiLabel *gui_DifficultyLabel = GuiLabelBuilder()
            .withLabel("Speed")
            .withTextDrawer(&m_textDrawer)
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .withRect(difficultyRect)
            .build();
    m_guiWindow->addGuiObject(gui_DifficultyLabel);
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
    //@Mira draw here screenTexture
    m_guiWindow->draw();

    // MOUSE
    game.getMouse()->draw();
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
}