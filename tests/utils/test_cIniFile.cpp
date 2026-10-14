#include <catch2/catch_test_macros.hpp>
#include "utils/cIniFile.h"

#include <filesystem>
#include <fstream>
#include <string>

TEST_CASE("cIniFile - loads a file whose UTF-8 name has non-ASCII characters", "[ini]")
{
    const std::filesystem::path dir = std::filesystem::temp_directory_path() / "d2tm_test_cIniFile";
    std::filesystem::create_directories(dir);
    const std::filesystem::path file = dir / u8"4PL_SinéadPlains.ini";
    {
        std::ofstream out(file);
        out << "[SKIRMISH]\nTitle=Sineads Plains\n";
    }

    const std::u8string utf8Name = file.u8string();
    cIniFile ini(std::string(utf8Name.begin(), utf8Name.end()));

    REQUIRE(ini.isLoadSuccess());
    REQUIRE(ini.getSection("SKIRMISH").getStringValue("Title") == "Sineads Plains");

    std::filesystem::remove_all(dir);
}
