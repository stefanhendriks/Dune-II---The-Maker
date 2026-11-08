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

    MusicChoice choice = jukeBox.choose(eMusicType::PEACE, 7);

    REQUIRE(choice.archiveTrack == 7);
    REQUIRE_FALSE(choice.file.has_value());
}

TEST_CASE("MusicJukeBox picks a file from the directory of the music type", "[musicjukebox]")
{
    TempRepertory dir;
    dir.touch("peace/a.ogg");
    dir.touch("attack/b.MP3");
    MusicJukeBox jukeBox(dir.root);
    jukeBox.setCustomMusicEnabled(true);

    REQUIRE(jukeBox.choose(eMusicType::PEACE, 1).file == dir.root / "peace" / "a.ogg");
    REQUIRE(jukeBox.choose(eMusicType::ATTACK, 1).file == dir.root / "attack" / "b.MP3");
}

TEST_CASE("MusicJukeBox falls back to the archive track when there is nothing to pick", "[musicjukebox]")
{
    TempRepertory dir;
    dir.touch("peace/readme.txt");
    MusicJukeBox jukeBox(dir.root);
    jukeBox.setCustomMusicEnabled(true);

    REQUIRE_FALSE(jukeBox.choose(eMusicType::PEACE, 1).file.has_value()); // no audio files
    REQUIRE_FALSE(jukeBox.choose(eMusicType::ATTACK, 1).file.has_value()); // missing directory
}

TEST_CASE("MusicJukeBox ignores the repertory for other music types", "[musicjukebox]")
{
    TempRepertory dir;
    dir.touch("peace/a.ogg");
    MusicJukeBox jukeBox(dir.root);
    jukeBox.setCustomMusicEnabled(true);

    REQUIRE_FALSE(jukeBox.choose(eMusicType::MENU, 1).file.has_value());
    REQUIRE_FALSE(jukeBox.choose(eMusicType::WIN, 1).file.has_value());
}
