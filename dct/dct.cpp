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
Block Splitting				- Completed
DCT 						- Completed
Quantization				- Completed
Entropy Coding				- Completed
*/

Matrix4f YCBCRfromRGB;
Matrix4f RGBfromYCBCR;
int quantMat[8][8] = {{16, 11, 10, 16, 25, 40, 51, 61},     {12, 12, 14, 19, 26, 58, 60, 55},
                      {14, 13, 16, 24, 40, 57, 69, 56},     {14, 17, 22, 29, 51, 87, 80, 62},
                      {18, 22, 37, 56, 68, 109, 103, 77},   {24, 35, 55, 64, 81, 104, 113, 92},
                      {49, 64, 78, 87, 103, 121, 120, 101}, {72, 92, 95, 98, 112, 100, 103, 99}};

struct RGBA8 {
  union {
    uint32_t i;
    struct {
      uint8_t r, g, b, a;
    };
  };
  RGBA8() {
    i = 0;
  }
  RGBA8(uint32_t iin) {
    i = iin;
  }
  RGBA8(uint8_t rin, uint8_t gin, uint8_t bin) {
    r = rin, g = gin, b = bin, a = 255;
  }
  RGBA8(uint8_t rin, uint8_t gin, uint8_t bin, uint8_t ain) {
    r = rin, g = gin, b = bin, a = ain;
  }
};

struct YCbCr32F {
  float y, cb, cr;
  YCbCr32F() {
    y = cb = cr = 0.0f;
  }
  YCbCr32F(float yin, float cbin, float crin) {
    y = yin, cb = cbin, cr = crin;
  }
};

enum MFlag { Turn, Up, Down };

struct RLEJam {
  int v, l;
  RLEJam() {
    v = l = 0;
  }
  RLEJam(int vin, int lin) {
    v = vin, l = lin;
  }
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

static float FToUint8t(float number) {
  return max(0.0f, min(255.0f, number));
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
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      YCbCr32F ycbcr = in.el(i, j);
      Vec4f rgbVec = RGBfromYCBCR * Vec4f(ycbcr.y, ycbcr.cb, ycbcr.cr, 1.0f);
      out.el(i, j) = RGBA8(FToUint8t(rgbVec.x), FToUint8t(rgbVec.y), FToUint8t(rgbVec.z));
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

static void BlockJoiner(Block8x8<YCbCr32F>* ycbcr, Block8x8<float> yb, Block8x8<float> cbb, Block8x8<float> crb) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      ycbcr->el(i, j) = YCbCr32F(yb.el(i, j), cbb.el(i, j), crb.el(i, j));
    }
  }
};

template <int N>
static float dct2(int n, int k) {
  return cos((M_PI / N) * (n + 0.5) * k);
}

static Block8x8<float> DCTransform(Block8x8<float> spatialdom) {
  Block8x8<float> freqdom;
  for (int u = 0; u < 8; ++u) {
    float cu = (u == 0) ? 0.5f / sqrt(2.0f) : 0.5f;
    for (int v = 0; v < 8; ++v) {
      float cv = (v == 0) ? 0.5f / sqrt(2.0f) : 0.5f;
      float s = 0.0f;
      for (int i = 0; i < 8; i++) {
        float ci = dct2<8>(i, u);
        for (int j = 0; j < 8; j++) {
          s += spatialdom.el(i, j) * (ci * dct2<8>(j, v));
        }
      }
      freqdom.el(u, v) = (s * cu * cv);
    }
  }
  return freqdom;
};

static Block8x8<float> IDCTransform(Block8x8<float> freqdom) {
  Block8x8<float> spatialdom;
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      float s = 0.0f;
      for (int u = 0; u < 8; u++) {
        float cu = (u == 0) ? 0.5 / sqrt(2.0f) : 0.5;
        for (int v = 0; v < 8; v++) {
          float cv = (v == 0) ? 0.5 / sqrt(2.0f) : 0.5;
          s += freqdom.el(u, v) * (cu * cv * dct2<8>(i, u) * dct2<8>(j, v));
        }
      }
      spatialdom.el(i, j) = s + 0.5f;
    }
  }
  return spatialdom;
};

// Divides the DCT matrix element by the corresponding quant matrix element.
static void Quantization(Block8x8<float>* dct) {
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      dct->el(i, j) = round(dct->el(i, j) / quantMat[i][j]);
    }
  }
};

// Multiplies the quantized matrix element by the corresponding quant matrix element.
static void Unquantize(Block8x8<float>* quantdct) {
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      quantdct->el(i, j) = round(quantdct->el(i, j) * quantMat[i][j]);
    }
  }
};

