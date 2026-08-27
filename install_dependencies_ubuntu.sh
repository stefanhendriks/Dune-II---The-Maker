#!/bin/bash
set -e

# Install for Ubuntu / Debian
# SDL3 is not in Ubuntu 24.04 standard repos, so we build it from source.

echo  ---------------------------------
echo  ---                           ---
echo  ---  Installing dependencies  ---
echo  ---                           ---
echo  ---------------------------------

sudo apt-get update -qq

# Build tools
sudo apt-get install -y --no-install-recommends \
  g++ cmake ninja-build pkg-config git \
  catch2

# SDL3 platform dependencies
sudo apt-get install -y --no-install-recommends \
  libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxi-dev libxss-dev libxtst-dev \
  libwayland-dev libxkbcommon-dev libegl1-mesa-dev libgl1-mesa-dev \
  libasound2-dev libpulse-dev

# SDL3_image dependencies
sudo apt-get install -y --no-install-recommends \
  libpng-dev libjpeg-dev libwebp-dev

# SDL3_ttf dependencies
sudo apt-get install -y --no-install-recommends \
  libfreetype-dev

# SDL3_mixer dependencies
sudo apt-get install -y --no-install-recommends \
  libmpg123-dev libvorbis-dev libopus-dev libflac-dev

BUILD_DIR=$(mktemp -d)
CMAKE_FLAGS="-DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -GNinja"

# SDL3
git clone --depth 1 --branch release-3.4.10 https://github.com/libsdl-org/SDL.git "$BUILD_DIR/SDL3"
cmake -S "$BUILD_DIR/SDL3" -B "$BUILD_DIR/SDL3/build" $CMAKE_FLAGS \
  -DSDL_TEST_LIBRARY=OFF
sudo cmake --build "$BUILD_DIR/SDL3/build" --target install -j4

# SDL3_image
git clone --depth 1 --branch release-3.4.4 https://github.com/libsdl-org/SDL_image.git "$BUILD_DIR/SDL3_image"
cmake -S "$BUILD_DIR/SDL3_image" -B "$BUILD_DIR/SDL3_image/build" $CMAKE_FLAGS \
  -DSDLIMAGE_SAMPLES=OFF
sudo cmake --build "$BUILD_DIR/SDL3_image/build" --target install -j4

# SDL3_ttf
git clone --depth 1 --branch release-3.2.2 https://github.com/libsdl-org/SDL_ttf.git "$BUILD_DIR/SDL3_ttf"
cmake -S "$BUILD_DIR/SDL3_ttf" -B "$BUILD_DIR/SDL3_ttf/build" $CMAKE_FLAGS \
  -DSDLTTF_SAMPLES=OFF
sudo cmake --build "$BUILD_DIR/SDL3_ttf/build" --target install -j4

# SDL3_mixer (SDLMIXER_MIDI_TIMIDITY compiles in a MIDI synthesizer with no external files needed)
git clone --depth 1 --branch release-3.2.4 https://github.com/libsdl-org/SDL_mixer.git "$BUILD_DIR/SDL3_mixer"
cmake -S "$BUILD_DIR/SDL3_mixer" -B "$BUILD_DIR/SDL3_mixer/build" $CMAKE_FLAGS \
  -DSDLMIXER_SAMPLES=OFF \
  -DSDLMIXER_MIDI_TIMIDITY=ON \
  -DSDLMIXER_MIDI_FLUIDSYNTH=OFF
sudo cmake --build "$BUILD_DIR/SDL3_mixer/build" --target install -j4

sudo ldconfig
