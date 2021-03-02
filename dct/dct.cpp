#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <cstring>

#include "linear.h"
#include "stb.h"

using namespace r3;
using namespace std;

/*
To Do -
Color space transformation	- Incomplete
Downsampling				- Incomplete
Block Splitting				- Incomplete
DCT 						- Completed
Quantization				- Incomplete
Entropy Coding				- Incomplete
*/

Matrix3f YCbCrConversion;
Matrix3f RGBConversion;

struct RGBA8 {
  union {
    uint32_t i;
    struct {
      uint8_t r, g, b, a;
    };
  };
};


struct YCbCrA {
	float y, cb, cr;
	uint8_t a;
};

template <typename RGBA>
static RGBA rgbasum(const RGBA& a, const RGBA& b) {
  RGBA d = a;
  d.r += b.r;
  d.g += b.g;
  d.b += b.b;
  d.a += b.a;
  return d;
}

template <typename RGBA>
static RGBA rgbadiff(const RGBA& a, const RGBA& b) {
  RGBA d = a;
  d.r -= b.r;
  d.g -= b.g;
  d.b -= b.b;
  d.a -= b.a;
  return d;
}

static RGBA8 operator+(const RGBA8& a, const RGBA8& b) {
  return rgbasum(a,b);
}

static RGBA8 operator-(const RGBA8& a, const RGBA8& b) {
  return rgbadiff(a,b);
}

struct RGBA32F {
  float r, g, b, a;
};

static RGBA32F operator+(const RGBA32F& a, const RGBA32F& b) {
  return rgbasum(a,b);
}

static RGBA32F operator-(const RGBA32F& a, const RGBA32F& b) {
  return rgbadiff(a,b);
}

static YCbCrA RGBA8toYCBCR(RGBA8 in){
	Vec3f rgb(in.r, in.g, in.b);
	Vec3f ycbcr = (YCbCrConversion * rgb) + Vec3f(16, 128, 128);
	YCbCrA out;
	out.y = ycbcr.x;
	out.cb = ycbcr.y;
	out.cr = ycbcr.z;
	out.a = in.a;
	return out;
};

static RGBA8 YCBCRtoRGB(YCbCrA in){
	Vec3f ycbcr(in.y, in.cb, in.cr);
	Vec3f rgb = RGBConversion * (ycbcr - Vec3f(16, 128, 128));
	RGBA8 out;
	out.r = rgb.x;
	out.g = rgb.y;
	out.b = rgb.z;
	out.a = in.a;
	return out;
};

template <typename T>
struct Block8x8 {
  T element[8][8];
  T& el(int i, int j) {
    return element[j][i];
  }
  T el(int i, int j) const {
    return element[j][i];
  }
};

template <int N>
static float dct2(int n, int k) {
  return cos((M_PI / N) * (n + 0.5) * k);
}

static Block8x8<RGBA32F> DCTransform(Block8x8<RGBA8> spatialdom) {
  Block8x8<RGBA32F> freqdom;

  for (int u = 0; u < 8; ++u) {
    float cu = (u == 0) ? 0.5f / sqrt(2.0f) : 0.5f;
    for (int v = 0; v < 8; ++v) {
      float cv = (v == 0) ? 0.5f / sqrt(2.0f) : 0.5f;
      RGBA32F s = {};

      for (int i = 0; i < 8; i++) {
        float ci = dct2<8>(i, u);
        for (int j = 0; j < 8; j++) {
          auto c = ci * dct2<8>(j, v);
          auto& pix = spatialdom.el(i, j);
          s.r += pix.r * c;
          s.g += pix.g * c;
          s.b += pix.b * c;
          s.a += pix.a * c;
        }
      }
      s.r *= cu * cv;
      s.g *= cu * cv;
      s.b *= cu * cv;
      s.a *= cu * cv;
      freqdom.el(u, v) = s;
    }
  }
  return freqdom;
};

static Block8x8<RGBA8> IDCTransform(Block8x8<RGBA32F> freqdom) {
  Block8x8<RGBA8> spatialdom;

  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      RGBA32F s = {};

      for (int u = 0; u < 8; u++) {
        float cu = (u == 0) ? 0.5 / sqrt(2.0f) : 0.5;
        for (int v = 0; v < 8; v++) {
          float cv = (v == 0) ? 0.5 / sqrt(2.0f) : 0.5;
          float c = cu * cv * dct2<8>(i, u) * dct2<8>(j, v);

          const auto& pix = freqdom.el(u, v);
          s.r += pix.r * c;
          s.g += pix.g * c;
          s.b += pix.b * c;
          s.a += pix.a * c;
        }
      }
      RGBA8 si;
      si.r = s.r + 0.5f;
      si.g = s.g + 0.5f;
      si.b = s.b + 0.5f;
      si.a = s.a + 0.5f;

      spatialdom.el(i, j) = si;

      if (i==0 && j==0) {
      	YCbCrA ycbcr = RGBA8toYCBCR(si);
      	RGBA8 rgbaBack = YCBCRtoRGB(ycbcr);
      	printf("R= %i\tY =%f\nG= %i\tCb=%f\nB= %i\tCr=%f\n\n", rgbaBack.r, ycbcr.y, rgbaBack.g, ycbcr.cb, rgbaBack.b, ycbcr.cr);
      }
    }
  }
  return spatialdom;
};

int main(int argc, char** argv) {
  YCbCrConversion.SetRow(0, Vec3f(0.258, 0.508, 0.0937));
  YCbCrConversion.SetRow(1, Vec3f(-0.148, -0.289, 0.445));
  YCbCrConversion.SetRow(2, Vec3f(0.445, -0.367, -0.071));
  RGBConversion = YCbCrConversion.Inverted();
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
  auto dif = new RGBA8[w*h]; 
  RGBA8 grey;
  grey.r = grey.g = grey.b = grey.a = 127;

  for (int i = 0; i < h; i += 8) {
    for (int j = 0; j < w; j += 8) {
      Block8x8<RGBA8> b;
      for (int ii = 0; ii < 8; ii++) {
        for (int jj = 0; jj < 8; jj++) {
          b.el(ii, jj) = pix[((i + ii) * w) + (j + jj)];
        }
      }
      b = IDCTransform(DCTransform(b));
      for (int ii = 0; ii < 8; ii++) {
        for (int jj = 0; jj < 8; jj++) {
          auto& ip = pix[((i + ii) * w) + (j + jj)];
          auto& dp = dif[((i + ii) * w) + (j + jj)];
          const auto& bp = b.el(ii,jj);
          dp = ip - bp + grey;
          dp.a = 255;
          ip = bp;
        }
      }
    }
  }
  image_store_png("out.png", w, h, 4, img);
  image_store_png("dif.png", w, h, 4, reinterpret_cast<unsigned char*>(dif));
  delete [] dif;
}
