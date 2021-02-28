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
   T& el(int i, int j) { return element[j][i]; }
   T el(int i, int j) const { return element[j][i]; }
};

static float dct2(int n, int k) {
	return (n == 0 ? 1.0f / sqrt(2.0f) : 1.0f ) * 0.5f * cos( (M_PI / 8.0) * (n + 0.5) * k);
}

static float dct3(int n, int k) {
	if (n == 0) {
		return 0.5 / sqrt(2.0f);
	}
	return 0.5f * cos( (M_PI / 8.0) * n * (k + 0.5));
}


static Block8x8<RGBA32F> DCTransform(Block8x8<RGBA8> spatialdom) {
	Block8x8<RGBA32F> freqdom;

	for (int u=0;u<8;++u) {
		for (int v=0;v<8;++v) {
			RGBA32F s = {};

			for (int i=0;i<8;i++) {
				float ci = dct2(i, u);
				for (int j=0;j<8;j++) {
					auto c = ci * dct2(j, v);
					auto& pix = spatialdom.el(i,j);
					s.r += pix.r * c;
					s.g += pix.g * c;
					s.b += pix.b * c;
					s.a += pix.a * c;
				}
			}
			freqdom.el(u,v) = s;
		}
	}
	return freqdom;
};

static Block8x8<RGBA8> IDCTransform(Block8x8<RGBA32F> freqdom) {
	Block8x8<RGBA8> spatialdom;

	for (int i=0;i<8;++i) {
		for (int j=0;j<8;++j) {
			RGBA32F s = {};

			for (int u=0;u<8;u++) {
				float cu = dct3(u, i);
				for (int v=0;v<8;v++) {
					float c =  cu * dct3(v, j);

					const auto& pix = freqdom.el(u, v);
					s.r += pix.r * c;
					s.g += pix.g * c;
					s.b += pix.b * c;
					s.a += pix.a * c;
				}
			}
			RGBA8 si;
			si.r = std::max(0.0f, std::min(255.0f, s.r));
			si.g = std::max(0.0f, std::min(255.0f, s.g));
			si.b = std::max(0.0f, std::min(255.0f, s.b));
			si.a = std::max(0.0f, std::min(255.0f, s.a));

			spatialdom.el(i,j) = si;
		}
	}
	return spatialdom;
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
      Block8x8<RGBA8> b;
      for (int ii = 0; ii < 8; ii++) {
      	for(int jj = 0; jj < 8; jj++) {
	        b.el(ii,jj) = pix[((i + ii) * w) + (j + jj)];
      	}
      }
      b = IDCTransform(DCTransform(b));
      for (int ii = 0; ii < 8; ii++) {
      	for(int jj = 0; jj < 8; jj++) {
	        pix[((i + ii) * w) + (j + jj)] = b.el(ii,jj);
	    }
      }
    }
  }
  image_store_png("out.png", w, h, 4, img);
}
