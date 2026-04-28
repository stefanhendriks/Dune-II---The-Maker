#include "cKeyBindings.h"
#include "utils/cLog.h"

#include <algorithm>
#include <sstream>
#include <format>

void cKeyBindings::loadDefaults()
{
    // Global
    bind(eKeyAction::SCREENSHOT,          {SDL_SCANCODE_F11});
    bind(eKeyAction::TOGGLE_MUSIC,        {SDL_SCANCODE_M,   SDL_SCANCODE_MUTE});
    bind(eKeyAction::MUSIC_VOLUME_DOWN,   {SDL_SCANCODE_O,   SDL_SCANCODE_VOLUMEDOWN});
    bind(eKeyAction::MUSIC_VOLUME_UP,     {SDL_SCANCODE_P,   SDL_SCANCODE_VOLUMEUP});
    bind(eKeyAction::GAME_SPEED_UP,       {SDL_SCANCODE_KP_PLUS});
    bind(eKeyAction::GAME_SPEED_DOWN,     {SDL_SCANCODE_KP_MINUS});
    // Alt-modified
    bind(eKeyAction::TOGGLE_FULLSCREEN,   {SDL_SCANCODE_RETURN}, false, true,  false);
    bind(eKeyAction::TOGGLE_CHEAT,        {SDL_SCANCODE_C},      false, true,  false);

    // Map scrolling
    bind(eKeyAction::SCROLL_LEFT,         {SDL_SCANCODE_LEFT});
    bind(eKeyAction::SCROLL_RIGHT,        {SDL_SCANCODE_RIGHT});
    bind(eKeyAction::SCROLL_UP,           {SDL_SCANCODE_UP});
    bind(eKeyAction::SCROLL_DOWN,         {SDL_SCANCODE_DOWN});

    // Gameplay
    bind(eKeyAction::CENTER_ON_HOME,      {SDL_SCANCODE_H});
    bind(eKeyAction::CENTER_ON_STRUCTURE, {SDL_SCANCODE_C});
    bind(eKeyAction::ZOOM_RESET,          {SDL_SCANCODE_Z});
    bind(eKeyAction::TOGGLE_FPS,          {SDL_SCANCODE_F});
    bind(eKeyAction::TOGGLE_TIME_DISPLAY, {SDL_SCANCODE_BACKSLASH});
    bind(eKeyAction::OPEN_MENU,           {SDL_SCANCODE_ESCAPE});
    bind(eKeyAction::DEPLOY_UNIT,         {SDL_SCANCODE_D});
    bind(eKeyAction::SET_PRIMARY,         {SDL_SCANCODE_P});

    // Unit commands
    bind(eKeyAction::ATTACK_MODE,               {SDL_SCANCODE_LCTRL, SDL_SCANCODE_RCTRL});
    bind(eKeyAction::ADD_TO_SELECTION,          {SDL_SCANCODE_LSHIFT, SDL_SCANCODE_RSHIFT});
    bind(eKeyAction::REPAIR_UNIT,               {SDL_SCANCODE_R});
    bind(eKeyAction::RETURN_TO_BASE,            {SDL_SCANCODE_F});
    bind(eKeyAction::SEND_TO_REFINERY,          {SDL_SCANCODE_D});
    bind(eKeyAction::SELECT_SAME_TYPE_ON_SCREEN,{SDL_SCANCODE_Z}, true, false, false);

    // Groups
    bind(eKeyAction::GROUP_1, {SDL_SCANCODE_1});
    bind(eKeyAction::GROUP_2, {SDL_SCANCODE_2});
    bind(eKeyAction::GROUP_3, {SDL_SCANCODE_3});
    bind(eKeyAction::GROUP_4, {SDL_SCANCODE_4});
    bind(eKeyAction::GROUP_5, {SDL_SCANCODE_5});

    // Editor
    bind(eKeyAction::EDITOR_SAVE,     {SDL_SCANCODE_S}, true, false, false);
    bind(eKeyAction::EDITOR_ZOOM_IN,  {SDL_SCANCODE_PAGEUP});
    bind(eKeyAction::EDITOR_ZOOM_OUT, {SDL_SCANCODE_PAGEDOWN});
    bind(eKeyAction::EDITOR_DISPLAY_GRID, {SDL_SCANCODE_G});
    bind(eKeyAction::EDITOR_DISPLAY_AXES, {SDL_SCANCODE_A});

    // UI / menus
    bind(eKeyAction::MENU_BACK,    {SDL_SCANCODE_ESCAPE});
    bind(eKeyAction::MENU_CONFIRM, {SDL_SCANCODE_RETURN});
    bind(eKeyAction::MENU_CANCEL,  {SDL_SCANCODE_BACKSPACE});

    // Debug
    bind(eKeyAction::DEBUG_WIN,                  {SDL_SCANCODE_F2});
    bind(eKeyAction::DEBUG_LOSE,                 {SDL_SCANCODE_F3});
    bind(eKeyAction::DEBUG_GIVE_CREDITS,         {SDL_SCANCODE_F4});
    bind(eKeyAction::DEBUG_DESTROY_AT_CURSOR,    {SDL_SCANCODE_F4}, false, false, true);
    bind(eKeyAction::DEBUG_REVEAL_MAP,           {SDL_SCANCODE_F5});
    bind(eKeyAction::DEBUG_DAMAGE_AT_CURSOR,     {SDL_SCANCODE_F5}, false, false, true);
    bind(eKeyAction::DEBUG_KILL_CARRYALLS,       {SDL_SCANCODE_F6});
    bind(eKeyAction::DEBUG_SPAWN_ORNITHOPTER,    {SDL_SCANCODE_F8});
    bind(eKeyAction::DEBUG_CLEAR_SHROUD_AT_CURSOR, {SDL_SCANCODE_F4}, true, false, false);
    bind(eKeyAction::DEBUG_SWITCH_PLAYER_0,      {SDL_SCANCODE_0});
    bind(eKeyAction::DEBUG_SWITCH_PLAYER_1,      {SDL_SCANCODE_1});
    bind(eKeyAction::DEBUG_SWITCH_PLAYER_2,      {SDL_SCANCODE_2});
    bind(eKeyAction::DEBUG_SWITCH_PLAYER_3,      {SDL_SCANCODE_3});
}

