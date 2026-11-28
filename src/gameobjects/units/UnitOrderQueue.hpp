/**
 * @file UnitOrderQueue.hpp
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

#include "UnitOrder.hpp"

#include <cstddef>
#include <deque>
#include <optional>

/**
 * The orders of a unit, first in first out. The front is the order the unit carries out now.
 * For now a new order always replaces the queue, so it holds at most one order (see #1823).
 */
class UnitOrderQueue {
public:
    // a new order replaces whatever the unit was told before
    void replace(const UnitOrder &order) {
        m_orders.clear();
        m_orders.push_back(order);
    }

    // the order the unit carries out now, if any
    [[nodiscard]] std::optional<UnitOrder> current() const {
        if (m_orders.empty()) return std::nullopt;
        return m_orders.front();
    }

    // the current order is done (or abandoned), the next one (if any) becomes current
    void completeCurrent() {
        if (!m_orders.empty()) {
            m_orders.pop_front();
        }
    }

    void clear() {
        m_orders.clear();
    }

    [[nodiscard]] bool empty() const {
        return m_orders.empty();
    }

    [[nodiscard]] std::size_t size() const {
        return m_orders.size();
    }

private:
    std::deque<UnitOrder> m_orders;
};
