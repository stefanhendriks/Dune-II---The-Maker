# Builds seed2map, the Dune II seed to map generator. Needs icon.map in the working directory to run.
# The .C sources use bool without stdbool.h, so they are built as C++.
CXX ?= g++
CXXFLAGS ?= -O2
# the tool's own random() (RAND.C) clashes with libc random() on Linux/macOS, so rename it
override CXXFLAGS += -w -Wno-narrowing -Drandom=seed_random

OBJS = MAIN.o MAPIO.o RAND.o SEED.o
TARGET = seed2map

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.C
	$(CXX) $(CXXFLAGS) -x c++ -c -o $@ $<

MAIN.o MAPIO.o SEED.o: SEED.H
MAIN.o SEED.o: SEED_PROTOS.H
MAIN.o MAPIO.o: MAPIO_PROTOS.H

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe

.PHONY: clean
