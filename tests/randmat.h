// generate some random matrices to test inversion

#include <stdlib.h>
#include <utility>

namespace {
float randfloat(float lower, float upper) {
  return (float(rand()) / RAND_MAX) * (upper - lower) - lower;
}

void swaprows(float *m) {
  int r = rand();
  int r1 = r & 0x3;
  int r2 = (r >> 2) & 0x3;
  if (r1 == r2) {
    return;
  }
  for (int i = 0; i < 4; i++) {
    std::swap(m[r1 * 4 + i], m[r2 * 4 + i]);
  }
}

} // namespace

// pass a pointer to 16 floats
void randmat(float *m) {
  int r = rand();
  if ((r % 1973) == 0) {
    for (int i = 0; i < 16; i++) {
      m[i] = (i & 3) == ((i >> 2) & 3) ? randfloat(-2.0f, 2.0f) : 0.0f;
    }
    if ((r % 17) == 0) {
      swaprows(m);
    }
    if ((r % 43) == 0) {
      swaprows(m);
    }
  } else {
    for (int i = 0; i < 16; i++) {
      m[i] = randfloat(-2.0f, 2.0f);
    }
  }

  if ((r % 73) == 0) {
    swaprows(m);
  }
}
