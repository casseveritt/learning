#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>

#include "stb.h"

using namespace std;

int main(int argc, char** argv) {
  string imgName = "Lenna.png";
  if (argc >= 2) imgName = argv[1];

  int w, h, n;
  const char* im = "images/";
  char* imgLocation = new char[strlen(im) + imgName.size() + 1];
  strcpy(imgLocation, im);
  strcat(imgLocation, imgName.c_str());

  unsigned char* img;

  img = image_load(imgLocation, &w, &h, &n);
  if (n != 4) {
    printf("warning: need 4-component pixels. n=%d\n", n);
  }
  union pixel {
    uint32_t i;
    struct {
      uint8_t r;
      uint8_t g;
      uint8_t b;
      uint8_t a;
    };
  };
  int size = w * h;
  pixel* pix = reinterpret_cast<pixel*>(img);
  for (int i = 0; i < size; i++) {
    pix[i].r = 0;
  }
  image_store_png("out.png", w, h, 4, img);
}
