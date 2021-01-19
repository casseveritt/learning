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
  int primes [nPrimes];
  int num = 3; // Start checking with 3
  while (nPrimes > 0){
    bool isPrime = true;
    for(int i=3;i<num/2;i++){

    }
    num+=2; // Why even check even numbers
    loop++;
  }

  

  return 0;
}