/**
 * @file BoxedInCells.cpp
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

#include "gameobjects/map/BoxedInCells.hpp"
#include "gameobjects/map/MapGeometry.hpp"

#include <algorithm>
#include <cstdlib>

namespace BoxedInCells {

bool isBoxedIn(const MapGeometry &geometry, int cell, const std::function<bool(int)> &canEnter)
{
    // breadth-first search that stops as soon as 2 other cells are reachable
    std::vector<int> visited = {cell};
    for (size_t i = 0; i < visited.size(); i++) {
        int x = geometry.getCellX(visited[i]);
        int y = geometry.getCellY(visited[i]);
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int neighbour = geometry.getCellWithMapBorders(x + dx, y + dy);
                if (neighbour < 0) continue;
                if (std::ranges::find(visited, neighbour) != visited.end()) continue;
                if (!canEnter(neighbour)) continue;
                visited.push_back(neighbour);
                if (visited.size() > 2) return false; // the start cell plus 2 reachable cells
            }
        }
    }
    return true;
}

std::vector<int> findClosestUsableCells(const MapGeometry &geometry, int cell, const std::function<bool(int)> &isUsable)
{
    int x = geometry.getCellX(cell);
    int y = geometry.getCellY(cell);
    int maxRadius = geometry.getWidth() + geometry.getHeight();
    std::vector<int> result;
    for (int r = 1; r <= maxRadius && result.empty(); r++) {
        for (int dx = -r; dx <= r; dx++) {
            int dy = r - std::abs(dx);
            for (int candidateY : {y - dy, y + dy}) {
                int candidate = geometry.getCellWithMapBorders(x + dx, candidateY);
                if (candidate >= 0 && isUsable(candidate)) {
                    result.push_back(candidate);
                }
                if (dy == 0) break; // y - 0 and y + 0 are the same cell
            }
        }
    }
    return result;
}

}
