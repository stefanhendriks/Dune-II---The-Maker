Dune II - The Maker
===================
A.k.a. D2TM. Is a [Dune 2](http://en.wikipedia.org/wiki/Dune_II) clone. At this moment the most known/stable version is [DEMO4 and DEMO 3.5](http://dune2themaker.fundynamic.com/?page_id=11) which are very old and using
Allegro & other libs. I am proud of that project, but it is time to rewrite it and build a game mostly focussing on skirmish
and networking gaming.

This is an attempt to rewrite the game from scratch using [SDL](http://www.libsdl.org/). If you are looking for the source code of the 'current' D2TM ([with demo binaries](http://dune2themaker.fundynamic.com/downloads/)), then you can find it [here](https://github.com/stefanhendriks/Dune-II---The-Maker/tree/d2tm_allegro)

I do this purely for fun, and try to keep it multi-platform. I test it often on Mac OSX and Ubuntu. I sometimes test it
on Windows (MinGW32 and MSYS). 

At this moment there is nothing built enough to call this a game, see the [TODO list](#todo-as-a-player-i-should-be-able-to) below what needs to be done. 

Compiling
=========
This project has makefiles for Windows (mingw32), Linux (ubuntu) and Mac OSX.

Dependencies:
- SDL
- SDL Image

After you check out this project, copy the appropiate makefile (with the extension matching your OS) as 'makefile'. After that hit make.

Example, OSX && [homebrew](http://mxcl.github.io/homebrew/):
------------------------------------------------------------
- Install XCode CLI tools
- Using brew, install dependencies: `brew install SDL && brew install sdl_image`
- Then: `cp makefile.osx makefile`
- `make`

Example, Ubuntu (inspired by [Lazy Foo](http://lazyfoo.net/SDL_tutorials/lesson01/linux/))
------------------------------------------------------------------------------------------
- Fetch SDL and its dependencies with apt-get (`apt-get install libsdl1.2-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libsdl-ttf2.0-dev`)
- `cp makefile.ubuntu makefile`
- `make`

TODO: ((as a player I) should be able to...)
============================================
<del>
  <ul>
    <li>move the camera around</li>
    <li>pixel perfect scrolling instead of cell by cell</li>
    <li>scroll by holding arrow keys instead of having to press/release all the time</li>
    <li>scroll by 'hitting mouse' on screen borders</li>
    <li>see units (draw them)</li>
    <li>see units drawn in center when bigger than 32x32 pixels</li>
    <li>see a shadow of a unit</li>
    <li>see units of my own color (palette trickery)</li>
    <li>select one unit</li>
</ul>
</del>

- draw a rectangle by holding left mouse button
- select multiple units by dragging rectangle over units and releasing left mouse button
- order a unit to move to a position
- see everything move in a certain pace (timed) at all time , regardless of my fps ([this post is helpful](http://gameprogrammingtutorials.blogspot.nl/2010/01/sdl-tutorial-series-part-5-dealing-with.html))
- order a unit to attack a position
- explore the terrain by clearing a Fog of War
- enjoy the terrain smoothened (smoothing algorithm)
- see structures (draw them)
- select my structure
- build another structure from a Construction Yard
- build infantry from a Barracks
- build troopers from a WOR
- build light vehicles from a Light Factory
- build heavy vehicles from a Heavy Factory
- build air units from a High-Tech
- upgrade my structures to gain access to other units
- upgrade my units (??)
- receive reinforcements from time to time
- mine spice with a harvester
- get my units transported much faster by a carry-all
- scroll by holding mouse key and move from that position


Controls
========
Thus far the following controls work:

Press `q` to quit anytime.
Move camera around with arrow keys or with mouse hitting borders of screen.
