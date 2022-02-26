CC=g++
CFLAGS= -lSDL2
DFLAGS= -g
DEPS = *.h
OBJ = main.o

%.o: %.c $(DEPS)
	$(CC) -c $@ $< $(DFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean :
	rm $(OBJ) main
