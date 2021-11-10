#include "rectangle.h"

using namespace r3;

void Rectangle::build(float side0, float side1, Vec3f rCol) {
  s0 = side0;
  s1 = side1;
  obj.begin(GL_TRIANGLE_STRIP);
  obj.normal(0, 0, -1);
  obj.color(rCol.x, rCol.y, rCol.z);

  obj.texCoord(0.0f, 1.0f);
  obj.position(0.0f, 0.0f, 0.0f);
  obj.texCoord(0.0f, 0.0f);
  obj.position(0.0f, s1, 0.0f);
  obj.texCoord(1.0f, 1.0f);
  obj.position(s0, 0.0f, 0.0f);
  obj.texCoord(1.0f, 0.0f);
  obj.position(s0, s1, 0.0f);
  obj.end();
}

void Rectangle::draw(const Scene& scene, Prog p) {
  obj.draw(scene, p);
}

bool Rectangle::intersect(Vec3f screenClick) {
  Matrix4f s = Matrix4f::Scale(Vec3f(1.0f/s0, 1.0f/s1, 0.0f));
  Matrix4f buttonFromScreen = s * obj.model.Inverted();
  Vec3f butClick = buttonFromScreen * screenClick;
  if (1.0f > butClick.x && butClick.x > 0.0f && 1.0f > butClick.y && butClick.y > 0.0f) {
    return true;
  }
  return false;
}