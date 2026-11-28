/**
 * @file UnitOrder.hpp
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

#include "include/enums/CombatEnums.h"

#include <variant>

enum class eTransferType {
    NONE,                               // nothing to transfer
    NEW_STAY,                           // bring a new unit, and let the carryall stay
    NEW_LEAVE,                          // bring a new unit, and let the carryall go back (and die)
    PICKUP,                             // carry an existing unit, bring to new location
    DIE,                                // will die when reaching goal-cell (added for proper animation)
};

enum class eUnitOrderKind {
    MOVE,       // drive to goalCell, optionally to enter a structure or pick up a unit
    ATTACK,     // attack a unit, structure or cell
    TRANSFER,   // carry-all: pick up unitToPickup, or bring a new unit of newUnitType, to goalCell
};

/**
 * One order given to a unit (by the player, an AI brain or the game itself), as it was given.
 * The unit carries it out with its own movement and combat state.
 */
struct UnitOrder {
    eUnitOrderKind kind = eUnitOrderKind::MOVE;
    int goalCell = -1;

    // MOVE
    int structureToEnter = -1;
    int unitToPickup = -1;
    eUnitActionIntent intent = INTENT_MOVE;

    // ATTACK
    int attackUnit = -1;
    int attackStructure = -1;
    int attackCell = -1;
    bool chaseWhenOutOfRange = true;

    // TRANSFER
    eTransferType transferType = eTransferType::NONE;
    int newUnitType = -1;
};

// carry-all: fly to unitId and pick it up
struct PickupUnitOrder {
    int unitId = -1;
};

// carry-all or frigate: fly to cell and set down what it carries; a new unit then drives to unitMoveToCell (-1 = stays)
struct DropUnitOrder {
    int cell = -1;
    int unitMoveToCell = -1;
};

// air unit: fly to the closest map edge and leave the map (the unit is removed there)
struct LeaveMapOrder {
};

// One order in a unit's queue; the variant is the kind of order
using QueuedOrder = std::variant<UnitOrder, PickupUnitOrder, DropUnitOrder, LeaveMapOrder>;
