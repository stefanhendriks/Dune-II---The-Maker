# This repository is no longer maintained, [a more recent rewrite can be found here though](https://github.com/Fundynamic/dune2themaker4j)

Dune II - The Maker
===================
## Looking for the source code of DEMO 3.5/DEMO 4 ?
If you are looking for the source code of the 'current' D2TM ([with demo binaries](http://dune2themaker.fundynamic.com/downloads/)), then you can find it [here](https://github.com/stefanhendriks/Dune-II---The-Maker/tree/d2tm_allegro)

## Original readme continues here
A.k.a. D2TM. Is a [Dune 2](http://en.wikipedia.org/wiki/Dune_II) clone. At this moment the most known/stable version is [DEMO4 and DEMO 3.5](http://dune2themaker.fundynamic.com/?page_id=11) which are very old and using
Allegro & other libs. I am proud of that project, but it is time to rewrite it and build a game mostly focussing on skirmish
and networking gaming.

This is an attempt to rewrite the game from scratch using [SFML 2.1](http://www.sfml-dev.org/resources.php). If you are looking for the source code of the 'current' D2TM ([with demo binaries](http://dune2themaker.fundynamic.com/downloads/)), then you can find it [here](https://github.com/stefanhendriks/Dune-II---The-Maker/tree/d2tm_allegro)

We try to keep it multi-platform. We test it often on Mac OSX and Ubuntu. Sometimes we test it
on Windows (MinGW32 and MSYS).

If you want to know more about the dependencies used, you can read the [documentation/tutorials here](#documentation).

Where is the binary?
--------------------
There is no binary available, instead you need to [compile it yourself](#compiling). Once we meet our milestones, we will release binaries for all targetted operating systems.

Contributing
============
- Check the [TODO list](#tododoingdone).
- Create (or checkout) a feature branch.
- Code.
- Finished? Submit it as a pull request.

Compiling
=========
This projct uses [cmake](http://www.cmake.org/) to generate a platform specific makefile.
Before running cmake, make sure you have [installed the required dependencies](#installing-dependencies)

After that you do:

```
cmake .
make
```

which should deliver a ```./d2tm``` binary file you should be able to execute.

Installing dependencies
-----------------------
This example is based on Mac OS X, the procedure is probably (somewhat) the same on Linux. For Windows, we have not yet tested this. We also assume you use [homebrew](http://brew.sh/).

_If you trust us you can also execute the `install_dependencies_osx.sh` file which does the stuff below for you_


First make sure you have installed cmake.

You will need the following dependencies:

1. SFML 2.1 [(download)](http://www.sfml-dev.org/download/sfml/2.1/) or [GIT repo](https://github.com/LaurentGomila/SFML)
2. SFGUI [Git repo, for Mac OS X](https://github.com/stefanhendriks/SFGUI/tree/fix-mac-osx-compiling) else go to [here](https://github.com/TankOs/SFGUI)
3. [Thor](https://github.com/Bromeon/Thor)
4. [Boost](http://www.boost.org/users/download/)

Install SFML
------------
- download SFML binaries for Mac OS X(compatible with clang++) [from here](http://www.sfml-dev.org/download/sfml/2.1/)
- unpack the package in /tmp, a 'SFML-2.1-osx-clang-universal' is being extracted.
- cd to 'SFML-2.1-osx-clang-universal' directory
- run ```sudo ./install.sh```
- done (you may remove SFML-2.1-osx-clang-universal)

OR:

- git clone [sfml](https://github.com/LaurentGomila/SFML)
- `cmake .`
- `make`
- `make install`

Install SFGUI
-------------
- git clone [this mac os x specific repo](https://github.com/stefanhendriks/SFGUI/tree/fix-mac-osx-compiling).
- `git submodule init`
- `git submodule update`
- `cmake .`
- `make`
- `make install`


Install Thor
------------
- cd /tmp
- git clone [Thor](https://github.com/Bromeon/Thor)
- cd to Thor
- `cmake .`
- `make`
- `make install`
- done (you may remove the Thor directory)

Now you are ready to [compile d2tm](#compiling).

Install Boost
-------------
- `brew install boost`


Controls
========
Thus far the following controls work:

Press `q` to quit anytime.
Move camera around with arrow keys or with mouse hitting borders of screen.

Right mouse click deselects all unit.
Left mouse click selects a unit.
Hold left mouse button and drag a rectangle over units to select them.

What is left to do?
===================
We use a [public Trello board](https://trello.com/b/3he26cm4/dune-ii-the-maker) to keep a list of things todo. You can also see here what we're working on right now and what has been finished. If you feel like contributing, please let us know so we can move the story to 'doing' , and refer to your feature branch in the meantime.

Documentation
=============
- [SFML 2.1](http://www.sfml-dev.org/tutorials/2.1/)
- [SFGUI](http://sfgui.sfml-dev.de/p/docs)
- [Boost](http://www.boost.org/doc/)
- [Thor](http://www.bromeon.ch/libraries/thor/v2.0/doc/index.html)

LICENSE
=======
The MIT License (MIT)

Copyright (c) 2013 S. Hendriks

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

