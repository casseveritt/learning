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

int nPrimes = 4;
std::vector<int> primes;
int numThreads = 4;
int num = 3;
std::mutex m;

static void findPrimes() {
  //printf("Made thread\n");
  int checkNum;
  bool locked = false;
  while (nPrimes > 0) {
    while(!locked){
      locked = m.try_lock();
    }
    checkNum = num;
    num += 2;
    m.unlock();
    locked = false;

  //printf("got here: %d\n", __LINE__);
    bool isPrime = true;
    for (int i=3;i<checkNum/2;i+=2) {
      float out = checkNum;
      out /= i;
      if(out == (int)out) {
        isPrime = false;
        break;
      }
    }

    if (isPrime) {
      while(!locked){
        locked = m.try_lock(); // Tries to lock other threads from altering primes list anc count
      }
      primes.push_back(checkNum);
      nPrimes--;
      m.unlock(); // Unlocks
      locked = false;
    }
  }
}

int main(void) {
  std::vector<std::thread> threads;
  int numPrimes = nPrimes;

  threads.resize(numThreads);
  for(int i=0;i<numThreads;i++) {
    threads[i] = std::thread(findPrimes);
  }
  for(auto& thread : threads) {
      //printf("joining %p", &thread);
    if (thread.joinable()) {
      thread.join();
    }
    //printf(" joined\n");
  }

  for (int i=0;i<numPrimes;i++) {
    printf("%i", primes[i]);
    if(i!=numPrimes-1) {
      printf(", ");
    }
  }

  printf("\n");

  return 0;
}