void cKeyBindings::loadFromSection(const cSection &section)
{
    // Mapping from INI key name to eKeyAction.
    // When a key is found in the section, only the scancode(s) are updated.
    // Modifier requirements (requireCtrl/requireAlt/requireShift) are preserved from loadDefaults().
    static const std::vector<std::pair<std::string, eKeyAction>> ACTIONS = {
        {"SCREENSHOT",                eKeyAction::SCREENSHOT},
        {"TOGGLE_MUSIC",              eKeyAction::TOGGLE_MUSIC},
        {"MUSIC_VOLUME_DOWN",         eKeyAction::MUSIC_VOLUME_DOWN},
        {"MUSIC_VOLUME_UP",           eKeyAction::MUSIC_VOLUME_UP},
        {"GAME_SPEED_UP",             eKeyAction::GAME_SPEED_UP},
        {"GAME_SPEED_DOWN",           eKeyAction::GAME_SPEED_DOWN},
        {"TOGGLE_FULLSCREEN",         eKeyAction::TOGGLE_FULLSCREEN},
        {"TOGGLE_CHEAT",              eKeyAction::TOGGLE_CHEAT},
        {"SCROLL_LEFT",               eKeyAction::SCROLL_LEFT},
        {"SCROLL_RIGHT",              eKeyAction::SCROLL_RIGHT},
        {"SCROLL_UP",                 eKeyAction::SCROLL_UP},
        {"SCROLL_DOWN",               eKeyAction::SCROLL_DOWN},
        {"CENTER_ON_HOME",            eKeyAction::CENTER_ON_HOME},
        {"CENTER_ON_STRUCTURE",       eKeyAction::CENTER_ON_STRUCTURE},
        {"ZOOM_RESET",                eKeyAction::ZOOM_RESET},
        {"TOGGLE_FPS",                eKeyAction::TOGGLE_FPS},
        {"TOGGLE_TIME_DISPLAY",       eKeyAction::TOGGLE_TIME_DISPLAY},
        {"OPEN_MENU",                 eKeyAction::OPEN_MENU},
        {"DEPLOY_UNIT",               eKeyAction::DEPLOY_UNIT},
        {"SET_PRIMARY",               eKeyAction::SET_PRIMARY},
        {"ATTACK_MODE",               eKeyAction::ATTACK_MODE},
        {"ADD_TO_SELECTION",          eKeyAction::ADD_TO_SELECTION},
        {"REPAIR",                    eKeyAction::REPAIR_UNIT},
        {"RETURN_TO_BASE",            eKeyAction::RETURN_TO_BASE},
        {"SEND_TO_REFINERY",          eKeyAction::SEND_TO_REFINERY},
        {"SELECT_SAME_TYPE_ON_SCREEN",eKeyAction::SELECT_SAME_TYPE_ON_SCREEN},
        {"GROUP_1",                   eKeyAction::GROUP_1},
        {"GROUP_2",                   eKeyAction::GROUP_2},
        {"GROUP_3",                   eKeyAction::GROUP_3},
        {"GROUP_4",                   eKeyAction::GROUP_4},
        {"GROUP_5",                   eKeyAction::GROUP_5},
        {"EDITOR_SAVE",               eKeyAction::EDITOR_SAVE},
        {"EDITOR_ZOOM_IN",            eKeyAction::EDITOR_ZOOM_IN},
        {"EDITOR_ZOOM_OUT",           eKeyAction::EDITOR_ZOOM_OUT},
        {"EDITOR_DISPLAY_GRID",       eKeyAction::EDITOR_DISPLAY_GRID},
        {"EDITOR_DISPLAY_AXES",       eKeyAction::EDITOR_DISPLAY_AXES},
        {"MENU_BACK",                 eKeyAction::MENU_BACK},
        {"MENU_CONFIRM",              eKeyAction::MENU_CONFIRM},
        {"MENU_CANCEL",               eKeyAction::MENU_CANCEL},
        {"DEBUG_WIN",                 eKeyAction::DEBUG_WIN},
        {"DEBUG_LOSE",                eKeyAction::DEBUG_LOSE},
        {"DEBUG_GIVE_CREDITS",        eKeyAction::DEBUG_GIVE_CREDITS},
        {"DEBUG_DESTROY_AT_CURSOR",   eKeyAction::DEBUG_DESTROY_AT_CURSOR},
        {"DEBUG_REVEAL_MAP",          eKeyAction::DEBUG_REVEAL_MAP},
        {"DEBUG_DAMAGE_AT_CURSOR",    eKeyAction::DEBUG_DAMAGE_AT_CURSOR},
        {"DEBUG_KILL_CARRYALLS",      eKeyAction::DEBUG_KILL_CARRYALLS},
        {"DEBUG_SPAWN_ORNITHOPTER",   eKeyAction::DEBUG_SPAWN_ORNITHOPTER},
        {"DEBUG_CLEAR_SHROUD_AT_CURSOR", eKeyAction::DEBUG_CLEAR_SHROUD_AT_CURSOR},
        {"DEBUG_SWITCH_PLAYER_0",     eKeyAction::DEBUG_SWITCH_PLAYER_0},
        {"DEBUG_SWITCH_PLAYER_1",     eKeyAction::DEBUG_SWITCH_PLAYER_1},
        {"DEBUG_SWITCH_PLAYER_2",     eKeyAction::DEBUG_SWITCH_PLAYER_2},
        {"DEBUG_SWITCH_PLAYER_3",     eKeyAction::DEBUG_SWITCH_PLAYER_3},
    };

    for (auto &[name, action] : ACTIONS) {
        if (section.hasValue(name)) {
            std::string value = section.getStringValue(name);
            s_KeyBinding parsed = parseBinding(value);
            if (!parsed.keys.empty()) {
                m_bindings[action] = parsed;
            } else {
                cLogger::getInstance()->log(LOG_WARN, COMP_INIT, "[KEYS]",
                    std::format("No valid key found for action '{}' (value: '{}'), keeping default", name, value));
            }
        }
    }
}

