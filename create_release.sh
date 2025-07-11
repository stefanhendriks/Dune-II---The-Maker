echo
echo "Prepare binary package (./bin) of D2TM."
echo
echo
echo "1. Removing bin directory"
rm -rf bin
echo "2. Creating new bin directory"
mkdir bin
echo "3. Creating campaign directory"
mkdir bin/campaign
mkdir bin/campaign/atreides
mkdir bin/campaign/ordos
mkdir bin/campaign/harkonnen
mkdir bin/campaign/maps
mkdir bin/campaign/maps/seed
mkdir bin/campaign/briefings
echo "4. Copying campaign files"
cp campaign/atreides/*.* bin/campaign/atreides
cp campaign/ordos/*.* bin/campaign/ordos
cp campaign/harkonnen/*.* bin/campaign/harkonnen
cp campaign/maps/*.* bin/campaign/maps
cp campaign/maps/seed/*.* bin/campaign/maps/seed
cp campaign/briefings/*.* bin/campaign/briefings
cp campaign/info.txt bin/campaign/info.txt
echo "5. Creating new data directory"
mkdir bin/data
mkdir bin/data/scenes
mkdir bin/data/bmp
echo "6. Copying data files"
cp data/*.dat bin/data
cp data/*.fon bin/data
cp data/*.ttf bin/data
cp data/bmp/*.* bin/data/bmp
echo "7. Copying scenes"
cp data/scenes/*.* bin/data/scenes
echo "8. Create save game directories"
mkdir bin/save
mkdir bin/save/atreides
mkdir bin/save/harkonnen
mkdir bin/save/ordos
mkdir bin/save/skirmish
echo "9. Create skirmish directory"
mkdir bin/skirmish
cp skirmish/*.ini bin/skirmish
echo "10. Copying dll files"
# cp dll/mingw32\*.* bin
echo "11. Copy executable"
cp build/d2tm bin
echo "12. Copy game rules file (game.ini)"
cp game.ini.org bin/game.ini
cp settings.ini bin
echo "13. Copy txt files"
cp *.txt bin
cp doc/versionhistory.txt bin
cp doc/controls.txt bin
cp d2tm.cfg bin
rm bin/CMakeLists.txt
echo "Done."