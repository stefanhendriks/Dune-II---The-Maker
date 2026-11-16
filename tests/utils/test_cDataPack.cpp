/**
 * @file test_cDataPack.cpp
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
#include "utils/cDataPack.hpp"
#include "utils/cPack.h"
#include "TempDir.hpp"

#include <string>
#include <utility>
#include <vector>

namespace {
// Writes a pack with the given (file path, entry name) pairs.
void writePack(const std::string &packName, const std::vector<std::pair<std::string, std::string>> &entries)
{
    WriterPack writer(packName);
    for (const auto &[path, name] : entries) {
        REQUIRE(writer.addFile(path, name));
    }
    REQUIRE(writer.writePackFilesOnDisk());
}

// Writes a tiny valid BMP image to disk, returns its path.
std::string writeBmp(const TempDir &dir, const std::string &name, int width, int height)
{
    SDL_Surface *surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
    REQUIRE(surface != nullptr);
    const std::string path = dir.file(name);
    REQUIRE(SDL_SaveBMP(surface, path.c_str()));
    SDL_DestroySurface(surface);
    return path;
}
}

TEST_CASE("DataPack - reports the number of files and names in index order", "[datapack]")
{
    TempDir dir("d2tm_datapack_names");
    const std::string packName = dir.file("test.pak");
    writePack(packName, {{dir.write("a.txt"), "first"}, {dir.write("b.txt"), "second"}, {dir.write("c.txt"), "third"}});

    DataPack pack(packName, nullptr);
    REQUIRE(pack.getNumberOfFile() == 3);
    REQUIRE(pack.getNames() == std::vector<std::string>{"FIRST", "SECOND", "THIRD"});
}

TEST_CASE("DataPack - getIndexFromName finds existing entries and returns -1 for missing ones", "[datapack]")
{
    TempDir dir("d2tm_datapack_index");
    const std::string packName = dir.file("test.pak");
    writePack(packName, {{dir.write("a.txt"), "alpha"}, {dir.write("b.txt"), "beta"}});

    DataPack pack(packName, nullptr);
    REQUIRE(pack.getIndexFromName("alpha") == 0);
    REQUIRE(pack.getIndexFromName("beta") == 1);
    REQUIRE(pack.getIndexFromName("gamma") == -1);
    REQUIRE(pack.getIndexFromName("") == -1);
    REQUIRE(pack.getIndexFromName("alph") == -1);
    REQUIRE(pack.getIndexFromName("alpha ") == -1);
}

TEST_CASE("DataPack - name lookup is case-insensitive", "[datapack]")
{
    TempDir dir("d2tm_datapack_case");
    const std::string packName = dir.file("test.pak");
    writePack(packName, {{dir.write("a.txt"), "MiXeD_Name"}});

    DataPack pack(packName, nullptr);
    REQUIRE(pack.getIndexFromName("mixed_name") == 0);
    REQUIRE(pack.getIndexFromName("MIXED_NAME") == 0);
    REQUIRE(pack.getIndexFromName("MiXeD_Name") == 0);
}

TEST_CASE("DataPack - duplicate names resolve to the first entry but both stay listed", "[datapack]")
{
    TempDir dir("d2tm_datapack_duplicate");
    const std::string packName = dir.file("test.pak");
    writePack(packName, {{dir.write("a.txt", "one"), "same"}, {dir.write("b.txt", "two"), "same"}, {dir.write("c.txt", "three"), "other"}});

    DataPack pack(packName, nullptr);
    REQUIRE(pack.getNumberOfFile() == 3);
    REQUIRE(pack.getIndexFromName("same") == 0);
    REQUIRE(pack.getIndexFromName("other") == 2);
    REQUIRE(pack.getText("same") == "one");
    REQUIRE(pack.getNames() == std::vector<std::string>{"SAME", "SAME", "OTHER"});
}

TEST_CASE("DataPack - getText returns the contents of text entries only", "[datapack]")
{
    TempDir dir("d2tm_datapack_text");
    const std::string packName = dir.file("test.pak");
    const std::string utf8 = "caf\xC3\xA9 \xE2\x9C\x93";
    writePack(packName, {{dir.write("a.txt", "hello world"), "greeting"},
        {dir.write("b.txt", utf8), "utf8"},
        {dir.write("c.txt", ""), "empty"},
        // extension .dat: only entries with extension txt count as text
        {dir.write("d.dat", "binary"), "notext"}});

    DataPack pack(packName, nullptr);
    REQUIRE(pack.getText("greeting") == "hello world");
    REQUIRE(pack.getText("GREETING") == "hello world");
    REQUIRE(pack.getText("utf8") == utf8);
    REQUIRE(pack.getText("missing") == std::nullopt);
    REQUIRE(pack.getText("notext") == std::nullopt);
}

TEST_CASE("DataPack - text with line breaks and NUL bytes is kept intact", "[datapack]")
{
    TempDir dir("d2tm_datapack_text_raw");
    const std::string packName = dir.file("test.pak");
    const std::string raw("line1\r\nline2\n\0tail", 18);
    writePack(packName, {{dir.write("a.txt", raw), "raw"}});

    DataPack pack(packName, nullptr);
    const auto text = pack.getText("raw");
    REQUIRE(text.has_value());
    REQUIRE(text->size() == raw.size());
    REQUIRE(*text == raw);
}

TEST_CASE("DataPack - entry name of 39 characters works, 40 is rejected by the writer", "[datapack]")
{
    TempDir dir("d2tm_datapack_namelen");
    const std::string packName = dir.file("test.pak");
    const std::string longest(39, 'x');
    {
        WriterPack writer(packName);
        REQUIRE(writer.addFile(dir.write("a.txt", "boundary"), longest));
        REQUIRE_FALSE(writer.addFile(dir.write("b.txt", "too long"), std::string(40, 'y')));
        REQUIRE(writer.writePackFilesOnDisk());
    }

    DataPack pack(packName, nullptr);
    REQUIRE(pack.getNumberOfFile() == 1);
    REQUIRE(pack.getText(longest) == "boundary");
}

TEST_CASE("DataPack - pack with zero entries is valid and empty", "[datapack]")
{
    TempDir dir("d2tm_datapack_zero");
    const std::string packName = dir.file("empty.pak");
    {
        WriterPack writer(packName);
        REQUIRE(writer.writePackFilesOnDisk());
    }

    DataPack pack(packName, nullptr);
    REQUIRE(pack.getNumberOfFile() == 0);
    REQUIRE(pack.getNames().empty());
    REQUIRE(pack.getIndexFromName("anything") == -1);
    REQUIRE(pack.getText("anything") == std::nullopt);
}

TEST_CASE("DataPack - a pack with header still reads its entries", "[datapack]")
{
    TempDir dir("d2tm_datapack_header");
    const std::string packName = dir.file("test.pak");
    {
        WriterPack writer(packName);
        REQUIRE(writer.setHeader("someone", 7));
        REQUIRE(writer.addFile(dir.write("a.txt", "with header"), "entry"));
        REQUIRE(writer.writePackFilesOnDisk());
    }

    DataPack pack(packName, nullptr);
    REQUIRE(pack.getNumberOfFile() == 1);
    REQUIRE(pack.getText("entry") == "with header");
}

TEST_CASE("DataPack - missing, empty and malformed pack files give an empty pack", "[datapack]")
{
    TempDir dir("d2tm_datapack_malformed");

    SECTION("file does not exist") {
        DataPack pack(dir.file("nope.pak"), nullptr);
        REQUIRE(pack.getNumberOfFile() == 0);
        REQUIRE(pack.getNames().empty());
        REQUIRE(pack.getIndexFromName("x") == -1);
    }
    SECTION("zero byte file") {
        DataPack pack(dir.write("empty.pak", ""), nullptr);
        REQUIRE(pack.getNumberOfFile() == 0);
        REQUIRE(pack.getText("x") == std::nullopt);
    }
    SECTION("wrong magic") {
        DataPack pack(dir.write("bad.pak", std::string("XXXX\x01\x00", 6)), nullptr);
        REQUIRE(pack.getNumberOfFile() == 0);
    }
    SECTION("file shorter than the magic") {
        DataPack pack(dir.write("short.pak", "D2T"), nullptr);
        REQUIRE(pack.getNumberOfFile() == 0);
    }
    SECTION("magic without a file count") {
        DataPack pack(dir.write("nocount.pak", "D2TM"), nullptr);
        REQUIRE(pack.getNumberOfFile() == 0);
    }
    SECTION("file count larger than the entry table in the file") {
        DataPack pack(dir.write("lying.pak", std::string("D2TM\xFF\xFF", 6)), nullptr);
        REQUIRE(pack.getNumberOfFile() == 0);
        REQUIRE(pack.getNames().empty());
    }
}

TEST_CASE("DataPack - surfaces and audio with an invalid index or unknown name are nullptr", "[datapack]")
{
    TempDir dir("d2tm_datapack_invalid");
    const std::string packName = dir.file("test.pak");
    writePack(packName, {{dir.write("a.txt"), "only"}});

    DataPack pack(packName, nullptr);
    REQUIRE(pack.getSurface(-1) == nullptr);
    REQUIRE(pack.getSurface(1) == nullptr);
    REQUIRE(pack.getSurface(1000) == nullptr);
    REQUIRE(pack.getSurface("missing") == nullptr);
    REQUIRE(pack.getAudio(-1) == nullptr);
    REQUIRE(pack.getAudio(1) == nullptr);
    REQUIRE(pack.getAudio("missing") == nullptr);
}

TEST_CASE("DataPack - getSurface loads an image by name and index and caches it", "[datapack]")
{
    TempDir dir("d2tm_datapack_surface");
    const std::string packName = dir.file("test.pak");
    writePack(packName, {{dir.write("a.txt", "filler"), "filler"}, {writeBmp(dir, "pic.bmp", 5, 3), "picture"}});

    DataPack pack(packName, nullptr);
    SDL_Surface *byName = pack.getSurface("picture");
    REQUIRE(byName != nullptr);
    REQUIRE(byName->w == 5);
    REQUIRE(byName->h == 3);
    REQUIRE(pack.getSurface("PICTURE") == byName);
    REQUIRE(pack.getSurface(pack.getIndexFromName("picture")) == byName);
}

TEST_CASE("DataPack - getSurface of an entry that is not an image gives nullptr", "[datapack]")
{
    TempDir dir("d2tm_datapack_notimage");
    const std::string packName = dir.file("test.pak");
    writePack(packName, {{dir.write("a.txt", "this is not an image"), "text"}});

    DataPack pack(packName, nullptr);
    REQUIRE(pack.getSurface("text") == nullptr);
}
