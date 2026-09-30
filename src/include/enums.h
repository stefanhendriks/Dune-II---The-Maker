/**
 * @file enums.h
 *
 * Backward-compatible compatibility layer.
 *
 * The monolithic enum file was split by domain to reduce coupling and make the code easier to reason about.
 * Individual enum declarations are now kept in focused headers under src/include/enums/ while this file keeps
 * the old include path working for existing code.
 */

#pragma once

#include "include/enums/CombatEnums.h"
#include "include/enums/HouseFlags.h"
#include "include/enums/BuildEnums.h"
#include "include/enums/ListEnums.h"
#include "include/enums/MapEnums.h"
