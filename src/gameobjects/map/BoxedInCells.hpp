/**
 * @file BoxedInCells.hpp
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
#include <vector>

class MapGeometry;

namespace BoxedInCells {

// A unit on `cell` is boxed in when it can reach fewer than 2 other cells, stepping in 8 directions
// over playable cells for which canEnter returns true.
bool isBoxedIn(const MapGeometry &geometry, int cell, const std::function<bool(int)> &canEnter);

// All playable cells at the smallest Manhattan distance r >= 1 from `cell` for which isUsable returns
// true. Empty when no playable cell is usable.
std::vector<int> findClosestUsableCells(const MapGeometry &geometry, int cell, const std::function<bool(int)> &isUsable);

}
