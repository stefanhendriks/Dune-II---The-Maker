Readme for Dune 2 The Maker Map Editor, Version 0.935

This version of the map editor is NOT the final version, however
it is fully functional and should serve your needs. Please report
any bugs you find at the forums, or if you would like something
changed/improved/added that's of course also possible.

Next version will be 1.0, I will release it when the random map
generator is finished. It's giving me a pretty hard time :P



====================   INSTALLATION   ======================

The editor has been written in the Java Programming Language.
This means you will first need the Java runtime environment, you 
can download it for free at http://www.java.com

Now the other file included in this Zip file is the program, it
is a so-called Executable Jar File. Think of it as any other 
executable, only it requires Java to run. Place the program
in your D2TM directory so all maps will be saved/loaded in the
right directory and double click on the program to start it.


====================   KEY COMMANDS   ======================
There are a few key commands to streamline the creation process

SHIFT - Z:	Select Sand
SHIFT - X:	Select Rock
SHIFT - C:	Select Spice
SHIFT - V:	Select Mountains (infantry-only terrain)
SHIFT - B:	Select Dunes
SHIFT - N:	Select Spice Dunes
SHIFT - A:	Decrease Brush Size (min 1)
SHIFT - S:	Increase Brush Size (max 10)

CTRL - N:	Create New Map
CTRL - S:	Save Map
CTRL-SHIFT - S:	Save Map in a new File
CTRL - L:	Load Map
CTRL - G:	Randomize Map (not implemented yet)

These commands are also accessible by mouse, the key commands
are only for your convenience.

To Enable/Disable players or select them to change their 
position you will have to use the gui, as you won't do that 
very often and I was too lazy to make key commands for them :P


=======================   FIXES   ==========================
Version 0.935

New:	Apply button for Map Mirrorer.

Version 0.93

New:	Map mirror functionality. This allows for easy creation
	of symmetric maps.


Version 0.92

New:	Support for user-choseable filenames

Fixed:	One more save bug: When you use the 'new map' from the
	menu, and you saved another map before creating the new
	one, saving the new map would overwrite the other one.


Version 0.91

Fixed:	Bug in loading player start positions: X/Y positions
	were calculated using x=(cell%63) and y=(cell/63).
	This has been changed to x=(cell%64) and y=(cell/64),
	(now really fixed)

New:	It is now possible to place Spice Dunes.

New:	When placing Spice over Dunes or Spice Dunes, Spice 
	Dunes will be placed instead.


Version 0.9

Fixed:	Bug in saving: Player start positions were not 
	calculated from current map width, but default map
	width (63). This would give problems when saving maps
	where width != 63.

====================   KNOWN ISSUES   ======================

 - 	The map loader only works for 63x63 maps. Any other 
	will crash the program. Saving works for all sizes,
	but D2TM does not support any other size yet.
