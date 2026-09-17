/**
 * @file cEventEmitter.h
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

#include "include/sGameEvent.h"

#include <functional>
#include <utility>

class cEventEmitter {
public:
    using DispatchFn = std::function<void(const s_GameEvent &)>;

    explicit cEventEmitter(DispatchFn dispatchFn)
        : m_dispatchFn(std::move(dispatchFn)) {
    }

    void emit(const s_GameEvent &event) const {
        m_dispatchFn(event);
    }

private:
    DispatchFn m_dispatchFn;
};
