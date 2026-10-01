#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <memory>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "drawers/SDLDrawer.hpp"
#include "drawers/cTextDrawer.h"
#include "game/cGameSettings.h"
#include "gui/GuiTextInput.h"
#include "gui/GuiWindow.h"

namespace {

std::filesystem::path repoRoot()
{
    return std::filesystem::path(__FILE__).parent_path().parent_path().parent_path();
}

class GuiWindowTestFixture {
public:
    GuiWindowTestFixture()
    {
        REQUIRE(SDL_Init(SDL_INIT_VIDEO));
        REQUIRE(TTF_Init());

        window = SDL_CreateWindow("d2tm-gui-window-tests", 128, 128, SDL_WINDOW_HIDDEN);
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

    ~GuiWindowTestFixture()
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

TEST_CASE_METHOD(GuiWindowTestFixture, "GuiWindow - child focus is reported and relative offsets are computed from window coordinates", "[gui][window]")
{
    GuiWindow window(drawer.get(), cRectangle(10, 20, 200, 120), textDrawer.get());
    auto input = std::make_unique<GuiTextInput>(drawer.get(), cRectangle(15, 25, 80, 20), textDrawer.get());

    input->onNotifyMouseEvent({MOUSE_LEFT_BUTTON_CLICKED, cPoint(20, 30)});
    REQUIRE(input->hasKeyboardFocus() == true);

    window.addGuiObject(std::move(input));
    REQUIRE(window.hasFocusedInput() == true);

    const auto rect = window.getRelativeRect(5, 7, 30, 40);
    REQUIRE(rect.getX() == 15);
    REQUIRE(rect.getY() == 27);
    REQUIRE(rect.getWidth() == 30);
    REQUIRE(rect.getHeight() == 40);
}