bool cKeyBindings::matches(const std::set<SDL_Scancode> &keys, const s_KeysCombo &combo, eKeyAction action) const
{
    auto it = m_bindings.find(action);
    if (it == m_bindings.end()) return false;

    const s_KeyBinding &binding = it->second;

    if (binding.requireCtrl  && !combo.ctrlPressed)  return false;
    if (binding.requireAlt   && !combo.altPressed)   return false;
    if (binding.requireShift && !combo.shiftPressed)  return false;

    for (SDL_Scancode sc : binding.keys) {
        if (keys.count(sc) > 0) return true;
    }
    return false;
}

void cKeyBindings::bind(eKeyAction action, std::initializer_list<SDL_Scancode> keys,
                        bool requireCtrl, bool requireAlt, bool requireShift)
{
    m_bindings[action] = s_KeyBinding{std::vector<SDL_Scancode>(keys), requireCtrl, requireAlt, requireShift};
}

s_KeyBinding cKeyBindings::parseBinding(const std::string &value)
{
    s_KeyBinding binding;
    std::istringstream ss(value);
    std::string token;
    while (std::getline(ss, token, ',')) {
        // trim whitespace
        auto start = token.find_first_not_of(" \t");
        auto end   = token.find_last_not_of(" \t");
        if (start == std::string::npos) continue;
        token = token.substr(start, end - start + 1);

        // Check for modifier tokens (case-insensitive)
        std::string lower = token;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if      (lower == "alt")   { binding.requireAlt   = true; }
        else if (lower == "ctrl")  { binding.requireCtrl  = true; }
        else if (lower == "shift") { binding.requireShift = true; }
        else {
            SDL_Scancode sc = SDL_GetScancodeFromName(token.c_str());
            if (sc != SDL_SCANCODE_UNKNOWN) {
                binding.keys.push_back(sc);
            } else {
                cLogger::getInstance()->log(LOG_WARN, COMP_INIT, "[KEYS]",
                    std::format("Unknown SDL scancode name: '{}'", token));
            }
        }
    }
    return binding;
}
