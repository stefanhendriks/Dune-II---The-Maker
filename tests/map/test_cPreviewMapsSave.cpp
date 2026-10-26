#include <catch2/catch_test_macros.hpp>
#include "data/gfxdata.h"
#include "drawers/SDLDrawer.hpp"
#include "gameobjects/map/cPreviewMaps.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace {

// saveSkirmishMap writes relative to the working directory ("skirmish/") and reloads the saved map
// (which needs a renderer), so run in a scratch dir with a software renderer that draws to a surface
class SaveFixture {
public:
    SaveFixture()
        : m_previous(fs::current_path()),
          m_dir(fs::temp_directory_path() / "d2tm_test_save_skirmish_map")
    {
        fs::remove_all(m_dir);
        fs::create_directories(m_dir / "skirmish");
        fs::current_path(m_dir);

        m_surface = SDL_CreateSurface(8, 8, SDL_PIXELFORMAT_RGBA32);
        m_renderer = SDL_CreateSoftwareRenderer(m_surface);
        m_drawer = std::make_unique<SDLDrawer>(m_renderer);
        previewMaps.setSDLDrawer(m_drawer.get());
    }

    ~SaveFixture()
    {
        previewMaps.destroy();
        m_drawer.reset();
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroySurface(m_surface);
        fs::current_path(m_previous);
        fs::remove_all(m_dir);
    }

    cPreviewMaps previewMaps;

private:
    fs::path m_previous;
    fs::path m_dir;
    SDL_Surface *m_surface = nullptr;
    SDL_Renderer *m_renderer = nullptr;
    std::unique_ptr<SDLDrawer> m_drawer;
};

s_PreviewMap makeMap(const std::string &name)
{
    s_PreviewMap map;
    map.width = 4;
    map.height = 4;
    map.terrainType.assign(16, TERRAIN_SAND);
    map.terrainType[5] = TERRAIN_ROCK;
    map.terrainType[6] = TERRAIN_SPICE;
    map.terrainType[9] = TERRAIN_MOUNTAIN;
    map.terrainType[10] = TERRAIN_HILL;
    map.iStartCell.fill(-1);
    map.name = name;
    map.author = "tester";
    map.description = "a test map";
    return map;
}

std::string readFile(const fs::path &path)
{
    std::ifstream in(path);
    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

} // namespace

TEST_CASE("cPreviewMaps - saveSkirmishMap normalizes the file name", "[map][save]")
{
    SaveFixture fixture;
    cPreviewMaps &previewMaps = fixture.previewMaps;

    SECTION("spaces become underscores and other characters are dropped") {
        REQUIRE(previewMaps.saveSkirmishMap(makeMap("My Map! #1")));
        REQUIRE(fs::exists("skirmish/My_Map_1.ini"));
    }

    SECTION("a name without usable characters falls back to custom_map") {
        REQUIRE(previewMaps.saveSkirmishMap(makeMap("!?*")));
        REQUIRE(fs::exists("skirmish/custom_map.ini"));
    }

    SECTION("a backup is always saved as backup.ini") {
        REQUIRE(previewMaps.saveSkirmishMap(makeMap("Some Name"), true));
        REQUIRE(fs::exists("skirmish/backup.ini"));
        REQUIRE_FALSE(fs::exists("skirmish/Some_Name.ini"));
    }
}

TEST_CASE("cPreviewMaps - saveSkirmishMap writes the inner map cells", "[map][save]")
{
    SaveFixture fixture;
    cPreviewMaps &previewMaps = fixture.previewMaps;

    REQUIRE(previewMaps.saveSkirmishMap(makeMap("content")));

    const std::string content = readFile("skirmish/content.ini");
    REQUIRE(content.find("Title = content") != std::string::npos);
    REQUIRE(content.find("Width = 2\nHeight = 2\n") != std::string::npos);
    // the border row/column is not saved, only the 2x2 inner cells
    REQUIRE(content.find("[MAP]\n^-\nRh\n") != std::string::npos);
}

TEST_CASE("cPreviewMaps - saveSkirmishMap fails on incomplete data", "[map][save]")
{
    SaveFixture fixture;
    cPreviewMaps &previewMaps = fixture.previewMaps;

    SECTION("too few terrain cells") {
        s_PreviewMap map = makeMap("incomplete");
        map.terrainType.pop_back();
        REQUIRE_FALSE(previewMaps.saveSkirmishMap(map));
        REQUIRE_FALSE(fs::exists("skirmish/incomplete.ini"));
    }

    SECTION("map smaller than 3x3") {
        s_PreviewMap map = makeMap("tiny");
        map.width = 2;
        map.height = 2;
        map.terrainType.resize(4);
        REQUIRE_FALSE(previewMaps.saveSkirmishMap(map));
        REQUIRE_FALSE(fs::exists("skirmish/tiny.ini"));
    }
}

TEST_CASE("cPreviewMaps - saveSkirmishMap fails on unknown terrain", "[map][save]")
{
    SaveFixture fixture;
    cPreviewMaps &previewMaps = fixture.previewMaps;

    REQUIRE(previewMaps.saveSkirmishMap(makeMap("unknown")));
    const std::string before = readFile("skirmish/unknown.ini");

    s_PreviewMap broken = makeMap("unknown");
    broken.terrainType[5] = -1; // inside the saved area
    REQUIRE_FALSE(previewMaps.saveSkirmishMap(broken));

    // the earlier save is left untouched
    REQUIRE(readFile("skirmish/unknown.ini") == before);
}
