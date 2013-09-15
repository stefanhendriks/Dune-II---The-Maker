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
- have foot-men (soldiers, infantry, trooper, troopers, fremen, all are alike)
<del>
  <ul>
    <li>draw soldier (as a single celled unit)</li>
  </ul>
</del>
  - draw two soldiers on cell
  - draw five soldiers on cell
  - animate moving of unit
  - unit can be moved between cells, respecting sub-cell
  - when sub-cell is taken, then unit will take other cell
  - when all sub-cells are taken, the infantry considers the cell occupied
  - can move on mountains
  - should be blocked by other infantry, only when other cell is full
  - should be blocked by units
  - can be squished by heavy units
- modify the properties of units/structures by modifying a rules.ini file
- move vehicles around larger obstacles (like bases, mountains, etc) (path-finding on 'really-need-it-basis')
- move (my) units appropriately
  - sandworms should only be able to travel through sand, dunes and spice, and be blocked by rock/mountains
  - heavy vehicles should be able to move over infantry (fatal for the infantry).
  - air units should not be blocked by anything
- defend myself with walls
  - draw walls
  - walls should obstruct terrain units of all kinds
  - walls smoothing
  - terrain smoothing adjustments with walls
- see structures (draw them)
- have my ground wheeled/tank vehicles be blocked by structures
- select my structure
- use my infantry to take over enemy structures
  - the more damaged the structure is, the more chance of taking over
  - (fatal for the infantry, but chance of capture)
- have enemies to defeat
  - enemy units should not be visible when shrouded
  - enemy units should not be selectable
  - only show health bar when mouse hovers over enemy unit
- see everything move in a certain pace (timed) at all time , regardless of my fps ([this post is helpful](http://gameprogrammingtutorials.blogspot.nl/2010/01/sdl-tutorial-series-part-5-dealing-with.html))
- destroy units so i can win the game destroying my enemy's units
- destroy structures so i can win the game destroying my enemy's structures
- order a unit to attack another unit in order to destroy it
- order a unit to attack a structure in order to destroy it
- order a unit to attack a tile so I can destroy a wall or something else (ie, a sandworm perhaps)
- sandworms should only be able to travel through sand, dunes and spice, and be blocked by rock/mountains
- fear the sandworm
  - sandworm eats units on sand/dunes/spice
  - sandworm can get 'full' and dissapear (? or wander around messing with your mind?)
- send my vehicles to a repair bay in order to get the patched up and use them fully again.
- send my harvester back to the refinery in order to get the credits earlier
- build another structure from a Construction Yard
- place built structures
- build infantry from a Barracks
- build troopers from a WOR
- build light vehicles from a Light Factory
- build heavy vehicles from a Heavy Factory
 - build tanks
  - tanks should be able to face their body to the adjecent cell and face their barrel to the target (as should turrets)
- build air units from a High-Tech
- upgrade my structures to gain access to other units
- upgrade my units (??)
- receive reinforcements from time to time
- mine spice with a harvester
- deploy an MCV
- get my units transported much faster by a carry-all
- scroll by holding mouse key and move from that position
- when a unit is bored, it should turn around randomly
- when a unit is on guard, it should target enemies within range, as should turrets
- play on a randomized map

*Unit types*
- Soldier
- Infantry (Perhaps we want to do this the C&C way.)
- Trooper
- Troopers (Perhaps we want to do this the C&C way.)
- Trike
- Raider Trike
- Quad
- Tank
- Harvester
- MCV
- Siege Tank
- Rocket Launcher
- Deviator
- Devastator
- Sonic Tank
- Carryall
- Ornithopter
- Frigate
- Saboteur
- Fremen (C&C style?)

*Structures*

- Concrete 1x1
- Concrete 4x4
- Wall
- Windtrap
- Refinery
- Silos
- Radar Outpost
- Barracks
- WOR
- Light Factory
- Heavy Factory
- Hi-Tech Factory
- Gun Turret
- Rocket Turret
- Repair Facility
- Starport
- House of IX
- Palace

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
    <li>explore the terrain by clearing a shroud</li>
    <li>enjoy the terrain smoothened (smoothing algorithm)</li>
    <li>load a map from file</li>
    <li>have my ground wheeled/tank vehicles be blocked by other vehicles, and mountains</li>
    <li>have air units not being blocked by any terrain type</li>
    <li>have an arbitrary number of units</li>
    <li>have air units not be blocked by any units</li>
    <li>have air units always be drawn over ground units</li>
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
