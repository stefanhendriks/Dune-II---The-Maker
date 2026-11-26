/**
 * @file AiParametersLoader.hpp
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

#include <string>

namespace brains {
struct s_AiParameters;
}

// Fills the AI tuning values from AI.ini. Each struct section has its own [SECTION].
// A key that is absent keeps the value already in the struct (the default the game always used).
class AiParametersLoader {
public:
    // Returns false when the file could not be opened (everything then keeps the default values)
    bool load(const std::string& filename, brains::s_AiParameters& parameters) const;
};
