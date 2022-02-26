CC=g++
CFLAGS= -lSDL2
DEPS = *.h
OBJ = main.o

%.o: %.c $(DEPS)
	$(CC) -c -g -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean :
	rm $(OBJ) main
