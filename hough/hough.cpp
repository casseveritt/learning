#include <math.h>
#include <string>
#include <fmt/core.h>

#include "linear.h"
#include "stb.h"

using namespace r3;
using namespace std;


int main(int argc, char** argv) {
  // Setting color space transform matricies
  string imgName = "images/";
  imgName += (argc >= 2) ? argv[1] : "diamond.png";

  int w, h, n;
  unsigned char* img;
  img = image_load(imgName.c_str(), &w, &h, &n);

  fmt::print("input image name={}, w={}, h={}, n={}\n", imgName, w, h, n);

  // image_store_png("out.png", w, h, 4, img);
  return 0;
}
