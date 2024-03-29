@echo off
cls 
echo.
echo Prepare binary package (./bin) of D2TM. 
echo.
echo Will copy "d2tm.exe" from the Default dir created by Clion.
echo Will copy DLL files from dll/mingw32
echo.
echo 1. Removing bin directory
rd /S /Q bin
echo 2. Creating new bin directory
mkdir bin
echo 3. Creating campaign directory
mkdir bin\campaign
mkdir bin\campaign\atreides
mkdir bin\campaign\ordos
mkdir bin\campaign\harkonnen
mkdir bin\campaign\maps
mkdir bin\campaign\maps\seed
mkdir bin\campaign\briefings
echo 4. Copying campaign files
copy campaign\atreides\*.* bin\campaign\atreides
copy campaign\ordos\*.* bin\campaign\ordos
copy campaign\harkonnen\*.* bin\campaign\harkonnen
copy campaign\maps\*.* bin\campaign\maps
copy campaign\maps\seed\*.* bin\campaign\maps\seed
copy campaign\briefings\*.* bin\campaign\briefings
copy campaign\info.txt bin\campaign\info.txt
echo 5. Creating new data directory
mkdir bin\data
mkdir bin\data\scenes
mkdir bin\data\bmp
echo 6. Copying data files
copy data\*.dat bin\data
copy data\*.fon bin\data
copy data\*.ttf bin\data
copy data\bmp\*.* bin\data\bmp
echo 7. Copying scenes
copy data\scenes\*.* bin\data\scenes
echo 8. Create save game directories
mkdir bin\save
mkdir bin\save\atreides
mkdir bin\save\harkonnen
mkdir bin\save\ordos
mkdir bin\save\skirmish
echo 9. Create skirmish directory
mkdir bin\skirmish
copy skirmish\*.ini bin\skirmish
echo 10. Copying dll files
copy dll\mingw32\*.* bin
echo 11. Copy executable from %1
copy cmake-build-release-mingw\*.exe bin
echo 12. Copy game rules file (game.ini)
copy game.ini.org bin\game.ini
echo 13. Copy txt files
copy *.txt bin
copy doc\versionhistory.txt bin
copy doc\controls.txt bin
copy windowed.bat bin
copy d2tm.cfg bin
del bin\CMakeLists.txt
echo Done.