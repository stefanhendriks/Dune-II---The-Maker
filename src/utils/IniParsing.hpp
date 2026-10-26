/**
 * @file IniParsing.hpp
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

#include "iniSections.h"
#include "iniWords.h"

#include <string>
#include <string_view>
#include <utility>
#include <vector>

// Pure string/INI parsing helpers used by cIni. They do not depend on game state.

int ToInt(const std::string& str);
float ToFloat(const std::string& str);
bool ToBool(const std::string& str);
std::string ToTrim(const std::string& input);
std::string removeQuote(const std::string& input);
// Split a comma-separated string into a vector of tokens (not trimmed)
std::vector<std::string> splitByComma(const std::string& input);
// Extract the trimmed value from a section tag like "[UNIT: Devastator]" given the prefix "[UNIT:"
std::string extractTagValue(const std::string& line, std::string_view prefix);
// [SECTION] -> "SECTION"
std::string extractSectionName(const std::string& line);
// "key=value" -> {"key", "value"}
std::pair<std::string, std::string> INI_SplitWord(const std::string& input);
// Reads out word[], does a string compare and returns the word id
eIniWord INI_WordType(const std::string& word, eIniSection section);
// Scenario section types
eIniSection SCEN_INI_SectionType(const std::string& section);
eIniSection GAME_INI_SectionType(const std::string& section, eIniSection last);
bool isCommentLine(const std::string& linefeed);
