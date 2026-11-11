/**
 * @file SoundPool.hpp
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

#include "utils/RNG.hpp"

#include <functional>
#include <span>
#include <string_view>

/**
 * A named set of interchangeable sound ids (variants of one sound).
 * Ids are listed explicitly, so no assumption is made about their adjacency in the pack.
 */
class SoundPool
{
public:
    // non-owning: the ids must have static storage duration (e.g. a namespace-scope constexpr array)
    constexpr explicit SoundPool(std::span<const int> ids) : m_ids(ids) {}

    constexpr int size() const {
        return static_cast<int>(m_ids.size());
    }

    // the variant at position `index`, wrapped around when index is out of range; -1 when the pool is empty
    constexpr int at(int index) const {
        if (m_ids.empty()) return -1;
        const int count = size();
        return m_ids[((index % count) + count) % count];
    }

    // a random variant; -1 when the pool is empty
    int pick() const {
        return at(RNG::rnd(size()));
    }

private:
    std::span<const int> m_ids;
};

/**
 * Fills `ids` with the pack index of each name, via `lookup` (-1 when a name is unknown).
 * Returns how many names could not be resolved. `names` and `ids` must have the same size.
 */
inline int resolveSoundIds(std::span<const std::string_view> names, std::span<int> ids,
                           const std::function<int(std::string_view)> &lookup) {
    int unresolved = 0;
    for (std::size_t i = 0; i < names.size() && i < ids.size(); i++) {
        ids[i] = lookup(names[i]);
        if (ids[i] < 0) {
            unresolved++;
        }
    }
    return unresolved;
}
