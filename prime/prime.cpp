#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <thread>
#include <algorithm>

#include <cstring>

/*
  You need dev packages to build and run this:
  sudo apt install libgles2-mesa-dev libglfw3-dev
*/

int nPrimes;
int primes [100];

static void findPrimes(int num, int mov) {
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
      primes[nPrimes-1] = num;
      nPrimes--;
    }
    num += mov;
  }
}

int main(void) {
  nPrimes = sizeof(primes)/sizeof(primes[0]);

  std::thread thread1(findPrimes, 3, 4);
  std::thread thread2(findPrimes, 5, 4);

  thread1.join();
  thread2.join();

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
