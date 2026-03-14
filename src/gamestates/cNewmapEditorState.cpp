#include "gamestates/cNewMapEditorState.h"

#include "d2tmc.h"
#include "config.h"

#include "gui/GuiButton.h"
#include "gui/GuiWindow.h"
#include "gui/GuiLabel.hpp"
#include "gui/GuiCheckBox.hpp"
#include "gui/GuiSlider.hpp"
#include "gui/GuiTextInput.h"
#include "context/GameContext.hpp"

#include <iostream>
#include <memory>

cNewMapEditorState::cNewMapEditorState(cGame &theGame, GameContext *ctx)
    : cGameState(theGame, ctx),
    m_textDrawer(ctx->getTextContext()->getBeneTextDrawer()),
    m_guiWindow(nullptr)
{
    constructWindow();
}

void cNewMapEditorState::constructWindow()
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

    // Title
    m_guiWindow->setTitle("New map editor configuration");

    int labelX = 55;
    int labelY = 45;
    int betweenY =35;

    // name
    const cRectangle &nameRect = m_guiWindow->getRelativeRect(labelX, labelY, 75, buttonHeight);
    GuiLabel *gui_NameLabel = GuiLabelBuilder()
        .withLabel("Name your new map:")
        .withTextDrawer(m_textDrawer)
        .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
        .withTextAlign(GuiTextAlignHorizontal::LEFT)
        .withRect(nameRect)
        .build();
    m_guiWindow->addGuiObject(gui_NameLabel);

    m_inputName = new GuiTextInput(
        m_guiWindow->getRelativeRect(labelX+200, labelY-2, 200, m_textDrawer->getFontHeight() + 4),
        m_textDrawer);
    m_guiWindow->addGuiObject(m_inputName);

    // author
    const cRectangle &authorRect = m_guiWindow->getRelativeRect(labelX, labelY+betweenY, 75, buttonHeight);
    GuiLabel *gui_authorLabel = GuiLabelBuilder()
        .withLabel("Name of author:")
        .withTextDrawer(m_textDrawer)
        .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
        .withTextAlign(GuiTextAlignHorizontal::LEFT)
        .withRect(authorRect)
        .build();
    m_guiWindow->addGuiObject(gui_authorLabel);
    
    m_inputAuthor = new GuiTextInput(
        m_guiWindow->getRelativeRect(labelX+200, labelY+betweenY-2, 200, m_textDrawer->getFontHeight() + 4),
        m_textDrawer);
    m_guiWindow->addGuiObject(m_inputAuthor);

    // Description
    const cRectangle &descriptionRect = m_guiWindow->getRelativeRect(labelX, labelY+betweenY*2, 75, buttonHeight);
    GuiLabel *gui_descriptionLabel = GuiLabelBuilder()
        .withLabel("Description:")
        .withTextDrawer(m_textDrawer)
        .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
        .withTextAlign(GuiTextAlignHorizontal::LEFT)
        .withRect(descriptionRect)
        .build();
    m_guiWindow->addGuiObject(gui_descriptionLabel);

    m_inputDescription = new GuiTextInput(
        m_guiWindow->getRelativeRect(labelX+200, labelY+betweenY*2-2, 200, m_textDrawer->getFontHeight() + 4),
        m_textDrawer);
    m_guiWindow->addGuiObject(m_inputDescription);

    // width
    const cRectangle &widthRect = m_guiWindow->getRelativeRect(labelX, labelY+betweenY*3, 75, buttonHeight);
    GuiLabel *gui_widthLabel = GuiLabelBuilder()
        .withLabel("Width size:")
        .withTextDrawer(m_textDrawer)
        .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
        .withTextAlign(GuiTextAlignHorizontal::LEFT)
        .withRect(widthRect)
        .build();
    m_guiWindow->addGuiObject(gui_widthLabel);

    m_cycleWidth = GuiCycleButtonBuilder()
        .withRect( m_guiWindow->getRelativeRect(labelX+200, labelY+betweenY*3, 50, buttonHeight) )
        .withValues( m_sizesMap )
        .withTextDrawer( m_textDrawer )
        .withTheme( GuiTheme::Light() )
        .build();
    m_guiWindow->addGuiObject(m_cycleWidth);

    // height
    const cRectangle &heightRect = m_guiWindow->getRelativeRect(labelX, labelY+betweenY*4, 75, buttonHeight);
    GuiLabel *gui_heightLabel = GuiLabelBuilder()
        .withLabel("Height size:")
        .withTextDrawer(m_textDrawer)
        .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
        .withTextAlign(GuiTextAlignHorizontal::LEFT)
        .withRect(heightRect)
        .build();
    m_guiWindow->addGuiObject(gui_heightLabel);

    m_cycleHeight = GuiCycleButtonBuilder()
        .withRect( m_guiWindow->getRelativeRect(labelX+200, labelY+betweenY*4, 50, buttonHeight) )
        .withValues( m_sizesMap )
        .withTextDrawer( m_textDrawer )
        .withTheme( GuiTheme::Light() )
        .build();
    m_guiWindow->addGuiObject(m_cycleHeight);

    // QUIT game
    int quit = mainMenuHeight - (buttonHeight + margin);// 464
    int width = (buttonWidth / 2);
    const cRectangle &quitRect = m_guiWindow->getRelativeRect(margin, quit, width, buttonHeight);
    GuiButton *gui_btn_Quit = GuiButtonBuilder()
        .withRect(quitRect)        
        .withLabel("Return to menu")
        .withTextDrawer(m_textDrawer)
        .withTextAlign(GuiTextAlignHorizontal::CENTER)
        .withTheme(GuiTheme::Light())
        .onClick([this]() {
            m_game.setNextStateToTransitionTo(GAME_MENU);
            m_game.initiateFadingOut();})
        .build();
    m_guiWindow->addGuiObject(gui_btn_Quit);

    // BACK to where we came from
    int back = mainMenuHeight - (buttonHeight + margin);// 444
    const cRectangle &backRect = m_guiWindow->getRelativeRect(margin + width + margin, back, (width - margin), buttonHeight);
    
    GuiButton *gui_btn_Back = GuiButtonBuilder()
        .withRect(backRect)        
        .withLabel("Create map !")
        .withTextDrawer(m_textDrawer)
        .withTheme(GuiTheme::Light())
        .onClick([this](){
            s_PreviewMap newMap = cPreviewMaps::createEmptyMap(
                m_inputName->getText(), m_inputAuthor->getText(), m_inputDescription->getText(),
                m_cycleWidth->getSelectedValue(), m_cycleHeight->getSelectedValue());
            m_game.loadMapFromEditor(&newMap);
            m_game.initiateFadingOut();})
        .build();
    m_guiWindow->addGuiObject(gui_btn_Back);
}

cNewMapEditorState::~cNewMapEditorState()
{
    delete m_guiWindow;
}

void cNewMapEditorState::thinkFast()
{

}

void cNewMapEditorState::draw() const
{
    m_guiWindow->draw();

    // MOUSE
    m_game.getMouse()->draw();
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
