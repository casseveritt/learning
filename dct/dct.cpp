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

  int mbd = 8;          // Macroblock dimensions
  int mbv = mbd * mbd;  // Macroblock volume

  int w, h, n;
  const char* im = "images/";
  char* imgLocation = new char[strlen(im) + imgName.size() + 1];
  strcpy(imgLocation, im);
  strcat(imgLocation, imgName.c_str());

  unsigned char* img;

  img = image_load(imgLocation, &w, &h, &n);
  if (n != 4) {
    // printf("warning: need 4-component pixels. n=%d\n", n);
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
  pixel* pix = reinterpret_cast<pixel*>(img);
  for (int i = 0; i < h; i += mbd) {
    for (int j = 0; j < w; j += mbd) {
      pixel avgCol = pix[(i * w) + j];
      for (int l = 1; l < mbv; l++) {
        int x = l % mbd, y = l / mbd;
        int coords = ((i + y) * w) + (j + x);
        avgCol.r = (avgCol.r + pix[coords].r) / 2;
        avgCol.g = (avgCol.g + pix[coords].g) / 2;
        avgCol.b = (avgCol.b + pix[coords].b) / 2;
        avgCol.a = (avgCol.a + pix[coords].a) / 2;
        float cx, cy;
        if (x == 0)
          cx = 1 / sqrt(mbd);
        else
          cx = sqrt(2) / sqrt(mbd);
        if (y == 0)
          cy = 1 / sqrt(mbd);
        else
          cy = sqrt(2) / sqrt(mbd);
        // dct[x][y] = cx * cy (sum(k=0 to m-1) sum(l=0 to n-1) pix[coords] * cos((2*k+1) *i*pi/2*m) * cos((2*l+1) *j*pi/2*n)
      }
      for (int l = 0; l < mbv; l++) {
        int x = l % mbd, y = l / mbd;
        pix[((i + y) * w) + (j + x)] = avgCol;
      }
    }
  }
  image_store_png("out.png", w, h, 4, img);
}
