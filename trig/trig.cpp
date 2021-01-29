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

static double factorial(int n) {
  double f = 1;
  for (int i = 2; i <= n; i++) {
    f *= i;
  }
  return f;
}

static double power(double n, int e) {
  double p = n;
  for (int i = 2; i <= e; i++) {
    p *= n;
  }
  return p;
}

static double tsin(float t) {
  float theta = fmod(t, toRadians(360));
  double sinTheta = 0, posSum = 0, negSum = 0;
  for (int i = 1001; i >= 1; i -= 4) {
    posSum += power(theta, i) / factorial(i);
  }
  for (int i = 1003; i >= 3; i -= 4) {
    negSum += power(theta, i) / factorial(i);
  }
  sinTheta = posSum - negSum;
  return (float)sinTheta;
}

static double tasin(float x) {
  double asinX = x;
  for (int i = 243; i >= 3; i -= 2) {
    double n = 1, d = 1;
    for (int j = i - 1; j >= 2; j -= 2) {
      n *= (j - 1);
      d *= j;
    }
    // printf("%lf / %lf\n", n, (d*i));
    asinX += (n * power(x, i)) / (d * i);
  }
  return (float)asinX;
}

static double tcos(float t) {
  float theta = fmod(t, toRadians(360));
  double cosTheta = 0, posSum = 0, negSum = 0;
  for (int i = 1004; i >= 4; i -= 4) {
    posSum += power(theta, i) / factorial(i);
  }
  for (int i = 1002; i >= 2; i -= 4) {
    negSum += power(theta, i) / factorial(i);
  }
  cosTheta = 1 + posSum - negSum;
  return (float)cosTheta;
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
  printf("My Sin:  %.10f\nMath Sin:%.10f\n\n", tsin(radNum), sin(radNum));
  printf("My Cos:  %.10f\nMath Cos:%.10f\n\n", tcos(radNum), cos(radNum));
  printf("My Tan:  %.10f\nMath Tan:%.10f\n\n", ttan(radNum), tan(radNum));
  printf("My ArcSin:  %.24f\nMath ArcSin:%.24f\n", tasin(tsin(radNum)), asin(sin(radNum)));

  return 0;
}
