#include "torus.h"

using namespace r3;

void Torus::build(float rad1, float rad2) {
  std::vector<Vec3f> torus;
  for (int i = 0; i < 37; i++) { // Degrees
    float tr = ToRadians(i * 10.0f);
    torus.push_back(Vec3f(sin(tr) * rad2 + rad1, cos(tr) * rad2, 0.0));
  }

  Vec3f center(rad1, 0, 0);
  torObj.begin(GL_TRIANGLES);
  torObj.color(0.0f, 0.0f, 0.0f);
  for (int j = 0; j < 36; j++) {
    Quaternionf q0(Vec3f(0, 1, 0), ToRadians(j * 10.0f));
    Quaternionf q1(Vec3f(0, 1, 0), ToRadians((j * 10.0f) + 10.0f));
    for (size_t i = 0; i < torus.size() - 1; i++) {
      Vec3f v00 = q0 * torus[i + 0];
      Vec3f v01 = q1 * torus[i + 0];
      Vec3f v10 = q0 * torus[i + 1];
      Vec3f v11 = q1 * torus[i + 1];
      Vec3f c0 = q0 * center;
      Vec3f c1 = q1 * center;
      torObj.normal((v00 - c0).Normalized());
      torObj.texCoord(j/36.0, i/18.0);
      torObj.position(v00);
      torObj.normal((v10 - c0).Normalized());
      torObj.texCoord(j/36.0, (i+1)/18.0);
      torObj.position(v10);
      torObj.normal((v01 - c1).Normalized());
      torObj.texCoord((j+1)/36.0, i/18.0);
      torObj.position(v01);

      torObj.normal((v01 - c1).Normalized());
      torObj.texCoord((j+1)/36.0, i/18.0);
      torObj.position(v01);
      torObj.normal((v10 - c0).Normalized());
      torObj.texCoord(j/36.0, (i+1)/18.0);
      torObj.position(v10);
      torObj.normal((v11 - c1).Normalized());
      torObj.texCoord((j+1)/36.0, (i+1)/18.0);
      torObj.position(v11);
    }
  }
  torObj.end();
}

void Torus::draw(const Scene &scene, Prog p) { torObj.draw(scene, p); }