#!/bin/sh
echo Make sure you have install clang version 4.2 or higher!
echo
echo This script attempts to install all dependencies of Dune II - The Maker for OS X.
echo If you wish to abort, press CTRL-C
read -n 1 -s

PROJECT_DIR=`pwd`
echo Project dir is $PROJECT_DIR
cd /tmp
if [ -d "d2tm" ]; then
  echo D2TM DIR ALREADY EXISTS - REMOVING
  rm -rf d2tm
fi
mkdir d2tm
cd d2tm

echo
echo
echo ================================================================
echo Installing SFML
echo ================================================================
echo
echo

# install SFML
git clone git@github.com:LaurentGomila/SFML.git SFML
cd SFML
cmake .
make install
cd ..

echo
echo
echo ================================================================
echo Installing SFGUI
echo ================================================================
echo
echo

# install SFGUI
git clone https://github.com/TankOs/SFGUI SFGUI
cd SFGUI
git checkout osx
git submodule init
git submodule update
cmake .
make install
cd ..

echo
echo
echo ================================================================
echo Installing Thor
echo ================================================================
echo
echo

# install thor
git clone git@github.com:Bromeon/Thor.git Thor
cd Thor
cmake .
make install
cd ..

echo
echo
echo ================================================================
echo Installing Boost with homebrew
echo ================================================================
echo
echo

# install boost with brew
brew install boost

cd $PROJECT_DIR

echo All dependencies installed.
echo All dependencies where checked out in /tmp/d2tm, and is still available. You can safely remove this if you desire to.
echo


