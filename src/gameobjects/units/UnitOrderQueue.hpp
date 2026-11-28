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

#include <algorithm>
#include <cstddef>
#include <deque>
#include <initializer_list>
#include <variant>

/**
 * The orders of a unit, first in first out. The front is the order the unit carries out now.
 * A new order replaces the queue; a carry-all gets a short sequence of orders at once (see #1823, #1825).
 */
class UnitOrderQueue {
public:
    // a new order replaces whatever the unit was told before
    void replace(const QueuedOrder &order) {
        m_orders.clear();
        m_orders.push_back(order);
    }

    // new orders, carried out one after the other, replace whatever the unit was told before
    void replace(std::initializer_list<QueuedOrder> orders) {
        m_orders.assign(orders);
    }

    // the order the unit carries out now, nullptr when there is none
    [[nodiscard]] const QueuedOrder *current() const {
        if (m_orders.empty()) return nullptr;
        return &m_orders.front();
    }

    // the order the unit carries out now when it is of type T, else nullptr
    template <typename T>
    [[nodiscard]] const T *currentAs() const {
        if (m_orders.empty()) return nullptr;
        return std::get_if<T>(&m_orders.front());
    }

    // true when an order of type T is current or waits in the queue
    template <typename T>
    [[nodiscard]] bool contains() const {
        return std::ranges::any_of(m_orders, [](const QueuedOrder &order) {
            return std::holds_alternative<T>(order);
        });
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
    std::deque<QueuedOrder> m_orders;
};
