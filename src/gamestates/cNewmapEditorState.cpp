#include "gamestates/cNewMapEditorState.h"

#include "game/cGame.h"

#include "gui/GuiButton.h"
#include "gui/GuiWindow.h"
#include "gui/GuiLabel.hpp"
#include "gui/GuiTextInput.h"
#include "context/GameContext.hpp"
#include "drawers/cTextDrawer.h"
#include "map/cPreviewMaps.h"

#include <cassert>

cNewMapEditorState::cNewMapEditorState(cGame &theGame, GameContext *ctx)
    : cGameState(theGame, ctx),
    m_textDrawer(ctx->getTextContext()->getBeneTextDrawer()),
    m_guiWindow(nullptr)
{
    assert(ctx != nullptr);
    constructWindow();
}

void cNewMapEditorState::constructWindow()
{
    int margin = m_game.m_gameSettings->getScreenH() * 0.3;
    int mainMenuFrameX = margin;
    int mainMenuFrameY = margin;
    int mainMenuWidth = m_game.m_gameSettings->getScreenW() - (margin * 2);
    int mainMenuHeight = m_game.m_gameSettings->getScreenH() - (margin * 2);

    margin = 4;
    int buttonHeight = (m_textDrawer->getFontHeight() + margin);
    int buttonWidth = mainMenuWidth - 8;

    const cRectangle &window = cRectangle(mainMenuFrameX, mainMenuFrameY, mainMenuWidth, mainMenuHeight);
    m_guiWindow = std::make_unique<GuiWindow>(m_renderDrawer, window, m_textDrawer);
    m_guiWindow->setTheme(cGuiThemeBuilder().light().build());

    // Title
    m_guiWindow->setTitle("New map editor configuration");

    int labelX = 55;
    int labelY = 45;
    int betweenY =35;

    // name
    const cRectangle &nameRect = m_guiWindow->getRelativeRect(labelX, labelY, 75, buttonHeight);
    auto gui_NameLabel = GuiLabelBuilder()
        .withLabel("Name your new map:")
        .withTextDrawer(m_textDrawer)
        .withRenderer(m_renderDrawer)
        .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
        .withTextAlign(GuiTextAlignHorizontal::LEFT)
        .withRect(nameRect)
        .build();
    m_guiWindow->addGuiObject(std::move(gui_NameLabel));

    m_inputName = std::make_unique<GuiTextInput>( m_renderDrawer,
        m_guiWindow->getRelativeRect(labelX+200, labelY-2, 200, m_textDrawer->getFontHeight() + 4),
        m_textDrawer);
    m_guiWindow->addGuiObject(std::move(m_inputName));

    // author
    const cRectangle &authorRect = m_guiWindow->getRelativeRect(labelX, labelY+betweenY, 75, buttonHeight);
    auto gui_authorLabel = GuiLabelBuilder()
        .withLabel("Name of author:")
        .withTextDrawer(m_textDrawer)
        .withRenderer(m_renderDrawer)
        .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
        .withTextAlign(GuiTextAlignHorizontal::LEFT)
        .withRect(authorRect)
        .build();
    m_guiWindow->addGuiObject(std::move(gui_authorLabel));
    
    m_inputAuthor = std::make_unique<GuiTextInput>(m_renderDrawer, 
        m_guiWindow->getRelativeRect(labelX+200, labelY+betweenY-2, 200, m_textDrawer->getFontHeight() + 4),
        m_textDrawer);
    m_guiWindow->addGuiObject(std::move(m_inputAuthor));

    // Description
    const cRectangle &descriptionRect = m_guiWindow->getRelativeRect(labelX, labelY+betweenY*2, 75, buttonHeight);
    auto gui_descriptionLabel = GuiLabelBuilder()
        .withLabel("Description:")
        .withTextDrawer(m_textDrawer)
        .withRenderer(m_renderDrawer)
        .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
        .withTextAlign(GuiTextAlignHorizontal::LEFT)
        .withRect(descriptionRect)
        .build();
    m_guiWindow->addGuiObject(std::move(gui_descriptionLabel));

    m_inputDescription = std::make_unique<GuiTextInput>( m_renderDrawer,
        m_guiWindow->getRelativeRect(labelX+200, labelY+betweenY*2-2, 200, m_textDrawer->getFontHeight() + 4),
        m_textDrawer);
    m_guiWindow->addGuiObject(std::move(m_inputDescription));

    // width
    const cRectangle &widthRect = m_guiWindow->getRelativeRect(labelX, labelY+betweenY*3, 75, buttonHeight);
    auto gui_widthLabel = GuiLabelBuilder()
        .withLabel("Width size:")
        .withTextDrawer(m_textDrawer)
        .withRenderer(m_renderDrawer)
        .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
        .withTextAlign(GuiTextAlignHorizontal::LEFT)
        .withRect(widthRect)
        .build();
    m_guiWindow->addGuiObject(std::move(gui_widthLabel));

    m_cycleWidth = std::move(GuiCycleButtonBuilder()
        .withRect( m_guiWindow->getRelativeRect(labelX+200, labelY+betweenY*3, 50, buttonHeight) )
        .withValues( m_sizesMap )
        .withTextDrawer( m_textDrawer )
        .withRenderer(m_renderDrawer)
        .withTheme(cGuiThemeBuilder().light().build())
        .build());
    m_guiWindow->addGuiObject(std::move(m_cycleWidth));

    // height
    const cRectangle &heightRect = m_guiWindow->getRelativeRect(labelX, labelY+betweenY*4, 75, buttonHeight);
    auto gui_heightLabel = GuiLabelBuilder()
        .withLabel("Height size:")
        .withTextDrawer(m_textDrawer)
        .withRenderer(m_renderDrawer)
        .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
        .withTextAlign(GuiTextAlignHorizontal::LEFT)
        .withRect(heightRect)
        .build();
    m_guiWindow->addGuiObject(std::move(gui_heightLabel));

    m_cycleHeight = std::move(GuiCycleButtonBuilder()
        .withRect( m_guiWindow->getRelativeRect(labelX+200, labelY+betweenY*4, 50, buttonHeight) )
        .withValues( m_sizesMap )
        .withTextDrawer( m_textDrawer )
        .withRenderer(m_renderDrawer)
        .withTheme( cGuiThemeBuilder().light().build() )
        .build());
    m_guiWindow->addGuiObject(std::move(m_cycleHeight));

    // QUIT game
    int quit = mainMenuHeight - (buttonHeight + margin);// 464
    int width = (buttonWidth / 2);
    const cRectangle &quitRect = m_guiWindow->getRelativeRect(margin, quit, width, buttonHeight);
    auto gui_btn_Quit = GuiButtonBuilder()
        .withRect(quitRect)        
        .withLabel("Return to menu")
        .withTextDrawer(m_textDrawer)
        .withRenderer(m_renderDrawer)
        .withTextAlign(GuiTextAlignHorizontal::CENTER)
        .withTheme(cGuiThemeBuilder().light().build())
        .onClick([this]() {
            m_game.setNextStateToTransitionTo(GAME_MENU);
            m_game.initiateFadingOut();})
        .build();
    m_guiWindow->addGuiObject(std::move(gui_btn_Quit));

    // BACK to where we came from
    int back = mainMenuHeight - (buttonHeight + margin);// 444
    const cRectangle &backRect = m_guiWindow->getRelativeRect(margin + width + margin, back, (width - margin), buttonHeight);
    
    auto gui_btn_Back = GuiButtonBuilder()
        .withRect(backRect)        
        .withLabel("Create map !")
        .withTextDrawer(m_textDrawer)
        .withRenderer(m_renderDrawer)
        .withTheme(cGuiThemeBuilder().light().build())
        .onClick([this](){
            s_PreviewMap newMap = cPreviewMaps::createEmptyMap(
                m_inputName->getText(), m_inputAuthor->getText(), m_inputDescription->getText(),
                m_cycleWidth->getSelectedValue(), m_cycleHeight->getSelectedValue());
            m_game.loadMapFromEditor(&newMap);
            m_game.initiateFadingOut();})
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
