/**
 * @file GraphicsID.hpp
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

#pragma once

#include <cstdint>

// Identify an image in a graphics pack, as returned by Graphics::surfaceId() / Graphics::textureId().
// The values are pack indices; `invalid` is the "not found / no image" value (the uint32 wrap of -1).
enum class SurfaceID : std::uint32_t {
    invalid = 0xFFFFFFFFu
};

enum class TextureID : std::uint32_t {
    invalid = 0xFFFFFFFFu
};

constexpr SurfaceID toSurfaceID(int packIndex)
{
    return static_cast<SurfaceID>(static_cast<std::uint32_t>(packIndex));
}

constexpr TextureID toTextureID(int packIndex)
{
    return static_cast<TextureID>(static_cast<std::uint32_t>(packIndex));
}

// the pack index of the surface, or -1 for SurfaceID::invalid
constexpr int toPackIndex(SurfaceID id)
{
    return static_cast<int>(static_cast<std::uint32_t>(id));
}

// the pack index of the texture, or -1 for TextureID::invalid
constexpr int toPackIndex(TextureID id)
{
    return static_cast<int>(static_cast<std::uint32_t>(id));
}
