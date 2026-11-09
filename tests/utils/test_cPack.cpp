/**
 * @file test_cPack.cpp
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
#include "utils/cPack.h"
#include "TempDir.hpp"

#include <string>

namespace {
std::string readAll(SDL_IOStream *io)
{
    std::string result(static_cast<size_t>(SDL_GetIOSize(io)), '\0');
    SDL_ReadIO(io, result.data(), result.size());
    SDL_CloseIO(io);
    return result;
}
}

TEST_CASE("WriterPack - addFile fails for a file that does not exist", "[pack]")
{
    TempDir dir("d2tm_pack_addfile");
    WriterPack writer(dir.file("test.pak"));
    REQUIRE_FALSE(writer.addFile(dir.file("missing.txt"), "missing"));
}

TEST_CASE("WriterPack - addFile accepts an existing file", "[pack]")
{
    TempDir dir("d2tm_pack_addfile_ok");
    const std::string source = dir.write("a.txt", "hello");
    WriterPack writer(dir.file("test.pak"));
    REQUIRE(writer.addFile(source, "a"));
}

TEST_CASE("ReaderPack - files written by WriterPack can be read back by name and index", "[pack]")
{
    TempDir dir("d2tm_pack_roundtrip");
    const std::string contentA = "first file contents";
    const std::string contentB = "second";
    const std::string contentC = "the third file is a bit longer than the others";
    const std::string packName = dir.file("test.pak");

    {
        WriterPack writer(packName);
        REQUIRE(writer.addFile(dir.write("a.txt", contentA), "file_a"));
        REQUIRE(writer.addFile(dir.write("b.txt", contentB), "file_b"));
        REQUIRE(writer.addFile(dir.write("c.txt", contentC), "file_c"));
        REQUIRE(writer.writePackFilesOnDisk());
    }

    ReaderPack reader(packName);
    REQUIRE(reader.getNumberOfFiles() == 3);

    REQUIRE(reader.getIndexFromName("file_a") == 0);
    REQUIRE(reader.getIndexFromName("file_b") == 1);
    REQUIRE(reader.getIndexFromName("file_c") == 2);

    REQUIRE(readAll(reader.getData(0)) == contentA);
    REQUIRE(readAll(reader.getData(1)) == contentB);
    REQUIRE(readAll(reader.getData(2)) == contentC);
}

TEST_CASE("ReaderPack - unknown name returns -1 and invalid index returns nullptr", "[pack]")
{
    TempDir dir("d2tm_pack_unknown");
    const std::string packName = dir.file("test.pak");
    {
        WriterPack writer(packName);
        REQUIRE(writer.addFile(dir.write("a.txt", "abc"), "file_a"));
        REQUIRE(writer.writePackFilesOnDisk());
    }

    ReaderPack reader(packName);
    REQUIRE(reader.getIndexFromName("nope") == -1);
    REQUIRE(reader.getData(1) == nullptr);
    REQUIRE(reader.getData(100) == nullptr);
}

TEST_CASE("ReaderPack - a file without the D2TM title is rejected", "[pack]")
{
    TempDir dir("d2tm_pack_invalid");
    const std::string bogus = dir.write("bogus.pak", "this is not a pack file at all");

    ReaderPack reader(bogus);
    REQUIRE(reader.getNumberOfFiles() == 0);
    REQUIRE(reader.getIndexFromName("anything") == -1);
}

TEST_CASE("ReaderPack - a missing file results in an empty pack", "[pack]")
{
    TempDir dir("d2tm_pack_missing");
    ReaderPack reader(dir.file("does_not_exist.pak"));
    REQUIRE(reader.getNumberOfFiles() == 0);
}
