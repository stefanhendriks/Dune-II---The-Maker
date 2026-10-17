[![Latest](https://img.shields.io/github/downloads/stefanhendriks/Dune-II---The-Maker/latest/total)]()
[![v0.7.0](https://img.shields.io/github/downloads/stefanhendriks/Dune-II---The-Maker/v0.7.0/total)]()
[![v0.6.0](https://img.shields.io/github/downloads/stefanhendriks/Dune-II---The-Maker/v0.6.0/total)]()
[![All](https://img.shields.io/github/downloads/stefanhendriks/Dune-II---The-Maker/total.svg)]()

[![Build & Release](https://github.com/stefanhendriks/Dune-II---The-Maker/actions/workflows/build_master.yml/badge.svg)](https://github.com/stefanhendriks/Dune-II---The-Maker/actions/workflows/build_master.yml)

![GitHub repo size](https://img.shields.io/github/repo-size/stefanhendriks/Dune-II---The-Maker?style=plastic)
![GitHub language count](https://img.shields.io/github/languages/count/stefanhendriks/Dune-II---The-Maker?style=plastic)
![GitHub top language](https://img.shields.io/github/languages/top/stefanhendriks/Dune-II---The-Maker?style=plastic)
![GitHub last commit](https://img.shields.io/github/last-commit/stefanhendriks/Dune-II---The-Maker?color=red&style=plastic)

Dune II - The Maker
===================
A.k.a. D2TM. Is a [Dune 2](http://en.wikipedia.org/wiki/Dune_II) remake.

- Website: [https://dune2themaker.fundynamic.com](https://dune2themaker.fundynamic.com)
- Discord: [https://dune2themaker.fundynamic.com/discord](https://dune2themaker.fundynamic.com/discord)
- Twitter: [https://twitter.com/dune2themaker](https://twitter.com/dune2themaker)

Report bugs via Discord, Twitter or add one in Github.

Read [the wiki](https://github.com/stefanhendriks/Dune-II---The-Maker/wiki) for more information about startup params, controls, and more.

# Sponsor
<span class="badge-buymeacoffee">
<a href="https://ko-fi.com/dune2themaker" title="Donate to this project using Buy Me A Coffee"><img src="https://img.shields.io/badge/buy%20me%20a%20coffee-donate-yellow.svg" alt="Buy Me A Coffee donate button" /></a>
</span>

# Compiling

## Prerequisites
- `CMake` version 3.21 or higher
- `GCC` version 14 or higher
- `SDL3` with `SDL3_image`, `SDL3_ttf` and `SDL3_mixer`
- `Catch2` version 3 (for the unit tests)

The `install_dependencies_*.sh` scripts (and the MSYS2 package list for Windows, see below) install the libraries.

Make sure you can run `make` (or `ninja`) from command line.

## Warnings as errors
CI treats all compiler warnings as errors. Local builds don't, by default, so a
work-in-progress build isn't blocked by them. To reproduce CI's strictness
locally, configure with `-DD2TM_WERROR=ON`:
```
    cmake .. -DD2TM_WERROR=ON
```

## Windows (MSYS2 UCRT64)
It depends on a MinGW 64-bit UCRT environment (UCRT64), via [MSYS2](https://www.msys2.org/). From
an MSYS2 UCRT64 shell, install the same packages CI installs (see the
`install:` line of the "Install MSYS2 with MinGW 64-bit" step in
[`build_pr.yml`](.github/workflows/build_pr.yml) for the current list) via
`pacman -S <packages>`.

- git clone this project
- create a `build` dir
```
    mkdir build
    cd build
    cmake .. -G "MinGW Makefiles"
    cmake --build . --target all -- -j 6
    ctest --output-on-failure
```

If you use `ninja` you can use `cmake .. -G Ninja` instead.

Or run `rebuildwin.bat` from the MSYS2 UCRT64 shell, which does a clean build in `build/` and runs the tests.

Once compilation is done you'll end up with a `d2tm.exe` file. The DLL's it needs are copied into `bin/` together with it by `create_release.bat` (see [Running](#running)).

## Ubuntu
- git clone this project
- run `./install_dependencies_ubuntu.sh` (only required once)
- create a `build` dir
```
    mkdir build
    cd build
    cmake ..
    make -j4
    ctest --output-on-failure
```

Or run `./rebuildmacos.sh`, which does a clean build in `build/`, runs the tests and copies the binary to `bin/`.

## MacOS
- git clone this project
- run `./install_dependencies_macosx.sh` (only required once)
- create a `build` dir
```
    mkdir build
    cd build
    cmake ..
    make -j4
    ctest --output-on-failure
```

Or run `./rebuildmacos.sh`, which does a clean build in `build/`, runs the tests and copies the binary to `bin/`.

## Tests
Unit tests (Catch2) live in `tests/` and are built together with the game as `d2tm_tests`. Run them from the `build` dir with `ctest --output-on-failure`. CI runs them on every platform.

## Clion
### Mac OS
Make sure you set up your toolchain to use `gcc-15` and `g++-15`. Ie, change your default toolchain
OR create a new one (and make sure CMake points to that one), with the following values:

| Field        | Value                    |
|--------------|--------------------------|
| C-Compiler   | /opt/homebrew/bin/gcc-15 |
| C++-Compiler | /opt/homebrew/bin/g++-15 |

Then make sure to (re)load `CmakeLists.txt`

# Running
Run `create_release.bat` (Windows) or `./create_release.sh` (Linux/macOS) once after building to produce a complete runnable package in `bin/`, including all required dependencies. Then run the executable from `bin/`.

## Music (MIDI soundfont)

D2TM plays music from MIDI files via FluidSynth. FluidSynth requires a General MIDI soundfont (.sf2) to render audio — without one, music is silent.

**Install a soundfont:**

- **Linux:** `sudo apt install fluid-soundfont-gm` (installs to `/usr/share/sounds/sf2/FluidR3_GM.sf2`)
- **macOS / Windows:** Download [FluidR3_GM.sf2](https://member.keymusician.com/Member/FluidR3_GM/index.html) or [GeneralUser GS](https://schristiancollins.com/generaluser.php) and place it anywhere.

**Point the game at it:**

Set the `SDL_SOUNDFONTS` environment variable to the path of your `.sf2` file before launching:

```sh
export SDL_SOUNDFONTS=/path/to/FluidR3_GM.sf2   # Linux/macOS
set SDL_SOUNDFONTS=C:\path\to\FluidR3_GM.sf2    # Windows
```

Add the `export` line to your shell profile (`~/.zshrc`, `~/.bashrc`) to make it permanent.

# Project status
See the [Project Board](https://github.com/stefanhendriks/Dune-II---The-Maker/projects/1) what's going on.

# Binaries
You have various options:
- Grab the [latest release from Github](https://github.com/stefanhendriks/Dune-II---The-Maker/releases)
- Grab a version from [D2TM's homepage](https://www.dune2themaker.com)
- Compile this project yourself

# Topics
In the code there are several known concepts; see below for extra information
about them:

- [Angles: how to calculate and use them to draw things](resources/doc/convertAngleToDrawIndex.md)