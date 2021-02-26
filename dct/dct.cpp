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

static Block8x8<RGBA32F> DCTransform(Block8x8<RGBA8> inMat) {
	Block8x8<RGBA32F> dctMat;

	for (int u=0;u<8;++u) {
		for (int v=0;v<8;++v) {
			double Cu, Cv;
			double sumr, sumg, sumb, suma;
			sumr = sumg = sumb = suma = 0.0;

			if (u == 0) Cu = 1.0 / sqrt(2.0);
			else Cu = 1.0;
			if (v == 0) Cv = 1.0 / sqrt(2.0);
			else Cv = (1.0);  

			for (int i=0;i<8;i++) {
				for (int j=0;j<8;j++) {
					double cosineWave = cos((2 * i + 1) * u * PI / 16.0) * cos((2 * j + 1) * v * PI / 16.0);
                    // Level around 0
                    RGBA8 pixel = inMat.element[i][j];
					sumr += pixel.r*cosineWave;
					sumg += pixel.g*cosineWave;
					sumb += pixel.b*cosineWave;
					suma += pixel.a*cosineWave;
				}
			}
			dctMat.element[u][v].r = (0.25 * Cu * Cv * sumr);
			dctMat.element[u][v].g = (0.25 * Cu * Cv * sumg);
			dctMat.element[u][v].b = (0.25 * Cu * Cv * sumb);
			dctMat.element[u][v].a = (0.25 * Cu * Cv * suma);
		}
	}
	return dctMat;
};

static Block8x8<RGBA8> IDCTransform(Block8x8<RGBA32F> dctMat) {
	Block8x8<RGBA8> outMat;

	for (int i=0;i<8;++i) {
		for (int j=0;j<8;++j) { 
			double Cu, Cv;
			double sumr, sumg, sumb, suma;
			sumr = sumg = sumb = suma = 0.0;

			for (int u=0;u<8;u++) {
				for (int v=0;v<8;v++) {
					double cosineWave = cos((2 * i + 1) * u * PI / 16.0) * cos((2 * j + 1) * v * PI / 16.0);

					if (u == 0) Cu = 1.0;
					else Cu = 1.0 / sqrt(2.0);
					if (v == 0) Cv = 1.0;
					else Cv = (1.0 / sqrt(2.0));

					// Level around 0

					RGBA32F dctElement = dctMat.element[u][v];
					sumr += dctElement.r * cosineWave;
					sumg += dctElement.g * cosineWave;
					sumb += dctElement.b * cosineWave;
					suma += dctElement.a * cosineWave;
				}
			}
			outMat.element[i][j].r = (0.25 * Cu * Cv * sumr);
			outMat.element[i][j].g = (0.25 * Cu * Cv * sumg);
			outMat.element[i][j].b = (0.25 * Cu * Cv * sumb);
			outMat.element[i][j].a = (0.25 * Cu * Cv * suma);
		}
	}
	return outMat;
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
      Block8x8<RGBA32F> dctMat = DCTransform(macroblock);
      macroblock = IDCTransform(dctMat);
      for (int ii = 0; ii < 8; ii++) {
      	for(int jj = 0; jj < 8; jj++) {
	        pix[((i + ii) * w) + (j + jj)] = macroblock.element[jj][ii];
	    }
      }
    }
  }
  image_store_png("out.png", w, h, 4, img);
}
