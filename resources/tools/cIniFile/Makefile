CC=g++
CFLAGS= 
DFLAGS= -g
DEPS = *.h
OBJ = cIniFile.o main.o

%.o: %.c $(DEPS)
	$(CC) -c $@ $< $(DFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean :
	rm $(OBJ) main
