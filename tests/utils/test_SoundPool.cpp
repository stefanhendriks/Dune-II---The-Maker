#include <catch2/catch_test_macros.hpp>

#include "utils/SoundPool.hpp"

#include <array>
#include <cstdint>

namespace {
constexpr audioID id(int i) { return toAudioID(i); }
constexpr std::array kIds = {id(7), id(42), id(3)};
constexpr std::array kAcks = {id(26), id(27), id(28)};
constexpr std::array kLose = {id(15), id(16), id(17)};
}

TEST_CASE("audioID invalid is the uint32 wrap of -1 and round trips pack indices", "[SoundPool]") {
    CHECK(toAudioID(-1) == audioID::invalid);
    CHECK(toPackIndex(audioID::invalid) == -1);
    CHECK(toPackIndex(toAudioID(42)) == 42);
    CHECK(static_cast<std::uint32_t>(audioID::invalid) == 0xFFFFFFFFu);
}

TEST_CASE("SoundPool at returns the listed ids, not adjacent ones", "[SoundPool]") {
    constexpr SoundPool pool{kIds};
    CHECK(pool.size() == 3);
    CHECK(pool.at(0) == id(7));
    CHECK(pool.at(1) == id(42));
    CHECK(pool.at(2) == id(3));
}

TEST_CASE("SoundPool at wraps out of range indices", "[SoundPool]") {
    constexpr SoundPool pool{kIds};
    CHECK(pool.at(3) == id(7));
    CHECK(pool.at(-1) == id(3));
}

TEST_CASE("empty SoundPool yields invalid", "[SoundPool]") {
    SoundPool pool{std::span<const audioID>{}};
    CHECK(pool.size() == 0);
    CHECK(pool.at(0) == audioID::invalid);
    CHECK(pool.pick() == audioID::invalid);
}

TEST_CASE("SoundPool pick only returns members", "[SoundPool]") {
    constexpr SoundPool pool{kAcks};
    for (int i = 0; i < 200; i++) {
        audioID got = pool.pick();
        CHECK((got == id(26) || got == id(27) || got == id(28)));
    }
}

TEST_CASE("lose music pool holds only the three lose tracks", "[SoundPool]") {
    constexpr SoundPool pool{kLose};
    CHECK(pool.size() == 3);
    for (int i = 0; i < 200; i++) {
        audioID got = pool.pick();
        CHECK((toPackIndex(got) >= 15 && toPackIndex(got) <= 17));
    }
}

TEST_CASE("resolveSoundIds fills ids from names via the lookup", "[SoundPool]") {
    constexpr std::array<std::string_view, 3> names = {"A", "B", "C"};
    std::array<audioID, 3> ids{};
    auto lookup = [](std::string_view n) { return n == "A" ? id(5) : n == "B" ? id(9) : audioID::invalid; };
    CHECK(resolveSoundIds(names, ids, lookup) == 1);
    CHECK(ids[0] == id(5));
    CHECK(ids[1] == id(9));
    CHECK(ids[2] == audioID::invalid);
}

TEST_CASE("a SoundPool over resolved ids picks the resolved values", "[SoundPool]") {
    static std::array<audioID, 2> ids{};
    constexpr std::array<std::string_view, 2> names = {"X", "Y"};
    resolveSoundIds(names, ids, [](std::string_view n) { return n == "X" ? id(11) : id(12); });
    SoundPool pool{ids};
    CHECK(pool.at(0) == id(11));
    CHECK(pool.at(1) == id(12));
}
