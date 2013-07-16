#include <cstdlib>
#include "random.h"

int rnd(int max) {
  if (max < 1) return 0;
  return (rand() % max);
}

bool flipCoin() {
  return rnd(100) < 50;
}
