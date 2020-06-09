#include "square.h"

using namespace r3;

void Square::build(float sd, float ss) {
  sqrDime = sd;
  sqrSize = ss;
  obj.begin(GL_TRIANGLE_STRIP);
  obj.normal(0, 1, 0);
  obj.color(0.0f, 0.0f, 0.0f);
  obj.texCoord(0.0f, 0.0f);
  obj.position(-sqrDime, 0.0f, -sqrDime);
  obj.texCoord(0.0f, sqrSize);
  obj.position(-sqrDime, 0.0f, sqrDime);
  obj.texCoord(sqrSize, 0.0f);
  obj.position(sqrDime, 0.0f, -sqrDime);
  obj.texCoord(sqrSize, sqrSize);
  obj.position(sqrDime, 0.0f, sqrDime);
  obj.end();
}

void Square::draw(const Scene& scene, Prog p) {
  obj.draw(scene, p);
}