static vector<RLEJam> RLEncoding(Block8x8<float> quantizedMat) {
  int r = 0, c = 0;  // Row, Column
  MFlag f = Turn;    // Movement behavior
  vector<RLEJam> out;
  RLEJam jam(quantizedMat.el(r, c), 0);
  for (int i = 0; i < 64; i++) {
    if (jam.v == quantizedMat.el(r, c))
      jam.l++;
    else {
      out.push_back(jam);
      jam = RLEJam(quantizedMat.el(r, c), 1);
    }
    switch (f) {
      case Turn:           // Hit wall
        if (r % 7 == 0) {  // Top or bottom wall
          if (r == 0) c++, f = Down;
          if (r == 7) c++, f = Up;
        } else {  // Side wall
          if (c == 0) r++, f = Up;
          if (c == 7) r++, f = Down;
        }
        break;
      case Up:  // Up-Right diagonal
        r--, c++;
        if (r % 7 == 0 || c % 7 == 0) f = Turn;
        break;
      case Down:  // Down-Left diagonal
        r++, c--;
        if (r % 7 == 0 || c % 7 == 0) f = Turn;
        break;
      default:
        break;
    }
  }
  out.push_back(jam);
  return out;
};

static Block8x8<float> RLDecoding(vector<RLEJam> encodedBlock) {
  int r = 0, c = 0, size = encodedBlock.size();  // Row, Column, and vector size
  MFlag f = Turn;                                // Movement behavior
  Block8x8<float> out;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < encodedBlock[i].l; j++) {
      out.element[c][r] = encodedBlock[i].v;
      switch (f) {
        case Turn:           // Hit wall
          if (r % 7 == 0) {  // Top or bottom wall
            if (r == 0) c++, f = Down;
            if (r == 7) c++, f = Up;
          } else {  // Side wall
            if (c == 0) r++, f = Up;
            if (c == 7) r++, f = Down;
          }
          break;
        case Up:  // Up-Right diagonal
          r--, c++;
          if (r % 7 == 0 || c % 7 == 0) f = Turn;
          break;
        case Down:  // Down-Left diagonal
          r++, c--;
          if (r % 7 == 0 || c % 7 == 0) f = Turn;
          break;
        default:
          break;
      }
    }
  }
  return out;
};

int main(int argc, char** argv) {
  // Setting color space transform matricies
  YCBCRfromRGB =
      Matrix4f(0.258, 0.508, 0.0937, 16.0, -0.148, -0.289, 0.445, 128.0, 0.445, -0.367, -0.071, 128.0, 0.0, 0.0, 0.0, 1.0);
  RGBfromYCBCR = YCBCRfromRGB.Inverted();

  char* imgName = "images/Lenna.png";
  if (argc >= 2) {
    imgName = "images/";
    strcat(imgName, argv[1]);
  }
  char* imgLocation = new char[sizeof(imgName) + 1];
  strcpy(imgLocation, imgName);
  int w, h, n;

  unsigned char* img;
  img = image_load(imgLocation, &w, &h, &n);

  auto pix = reinterpret_cast<RGBA8*>(img);
  auto dif = new RGBA8[w * h];

  for (int i = 0; i < h; i += 8) {
    for (int j = 0; j < w; j += 8) {
      Block8x8<RGBA8> b;
      Block8x8<YCbCr32F> ctb;
      vector<Block8x8<float>> channels(3);
      vector<vector<RLEJam>> encodedCh(3);
      for (int ii = 0; ii < 8; ii++) {
        for (int jj = 0; jj < 8; jj++) {
          b.el(ii, jj) = pix[((i + ii) * w) + (j + jj)];
        }
      }
      // Image encoding
      ctb = RGBtoYCBCR(b);
      BlockSplitter(ctb, &channels[0], &channels[1], &channels[2]);
      for (int c = 0; c < 3; c++) {
        channels[c] = DCTransform(channels[c]);
        Quantization(&channels[c]);
        encodedCh[c] = RLEncoding(channels[c]);
      }
      // Image decoding
      for (int c = 0; c < 3; c++) {
        channels[c] = RLDecoding(encodedCh[c]);
        Unquantize(&channels[c]);
        channels[c] = IDCTransform(channels[c]);
      }
      BlockJoiner(&ctb, channels[0], channels[1], channels[2]);
      b = YCBCRtoRGB(ctb);
      for (int ii = 0; ii < 8; ii++) {
        for (int jj = 0; jj < 8; jj++) {
          auto& ip = pix[((i + ii) * w) + (j + jj)];
          auto& dp = dif[((i + ii) * w) + (j + jj)];
          const auto& bp = b.el(ii, jj);
          int diffR = pow(2, abs(ip.r - bp.r));
          int diffG = pow(2, abs(ip.g - bp.g));
          int diffB = pow(2, abs(ip.b - bp.b));
          dp = RGBA8(min(255, diffR), min(255, diffG), min(255, diffB));
          ip = bp;
        }
      }
    }
  }
  image_store_png("out.png", w, h, 4, img);
  image_store_png("dif.png", w, h, 4, reinterpret_cast<unsigned char*>(dif));
  delete[] dif;
}
