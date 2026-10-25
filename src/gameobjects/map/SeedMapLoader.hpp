/**
 * @file SeedMapLoader.hpp
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

class cMap;
class MapGeometry;
struct s_PreviewMap;

namespace SeedMapLoader {

// Generate the map for a seed and create its cells on the given map (map must be 64x64)
void loadIntoMap(cMap *map, const MapGeometry *mapGeometry, int seed);

// Generate the map for a seed as a preview map (terrain only: no surface/texture, no start cells)
s_PreviewMap toPreviewMap(int seed);

}
