/**
 * @file test_cMatrix.cpp
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
#include "utils/cMatrix.h"

TEST_CASE("Matrix - dimensions are reported", "[matrix]")
{
    Matrix<int> m({1, 2, 3, 4, 5, 6}, 2, 3);
    REQUIRE(m.getRows() == 2);
    REQUIRE(m.getCols() == 3);
}

TEST_CASE("Matrix - data is stored row by row", "[matrix]")
{
    Matrix<int> m({1, 2, 3, 4, 5, 6}, 2, 3);
    REQUIRE(m[0][0] == 1);
    REQUIRE(m[0][2] == 3);
    REQUIRE(m[1][0] == 4);
    REQUIRE(m[1][2] == 6);
}

TEST_CASE("Matrix - elements can be modified", "[matrix]")
{
    Matrix<int> m({1, 2, 3, 4}, 2, 2);
    m[1][1] = 99;
    REQUIRE(m[1][1] == 99);
    REQUIRE(m[0][0] == 1);
}

TEST_CASE("Matrix - const access", "[matrix]")
{
    const Matrix<int> m({1, 2, 3, 4}, 2, 2);
    REQUIRE(m[1][0] == 3);
}

TEST_CASE("Matrix - constructor rejects data that does not fit rows * cols", "[matrix]")
{
    REQUIRE_THROWS_AS(Matrix<int>({1, 2, 3}, 2, 2), std::invalid_argument);
    REQUIRE_THROWS_AS(Matrix<int>({1, 2, 3, 4, 5}, 2, 2), std::invalid_argument);
}

TEST_CASE("Matrix - out of bounds access throws", "[matrix]")
{
    Matrix<int> m({1, 2, 3, 4}, 2, 2);
    REQUIRE_THROWS_AS(m[2][0], std::out_of_range);
    REQUIRE_THROWS_AS(m[0][2], std::out_of_range);

    const Matrix<int>& constM = m;
    REQUIRE_THROWS_AS(constM[2][0], std::out_of_range);
    REQUIRE_THROWS_AS(constM[0][2], std::out_of_range);
}

TEST_CASE("Matrix - empty matrix is valid but has no elements", "[matrix]")
{
    Matrix<int> m({}, 0, 0);
    REQUIRE(m.getRows() == 0);
    REQUIRE(m.getCols() == 0);
    REQUIRE_THROWS_AS(m[0][0], std::out_of_range);
}
