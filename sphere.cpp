#include "sphere.h"

using namespace r3;

void Sphere::build(float radi) {
  std::vector<Vec3f> circle;
  for (int i = 0; i < 19; i++) {  // Degrees
    float tr = ToRadians(i * 10.0f - 90.0f);
    circle.push_back(Vec3f(cos(tr) * radi, sin(tr) * radi, 0.0));
  }

  obj.begin(GL_TRIANGLES);
  obj.color(0.9f, 0.9f, 0.9f);
  const float csz = float(circle.size());
  for (int j = 0; j < 36; j++) {
    Quaternionf q0(Vec3f(0, 1, 0), ToRadians(j * 10.0f));
    Quaternionf q1(Vec3f(0, 1, 0), ToRadians((j * 10.0f) + 10.0f));
    for (size_t i = 0; i < csz - 1; i++) {
      Vec3f v00 = q0 * circle[i + 0];
      Vec3f v01 = q1 * circle[i + 0];
      Vec3f v10 = q0 * circle[i + 1];
      Vec3f v11 = q1 * circle[i + 1];
      obj.normal(v00.Normalized());
      obj.texCoord(j / 36.0, i / csz);
      obj.position(v00);
      obj.normal(v10.Normalized());
      obj.texCoord(j / 36.0, (i + 1) / csz);
      obj.position(v10);
      obj.normal(v01.Normalized());
      obj.texCoord((j + 1) / 36.0, i / csz);
      obj.position(v01);

      obj.normal(v01.Normalized());
      obj.texCoord((j + 1) / 36.0, i / csz);
      obj.position(v01);
      obj.normal(v10.Normalized());
      obj.texCoord(j / 36.0, (i + 1) / csz);
      obj.position(v10);
      obj.normal(v11.Normalized());
      obj.texCoord((j + 1) / 36.0, (i + 1) / csz);
      obj.position(v11);
    }
  }
  obj.end();
}

void Sphere::draw(const Scene& scene, Prog p) {
  obj.draw(scene, p);
}