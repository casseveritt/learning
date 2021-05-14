#include "multrect.h"

using namespace r3;

void MultRect::build(const std::vector<Rect>& rects) {
  obj.begin(GL_TRIANGLES);
  obj.normal(0, 0, -1);
  obj.color(0.9f, 0.9f, 0.9f);
  for (Rect rect : rects) {
    // (0, 0), (0, 1), (1, 0)
    obj.texCoord(0.0f, 0.0f);
    obj.position(rect.x0 * s0, 1 - rect.y0 * s1, 0.0f);

    obj.texCoord(0.0f, 1.0f);
    obj.position(rect.x0 * s0, 1 - (rect.y0 + 1) * s1, 0.0f);

    obj.texCoord(1.0f, 0.0f);
    obj.position((rect.x0 + 1) * s0, 1 - rect.y0 * s1, 0.0f);

    // (0, 1), (1, 0), (1, 1)
    obj.texCoord(1.0f, 0.0f);
    obj.position((rect.x0 + 1) * s0, 1 - rect.y0 * s1, 0.0f);

    obj.texCoord(0.0f, 1.0f);
    obj.position(rect.x0 * s0, 1 - (rect.y0 + 1) * s1, 0.0f);

    obj.texCoord(1.0f, 1.0f);
    obj.position((rect.x0 + 1) * s0, 1 - (rect.y0 + 1) * s1, 0.0f);
  }
  obj.end();
}

void MultRect::draw(const Scene& scene, Prog p) {
  obj.draw(scene, p);
}