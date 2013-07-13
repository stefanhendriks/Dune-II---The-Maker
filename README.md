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

Example, for Mac OS X:
======================
- Install XCode CLI tools
- If you are using brew, do: brew install SDL && brew install sdl_image
- Then: `cp makefile.osx makefile`
- `make`

