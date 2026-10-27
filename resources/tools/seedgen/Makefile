# Builds seed2map, the Dune II seed to map generator.
# The .C sources use bool without stdbool.h, so they are built as C++.
CXX ?= g++
CXXFLAGS ?= -O2
override CXXFLAGS += -w -Wno-narrowing -include compat.h

OBJS = main.o mapio.o rand.o seed.o
TARGET = seed2map

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CXX) $(CXXFLAGS) -x c++ -c -o $@ $<

main.o mapio.o rand.o seed.o: compat.h
main.o mapio.o seed.o: seed.h
main.o seed.o: seed_protos.h
main.o mapio.o: mapio_protos.h

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe

.PHONY: clean
