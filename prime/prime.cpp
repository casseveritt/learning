#include <bits/stdc++.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>

/*
  You need dev packages to build and run this:
  sudo apt install libgles2-mesa-dev libglfw3-dev
*/

int nThreads = 1;         // Number of threads
int nPrimes = 1000;         // Number of primes to find
std::vector<int> primes;  // Vector containing all primes found
int num = 2;              // Start checking with 2
double t0, t1, t2;        // Time variables
std::mutex m;             // A lock for threads

static double getTimeInSeconds() {  // Returns time in seconds
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return double(int64_t(ts.tv_sec) * int64_t(1e9) + int64_t(ts.tv_nsec)) * 1e-9;
}

// Implement me
static bool isPrime(int number) {
    if (number == 2) {
      return true;
    }
    if ((number&1) == 0) {
      return false;
    }
    for (int i = 2; i <= sqrt(number); i++) {
      if (number % i == 0) {
        return false;
      }
    }
    return true;
}

int main(int argc, char** argv) {
  if (argc >= 2) {
    nPrimes = atoi(argv[1]);  // ./prime X-Sets nPrimes
  }
  if (argc == 3) {
    nThreads = atoi(argv[2]);  // ./prime x X-Sets nThreads
  }

  int numPrimes = nPrimes;           // Variable to remember total number of primes
  std::vector<std::thread> threads;  // Vector of threads

  t0 = getTimeInSeconds();  // Start Time
  while (nPrimes > 0) {     // Finds primes
    if (isPrime(num)) {
      primes.push_back(num);  // Pushes num onto the back of the primes vector
      nPrimes--;
    }
    num++;
  }
  t2 = getTimeInSeconds();  // End Time

  sort(primes.begin(), primes.end());
  primes.resize(numPrimes);
  for (int i = 0; i < 10; i++) {  // Prints out all primes found
    printf("%i", primes[i]);
    if (i != numPrimes - 1) {
      printf(", ");
    }
  }
  printf("\nTime in msec: %lf\tnumThreads: %i\n", (t2 - t0) * 1000, nThreads);

  return 0;
}