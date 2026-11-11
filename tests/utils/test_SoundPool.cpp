#include <catch2/catch_test_macros.hpp>

#include "utils/SoundPool.hpp"

#include <array>

namespace {
constexpr std::array kIds = {7, 42, 3};
constexpr std::array kAcks = {26, 27, 28};
constexpr std::array kLose = {15, 16, 17};
}

TEST_CASE("SoundPool at returns the listed ids, not adjacent ones", "[SoundPool]") {
    constexpr SoundPool pool{kIds};
    CHECK(pool.size() == 3);
    CHECK(pool.at(0) == 7);
    CHECK(pool.at(1) == 42);
    CHECK(pool.at(2) == 3);
}

TEST_CASE("SoundPool at wraps out of range indices", "[SoundPool]") {
    constexpr SoundPool pool{kIds};
    CHECK(pool.at(3) == 7);
    CHECK(pool.at(-1) == 3);
}

TEST_CASE("empty SoundPool yields -1", "[SoundPool]") {
    SoundPool pool{std::span<const int>{}};
    CHECK(pool.size() == 0);
    CHECK(pool.at(0) == -1);
    CHECK(pool.pick() == -1);
}

TEST_CASE("SoundPool pick only returns members", "[SoundPool]") {
    constexpr SoundPool pool{kAcks};
    for (int i = 0; i < 200; i++) {
        int id = pool.pick();
        CHECK((id == 26 || id == 27 || id == 28));
    }
}

TEST_CASE("lose music pool holds only the three lose tracks", "[SoundPool]") {
    constexpr SoundPool pool{kLose};
    CHECK(pool.size() == 3);
    for (int i = 0; i < 200; i++) {
        int id = pool.pick();
        CHECK((id >= 15 && id <= 17));
    }
}

TEST_CASE("resolveSoundIds fills ids from names via the lookup", "[SoundPool]") {
    constexpr std::array<std::string_view, 3> names = {"A", "B", "C"};
    std::array<int, 3> ids{};
    auto lookup = [](std::string_view n) { return n == "A" ? 5 : n == "B" ? 9 : -1; };
    CHECK(resolveSoundIds(names, ids, lookup) == 1);
    CHECK(ids[0] == 5);
    CHECK(ids[1] == 9);
    CHECK(ids[2] == -1);
}

TEST_CASE("a SoundPool over resolved ids picks the resolved values", "[SoundPool]") {
    static std::array<int, 2> ids{};
    constexpr std::array<std::string_view, 2> names = {"X", "Y"};
    resolveSoundIds(names, ids, [](std::string_view n) { return n == "X" ? 11 : 12; });
    SoundPool pool{ids};
    CHECK(pool.at(0) == 11);
    CHECK(pool.at(1) == 12);
}
