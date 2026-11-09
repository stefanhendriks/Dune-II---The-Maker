/**
 * @file NullStateCallbacks.hpp
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

#pragma once

#include "game/IStateCallbacks.hpp"
#include "definitions.h"

#include <string>
#include <vector>

/**
 * IStateCallbacks that does nothing but write down which callbacks were called, in order.
 */
class NullStateCallbacks : public IStateCallbacks {
public:
    void onTransitionStarting(eGameState, eGameState) override {
        m_calls.emplace_back("starting");
    }

    void playMusic(eMusicType musicType) override {
        switch (musicType) {
            case eMusicType::MENU:
                m_calls.emplace_back("music:menu");
                break;
            case eMusicType::CONQUEST:
                m_calls.emplace_back("music:conquest");
                break;
            case eMusicType::BRIEFING:
                m_calls.emplace_back("music:briefing");
                break;
            default:
                m_calls.emplace_back("music:other");
                break;
        }
    }

    void setCheatMode(bool enabled) override {
        m_calls.emplace_back(enabled ? "cheat:on" : "cheat:off");
    }

    void setMouseNormal() override {
        m_calls.emplace_back("mouseNormal");
    }

    void takeBackgroundScreen() override {
        m_calls.emplace_back("backgroundScreen");
    }

    void onTransitionToPlaying(cGameState *, bool isNew) override {
        m_calls.emplace_back(isNew ? "playing:new" : "playing:existing");
    }

    void onTransitionToPlayingFromOptions() override {
        m_calls.emplace_back("playing:fromOptions");
    }

    void onTransitionToOptions(bool fromPlaying) override {
        m_calls.emplace_back(fromPlaying ? "options:fromPlaying" : "options:new");
    }

    void onTransitionToOptionsExisting(cGameState *, eGameState from) override {
        m_calls.emplace_back("options:existing");
        m_optionsExistingFrom = from;
    }

    void onTransitionToRegionNew(cGameState *) override {
        m_calls.emplace_back("region:new");
    }

    void onTransitionToRegionExisting(cGameState *, bool fromWinLoseBrief) override {
        m_calls.emplace_back(fromWinLoseBrief ? "region:existing:fromBrief" : "region:existing");
    }

    void onTransitionToSkirmish() override {
        m_calls.emplace_back("skirmish");
    }

    void onEnteringTellHouse() override {
        m_calls.emplace_back("tellHouse");
    }

    void onEnteringLoseBrief() override {
        m_calls.emplace_back("loseBrief");
    }

    [[nodiscard]] bool called(const std::string &call) const {
        for (const auto &c : m_calls) {
            if (c == call) return true;
        }
        return false;
    }

    std::vector<std::string> m_calls;
    eGameState m_optionsExistingFrom = eGameState::INITIALIZE;
};
