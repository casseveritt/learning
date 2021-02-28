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

					if (u == 0) Cu = sqrt(2.0);
					else Cu = 1.0;
					if (v == 0) Cv = sqrt(2.0);
					else Cv = (1.0);

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
			if (i == 6 && j == 6) outMat.element[i][j].a = 255;
		}
	}
	return outMat;
};

/*
static Block8x8<RGBA32F> DCTransform(Block8x8<RGBA8> inMat) {
	Block8x8<RGBA32F> dctMat;
	for (int u=0;u<8;++u) {
		for (int v=0;v<8;++v) {
			dctMat.element[u][v] = 0;
			for (int i=0;i<8;i++) {
				for (int j=0;j<8;j++) {
					dctMat.element[u][v] += inMat[i][j] * cos(M_PI/8*(i+0.5)*u)*cos(M_PI/8*(j+0.5)*v);
				}               
			}
		}
	}
	return dctMat;  
};

static Block8x8<RGBA8> IDCTransform(Block8x8<RGBA32F> dctMat) {
	Block8x8<RGBA8> outMat;
	for (int u=0;u<8;++u) {
		for (int v=0;v<8;++v) {
			outMat.element[u][v] = 0.25*dctMat.element[0][0];
			for(int i=1;i<8;i++) Matrix[u][v] += 0.5*DCTMatrix[i][0];
			for(int j=1;j<8;j++) Matrix[u][v] += 0.5*DCTMatrix[0][j];
			for (int i=1;i<8;i++) {
				for (int j=1;j<8;j++) {
					outMat.element[u][v] += dctMat.element[i][j] * cos(M_PI/8*(u+0.5)*i)*cos(M_PI/8*(v+0.5)*j);
				}
			}
			outMat.element[u][v] *= (0.25*0.25);
		}
	}
	return outMat;
};
*/

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
      Block8x8<RGBA8> macroblock;
      for (int ii = 0; ii < 8; ii++) {
      	for(int jj = 0; jj < 8; jj++) {
	        macroblock.element[jj][ii] = pix[((i + ii) * w) + (j + jj)];
      	}
      }
      macroblock = IDCTransform(DCTransform(macroblock));
      for (int ii = 0; ii < 8; ii++) {
      	for(int jj = 0; jj < 8; jj++) {
	        pix[((i + ii) * w) + (j + jj)] = macroblock.element[jj][ii];
	    }
      }
    }
  }
  image_store_png("out.png", w, h, 4, img);
}
