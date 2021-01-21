#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mutex>
#include <thread>
#include <vector>

#include <cstring>

/*
  You need dev packages to build and run this:
  sudo apt install libgles2-mesa-dev libglfw3-dev
*/

int nPrimes = 10;
std::vector<int> primes;
int numThreads = 4;
int num = 3;
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

static void findPrimes() {
  int checkNum;
  bool locked = false;
  while (nPrimes > 0) {
    while (!locked) {
      locked = m.try_lock();
    }
    checkNum = num;
    num += 2;
    m.unlock();
    locked = false;

    bool isPrime = checkPrime(checkNum);

    if (isPrime && nPrimes > 0) {
      while (!locked) {
        locked = m.try_lock();  // Tries to lock other threads from altering primes list anc count
      }
      primes.push_back(checkNum);
      nPrimes--;
      m.unlock();  // Unlocks
      locked = false;
    }
  }
}

static double GetTimeInSeconds() {
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return double(int64_t(ts.tv_sec) * int64_t(1e9) + int64_t(ts.tv_nsec)) * 1e-9;
}

int main(int argc, char** argv) {
  double t0 = GetTimeInSeconds();

  if (argc >= 2) {
    nPrimes = atoi(argv[1]);
  }
  if (argc == 3) {
    numThreads = atoi(argv[2]);
  }

  std::vector<std::thread> threads;
  int numPrimes = nPrimes;

  threads.resize(numThreads);
  for (int i = 0; i < numThreads; i++) {
    threads[i] = std::thread(findPrimes);
  }
  for (auto& thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }

  double t1 = GetTimeInSeconds();

  for (int i = 0; i < numPrimes; i++) {
    printf("%i", primes[i]);
    if (i != numPrimes - 1) {
      printf(", ");
    }
  }
  printf("\nTime in msec: %lf\n", (t1 - t0) * 1000);

  return 0;
}
