bin_PROGRAMS = d2tm


# set the include path found by configure
INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/structures -I/usr/local/include

# the library search path.
d2tm_LDFLAGS = $(all_libraries) 
d2tm_CXXFLAGS = -DUNIX
d2tm_LDADD = -lfblend -lalleg-4.2.0 -lalleg_unsharable -lalfont -lalmp3 -lraknet

d2tm_SOURCES = ai.cpp bullet.cpp cGame_draw.cpp cGame_logic.cpp cMentat.cpp \
	cParticle.cpp cPlayer.cpp cStructure.cpp cUnit.cpp common.cpp d2tm_math.cpp gui.cpp \
	ini.cpp keyval.cpp main.cpp map.cpp mentat/cAtreidesMentat.cpp \
	mentat/cHarkonnenMentat.cpp mentat/cOrdosMentat.cpp network.cpp regions.cpp \
	structures/cBarracks.cpp structures/cConstYard.cpp structures/cGunTurret.cpp \
	structures/cHeavyFactory.cpp structures/cHighTech.cpp structures/cIx.cpp \
	structures/cLightFactory.cpp structures/cOutPost.cpp structures/cPalace.cpp structures/cRefinery.cpp \
	structures/cRepairFacility.cpp structures/cRocketTurret.cpp structures/cSpiceSilo.cpp \
	structures/cStarPort.cpp structures/cWindTrap.cpp structures/cWor.cpp \
	timers.cpp cTimeManager.cpp cDrawManager.cpp cMMEAllegro.cpp


