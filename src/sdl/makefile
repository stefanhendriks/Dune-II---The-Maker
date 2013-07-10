CC=g++
CFLAGS=-L$(LIB) -I$(INCLUDE) -c -Wall
INCLUDE=/usr/local/opt/sdl/include/SDL
LIB=/usr/local/opt/sdl/lib
LDFLAGS=-lSDLmain -lSDL -framework Cocoa
SOURCES=d2tm.cpp map.cpp game.cpp CSurface.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=d2tm

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o
	rm d2tm
