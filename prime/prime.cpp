#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstring>

/*
  You need dev packages to build and run this:
  sudo apt install libgles2-mesa-dev libglfw3-dev
*/

int loop = 0;
int primesList = 10;

int main(void) {
  int nPrimes = primesList;
  int primes [nPrimes];
  int num = 3;  // Start checking with 3
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
    num += 2;  // Why bother checking even numbers?
    loop++;
  }

  for (int i=primesList-1;i>-1;i--) {
    printf("%i", primes[i]);
    if(i!=0) {
      printf(", ");
    }
  }

  printf("\n");

  return 0;
}
