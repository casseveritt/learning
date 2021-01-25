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

// Very useful webpage:
// https://www.codeproject.com/Articles/570700/SquareplusRootplusalgorithmplusforplusC

double num = 8773;
double t0, t1, t2;

static double getTimeInSeconds() {
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return double(int64_t(ts.tv_sec) * int64_t(1e9) + int64_t(ts.tv_nsec)) * 1e-9;
}

static double powTen(int n) {
  double rst = 1.0;
  if (n >= 0) {
    for (int i = 0; i < n; i++) {
      rst *= 10.0;
    }
  } else {
    for (int i = 0; i < (0 - n); i++) {
      rst *= 0.1;
    }
  }
  return rst;
}

static double sqrRoot(double a) {
  double z = a, rst = 0.0, j = 1.0;
  int max = 24;                    // to define maximum digit
  for (int i = max; i > 0; i--) {  // value must be bigger then 0
    if (z - ((2 * rst) + (j * powTen(i))) * (j * powTen(i)) >= 0) {
      while (z - ((2 * rst) + (j * powTen(i))) * (j * powTen(i)) >= 0) {
        j++;
        if (j >= 10) break;
      }
      j--;                                                   // correct the extra value by minus one to j
      z -= ((2 * rst) + (j * powTen(i))) * (j * powTen(i));  // find value of z
      rst += j * powTen(i);                                  // find sum of a
      j = 1.0;
    }
  }

  for (int i = 0; i >= 0 - max; i--) {
    if (z - ((2 * rst) + (j * powTen(i))) * (j * powTen(i)) >= 0) {
      while (z - ((2 * rst) + (j * powTen(i))) * (j * powTen(i)) >= 0) j++;
      j--;
      z -= ((2 * rst) + (j * powTen(i))) * (j * powTen(i));  // find value of z
      rst += j * powTen(i);                                  // find sum of a
      j = 1.0;
    }
  }
  // find the number on each digit
  return rst;
}

int main(int argc, char** argv) {
  int opt = 0;
  while ((opt = getopt(argc, argv, "n:t:")) != -1) {
    switch (opt) {
      case 'n':
        if (optarg) num = atof(optarg);
        break;
      default: /* '?' */
        fprintf(stderr, "Usage: %s [-a algorithm (l|c|b)] [-n numPrimes] [-t numThreads]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  t0 = getTimeInSeconds();  // Start time
  double rootOut = sqrRoot(num);
  /*for (int i = 0; i < numThreads; i++) {  // Initalize threads
    threads[i] = std::thread(findPrimes);
  }
  for (auto& thread : threads) {  // Join threads
    if (thread.joinable()) {
      thread.join();
    }
  }*/
  t2 = getTimeInSeconds();  // End time

  /*for (int i = 0; i < numPrimes; i++) {  // Print primes
    printf("%i", primes[i]);
    if (i != numPrimes - 1) {
      printf(", ");
    }
  }*/
  // if ((int)primes.size() < numPrimes) printf("\nTimed out\nTotal primes found: %i", (int)primes.size());
  printf("Number: %lf\nRoot:\t\t%.24lf\nMath.h Root:\t%.24lf\nTime in msec: %lf\n", num, rootOut, sqrt(num), (t2 - t0) * 1000);

  return 0;
}
