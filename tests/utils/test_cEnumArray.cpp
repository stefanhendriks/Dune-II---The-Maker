/**
 * @file test_cEnumArray.cpp
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

#include <catch2/catch_test_macros.hpp>
#include "utils/cEnumArray.h"

#include <type_traits>

namespace {
enum class eTestKind {
    FIRST,
    SECOND,
    THIRD,
    COUNT
};
}

TEST_CASE("EnumArray - size equals the COUNT enumerator", "[enumarray]")
{
    EnumArray<int, eTestKind> arr;
    REQUIRE(arr.size() == 3);
}

TEST_CASE("EnumArray - elements are addressed by enum value", "[enumarray]")
{
    EnumArray<int, eTestKind> arr;
    arr[eTestKind::FIRST] = 10;
    arr[eTestKind::SECOND] = 20;
    arr[eTestKind::THIRD] = 30;

    REQUIRE(arr[eTestKind::FIRST] == 10);
    REQUIRE(arr[eTestKind::SECOND] == 20);
    REQUIRE(arr[eTestKind::THIRD] == 30);
}

TEST_CASE("EnumArray - fill sets every element", "[enumarray]")
{
    EnumArray<int, eTestKind> arr;
    arr.fill(7);
    for (int v : arr) {
        REQUIRE(v == 7);
    }
}

TEST_CASE("EnumArray - iteration visits elements in enum order", "[enumarray]")
{
    EnumArray<int, eTestKind> arr;
    arr[eTestKind::FIRST] = 1;
    arr[eTestKind::SECOND] = 2;
    arr[eTestKind::THIRD] = 3;

    int expected = 1;
    for (int v : arr) {
        REQUIRE(v == expected);
        expected++;
    }
    REQUIRE(expected == 4);
}

TEST_CASE("EnumArray - const access and const iteration", "[enumarray]")
{
    EnumArray<int, eTestKind> arr;
    arr.fill(5);
    arr[eTestKind::SECOND] = 9;

    const auto& constArr = arr;
    REQUIRE(constArr[eTestKind::SECOND] == 9);
    REQUIRE(constArr[eTestKind::FIRST] == 5);
    static_assert(std::is_const_v<std::remove_reference_t<decltype(constArr[eTestKind::FIRST])>>);

    int sum = 0;
    for (int v : constArr) {
        sum += v;
    }
    REQUIRE(sum == 19);
}
