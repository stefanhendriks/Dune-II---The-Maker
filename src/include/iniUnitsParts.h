/**
 * @file iniUnitsParts.h
 *
 * Field indexes used when reading unit-part payloads from INI data.
 */

#pragma once

enum class eIniUnitsPart {
    CONTROLLER = 0,
    TYPE,
    HP,
    CELL,
    FACING_BODY,
    FACING_HEAD,
    COUNT
};
