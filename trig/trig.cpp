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

#define tPi 3.1415926535897932384626433832795

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

static double tfloor(double f) {
  union {
    double d;
    uint64_t u;
  };
  d = f;
  u = u & ~(1ULL << 63);
  d -= (d - uint64_t(d));
  if (f >= 0) {
    return d;
  } else {
    return -d - 1;
  }
}

static double tfmod(double t, double divisor) {
  t /= divisor;
  t -= tfloor(t);
  return t * divisor;
}

static double tsin(double t) {
  double theta = tfmod(t, toRadians(360));
  double sinTheta = 0;
  for (int i = 43; i >= 3; i -= 4) {
    sinTheta -= power(theta, i) / factorial(i);
    sinTheta += power(theta, i - 2) / factorial(i - 2);
  }
  return sinTheta;
}

static double tasin(double x) {  // Tried taylor-series
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
  double theta = tfmod(t, toRadians(360));
  double cosTheta = 1;
  for (int i = 384; i >= 4; i -= 4) {
    cosTheta += power(theta, i) / factorial(i);
    cosTheta -= power(theta, (i - 2)) / factorial((i - 2));
  }
  return cosTheta;
}

static double ttan(double t) {
  double theta = tfmod(t, toRadians(360));
  double tanTheta = theta;
  for (int i = 203; i >= 3; i -= 2) {
    // if (t == toRadians(716)) printf("tanTheta += (%i^%i / %i!) * %lf^%i\n", 2, (i-2), i, theta, i);
    tanTheta += (power(2, (i - 2)) / factorial(i)) * power(theta, i);
  }
  // if (t == toRadians(716)) printf("tanTheta = %lf\n", tanTheta);
  return tanTheta;
  // return tsin(t) / tcos(t);
}

static double tatan(double t) {  // Tried taylor-series and horners method
  double theta = tfmod(t, tan(toRadians(360)));
  double atanTheta = theta, posSum = 0, negSum = 0;
  for (int i = 45; i >= 5; i -= 4) {
    posSum += pow(theta, i) / i;
  }
  for (int i = 43; i >= 3; i -= 4) {
    negSum += pow(theta, i) / i;
  }
  atanTheta += (posSum - negSum);
  return atanTheta;
}

// I am struggling to find a functioning method for finding inverse trig functions, or I am at least
// not implementing them effectively, I have tried the taylor-series and horners method so far.

int main(int argc, char** argv) {
  int opt = 0;
  while ((opt = getopt(argc, argv, "n:")) != -1) {
    switch (opt) {
      case 'n':
        if (optarg) num = atof(optarg);
        break;
      default: /* '?' */
        fprintf(stderr, "Usage: %s [-n number]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  double se = 0, te = 0;

  for (int i = -720; i <= 720; i += 4) {
    double r = toRadians(i);
    double t = tan(r);
    double dat = atan(t);
    dat -= tatan(t);
    dat = fabsf(dat);
    double dt = tan(r);
    // if (i == 716) printf("r = %lf\n", r);
    dt -= ttan(r);
    // if (i == 716) printf("Math tan = %lf\n", tan(r));
    dt = fabsf(dt);
    te += dt;
    double ds = sin(r);
    ds -= tsin(r);
    ds = fabsf(ds);
    se += ds;
    double dc = cos(r);
    dc -= tcos(r);
    dc = fabsf(dc);
    // if (dat != 0.0 || dt != 0.0 || ds != 0.0 || dc != 0.0) {
    printf("Theta: %i\tdiffs: at: %e t: %e s: %e c: %e\n", i, dat, dt, ds, dc);
    //}
  }

  printf("\nse=%le\nte=%le\n", se, te);

  double radNum = toRadians(num);
  double tSin = tsin(radNum), mSin = sin(radNum);
  // tSin -= 0.7071067811865475244008443621048490392848359376884740365883398689;
  // mSin -= 0.7071067811865475244008443621048490392848359376884740365883398689;
  float tCos = tcos(radNum), mCos = cos(radNum);
  float tTan = ttan(radNum), mTan = tan(radNum);
  float tATan = tatan(tan(radNum)), mATan = atan(tan(radNum));

  printf("\nTheta: %f\n\n", num);
  printf("My Sin:  %e\nMath Sin:%e\n\n", tSin, mSin);
  printf("My Cos:  %.10f\nMath Cos:%.10f\n\n", tCos, mCos);
  printf("My Tan:  %.10f\nMath Tan:%.10f\n\n", tTan, mTan);
  printf("My ArcTan:  %.24f\nMath ArcTan:%.24f\n", tATan, mATan);

  return 0;
}