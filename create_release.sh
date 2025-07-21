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
cp resources/bin/campaign/atreides/*.* bin/campaign/atreides
cp resources/bin/campaign/ordos/*.* bin/campaign/ordos
cp resources/bin/campaign/harkonnen/*.* bin/campaign/harkonnen
cp resources/bin/campaign/maps/*.* bin/campaign/maps
cp resources/bin/campaign/maps/seed/*.* bin/campaign/maps/seed
cp resources/bin/campaign/briefings/*.* bin/campaign/briefings
cp resources/bin/campaign/info.txt bin/campaign/info.txt
echo "5. Creating new data directory"
mkdir bin/data
mkdir bin/data/scenes
mkdir bin/data/bmp
echo "6. Copying data files"
cp resources/bin/data/*.dat bin/data
cp resources/bin/data/*.fon bin/data
cp resources/bin/data/*.ttf bin/data
cp resources/bin/data/bmp/*.* bin/data/bmp
echo "7. Copying scenes"
cp resources/bin/data/scenes/*.* bin/data/scenes
echo "8. Create save game directories"
mkdir bin/save
mkdir bin/save/atreides
mkdir bin/save/harkonnen
mkdir bin/save/ordos
mkdir bin/save/skirmish
echo "9. Create skirmish directory"
mkdir bin/skirmish
cp resources/bin/skirmish/*.ini bin/skirmish
echo "10. Copying dll files"
echo "TODO TODO TODO"
echo "11. Copy executable"
cp build/d2tm bin
echo "12. Copy game rules file (game.ini)"
cp resources/game.ini.org bin/game.ini
cp resources/bin/settings.ini bin
echo "13. Copy txt files"
cp resources/bin/doc/versionhistory.txt bin
cp resources/bin/doc/controls.txt bin
cp resources/bin/d2tm.cfg bin
echo "Done."