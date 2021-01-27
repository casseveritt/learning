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

float num = 90;
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

static float tsin(float t) {
  float theta = t;
  float sinTheta = theta;
  bool sign = false;
  for (int i = 3; i < 14; i += 2) {
    double numPow = pow(theta, i);
    double numFac = factorial(i);
    // printf("Pow: %f\nFac: %f\n",numPow, numFac);
    if (sign)
      sinTheta += (numPow / numFac);
    else
      sinTheta -= (numPow / numFac);
    sign = !sign;
  }
  return sinTheta;
}

static float tcos(float t) {
  float theta = t;
  double cosTheta = 1;
  bool sign = false;
  for (int i = 2; i < 13; i += 2) {
    double numPow = pow(theta, i);
    double numFac = factorial(i);
    if (sign)
      cosTheta += (numPow / numFac);
    else
      cosTheta -= (numPow / numFac);
    sign = !sign;
  }
  return cosTheta;
}

static float ttan(float theta) {
  return tsin(theta) / tcos(theta);
}

int main(int argc, char** argv) {
  int opt = 0;
  while ((opt = getopt(argc, argv, "n:")) != -1) {
    switch (opt) {
      case 'n':
        if (optarg) num = atof(optarg);
        break;
      default: /* '?' */
        fprintf(stderr, "Usage: %s [-a algorithm (l|c|b)] [-n numPrimes] [-t numThreads]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  float radNum = toRadians(num);

  printf("Theta: %f\n\n", num);
  printf("My Sin:  %f\nMath Sin:%f\n\n", tsin(radNum), sin(radNum));
  printf("My Cos:  %f\nMath Cos:%f\n\n", tcos(radNum), cos(radNum));
  printf("My Tan:  %f\nMath Tan:%f\n", ttan(radNum), tan(radNum));

  return 0;
}
