#include <stdio.h>

void print(int i, int j) {
  printf("(%d, %d) ", i, j);
}

bool outOfBounds(int i, int j, int max) {
  return i < 0 || max < i || j < 0 || max < j;
}

void step(int& i, int& j, bool iPosjNeg) {
    if (iPosjNeg) {
      i++;
      j--;
    } else {
      i--;
      j++;
    }
}

int main(int argc, char **argv) {
  int i = 0, j = 0;
  constexpr int max = 7;
  bool iPosjNeg = true;

  print(i, j);
  while( i < max || j < max) {
    step(i, j, iPosjNeg);
    if (outOfBounds(i, j, max)) {
      printf("\n");
      if (iPosjNeg) {
        i++;
      } else {
        j++;
      }
      iPosjNeg = !iPosjNeg;
      while(outOfBounds(i, j, max)) {
        step(i, j, iPosjNeg);
      }
    }
    print(i, j);
  }
  printf("\n");
  return 0;
}
