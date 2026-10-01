#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <memory>
#include <bitset>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "controls/cKeyboardEvent.h"
#include "drawers/SDLDrawer.hpp"
#include "drawers/cTextDrawer.h"
#include "game/cGameSettings.h"
#include "gui/GuiTextInput.h"

namespace {

std::filesystem::path repoRoot()
{
    return std::filesystem::path(__FILE__).parent_path().parent_path().parent_path();
}

class GuiTextInputTestFixture {
public:
    GuiTextInputTestFixture()
    {
        REQUIRE(SDL_Init(SDL_INIT_VIDEO));
        REQUIRE(TTF_Init());

        window = SDL_CreateWindow("d2tm-gui-textinput-tests", 128, 128, SDL_WINDOW_HIDDEN);
        REQUIRE(window != nullptr);

        renderer = SDL_CreateRenderer(window, nullptr);
        REQUIRE(renderer != nullptr);

        drawer = std::make_unique<SDLDrawer>(renderer);

        const auto fontPath = repoRoot() / "bin" / "data" / "arrakeen.fon";
        font = std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)>(TTF_OpenFont(fontPath.string().c_str(), 16), TTF_CloseFont);
        REQUIRE(font != nullptr);

        settings = std::make_unique<cGameSettings>();
        textDrawer = std::make_unique<cTextDrawer>(font.get(), settings.get(), drawer.get());
    }

    ~GuiTextInputTestFixture()
    {
        textDrawer.reset();
        settings.reset();
        font.reset();
        drawer.reset();

        if (renderer != nullptr) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }

        if (window != nullptr) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }

        TTF_Quit();
        SDL_Quit();
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::unique_ptr<SDLDrawer> drawer;
    std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)> font{nullptr, TTF_CloseFont};
    std::unique_ptr<cGameSettings> settings;
    std::unique_ptr<cTextDrawer> textDrawer;
};

} // namespace

TEST_CASE_METHOD(GuiTextInputTestFixture, "GuiTextInput - click focuses the field and keyboard text appends to the value", "[gui][text_input]")
{
    GuiTextInput input(drawer.get(), cRectangle(10, 10, 120, 20), textDrawer.get());

    input.onNotifyMouseEvent({MOUSE_LEFT_BUTTON_CLICKED, cPoint(15, 15)});
    REQUIRE(input.hasKeyboardFocus() == true);

    std::bitset<SDL_SCANCODE_COUNT> keys;
    cKeyboardEvent addTextEvent(eKeyEventType::PRESSED, keys, s_KeysCombo{}, nullptr, "D2");
    input.onNotifyKeyboardEvent(addTextEvent);

    REQUIRE(input.getText() == "D2");
}

TEST_CASE_METHOD(GuiTextInputTestFixture, "GuiTextInput - backspace removes the last character from the current text", "[gui][text_input]")
{
    GuiTextInput input(drawer.get(), cRectangle(10, 10, 120, 20), textDrawer.get());
    input.setText("AB");
    input.setFocused(true);

    std::bitset<SDL_SCANCODE_COUNT> keys;
    keys.set(SDL_SCANCODE_BACKSPACE);
    cKeyboardEvent backspaceEvent(eKeyEventType::PRESSED, keys, s_KeysCombo{}, nullptr, "");
    input.onNotifyKeyboardEvent(backspaceEvent);

    REQUIRE(input.getText() == "A");
}

TEST_CASE_METHOD(GuiTextInputTestFixture, "GuiTextInput - pressing enter triggers the callback with the current text", "[gui][text_input]")
{
    GuiTextInput input(drawer.get(), cRectangle(10, 10, 120, 20), textDrawer.get());
    input.setFocused(true);
    input.setText("Ready");

    std::string submitted;
    input.setOnEnter([&submitted](const std::string& value) {
        submitted = value;
    });

    std::bitset<SDL_SCANCODE_COUNT> keys;
    keys.set(SDL_SCANCODE_RETURN);
    cKeyboardEvent enterEvent(eKeyEventType::PRESSED, keys, s_KeysCombo{}, nullptr, "");
    input.onNotifyKeyboardEvent(enterEvent);

    REQUIRE(submitted == "Ready");
}
