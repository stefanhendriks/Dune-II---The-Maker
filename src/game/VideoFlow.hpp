/**
 * @file VideoFlow.hpp
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

#include <functional>
#include <string>
#include <utility>

/**
 * Narrow entry point to playing a video (a .pak file with frames), from anywhere in the game.
 * Holds a callback so it does not depend on cGame.
 */
class VideoFlow {
public:
    using PlayCallback = std::function<void(const std::string &)>;

    explicit VideoFlow(PlayCallback play)
        : m_play(std::move(play)) {
    }

    void play(const std::string &filename) const {
        m_play(filename);
    }

private:
    PlayCallback m_play;
};
