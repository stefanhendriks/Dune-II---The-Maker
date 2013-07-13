Dune II - The Maker
===================

This is a rewrite attempt, using SDL.

Compiling
=========
This project has makefiles for Windows (mingw32), Linux (ubuntu) and Mac OSX.

Dependencies:
- SDL
- SDL Image

After you check out this project, copy the appropiate makefile (with the extension matching your OS) as 'makefile'. After that hit make.

Example, OSX && Brewfor Mac OS X:
======================
- Install XCode CLI tools
- Using brew, install dependencies: `brew install SDL && brew install sdl_image`
- Then: `cp makefile.osx makefile`
- `make`

