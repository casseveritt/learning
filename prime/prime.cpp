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

int numThreads = 8;
int nPrimes = 1000000;
std::vector<int> primes;
std::atomic<int> num(2);
std::mutex m;
double t0, t1, t2;

static double getTimeInSeconds() {
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return double(int64_t(ts.tv_sec) * int64_t(1e9) + int64_t(ts.tv_nsec)) * 1e-9;
}

static bool checkPrime(int checkNum) {
  int numSqrt = sqrt(checkNum);
  for (int i = 3; i <= numSqrt; i += 2) {
    if (checkNum % i == 0) {
      return false;
    }
  }
  return true;
}

static int getNextNum() {  // returns next number to check
  if (num.load() == 2) return num.fetch_add(1);
  return num.fetch_add(2);
}

static void pushPrime(int primeNum) {  // pushes prime number onto list
  const std::lock_guard<std::mutex> lock(m);
  primes.push_back(primeNum);
  nPrimes--;
}

static void findPrimes() {
  int checkPrimeCalls = 0;
  int primesFound = 0;
  while (nPrimes > 0) {
    t1 = getTimeInSeconds();
    if ((t1 - t0) >= 15) break;
    int checkNum = getNextNum();
    checkPrimeCalls++;
    if (checkPrime(checkNum)) {
      primesFound++;
      pushPrime(checkNum);
    }
  }
  // Print thread work load.
  printf("checkPrimeCalls: %i\nPrimes Found: %i\n\n", checkPrimeCalls, primesFound);
}

int main(int argc, char** argv) {
  int opt = 0;
  while ((opt = getopt(argc, argv, "n:t:")) != -1) {
    switch (opt) {
      case 'n':
        if (optarg) nPrimes = atoi(optarg);
        break;
      case 't':
        if (optarg) numThreads = atoi(optarg);
        break;
      default: /* '?' */
        fprintf(stderr, "Usage: %s [-n numPrimes] [-t numThreads]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }
  int numPrimes = nPrimes;

  t0 = getTimeInSeconds();  // Start time
  std::vector<std::thread> threads;
  threads.resize(numThreads);
  for (int i = 0; i < numThreads; i++) {  // Initalize threads
    threads[i] = std::thread(findPrimes);
  }
  for (auto& thread : threads) {  // Join threads
    if (thread.joinable()) {
      thread.join();
    }
  }
  t2 = getTimeInSeconds();  // End time

  sort(primes.begin(), primes.end());
  if (nPrimes <= 0) primes.resize(numPrimes);
  /*for (int i = 0; i < numPrimes; i++) {  // Print primes
    printf("%i", primes[i]);
    if (i != numPrimes - 1) {
      printf(", ");
    }
  }*/
  if ((int)primes.size() < numPrimes) printf("\nTimed out\nTotal primes found: %i", (int)primes.size());
  printf("\nMax Prime: %i", primes.back());
  printf("\nThreads: %i\nTime in msec: %lf\n", numThreads, (t2 - t0) * 1000);

  return 0;
}
