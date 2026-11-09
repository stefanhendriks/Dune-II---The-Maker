/**
 * @file FakeStateCreator.hpp
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

#include "game/IStateCreator.hpp"

#include <array>
#include <cstddef>
#include <memory>
#include <vector>

/**
 * IStateCreator that hands out distinct fake cGameState pointers.
 *
 * A real cGameState needs a full GameContext, which a unit test cannot build. The state machine only compares
 * and passes on the pointers and never dereferences them, so the fakes are unique addresses that are never
 * used as an object. Destroyed addresses stay allocated until the end of the test, so a state that is created
 * again always gets a different pointer.
 */
class FakeStateCreator : public IStateCreator {
public:
    cGameState *getState(eGameState gameState) override {
        auto &slot = m_slots[static_cast<size_t>(gameState)];
        if (slot == nullptr) {
            m_allocations.push_back(std::make_unique<std::byte>());
            slot = reinterpret_cast<cGameState *>(m_allocations.back().get());
            m_created.push_back(gameState);
        }
        return slot;
    }

    [[nodiscard]] bool hasState(eGameState gameState) const override {
        return m_slots[static_cast<size_t>(gameState)] != nullptr;
    }

    void destroyState(eGameState gameState) override {
        m_slots[static_cast<size_t>(gameState)] = nullptr;
        m_destroyed.push_back(gameState);
    }

    // what the real creator does for the VIDEO state
    cGameState *createVideoState() {
        return getState(eGameState::VIDEO);
    }

    std::vector<eGameState> m_created;
    std::vector<eGameState> m_destroyed;

private:
    std::array<cGameState *, static_cast<size_t>(eGameState::COUNT)> m_slots{};
    std::vector<std::unique_ptr<std::byte>> m_allocations;
};
