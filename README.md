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

Example, OSX && [homebrew](http://mxcl.github.io/homebrew/):
======================
- Install XCode CLI tools
- Using brew, install dependencies: `brew install SDL && brew install sdl_image`
- Then: `cp makefile.osx makefile`
- `make`


Example, Ubuntu (inspired by [Lazy Foo](http://lazyfoo.net/SDL_tutorials/lesson01/linux/))
===============
- Fetch SDL and its dependencies with apt-get (`apt-get install libsdl1.2-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libsdl-ttf2.0-dev`)
- `cp makefile.ubuntu makefile`
- `make`
