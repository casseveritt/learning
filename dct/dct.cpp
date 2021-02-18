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
  //int size = w * h;
  pixel* pix = reinterpret_cast<pixel*>(img);
  for (int i = 0; i < h; i+=8) {
 	for (int j = 0; j < w; j+=8) {
 		pixel avgCol = pix[(i*w)+j];
		for(int l=1;l<64;l++) {
			int x = l%8, y = l/8;
    		avgCol.r = (avgCol.r + pix[((i+y)*w)+(j+x)].r)/2;
    		avgCol.g = (avgCol.g + pix[((i+y)*w)+(j+x)].g)/2;
    		avgCol.b = (avgCol.b + pix[((i+y)*w)+(j+x)].b)/2;
    		avgCol.a = (avgCol.a + pix[((i+y)*w)+(j+x)].a)/2;
  		}
  		for(int l=0;l<64;l++) {
			int x = l%8, y = l/8;
    		pix[((i+y)*w)+(j+x)].r = avgCol.r;
    		pix[((i+y)*w)+(j+x)].g = avgCol.g;
    		pix[((i+y)*w)+(j+x)].b = avgCol.b;
    		pix[((i+y)*w)+(j+x)].a = avgCol.a;
  		}
  	}
  }
  image_store_png("out.png", w, h, 4, img);
}
