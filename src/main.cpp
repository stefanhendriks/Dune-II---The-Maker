/**
 * @file main.cpp
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

#include "game/cGame.h"

#include "config.h"
#include "utils/Log.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "utils/cHandleArgument.h"
#include "utils/CrashHandler.h"
#include "utils/common.h"
#include "game/Preferences.hpp"
#include "game/PreferencesPath.hpp"


#include <algorithm>
#include <string>
#include <string_view>

#include <iostream>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#ifdef _WIN32
#include <windows.h>
#include <cstdio>
#endif

#ifdef _WIN32
// The game is a GUI subsystem executable (-mwindows), so it has no console of its own.
// Attach to the console of the terminal that started it, so --help and errors can be seen.
static void attachParentConsole()
{
    if (GetStdHandle(STD_OUTPUT_HANDLE) != nullptr) return; // output already redirected
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) return;
    if (freopen("CONOUT$", "w", stdout) == nullptr) return;
    if (freopen("CONOUT$", "w", stderr) == nullptr) return;
}
#endif

constexpr const char *PREFERENCES_FILE = "preferences.ini";

int main(int argc, char **argv)
{
#ifdef _WIN32
    attachParentConsole();
#endif
    std::unique_ptr<cLog> m_log = std::make_unique<cLog>("nlog.txt");
    Logger::init(m_log.get());
    // m_log is destroyed when main returns, make sure the crash handlers never write through a dangling pointer
    struct LoggerReset {
        ~LoggerReset() { Logger::init(nullptr); }
    } loggerReset;
    CrashHandler::install();
    Logger::info(COMP_NONE, "main", "=== Dune II - The Maker ===");
    Logger::info(COMP_VERSION, "main", "=== Version information ===");
    Logger::info(COMP_VERSION, "Initializing", "Version {}, Compiled at {} , {}", D2TM_VERSION, __DATE__, __TIME__);

    cGame game;

    std::unique_ptr<InitialGameSettings> settings = loadSettingsFromIni("settings.ini");
    
    // -prefs: start from the options saved at the end of the previous run, the other arguments still override them
    if (std::any_of(argv + 1, argv + argc, [](const char *arg) { return std::string_view(arg) == "-prefs"; })) {
        std::string prefsError;
        const auto prefsFile = PreferencesPath::file(PREFERENCES_FILE);
        if (!prefsFile) {
            Logger::warn(COMP_GAMERULES, "Preferences", "Could not get the user data directory, preferences are not loaded");
        } else if (!Preferences::load(*prefsFile, *settings, prefsError)) {
            Logger::warn(COMP_GAMERULES, "Preferences", "Preferences are not loaded: {}", prefsError);
        }
    }

    // read command-line arguments, can override settings.ini
    cHandleArgument handleArg;
    try {
        int parseResult = handleArg.handleArguments(argc, argv, settings.get());
        if (parseResult == cHandleArgument::HELP_REQUESTED) {
            return 0;
        }
        if (parseResult < 0) {
            m_log->error(COMP_GAMERULES, "HandleArgument", "Incorrect arguments");
            return 1;
        }
    }
    catch (std::runtime_error &e) {
        m_log->error(eLogComponent::COMP_GAMERULES, "HandleArgument", "Runtime_error on parsing : {}", e.what());
        std::cerr << std::format("Error: {}\n\n", e.what());
        return 1;
    }

    m_log->setDebug(settings->debugMode);
    
    game.applySettings(std::move(settings));

    try {
        if (game.setupGame()) {
            game.run();

            if (const auto prefsFile = PreferencesPath::file(PREFERENCES_FILE)) {
                std::string prefsError;
                if (!Preferences::save(*prefsFile, game.currentSettings(), prefsError)) {
                    Logger::warn(COMP_GAMERULES, "Preferences", "Could not save preferences: {}", prefsError);
                }
            }
        }

        game.shutdown();
    }
    catch (std::runtime_error &e) {
        m_log->error(eLogComponent::COMP_NONE, "Unknown", "Error: {}", e.what());
        std::cerr << std::format("Error: {}\n\n", e.what());
    }

    std::cout << std::format("Thank you for playing Dune 2 - The Maker\n");
    return 0;
}