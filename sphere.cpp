#include "sphere.h"

using namespace r3;

void Sphere::build(float radi) {
  std::vector<Vec3f> circle;
  for (int i = 0; i < 19; i++) { // Degrees
    float tr = ToRadians(i * 10.0f - 90.0f);
    circle.push_back(Vec3f(cos(tr) * radi, sin(tr) * radi, 0.0));
  }

  sphObj.begin(GL_TRIANGLES);
  sphObj.color(0.9f, 0.9f, 0.9f);
  const float csz = float(circle.size());
  for (int j = 0; j < 36; j++) {
    Quaternionf q0(Vec3f(0, 1, 0), ToRadians(j * 10.0f));
    Quaternionf q1(Vec3f(0, 1, 0), ToRadians((j * 10.0f) + 10.0f));
    for (size_t i = 0; i < csz - 1; i++) {
      Vec3f v00 = q0 * circle[i + 0];
      Vec3f v01 = q1 * circle[i + 0];
      Vec3f v10 = q0 * circle[i + 1];
      Vec3f v11 = q1 * circle[i + 1];
      sphObj.normal(v00.Normalized());
      sphObj.texCoord(j/36.0, i/csz);
      sphObj.position(v00);
      sphObj.normal(v10.Normalized());
      sphObj.texCoord(j/36.0, (i+1)/csz);
      sphObj.position(v10);
      sphObj.normal(v01.Normalized());
      sphObj.texCoord((j+1)/36.0, i/csz);
      sphObj.position(v01);

      sphObj.normal(v01.Normalized());
      sphObj.texCoord((j+1)/36.0, i/csz);
      sphObj.position(v01);
      sphObj.normal(v10.Normalized());
      sphObj.texCoord(j/36.0, (i+1)/csz);
      sphObj.position(v10);
      sphObj.normal(v11.Normalized());
      sphObj.texCoord((j+1)/36.0, (i+1)/csz);
      sphObj.position(v11);
    }
  }
  sphObj.end();
}

void Sphere::draw(const Scene &scene, Prog p) { sphObj.draw(scene, p); }