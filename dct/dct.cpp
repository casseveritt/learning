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
Color space transformation	- Completed
Downsampling				- Incomplete
Block Splitting				- Incomplete
DCT 						- Completed
Quantization				- Incomplete
Entropy Coding				- Incomplete
*/

Matrix4f YCBCRfromRGB;
Matrix4f RGBfromYCBCR;

struct RGBA8 {
  union {
    uint32_t i;
    struct {
      uint8_t r, g, b, a;
    };
  };
};

struct YCbCr32F {
  float y, cb, cr;
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
  return rgbasum(a, b);
}

static RGBA8 operator-(const RGBA8& a, const RGBA8& b) {
  return rgbadiff(a, b);
}

struct RGBA32F {
  float r, g, b, a;
};

static RGBA32F operator+(const RGBA32F& a, const RGBA32F& b) {
  return rgbasum(a, b);
}

static RGBA32F operator-(const RGBA32F& a, const RGBA32F& b) {
  return rgbadiff(a, b);
}

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

static Block8x8<YCbCr32F> RGBtoYCBCR(Block8x8<RGBA8> in) {
  Block8x8<YCbCr32F> out = {};
  YCbCr32F ycbcr;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      RGBA8 rgb = in.el(i, j);
      Vec4f ycbcrVec = YCBCRfromRGB * Vec4f(rgb.r, rgb.g, rgb.b, 1.0f);
      ycbcr.y = ycbcrVec.x;
      ycbcr.cb = ycbcrVec.y;
      ycbcr.cr = ycbcrVec.z;
      out.el(i, j) = ycbcr;
    }
  }
  return out;
};

static Block8x8<RGBA8> YCBCRtoRGB(Block8x8<YCbCr32F> in) {
  Block8x8<RGBA8> out = {};
  RGBA8 rgb;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      YCbCr32F ycbcr = in.el(i, j);
      Vec4f rgbVec = RGBfromYCBCR * Vec4f(ycbcr.y, ycbcr.cb, ycbcr.cr, 1.0f);
      rgb.r = rgbVec.x;
      rgb.g = rgbVec.y;
      rgb.b = rgbVec.z;
      rgb.a = 255;
      out.el(i, j) = rgb;
    }
  }
  return out;
};

static void BlockSplitter(Block8x8<YCbCr32F> ycbcr, Block8x8<float>* yb, Block8x8<float>* cbb, Block8x8<float>* crb) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      yb->el(i, j) = ycbcr.el(i, j).y;
      cbb->el(i, j) = ycbcr.el(i, j).cb;
      crb->el(i, j) = ycbcr.el(i, j).cr;
    }
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
    }
  }
  return spatialdom;
};

int main(int argc, char** argv) {
  YCBCRfromRGB.SetRow(0, Vec4f(0.258, 0.508, 0.0937, 16.0));
  YCBCRfromRGB.SetRow(1, Vec4f(-0.148, -0.289, 0.445, 128.0));
  YCBCRfromRGB.SetRow(2, Vec4f(0.445, -0.367, -0.071, 128.0));
  YCBCRfromRGB.SetRow(3, Vec4f(0.0, 0.0, 0.0, 1.0));
  RGBfromYCBCR = YCBCRfromRGB.Inverted();
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
  auto dif = new RGBA8[w * h];
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
      Block8x8<YCbCr32F> ctb = RGBtoYCBCR(b);
      Block8x8<float> yBlock, cbBlock, crBlock;
      BlockSplitter(ctb, &yBlock, &cbBlock, &crBlock);
      b = IDCTransform(DCTransform(b));
      for (int ii = 0; ii < 8; ii++) {
        for (int jj = 0; jj < 8; jj++) {
          auto& ip = pix[((i + ii) * w) + (j + jj)];
          auto& dp = dif[((i + ii) * w) + (j + jj)];
          const auto& bp = b.el(ii, jj);
          dp = ip - bp + grey;
          dp.a = 255;
          ip = bp;
        }
      }
    }
  }
  image_store_png("out.png", w, h, 4, img);
  image_store_png("dif.png", w, h, 4, reinterpret_cast<unsigned char*>(dif));
  delete[] dif;
}
