#include <bits/stdc++.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <atomic>
#include <cmath>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>

/*
  You need dev packages to build and run this:
  sudo apt install libgles2-mesa-dev libglfw3-dev
*/

double t0, t1;

static double getTimeInSeconds() {
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return double(int64_t(ts.tv_sec) * int64_t(1e9) + int64_t(ts.tv_nsec)) * 1e-9;
}

static float toRadians(float degrees) {
  return degrees * (M_PI / 180.0f);
}
static float toDegrees(float radians) {
  return radians * (180.0f / M_PI);
}

static int factorial(int n) {
  int out = 1, temp = n;
  while (temp > 1) {
    out *= temp;
    temp--;
  }
  return out;
}

static float tsin(float theta) {
  float sineTheta = theta;
  bool sign = false;
  for (int i = 3; i < 20; i += 2) {
    if (sign)
      sineTheta += (pow(theta, i) / factorial(i));
    else
      sineTheta -= (pow(theta, i) / factorial(i));
    sign = !sign;
  }
  return sineTheta;
}

int main(int argc, char** argv) {
  int opt = 0;
  while ((opt = getopt(argc, argv, "n:")) != -1) {
    switch (opt) {
      case 'n':
        // if (optarg) num = atof(optarg);
        break;
      default: /* '?' */
        fprintf(stderr, "Usage: %s [-a algorithm (l|c|b)] [-n numPrimes] [-t numThreads]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  float theta = 40;

  printf("Theta: %f\n\nMy Sin:  %.10f\nMath Sin:%.10f\n", theta, tsin(toRadians(theta)), sin(toRadians(theta)));

  return 0;
}
