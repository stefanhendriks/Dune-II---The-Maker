/**
 * @file cGameObjectContextCreator.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <memory>

class cGameObjectContext;

class cGameObjectsContextCreator {
public:
    cGameObjectsContextCreator() = default;
    ~cGameObjectsContextCreator() = default;

    static std::unique_ptr<cGameObjectContext> create();
};
