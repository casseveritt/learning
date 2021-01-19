#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstring>

/*
  You need dev packages to build and run this:
  sudo apt install libgles2-mesa-dev libglfw3-dev
*/

int loop = 0;

int main(void) {
  int nPrimes = 10;
  std::vector<int> primes;
  int num = 3;  // Start checking with 3
  while (nPrimes > 0) {
    bool isPrime = true;
    for (int i = 2; i < num / 2; i++) {
    }
    if (isPrime) {
      primes.push_back(num);
      nPrimes--;
    }
    num += 2;  // Why bother checking even numbers?
    loop++;
  }

  for () {
  }

  return 0;
}
