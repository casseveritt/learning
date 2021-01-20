#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <thread>
#include <algorithm>
#include <mutex>

#include <cstring>

/*
  You need dev packages to build and run this:
  sudo apt install libgles2-mesa-dev libglfw3-dev
*/

int nPrimes;
int primes [100];
int numThreads = 4;
std::mutex m;

static void findPrimes(int num, int mov) {
  int findPrim = nPrimes;
  while (nPrimes > 0) {
    bool isPrime = true;
    for (int i=3;i<num/2;i+=2) {
      float out = num;
      out /= i;
      if(out == (int)out) {
        isPrime = false;
        break;
      }
    }
    if (isPrime) {
      m.try_lock(); // Tries to lock other threads from altering primes list anc count
      primes[nPrimes-1] = num;
      nPrimes--;
      m.unlock(); // Unlocks
    }
    num += mov;
  }
}

int main(void) {
  nPrimes = sizeof(primes)/sizeof(primes[0]);

  std::vector<std::thread> threads;

  for(int i=0;i<numThreads;i++) {
    threads.push_back(std::thread(findPrimes, 3 + (2*i), (2*numThreads)));
  }

  for(auto& thr : threads) {
    thr.join();
  }

  int len = sizeof(primes)/sizeof(primes[0]);
  std::sort(primes, primes+len);
  for (int i=0;i<len;i++) {
    printf("%i", primes[i]);
    if(i!=len-1) {
      printf(", ");
    }
  }

  printf("\n");

  return 0;
}
