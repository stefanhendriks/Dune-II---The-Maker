Dune II - The Maker
===================
A.k.a. D2TM. Is a [Dune 2](http://en.wikipedia.org/wiki/Dune_II) clone. At this moment the most known/stable version is [DEMO4 and DEMO 3.5](http://dune2themaker.fundynamic.com/?page_id=11) which are very old and using
Allegro & other libs. I am proud of that project, but it is time to rewrite it and build a game mostly focussing on skirmish
and networking gaming.

This is an attempt to rewrite the game from scratch using [SDL 1.2](http://www.libsdl.org/). If you are looking for the source code of the 'current' D2TM ([with demo binaries](http://dune2themaker.fundynamic.com/downloads/)), then you can find it [here](https://github.com/stefanhendriks/Dune-II---The-Maker/tree/d2tm_allegro)

I do this purely for fun, and try to keep it multi-platform. I test it often on Mac OSX and Ubuntu. I sometimes test it
on Windows (MinGW32 and MSYS).


Current State
-------------
If you want to know what is possible with the current version, see the [DONE features list](#done).
If you want to know what still has to be done, (you might want to contribute?), see the [TODO list](#todo) below what needs to be done.

There is no binary available, instead you need to [compile it yourself](#compiling).

Contributing
------------
Check the [TODO list](#todo). Create (or checkout) a feature branch. Code. And once you're finished
submit it as a pull request.

Compiling
=========
This project has makefiles for Windows (mingw32), Linux (ubuntu) and Mac OSX.

Dependencies:
- SDL 1.2
- SDL Image
- SDL Gfx

After you check out this project, copy the appropiate makefile (with the extension matching your OS) as 'makefile'. After that hit make.

Example, OSX && [homebrew](http://mxcl.github.io/homebrew/):
------------------------------------------------------------
- Install XCode CLI tools
- Using brew, install dependencies: `brew install SDL sdl_image sdl_gfx`
- Then: `cp makefile.osx makefile`
- `make`

Example, Ubuntu (inspired by [Lazy Foo](http://lazyfoo.net/SDL_tutorials/lesson01/linux/))
------------------------------------------------------------------------------------------
- Fetch g++ and its dependencies with apt-get:

        apt-get install g++
- Fetch SDL and its dependencies with apt-get:

        apt-get install libsdl1.2-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libsdl-ttf2.0-dev libsdl-gfx1.2-dev
- `cp makefile.ubuntu makefile`
- `make`

As a player I should be able to...
==================================
Features need to be written in such a way that it is obvious what the benefit is for the player.

TODO
----
- explore the terrain by clearing a Shroud
<del>
  <ul>
    <li>Units are able to clear shroud (in a circular pattern)</li>
    <li>Units can have different 'sight' (ie reveal more shroud)</li>
  </ul>
</del>
 - Shroud drawing should be smoothened (ie, nice rounded corners). Preferably with also some shadow-like-border (as in original D2TM).
- (as a player I) should have enemies to defeat
 - enemy units should not be visible when shrouded
 - enemy units should not be selectable
 - only show health bar when mouse hovers over enemy unit
- see everything move in a certain pace (timed) at all time , regardless of my fps ([this post is helpful](http://gameprogrammingtutorials.blogspot.nl/2010/01/sdl-tutorial-series-part-5-dealing-with.html))
- order a unit to attack a position
- sandworms should only be able to travel through sand, dunes and spice, and be blocked by rock/mountains
- enjoy the terrain smoothened (smoothing algorithm)
- see structures (draw them)
- select my structure
- build another structure from a Construction Yard
- build infantry from a Barracks
- build troopers from a WOR
- build light vehicles from a Light Factory
- build heavy vehicles from a Heavy Factory
 - build tanks
  - tanks should be able to face their body to the adjecent cell and face their barrel to the target
- build air units from a High-Tech
- upgrade my structures to gain access to other units
- upgrade my units (??)
- receive reinforcements from time to time
- mine spice with a harvester
- get my units transported much faster by a carry-all
- scroll by holding mouse key and move from that position
- when a unit is bored, it should turn around randomly

*Unit types*
- Soldier
- Infantry (Perhaps we want to do this the C&C way.)
- Trooper
- Troopers (Perhaps we want to do this the C&C way.)
- Trike
- Raider Trike
- Quad
- Tank
- Siege Tank
- Rocket Launcher
- Deviator
- Devastator
- Sonic Tank
- Cary-all
- Ornithopter
- Frigate
- Saboteur
- Fremen (C&C style?)


DONE
----
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
    <li>draw a rectangle by holding left mouse button</li>
    <li>select multiple units by dragging rectangle over units and releasing left mouse button</li>
    <li>order a unit to move to a position</li>
  </ul>
</del>

Controls
========
Thus far the following controls work:

Press `q` to quit anytime.
Move camera around with arrow keys or with mouse hitting borders of screen.

Right mouse click deselects all unit.
Left mouse click selects a unit.
Hold left mouse button and drag a rectangle over units to select them.
