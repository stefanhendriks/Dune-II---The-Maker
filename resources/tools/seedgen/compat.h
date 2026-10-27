/* Force-included by the Makefile. The tool's own random() (RAND.C) clashes with libc random() on
   Linux/macOS. Pull in libc first, then rename ours, so the sources can stay untouched. */
#include <stdlib.h>
#include <stdio.h>
#define random seed_random
