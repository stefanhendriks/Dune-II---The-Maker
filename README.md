Dune II - The Maker
===================
A.k.a. D2TM. Is a [Dune 2](http://en.wikipedia.org/wiki/Dune_II) clone. At this moment the most known/stable version is [DEMO4 and DEMO 3.5](http://dune2themaker.fundynamic.com/?page_id=11) which are very old and using
Allegro & other libs. I am proud of that project, but it is time to rewrite it and build a game mostly focussing on skirmish
and networking gaming.

This is an attempt to rewrite the game from scratch using [SFML 2.1](http://www.sfml-dev.org/resources.php). If you are looking for the source code of the 'current' D2TM ([with demo binaries](http://dune2themaker.fundynamic.com/downloads/)), then you can find it [here](https://github.com/stefanhendriks/Dune-II---The-Maker/tree/d2tm_allegro)

I do this purely for fun, and try to keep it multi-platform. I test it often on Mac OSX and Ubuntu. I sometimes test it
on Windows (MinGW32 and MSYS).


Current State
-------------
If you want to know what is possible with the current version, see the [DONE features list](#tododoingdone).
If you want to know what still has to be done, (you might want to contribute?), see the [TODO list](#tododoingdone) below what needs to be done.

There is no binary available, instead you need to [compile it yourself](#compiling).

Contributing
------------
Check the [TODO list](#tododoingdone). Create (or checkout) a feature branch. Code. And once you're finished
submit it as a pull request.

Compiling
=========
This projct uses [cmake](http://www.cmake.org/) to generate a platform specific makefile. 
Before running cmake, make sure you have [installed the required dependencies](#installing-dependencies)

After that you do:

```
cmake .
make
```

which should deliver a ```./d2tm``` binary file.

Installing dependencies
-----------------------
This example is based on Mac OS X, the procedure is probably the same on Linux. For Windows, we have not yet tested this.

First make sure you have installed cmake.

You will need two dependencies, [SFML 2.1](http://www.sfml-dev.org/download/sfml/2.1/) and [Thor](https://github.com/Bromeon/Thor).

Install SFML
------------
- download SFML binaries for Mac OS X(compatible with clang++) [from here](http://www.sfml-dev.org/download/sfml/2.1/)
- unpack the package in /tmp, a 'SFML-2.1-osx-clang-universal' is being extracted.
- cd to 'SFML-2.1-osx-clang-universal' directory
- run ```sudo ./install.sh```
- done (you may remove SFML-2.1-osx-clang-universal)

Install Thor
------------
- cd /tmp
- git clone [Thor](https://github.com/Bromeon/Thor)
- cd to Thor
- ```
cmake .
make
make install
```
- done (you may remove the Thor directory)

Now you are ready to [compile d2tm](#compiling).


As a player I should be able to...
==================================
Features need to be written in such a way that it is obvious what the benefit is for the player.

TODO/DOING/DONE
---------------
We keep a list of stories/features on our [public Trello board](https://trello.com/b/3he26cm4/dune-ii-the-maker)

Controls
========
Thus far the following controls work:

Press `q` to quit anytime.
Move camera around with arrow keys or with mouse hitting borders of screen.

Right mouse click deselects all unit.
Left mouse click selects a unit.
Hold left mouse button and drag a rectangle over units to select them.
