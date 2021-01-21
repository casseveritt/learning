#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include <bits/stdc++.h> 

#include <cstring>

/*
  You need dev packages to build and run this:
  sudo apt install libgles2-mesa-dev libglfw3-dev
*/

int nPrimes = 1000;
std::vector<int> primes;
int numThreads = 8;
std::atomic<int> num (2);
std::mutex m;

static bool checkPrime(int checkNum) {
  for (int i = 3; i < checkNum / 2; i += 2) {
    float quotient = checkNum;
    quotient /= i;
    if (quotient == (int)quotient) {
      return false;
    }
  }
  return true;
}

static int getNextNum() { // returns next number to check
  if(num.load() == 2) return num.fetch_add(1);
  return num.fetch_add(2);
}

static void pushPrime(int primeNum) { // pushes prime number onto list
  const std::lock_guard<std::mutex> lock(m);
  primes.push_back(primeNum);
  nPrimes--;
}

static void findPrimes() {
  while (nPrimes > 0) {
    int checkNum = getNextNum();
    if (checkPrime(checkNum)) {
      pushPrime(checkNum);
    }
  }
}

static double getTimeInSeconds() {
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return double(int64_t(ts.tv_sec) * int64_t(1e9) + int64_t(ts.tv_nsec)) * 1e-9;
}

int main(int argc, char** argv) {
  if (argc >= 2) {
    nPrimes = atoi(argv[1]);
  }
  if (argc == 3) {
    numThreads = atoi(argv[2]); // Set number of threads to make
  }
  int numPrimes = nPrimes;

  double t0 = getTimeInSeconds(); // Start time
  std::vector<std::thread> threads;
  threads.resize(numThreads);
  for (int i = 0; i < numThreads; i++) { // Initalize threads
    threads[i] = std::thread(findPrimes);
  }
  for (auto& thread : threads) { // Join threads
    if (thread.joinable()) {
      thread.join();
    }
  }
  double t1 = getTimeInSeconds(); // End time

  sort(primes.begin(), primes.end());
  for (int i = 0; i < numPrimes; i++) { // Print primes
    printf("%i", primes[i]);
    if (i != numPrimes - 1) {
      printf(", ");
    }
  }
  printf("\nThreads: %i\nTime in msec: %lf\n", numThreads, (t1 - t0) * 1000);

  return 0;
}
