[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/Q5Q041QFG)
[![Latest](https://img.shields.io/github/downloads/stefanhendriks/Dune-II---The-Maker/latest/total)]()
[![All](https://img.shields.io/github/downloads/stefanhendriks/Dune-II---The-Maker/total.svg)]()

This project uses [Deleaker](https://www.deleaker.com/) to find memory leaks and more.

Dune II - The Maker
===================
A.k.a. D2TM. Is a [Dune 2](http://en.wikipedia.org/wiki/Dune_II) remake.

## Bit of history
This is also the version where I learned to code C/C++ for the first time and that shows. This is the 3rd iteration, of my many attempts to write an RTS engine for myself in my teens.

The very first, succesfull iteration, in BASIC is [Arrakis](http://arrakis.dune2k.com/downloads.html) ( [SourceCode](https://github.com/Fundynamic/arrakis)). You may also find the other iterations
of my Dune clones.

After reading [this post](https://forum.dune2k.com/topic/27425-i-dont-want-to-get-your-hopes-up-but-i-cant-deny-it-much-either/?do=findComment&comment=397890) I felt like I had to clean things up a bit. Atleast get
things back into a state where I could easily work/improve on this WIP version.

It felt better to have the D2TM github point to the (almost) latest WIP version (337). [I mentioned a SFML rewrite](http://dune2themaker.fundynamic.com/?p=288); which I have not removed, it is not being developed further and is kept it [in this branch](https://github.com/stefanhendriks/Dune-II---The-Maker/tree/rewrite-sfml)

## Binaries
You have various options:
- Grab the [latest release from Github](https://github.com/stefanhendriks/Dune-II---The-Maker/releases)
- Grab a version from [D2TM's homepage](https://www.dune2themaker.com)
- Compile this project yourself

## Compiling
For [version 0.5.0](https://github.com/stefanhendriks/Dune-II---The-Maker/milestone/1) I have worked with CLion which uses CMake. Hence the `CMakeList.txt` file in the root of this project.

I've also used MinGW32, and the required libraries (ALFont, ALMP3, fblend, etc) are in the [lib folder](https://github.com/stefanhendriks/Dune-II---The-Maker/tree/master/lib/mingw).

