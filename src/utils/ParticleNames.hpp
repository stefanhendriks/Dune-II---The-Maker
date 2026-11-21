/**
 * @file ParticleNames.hpp
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

#include <string_view>

// The particle id for a particle name as written in the rules INI files, without the D2TM_PARTICLE_ prefix
// (for example "EXPLOSION_ROCKET_SMALL"). Returns -1 when the name is unknown.
int particleIdFromName(std::string_view name);
