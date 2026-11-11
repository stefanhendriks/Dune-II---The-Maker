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

#include <iostream>
#include <sstream>
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

TEST_CASE("ReaderPack - a duplicate entry name warns on stderr and name lookup keeps the first", "[pack]")
{
    TempDir dir("d2tm_pack_duplicate");
    const std::string packName = dir.file("dup.pak");
    {
        WriterPack writer(packName);
        REQUIRE(writer.addFile(dir.write("a.txt", "one"), "same_name"));
        REQUIRE(writer.addFile(dir.write("b.txt", "two"), "same_name"));
        REQUIRE(writer.writePackFilesOnDisk());
    }

    std::ostringstream captured;
    std::streambuf *original = std::cerr.rdbuf(captured.rdbuf());
    ReaderPack reader(packName);
    std::cerr.rdbuf(original);

    CHECK(reader.getNumberOfFiles() == 2);
    CHECK(reader.getIndexFromName("same_name") == 0);
    CHECK(captured.str().find("duplicate entry name 'same_name'") != std::string::npos);
}

TEST_CASE("ReaderPack - a pack without header has no author and version 0", "[pack]")
{
    TempDir dir("d2tm_pack_noheader");
    const std::string packName = dir.file("old.pak");
    {
        WriterPack writer(packName);
        REQUIRE(writer.addFile(dir.write("a.txt", "old"), "file_a"));
        REQUIRE(writer.writePackFilesOnDisk());
    }

    ReaderPack reader(packName);
    REQUIRE(reader.getAuthor().empty());
    REQUIRE(reader.getVersion() == 0);
    REQUIRE(reader.getNumberOfFiles() == 1);
    REQUIRE(readAll(reader.getData(0)) == "old");
}

TEST_CASE("ReaderPack - author and version are read from the pack header", "[pack]")
{
    TempDir dir("d2tm_pack_header");
    const std::string packName = dir.file("new.pak");
    {
        WriterPack writer(packName);
        REQUIRE(writer.setHeader("core", 2));
        REQUIRE(writer.addFile(dir.write("a.txt", "first"), "file_a"));
        REQUIRE(writer.addFile(dir.write("b.txt", "second"), "file_b"));
        REQUIRE(writer.writePackFilesOnDisk());
    }

    ReaderPack reader(packName);
    REQUIRE(reader.getAuthor() == "core");
    REQUIRE(reader.getVersion() == 2);
    REQUIRE(reader.getNumberOfFiles() == 2);
    REQUIRE(reader.getIndexFromName("file_b") == 1);
    REQUIRE(readAll(reader.getData(0)) == "first");
    REQUIRE(readAll(reader.getData(1)) == "second");
}

TEST_CASE("ReaderPack - a header that is cut off is rejected", "[pack]")
{
    TempDir dir("d2tm_pack_truncated");
    const std::string truncated = dir.write("cut.pak", "D2TMco");

    ReaderPack reader(truncated);
    REQUIRE(reader.getNumberOfFiles() == 0);
}

TEST_CASE("ReaderPack - an old pack whose first entry name looks like header text is read as old", "[pack]")
{
    TempDir dir("d2tm_pack_lookalike");
    const std::string packName = dir.file("old.pak");
    {
        WriterPack writer(packName);
        REQUIRE(writer.addFile(dir.write("a.txt", "data"), "core"));
        REQUIRE(writer.writePackFilesOnDisk());
    }

    ReaderPack reader(packName);
    REQUIRE(reader.getAuthor().empty());
    REQUIRE(reader.getIndexFromName("core") == 0);
    REQUIRE(readAll(reader.getData(0)) == "data");
}

TEST_CASE("WriterPack - addFile rejects entry names over 39 characters", "[pack]")
{
    TempDir dir("d2tm_pack_longname");
    const std::string source = dir.write("a.txt", "x");
    WriterPack writer(dir.file("test.pak"));
    REQUIRE(writer.addFile(source, std::string(39, 'a')));
    REQUIRE_FALSE(writer.addFile(source, std::string(40, 'a')));
}

TEST_CASE("WriterPack - setHeader rejects an author or version that does not fit", "[pack]")
{
    TempDir dir("d2tm_pack_headerrange");
    WriterPack writer(dir.file("test.pak"));
    REQUIRE_FALSE(writer.setHeader(std::string(15, 'a'), 1));
    REQUIRE_FALSE(writer.setHeader("core", 10000));
    REQUIRE(writer.setHeader(std::string(14, 'a'), 9999));
}

TEST_CASE("ReaderPack - a text entry is read back as UTF-8 text by name", "[pack]")
{
    TempDir dir("d2tm_pack_text");
    const std::string text = "Welcome, Atreides.\nCaf\xC3\xA9 \xE2\x80\x93 line two";
    const std::string packName = dir.file("test.pak");

    {
        WriterPack writer(packName);
        REQUIRE(writer.addFile(dir.write("a.bin", "binary"), "data_a"));
        REQUIRE(writer.addFile(dir.write("intro.txt", text), "intro"));
        REQUIRE(writer.writePackFilesOnDisk());
    }

    ReaderPack reader(packName);
    REQUIRE(reader.getExtension(1) == "TXT");
    REQUIRE(reader.getText("intro") == text);
    // not a text entry, or unknown
    REQUIRE_FALSE(reader.getText("data_a").has_value());
    REQUIRE_FALSE(reader.getText("nope").has_value());
    // the raw bytes are still reachable the usual way
    REQUIRE(readAll(reader.getData(0)) == "binary");
}

TEST_CASE("ReaderPack - a pack without text entries loads unchanged", "[pack]")
{
    TempDir dir("d2tm_pack_notext");
    const std::string packName = dir.file("test.pak");
    {
        WriterPack writer(packName);
        REQUIRE(writer.addFile(dir.write("a.dat", "aaa"), "file_a"));
        REQUIRE(writer.addFile(dir.write("b.dat", "bbbb"), "file_b"));
        REQUIRE(writer.writePackFilesOnDisk());
    }

    ReaderPack reader(packName);
    REQUIRE(reader.getNumberOfFiles() == 2);
    REQUIRE(readAll(reader.getData(1)) == "bbbb");
    REQUIRE_FALSE(reader.getText("file_a").has_value());
    REQUIRE(reader.getExtension(5).empty());
}
