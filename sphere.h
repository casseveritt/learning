#include "object.h"

using namespace r3;

class Sphere {

public:
  struct Vert {
    Vec3f col;
    Vec3f norm;
    Vec3f pos;
  };
  Object sphObj;

  void build(float x, float y, float z, float radi = 0.5);

  void draw(Prog p);
};