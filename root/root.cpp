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

static double simpleSqrt(double n) {
  flt.f = n;
  flt.i = (flt.i & 0x807fffff) | ((((((flt.i >> 23) & 0xff) - 127) / 2) + 127) << 23);
  double temp = 0, root = flt.f;
  while (root != temp && (root * root != n)) {
    temp = root;
    root = (n / temp + temp) / 2;
    iter++;
  }
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

  t0 = getTimeInSeconds();  // Start time
  double simpRoot = simpleSqrt(num);
  t1 = getTimeInSeconds();  // End time
  executeTime0 = (t1 - t0) * 1000;

  t0 = getTimeInSeconds();  // Start time
  double mathRoot = sqrt(num);
  t1 = getTimeInSeconds();  // End time
  executeTime1 = (t1 - t0) * 1000;

  printf("Number:\t%lf\n\nSimp:\t%lf\nIterations: %i\nMath:\t%lf\n\n", num, simpRoot, iter, mathRoot);
  printf("Simp time in msec: %lf\nMath time in msec: %lf\n", executeTime0, executeTime1);

  return 0;
}
