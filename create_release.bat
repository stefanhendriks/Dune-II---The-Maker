@echo off
cls 
echo.
echo Prepare binary package (./bin) of D2TM. 
echo.
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
copy resources\bin\campaign\atreides\*.* bin\campaign\atreides
copy resources\bin\campaign\ordos\*.* bin\campaign\ordos
copy resources\bin\campaign\harkonnen\*.* bin\campaign\harkonnen
copy resources\bin\campaign\maps\*.* bin\campaign\maps
copy resources\bin\campaign\maps\seed\*.* bin\campaign\maps\seed
copy resources\bin\campaign\briefings\*.* bin\campaign\briefings
copy resources\bin\campaign\info.txt bin\campaign\info.txt
echo 5. Creating new data directory
mkdir bin\data
mkdir bin\data\scenes
mkdir bin\data\bmp
echo 6. Copying data files
copy resources\bin\data\*.dat bin\data
copy resources\bin\data\*.fon bin\data
copy resources\bin\data\*.ttf bin\data
echo 7. Copying scenes
copy resources\bin\data\scenes\*.* bin\data\scenes
echo 8. Create save game directories
mkdir bin\save
mkdir bin\save\atreides
mkdir bin\save\harkonnen
mkdir bin\save\ordos
mkdir bin\save\skirmish
echo 9. Create skirmish directory
mkdir bin\skirmish
copy resources\bin\skirmish\*.ini bin\skirmish
echo 10. Copying dll files
copy resources\dll\*.* bin
echo 11. Copy executable from build dir
copy build\*.exe bin
echo 12. Copy game rules file (game.ini)
copy resources\game.ini.org bin\game.ini
copy resources\bin\settings.ini bin
echo 13. Copy txt files
copy resources\bin\doc\versionhistory.txt bin
copy resources\bin\doc\controls.txt bin
copy resources\bin\windowed.bat bin
copy resources\bin\d2tm.cfg bin
echo Done.