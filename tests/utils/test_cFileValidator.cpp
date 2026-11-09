/**
 * @file test_cFileValidator.cpp
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
#include "utils/cFileValidator.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace {
// Creates a fresh, empty directory for the lifetime of the object
struct TempDir {
    fs::path path;

    explicit TempDir(const std::string &name)
        : path(fs::temp_directory_path() / name)
    {
        fs::remove_all(path);
        fs::create_directories(path);
    }

    ~TempDir()
    {
        std::error_code ec;
        fs::remove_all(path, ec);
    }

    void touch(const std::string &file) const
    {
        std::ofstream(path / file) << "x";
    }
};
}

TEST_CASE("eGameDirFileNameString - maps every value to its name", "[filevalidator]")
{
    REQUIRE(eGameDirFileNameString(eGameDirFileName::ARRAKEEN) == "ARRAKEEN");
    REQUIRE(eGameDirFileNameString(eGameDirFileName::BENEGESS) == "BENEGESS");
    REQUIRE(eGameDirFileNameString(eGameDirFileName::SMALL) == "SMALL");
    REQUIRE(eGameDirFileNameString(eGameDirFileName::GFXDATA) == "GFXDATA");
    REQUIRE(eGameDirFileNameString(eGameDirFileName::GFXINTER) == "GFXINTER");
    REQUIRE(eGameDirFileNameString(eGameDirFileName::GFXWORLD) == "GFXWORLD");
    REQUIRE(eGameDirFileNameString(eGameDirFileName::GFXMENTAT) == "GFXMENTAT");
    REQUIRE(eGameDirFileNameString(eGameDirFileName::GFXAUDIO) == "GFXAUDIO");
    REQUIRE(eGameDirFileNameString(eGameDirFileName::GFXEDITOR) == "GFXEDITOR");
}

TEST_CASE("cFileValidator - getName and getFullName use the configured resources", "[filevalidator]")
{
    cFileValidator validator("some/dir");
    validator.addResources({{eGameDirFileName::GFXDATA, "gfxdata.dat"}});

    REQUIRE(validator.getName(eGameDirFileName::GFXDATA) == "gfxdata.dat");
    REQUIRE(validator.getFullName(eGameDirFileName::GFXDATA) == "some/dir/gfxdata.dat");
}

TEST_CASE("cFileValidator - fileExists is false when the directory is missing", "[filevalidator]")
{
    cFileValidator validator("this/directory/does/not/exist");
    REQUIRE_FALSE(validator.fileExists());
}

TEST_CASE("cFileValidator - fileExists is true when all configured files exist", "[filevalidator]")
{
    TempDir dir("d2tm_filevalidator_ok");
    dir.touch("gfxdata.dat");
    dir.touch("gfxaudio.dat");

    cFileValidator validator(dir.path.string());
    validator.addResources({
        {eGameDirFileName::GFXDATA, "gfxdata.dat"},
        {eGameDirFileName::GFXAUDIO, "gfxaudio.dat"},
    });
    REQUIRE(validator.fileExists());
}

TEST_CASE("cFileValidator - fileExists is true for an existing directory without resources", "[filevalidator]")
{
    TempDir dir("d2tm_filevalidator_empty");
    cFileValidator validator(dir.path.string());
    REQUIRE(validator.fileExists());
}

TEST_CASE("cFileValidator - fileExists is false when a configured file is missing", "[filevalidator]")
{
    TempDir dir("d2tm_filevalidator_missing");
    dir.touch("gfxdata.dat");

    cFileValidator validator(dir.path.string());
    validator.addResources({
        {eGameDirFileName::GFXDATA, "gfxdata.dat"},
        {eGameDirFileName::GFXAUDIO, "gfxaudio.dat"},
    });
    REQUIRE_FALSE(validator.fileExists());
}

TEST_CASE("cFileValidator - fileExists is false when a configured filename is empty", "[filevalidator]")
{
    TempDir dir("d2tm_filevalidator_emptyname");
    cFileValidator validator(dir.path.string());
    validator.addResources({{eGameDirFileName::GFXDATA, ""}});
    REQUIRE_FALSE(validator.fileExists());
}
