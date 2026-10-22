@echo off
cls 
echo.
echo Prepare binary package (./bin) of D2TM. 
echo.
echo.
echo 1. Removing bin directory
if exist bin rd /S /Q bin || exit /b 1
echo 2. Creating new bin directory
mkdir bin || exit /b 1
echo 3. Creating campaign directory
mkdir bin\campaign || exit /b 1
mkdir bin\campaign\atreides || exit /b 1
mkdir bin\campaign\ordos || exit /b 1
mkdir bin\campaign\harkonnen || exit /b 1
mkdir bin\campaign\sardaukar || exit /b 1
mkdir bin\campaign\maps || exit /b 1
mkdir bin\campaign\maps\seed || exit /b 1
mkdir bin\campaign\briefings || exit /b 1
echo 4. Copying campaign files
copy resources\bin\campaign\atreides\*.* bin\campaign\atreides || exit /b 1
copy resources\bin\campaign\ordos\*.* bin\campaign\ordos || exit /b 1
copy resources\bin\campaign\harkonnen\*.* bin\campaign\harkonnen || exit /b 1
copy resources\bin\campaign\sardaukar\*.* bin\campaign\sardaukar || exit /b 1
copy resources\bin\campaign\maps\*.* bin\campaign\maps || exit /b 1
copy resources\bin\campaign\maps\seed\*.* bin\campaign\maps\seed || exit /b 1
copy resources\bin\campaign\briefings\*.* bin\campaign\briefings || exit /b 1
copy resources\bin\campaign\info.txt bin\campaign\info.txt || exit /b 1
echo 5. Creating new data directory
mkdir bin\data || exit /b 1
mkdir bin\data\scenes || exit /b 1
mkdir bin\data\bmp || exit /b 1
echo 6. Copying data files
copy resources\bin\data\*.dat bin\data || exit /b 1
copy resources\bin\data\*.fon bin\data || exit /b 1
copy resources\bin\data\*.ttf bin\data || exit /b 1
echo 7. Copying scenes
copy resources\bin\data\scenes\*.* bin\data\scenes || exit /b 1
echo 8. Create save game directories
mkdir bin\save || exit /b 1
mkdir bin\save\atreides || exit /b 1
mkdir bin\save\harkonnen || exit /b 1
mkdir bin\save\ordos || exit /b 1
mkdir bin\save\skirmish || exit /b 1
echo 9. Create skirmish directory
mkdir bin\skirmish || exit /b 1
copy resources\bin\skirmish\*.ini bin\skirmish || exit /b 1
echo 10. Copying dll files
copy resources\dll\*.* bin || exit /b 1
echo 11. Copy executable from build dir
copy build\*.exe bin || exit /b 1
echo 12. Copy game rules file (game.ini)
copy resources\game.ini.org bin\game.ini || exit /b 1
copy resources\bin\settings.ini bin || exit /b 1
echo 13. Copy txt files
copy resources\doc\versionhistory.txt bin || exit /b 1
copy resources\doc\controls.txt bin || exit /b 1
copy resources\bin\windowed.bat bin || exit /b 1
echo Done.
