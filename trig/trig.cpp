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

static double toRadians(double degrees) {
  return degrees * (M_PI / 180.0f);
}
static double toDegrees(double radians) {
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

static double tsin(double t) {
  double theta = fmod(t, toRadians(360));
  double sinTheta = 0, posSum = 0, negSum = 0;
  for (int i = 381; i >= 1; i -= 4) {
    posSum += power(theta, i) / factorial(i);
  }
  for (int i = 383; i >= 3; i -= 4) {
    negSum += power(theta, i) / factorial(i);
  }
  sinTheta = posSum - negSum;
  return sinTheta;
}

static double tasin(double x) {
  double asinX = x;
  for (int i = 243; i >= 3; i -= 2) {
    double n = 1, d = 1;
    for (int j = 2; j <= (i - 1); j += 2) {
      n *= (j - 1);
      d *= j;
    }
    asinX += (n * power(x, i)) / (d * i);
  }
  return asinX;
}

static double tcos(double t) {
  double theta = fmod(t, toRadians(360));
  double cosTheta = 0, posSum = 0, negSum = 0;
  for (int i = 384; i >= 4; i -= 4) {
    posSum += power(theta, i) / factorial(i);
  }
  for (int i = 382; i >= 2; i -= 4) {
    negSum += power(theta, i) / factorial(i);
  }
  cosTheta = 1 + posSum - negSum;
  return cosTheta;
}

static double ttan(double theta) {
  return (tsin(theta) / tcos(theta));
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

  float radNum;

  for (int i = 0; i <= 360; i++) {
    radNum = toRadians(i);
    float tASin = tasin(tsin(radNum)), mASin = asin(sin(radNum));
    if (tASin != mASin) {
      printf("Theta: %i\tArcSin Diff:  %.24f\n", i, fabs(tASin - mASin));
    }
  }

  radNum = toRadians(num);
  float tSin = tsin(radNum), mSin = sin(radNum);
  float tCos = tcos(radNum), mCos = cos(radNum);
  float tTan = ttan(radNum), mTan = tan(radNum);
  float tASin = tasin(tsin(radNum)), mASin = asin(sin(radNum));

  printf("\nTheta: %f\n\n", num);
  printf("My Sin:  %.10f\nMath Sin:%.10f\n\n", tSin, mSin);
  printf("My Cos:  %.10f\nMath Cos:%.10f\n\n", tCos, mCos);
  printf("My Tan:  %.10f\nMath Tan:%.10f\n\n", tTan, mTan);
  printf("My ArcSin:  %.24f\nMath ArcSin:%.24f\n", tASin, mASin);

  return 0;
}
