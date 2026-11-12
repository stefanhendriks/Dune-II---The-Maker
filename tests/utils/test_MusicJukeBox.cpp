#include <catch2/catch_test_macros.hpp>

#include "context/MusicJukeBox.hpp"

#include <fstream>

namespace fs = std::filesystem;

namespace {
struct TempRepertory {
    fs::path root = fs::temp_directory_path() / "d2tm_jukebox_test";

    TempRepertory()
    {
        fs::remove_all(root);
        fs::create_directories(root);
    }
    ~TempRepertory()
    {
        std::error_code ec;
        fs::remove_all(root, ec);
    }
    void touch(const std::string &relative) const
    {
        fs::create_directories((root / relative).parent_path());
        std::ofstream(root / relative) << "x";
    }
};
} // namespace

TEST_CASE("MusicJukeBox without opt-in always returns the archive track", "[musicjukebox]")
{
    TempRepertory dir;
    dir.touch("peace/a.ogg");
    MusicJukeBox jukeBox(dir.root);

    MusicChoice choice = jukeBox.choose(eMusicType::PEACE, toAudioID(7), 0);

    REQUIRE(choice.archiveTrack == toAudioID(7));
    REQUIRE_FALSE(choice.file.has_value());
}

TEST_CASE("MusicJukeBox picks a file from the directory of the music type", "[musicjukebox]")
{
    TempRepertory dir;
    dir.touch("peace/a.ogg");
    dir.touch("attack/b.MP3");
    MusicJukeBox jukeBox(dir.root);
    jukeBox.setCustomMusicEnabled(true);

    REQUIRE(jukeBox.choose(eMusicType::PEACE, toAudioID(1), 0).file == dir.root / "peace" / "a.ogg");
    REQUIRE(jukeBox.choose(eMusicType::ATTACK, toAudioID(1), 0).file == dir.root / "attack" / "b.MP3");
}

TEST_CASE("MusicJukeBox falls back to the archive track when there is nothing to pick", "[musicjukebox]")
{
    TempRepertory dir;
    dir.touch("peace/readme.txt");
    MusicJukeBox jukeBox(dir.root);
    jukeBox.setCustomMusicEnabled(true);

    REQUIRE_FALSE(jukeBox.choose(eMusicType::PEACE, toAudioID(1), 0).file.has_value()); // no audio files
    REQUIRE_FALSE(jukeBox.choose(eMusicType::ATTACK, toAudioID(1), 0).file.has_value()); // missing directory
}

TEST_CASE("MusicJukeBox picks a file for every music type without a house", "[musicjukebox]")
{
    TempRepertory dir;
    dir.touch("win/a.ogg");
    dir.touch("lose/a.ogg");
    dir.touch("menu/a.ogg");
    dir.touch("conquest/a.ogg");
    MusicJukeBox jukeBox(dir.root);
    jukeBox.setCustomMusicEnabled(true);

    REQUIRE(jukeBox.choose(eMusicType::WIN, toAudioID(1), 0).file == dir.root / "win" / "a.ogg");
    REQUIRE(jukeBox.choose(eMusicType::LOSE, toAudioID(1), 0).file == dir.root / "lose" / "a.ogg");
    REQUIRE(jukeBox.choose(eMusicType::MENU, toAudioID(1), 0).file == dir.root / "menu" / "a.ogg");
    REQUIRE(jukeBox.choose(eMusicType::CONQUEST, toAudioID(1), 0).file == dir.root / "conquest" / "a.ogg");
}

TEST_CASE("MusicJukeBox picks briefing music from the folder of the house", "[musicjukebox]")
{
    TempRepertory dir;
    dir.touch("briefing/atreides/a.ogg");
    dir.touch("briefing/sardaukar/s.ogg");
    MusicJukeBox jukeBox(dir.root);
    jukeBox.setCustomMusicEnabled(true);

    REQUIRE(jukeBox.choose(eMusicType::BRIEFING, toAudioID(1), ATREIDES).file == dir.root / "briefing/atreides" / "a.ogg");
    REQUIRE(jukeBox.choose(eMusicType::BRIEFING, toAudioID(1), SARDAUKAR).file == dir.root / "briefing/sardaukar" / "s.ogg");
    REQUIRE_FALSE(jukeBox.choose(eMusicType::BRIEFING, toAudioID(1), ORDOS).file.has_value()); // empty/missing folder
    REQUIRE_FALSE(jukeBox.choose(eMusicType::BRIEFING, toAudioID(1), FREMEN).file.has_value()); // no folder for this house
    REQUIRE_FALSE(jukeBox.choose(eMusicType::BRIEFING, toAudioID(1), GENERALHOUSE).file.has_value());
}

TEST_CASE("MusicJukeBox only looks in the folder of the requested music type", "[musicjukebox]")
{
    TempRepertory dir;
    dir.touch("peace/a.ogg");
    MusicJukeBox jukeBox(dir.root);
    jukeBox.setCustomMusicEnabled(true);

    REQUIRE_FALSE(jukeBox.choose(eMusicType::MENU, toAudioID(1), 0).file.has_value());
    REQUIRE_FALSE(jukeBox.choose(eMusicType::WIN, toAudioID(1), 0).file.has_value());
}

TEST_CASE("MusicJukeBox creates the directory for every music type", "[musicjukebox]")
{
    TempRepertory dir;
    MusicJukeBox jukeBox(dir.root);

    jukeBox.createRepertoryDirectories();

    for (const char *sub : {"win", "lose", "attack", "peace", "menu", "conquest",
                            "briefing/atreides", "briefing/harkonnen", "briefing/ordos", "briefing/sardaukar"}) {
        REQUIRE(fs::is_directory(dir.root / sub));
    }
}
