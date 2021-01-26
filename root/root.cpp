#include <bits/stdc++.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <atomic>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>

/*
  You need dev packages to build and run this:
  sudo apt install libgles2-mesa-dev libglfw3-dev
*/

double num = 8773;
int iter = 0;
double t0, t1, t2;
union finley {
  float f;
  uint32_t i;
};
finley flt;

static double getTimeInSeconds() {
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return double(int64_t(ts.tv_sec) * int64_t(1e9) + int64_t(ts.tv_nsec)) * 1e-9;
}

#define T double
static T simpleSqrt(T n) {
  flt.f = n;
  flt.i = (flt.i & 0x807fffff) | ((((((flt.i >> 23) & 0xff) - 127) / 2) + 127) << 23);
  T temp = 0;
  T root = flt.f;
  int lociter = 0;
  while ((temp != root) && (fabs((root * root) - n) > 1e-14)) {
    temp = root;
    root = (n / temp + temp) / 2;
    lociter++;
    if (lociter > 1000) {
      printf("root = %lf, n = %lf, r*r = %.16lf, (r*r-n) = %le\n", root, n, root * root, root * root - n);
      if (lociter > 1020) {
        exit(1);
      }
    }
  }
  iter += lociter;
  return root;
}

int main(int argc, char** argv) {
  int opt = 0;
  while ((opt = getopt(argc, argv, "n:f:")) != -1) {
    switch (opt) {
      case 'n':
        if (optarg) num = atof(optarg);
        break;
      default: /* '?' */
        fprintf(stderr, "Usage: %s [-a algorithm (l|c|b)] [-n numPrimes] [-t numThreads]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  double executeTime0, executeTime1;

  auto loopy = [](T (*func)(T n)) {
    T v;
    for (int i = 0; i < 1000; i++) {
      v = func(num);
    }
    return v;
  };

  t0 = getTimeInSeconds();  // Start time
  double simpRoot = loopy(simpleSqrt);
  t1 = getTimeInSeconds();  // End time
  executeTime0 = (t1 - t0) * 1000;

  t0 = getTimeInSeconds();  // Start time
  double mathRoot = loopy(sqrt);
  t1 = getTimeInSeconds();  // End time
  executeTime1 = (t1 - t0) * 1000;

  printf("Number:\t%lf\n\nSimp:\t%lf\nIterations: %i\nMath:\t%lf\n\n", num, simpRoot, iter / 1000, mathRoot);
  printf("Simp time in msec: %lf\nMath time in msec: %lf\n", executeTime0, executeTime1);

  return 0;
}
