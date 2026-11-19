/**
 * @file test_GraphicsID.cpp
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
#include "utils/GraphicsID.hpp"
#include "utils/Graphics.hpp"
#include "utils/cPack.h"
#include "TempDir.hpp"

#include <string>

TEST_CASE("GraphicsID - pack index round trips through SurfaceID and TextureID", "[graphics]")
{
    for (const int index : {0, 1, 118, 4000}) {
        REQUIRE(toPackIndex(toSurfaceID(index)) == index);
        REQUIRE(toPackIndex(toTextureID(index)) == index);
    }
}

TEST_CASE("GraphicsID - invalid is the id of pack index -1", "[graphics]")
{
    REQUIRE(toSurfaceID(-1) == SurfaceID::invalid);
    REQUIRE(toTextureID(-1) == TextureID::invalid);
    REQUIRE(toPackIndex(SurfaceID::invalid) == -1);
    REQUIRE(toPackIndex(TextureID::invalid) == -1);
}

TEST_CASE("Graphics - surfaceId and textureId resolve names and give invalid for missing ones", "[graphics]")
{
    TempDir dir("d2tm_graphics_ids");
    const std::string packName = dir.file("test.pak");
    {
        WriterPack writer(packName);
        REQUIRE(writer.addFile(dir.write("a.txt"), "first"));
        REQUIRE(writer.addFile(dir.write("b.txt"), "second"));
        REQUIRE(writer.writePackFilesOnDisk());
    }

    // the constructor only stores the renderer, ids are resolved against the pack alone
    auto *fakeRenderer = reinterpret_cast<SDL_Renderer *>(1);
    Graphics graphics(fakeRenderer, packName);

    REQUIRE(graphics.getSurfaceID("first") == toSurfaceID(0));
    REQUIRE(graphics.getSurfaceID("second") == toSurfaceID(1));
    REQUIRE(graphics.getTextureID("second") == toTextureID(1));
    REQUIRE(graphics.getSurfaceID("missing") == SurfaceID::invalid);
    REQUIRE(graphics.getTextureID("missing") == TextureID::invalid);
}
