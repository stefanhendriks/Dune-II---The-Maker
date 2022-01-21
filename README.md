[![Latest](https://img.shields.io/github/downloads/stefanhendriks/Dune-II---The-Maker/latest/total)]()
[![All](https://img.shields.io/github/downloads/stefanhendriks/Dune-II---The-Maker/total.svg)]()
[![CI](https://github.com/stefanhendriks/Dune-II---The-Maker/actions/workflows/build_cmake.yml/badge.svg)](https://github.com/stefanhendriks/Dune-II---The-Maker/actions/workflows/build_cmake.yml)
<span class="badge-buymeacoffee">
<a href="https://ko-fi.com/dune2themaker" title="Donate to this project using Buy Me A Coffee"><img src="https://img.shields.io/badge/buy%20me%20a%20coffee-donate-yellow.svg" alt="Buy Me A Coffee donate button" /></a>
</span>

![GitHub repo size](https://img.shields.io/github/repo-size/stefanhendriks/Dune-II---The-Maker?style=plastic)
![GitHub language count](https://img.shields.io/github/languages/count/stefanhendriks/Dune-II---The-Maker?style=plastic)
![GitHub top language](https://img.shields.io/github/languages/top/stefanhendriks/Dune-II---The-Maker?style=plastic)
![GitHub last commit](https://img.shields.io/github/last-commit/stefanhendriks/Dune-II---The-Maker?color=red&style=plastic)

This project uses [Deleaker](https://www.deleaker.com/) to find memory leaks and more.

Dune II - The Maker
===================
A.k.a. D2TM. Is a [Dune 2](http://en.wikipedia.org/wiki/Dune_II) remake.

- Website: [https://dune2themaker.fundynamic.com](https://dune2themaker.fundynamic.com)
- Discord: [https://dune2themaker.fundynamic.com/discord](https://dune2themaker.fundynamic.com/discord)
- Twitter: [https://twitter.com/dune2themaker](https://twitter.com/dune2themaker)

Report bugs via Discord, Twitter or add one in Github.

Read [the wiki](https://github.com/stefanhendriks/Dune-II---The-Maker/wiki) for more information about startup params, controls, and more.

## Compiling
This project uses `CMake`, you need atleast version 3.21. It currently depends
on MinGW32. Make sure you can run `make` (or `ninja`) from command line.

Then:
- git clone this project
- create a `build` dir
```
    mkdir build
    cd build
    cmake .. -G "MinGW Makefiles"
    cmake --build . --target all -- -j 6
```

If you use `ninja` you can use `cmake .. -G Ninja` instead.

Once compilation is done you'll end up with a `d2tm.exe` file and several DLL's.

## Running
Easiest is to run the executable from the root. Shared libraries that are required are: alfont.dll, alleg42.dll, almp3.dll, libwinpthread-1.dll and mscvr70.dll.
The required libraries (Allegro 4, ALFont, ALMP3) are in the [dll folder](https://github.com/stefanhendriks/Dune-II---The-Maker/tree/master/dll/mingw32).
libwinpthread-1.dll Could be on your MinGW path. If it is not on the path, copy it from your mingw32 installation.

## Project status
See the [Project Board](https://github.com/stefanhendriks/Dune-II---The-Maker/projects/1) what's going on.

## Binaries
You have various options:
- Grab the [latest release from Github](https://github.com/stefanhendriks/Dune-II---The-Maker/releases)
- Grab a version from [D2TM's homepage](https://www.dune2themaker.com)
- Compile this project yourself
