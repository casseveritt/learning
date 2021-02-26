#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>

#include "stb.h"

#define PI 3.14

using namespace std;

struct RGBA8 {
   union {
       uint32_t i;
       struct {
           uint8_t r,g,b,a;
       };
    };
};

struct RGBA32F {
    float r,g,b,a;
};

template <typename T> struct Block8x8 {
   T element[8][8];
};

static Block8x8<float> DCTransform(Block8x8<RGBA8> inMat) {
	Block8x8<float> dctMat;

	double dct, sum, Cu, Cv;

	int i, j, u, v;

	float redLevel;

	for (u = 0; u < 8; ++u) {
		for (v = 0; v < 8; ++v) {

			if (u == 0) Cu = 1.0 / sqrt(2.0);
			else Cu = 1.0;
			if (v == 0) Cv = 1.0 / sqrt(2.0);
			else Cu = (1.0);  

			sum = 0.0;  

			for (i=0;i<8;i++) {
				for (j=0;j<8;j++) {

                    // Level around 0
                    double cosineWave = cos((2 * i + 1) * u * PI / 16.0) * cos((2 * j + 1) * v * PI / 16.0);
                    RGBA8 pixel = inMat.element[i][j];
					redLevel = pixel.r;
					dct = redLevel*cosineWave;
					sum += dct;

				}               
			}
			dctMat.element[u][v] = 0.25 * Cu * Cv * sum;
		}
	}
	return dctMat;
};

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
    // printf("warning: need 4-component pixels. n=%d\n", n);
  }
  RGBA8* pix = reinterpret_cast<RGBA8*>(img);
  for (int i = 0; i < h; i += 8) {
    for (int j = 0; j < w; j += 8) {
      RGBA8 avgCol = pix[(i * w) + j];
      Block8x8<RGBA8> macroblock;
      for (int ii = 0; ii < 8; ii++) {
      	for(int jj = 0; jj < 8; jj++) {
	        int coords = ((i + ii) * w) + (j + jj);
	        macroblock.element[jj][ii] = pix[coords];
	        avgCol.r = (avgCol.r + pix[coords].r) / 2;
	        avgCol.g = (avgCol.g + pix[coords].g) / 2;
	        avgCol.b = (avgCol.b + pix[coords].b) / 2;
	        avgCol.a = (avgCol.a + pix[coords].a) / 2;
      	}
      }
      for (int ii = 0; ii < 8; ii++) {
      	for(int jj = 0; jj < 8; jj++) {
	        pix[((i + ii) * w) + (j + jj)] = avgCol;
	    }
      }
    }
  }
  image_store_png("out.png", w, h, 4, img);
}
