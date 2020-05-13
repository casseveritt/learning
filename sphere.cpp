#include "sphere.h"

using namespace r3;

void Sphere::build(float x, float y, float z, float radi) {
  Vec3f center = Vec3f(x, y + radi, z);
  std::vector<Vec3f> circle;
  for (int i = 0; i < 37; i++) { // Degrees
    float tr = ToRadians(i * 10.0f);
    circle.push_back(Vec3f(sin(tr) * radi, cos(tr) * radi + radi + y, 0.0));
  }

  sphObj.begin(GL_TRIANGLES);
  sphObj.color(0.9f, 0.9f, 0.9f);
  for (int j = 0; j < 18; j++) {
    Quaternionf q0(Vec3f(0, 1, 0), ToRadians(j * 10.0f));
    Quaternionf q1(Vec3f(0, 1, 0), ToRadians((j * 10.0f) + 10.0f));
    for (size_t i = 0; i < circle.size() - 1; i++) {
      Vec3f v00 = q0 * circle[i + 0] + Vec3f(x, y, z);
      Vec3f v01 = q1 * circle[i + 0] + Vec3f(x, y, z);
      Vec3f v10 = q0 * circle[i + 1] + Vec3f(x, y, z);
      Vec3f v11 = q1 * circle[i + 1] + Vec3f(x, y, z);
      sphObj.normal((v00 - center).Normalized());
      sphObj.position(v00);
      sphObj.normal((v10 - center).Normalized());
      sphObj.position(v10);
      sphObj.normal((v01 - center).Normalized());
      sphObj.position(v01);

      sphObj.normal((v01 - center).Normalized());
      sphObj.position(v01);
      sphObj.normal((v10 - center).Normalized());
      sphObj.position(v10);
      sphObj.normal((v11 - center).Normalized());
      sphObj.position(v11);
    }
  }
  sphObj.end();
}

void Sphere::draw(const Scene &scene, Prog p) { sphObj.draw(scene, p); }