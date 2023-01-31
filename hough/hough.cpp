#include <fmt/core.h>
#include <math.h>

#include <string>

#include "linear.h"
#include "stb.h"

using namespace r3;
using namespace std;

template <typename PixelType>
class Image {
 public:
  Image() = delete;
  Image(PixelType* data, int width, int height) : d(data), w(width), h(height) {}

  int index(int x, int y) const {
    return y * h + x;
  }

  PixelType& operator()(int x, int y) {
    return d[index(x, y)];
  }

 private:
  PixelType* d;
  const int w;
  const int h;
};

int main(int argc, char** argv) {
  // Setting color space transform matricies
  string imgName = "images/";
  imgName += (argc >= 2) ? argv[1] : "diamond.png";

  int w, h, n;
  unsigned char* imgdata;
  imgdata = image_load(imgName.c_str(), &w, &h, &n);

  vector<float> fimgdata(w * h);
  for (int i = 0; i < (w * h); i++) {
    fimgdata[i] = imgdata[i] / 255.0f;
  }

  Image<float> fimg(fimgdata.data(), w, h);

  fmt::print("input image name={}, w={}, h={}, n={}, samp={}\n", imgName, w, h, n, fimg(w / 2, h / 2));

  for (int j = 0; j < h - 1; j += 2) {
    for (int i = 0; i < w - 1; i += 2) {
      const int l = i;
      const int r = i + 1;
      const int u = j;
      const int d = j + 1;
      float xr = (fimg(r, u) + fimg(r, d)) * 0.5f;
      float xl = (fimg(l, u) + fimg(l, d)) * 0.5f;
      float yu = (fimg(r, u) + fimg(l, u)) * 0.5f;
      float yd = (fimg(r, d) + fimg(l, d)) * 0.5f;
      Vec2f g(xr - xl, yu - yd);
      if (g.Length() > 0.5f) {
        fmt::print("{} {}\n", fimg(l, u), fimg(r, u));
        fmt::print("{} {}\n", fimg(l, d), fimg(r, d));
        float angle = ToDegrees(atan2f(g.y, g.x));
        fmt::print("pix({}, {}), angle={}, g=({}, {})\n", i, j, angle, g.x, g.y);
      }
    }
  }

  for (int i = 0; i < (w * h); i++) {
    imgdata[i] = fimgdata[i] * 255.0f;
  }

  image_store_png("out.png", w, h, 1, imgdata);
  return 0;
}
