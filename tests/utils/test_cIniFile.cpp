#include <catch2/catch_test_macros.hpp>
#include "utils/cIniFile.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
class TemporaryDirectory {
public:
    TemporaryDirectory()
    {
        const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
        m_path = std::filesystem::temp_directory_path() / ("d2tm_test_cIniFile_" + std::to_string(timestamp));
        std::filesystem::create_directories(m_path);
    }

    ~TemporaryDirectory()
    {
        std::error_code error;
        std::filesystem::remove_all(m_path, error);
    }

    const std::filesystem::path& path() const
    {
        return m_path;
    }

private:
    std::filesystem::path m_path;
};

void writeFile(const std::filesystem::path& path, const std::string& contents)
{
    std::ofstream output(path);
    REQUIRE(output.good());
    output << contents;
    REQUIRE(output.good());
}

std::string readFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    REQUIRE(input.good());
    return { std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>() };
}
}

TEST_CASE("cIniFile loads sections, values, comments, and raw data", "[ini]")
{
    TemporaryDirectory directory;
    const auto file = directory.path() / "config.ini";
    writeFile(file,
              "; full-line comment\n"
              "# another comment\n"
              "\n"
              "[GENERAL]\n"
              "  title = \"Arrakis\"  \n"
              "count = -42\n"
              "ratio = 2.75\n"
              "enabled = TrUe\n"
              "legacy_enabled = ON\n"
              "numeric_enabled = 1\n"
              "disabled = off\n"
              "\n"
              "unparsed line\n"
              "bad=\n"
              "=bad\n"
              "[SECOND]\n"
              "name=Caladan\n");

    cIniFile ini(file.string());

    REQUIRE(ini.isLoadSuccess());
    REQUIRE(ini.hasSection("GENERAL"));
    REQUIRE(ini.hasSection("SECOND"));
    REQUIRE(ini.getStringValue("GENERAL", "title") == "Arrakis");
    REQUIRE(ini.getSection("GENERAL").getInt("count") == -42);
    REQUIRE(ini.getSection("GENERAL").getDouble("ratio") == 2.75);
    REQUIRE(ini.getSection("GENERAL").getBoolean("enabled"));
    REQUIRE(ini.getSection("GENERAL").getBoolean("legacy_enabled"));
    REQUIRE(ini.getSection("GENERAL").getBoolean("numeric_enabled"));
    REQUIRE_FALSE(ini.getSection("GENERAL").getBoolean("disabled"));
    REQUIRE(ini.getStringValue("SECOND", "name") == "Caladan");
    REQUIRE((ini.getSection("GENERAL").getData() == std::vector<std::string>{ "unparsed line", "bad=", "=bad" }));
}

TEST_CASE("cIniFile returns empty and zero defaults for missing values", "[ini]")
{
    TemporaryDirectory directory;
    const auto file = directory.path() / "config.ini";
    writeFile(file, "[SETTINGS]\npresent=value\n");

    cIniFile ini(file.string());
    const auto section = ini.getSection("SETTINGS");

    REQUIRE(ini.getStringValue("SETTINGS", "missing").empty());
    REQUIRE(ini.getStringValue("missing", "key").empty());
    REQUIRE(section.getInt("missing") == 0);
    REQUIRE(section.getDouble("missing") == 0.0);
    REQUIRE_FALSE(section.getBoolean("missing"));
    REQUIRE_FALSE(section.hasValue("missing"));
    REQUIRE_THROWS_AS(ini.getSection("missing"), std::invalid_argument);
}

TEST_CASE("cIniFile setValue creates sections and replaces existing values", "[ini]")
{
    TemporaryDirectory directory;
    const auto file = directory.path() / "saved.ini";
    cIniFile ini;

    ini.setValue("FIRST", "zeta", "last");
    ini.setValue("FIRST", "alpha", "initial");
    ini.setValue("FIRST", "alpha", "updated");
    ini.setValue("SECOND", "name", "Caladan");

    REQUIRE(ini.hasSection("FIRST"));
    REQUIRE(ini.hasSection("SECOND"));
    REQUIRE(ini.getStringValue("FIRST", "alpha") == "updated");
    REQUIRE(ini.getStringValue("FIRST", "zeta") == "last");
    REQUIRE(ini.save(file.string()));
    REQUIRE(readFile(file) ==
            "[FIRST]\n"
            "alpha=updated\n"
            "zeta=last\n"
            "\n"
            "[SECOND]\n"
            "name=Caladan\n");

    cIniFile loaded(file.string());
    REQUIRE(loaded.isLoadSuccess());
    REQUIRE(loaded.getStringValue("FIRST", "alpha") == "updated");
    REQUIRE(loaded.getStringValue("SECOND", "name") == "Caladan");
}

TEST_CASE("cIniFile saves and loads using a UTF-8 filename", "[ini]")
{
    TemporaryDirectory directory;
    const std::filesystem::path file = directory.path() / u8"4PL_SinéadPlains.ini";
    writeFile(file, "[SKIRMISH]\nTitle=Sineads Plains\n");

    const std::u8string utf8Name = file.u8string();
    const std::string fileName(utf8Name.begin(), utf8Name.end());
    cIniFile ini(fileName);

    REQUIRE(ini.isLoadSuccess());
    REQUIRE(ini.getStringValue("SKIRMISH", "Title") == "Sineads Plains");
    ini.setValue("SKIRMISH", "Title", "Sinéad Plains");
    REQUIRE(ini.save(fileName));

    cIniFile reloaded(fileName);
    REQUIRE(reloaded.isLoadSuccess());
    REQUIRE(reloaded.getStringValue("SKIRMISH", "Title") == "Sinéad Plains");
}

TEST_CASE("cIniFile reports failures to open files for loading or saving", "[ini]")
{
    TemporaryDirectory directory;
    const auto missingFile = directory.path() / "missing.ini";
    const auto unavailableFile = directory.path() / "missing-directory" / "saved.ini";

    cIniFile ini(missingFile.string());

    REQUIRE_FALSE(ini.isLoadSuccess());
    REQUIRE_FALSE(ini.save(unavailableFile.string()));
}
