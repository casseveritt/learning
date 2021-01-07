#include "torus.h"

using namespace r3;

void Torus::build(float rad1, float rad2) {
  form = 3;
  std::vector<Vec3f> torus;
  bigr = rad1;
  littler = rad2;
  for (int i = 0; i < 37; i++) {  // Degrees
    float tr = ToRadians(i * 10.0f);
    torus.push_back(Vec3f(sin(tr) * rad2 + rad1, cos(tr) * rad2, 0.0));
  }

  Vec3f center(rad1, 0, 0);
  obj.begin(GL_TRIANGLES);
  obj.color(0.0f, 0.0f, 0.0f);
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
      obj.normal((v00 - c0).Normalized());
      obj.texCoord(j / 36.0, i / 18.0);
      obj.position(v00);
      obj.normal((v10 - c0).Normalized());
      obj.texCoord(j / 36.0, (i + 1) / 18.0);
      obj.position(v10);
      obj.normal((v01 - c1).Normalized());
      obj.texCoord((j + 1) / 36.0, i / 18.0);
      obj.position(v01);

      obj.normal((v01 - c1).Normalized());
      obj.texCoord((j + 1) / 36.0, i / 18.0);
      obj.position(v01);
      obj.normal((v10 - c0).Normalized());
      obj.texCoord(j / 36.0, (i + 1) / 18.0);
      obj.position(v10);
      obj.normal((v11 - c1).Normalized());
      obj.texCoord((j + 1) / 36.0, (i + 1) / 18.0);
      obj.position(v11);
    }
  }
  obj.end();
}

void Torus::draw(const Scene& scene, Prog p) {
  obj.draw(scene, p);
}

/*float Torus::eval(Vec3f p0) {
  float brSqr = bigr * bigr;
  float lrSqr = littler * littler;
  float x2 = p0.x * p0.x;
  float y2 = p0.y * p0.y;
  float z2 = p0.z * p0.z;
  float a = x2 + y2 + z2 + brSqr - lrSqr;
  float b = 4 * brSqr * (x2 + z2);
  float c = a * a - b;
  return c;
}

bool Torus::intersect(Vec3f p0, Vec3f p1) {
  Vec3f rayDir = (p1 - p0).Normalized();
  Matrix4f objFromWorld = obj.modelPose.GetMatrix4().Inverted();
  p0 = objFromWorld * p0;
  p1 = objFromWorld * p1;
  float t = 0;
  float prev_f = 0.0f;
  for (int i = 0; i < 50; i++) {
    float f = eval(p0 + t * rayDir);
    const float epsilon = 0.01f;
    float f1 = eval(p0 + (rayDir * (t + epsilon)));
    float dfdt = (f1 - f) / epsilon;
    if (i == 0) {
      prev_f = f;
    }
    t += (f / 2) / -dfdt;
    if (fabs(f) <= 0.001f) {
      return true;
    }
    if (prev_f < f) {
      break;
    }
  }
  return false;
}*/

bool Torus::intersect(Vec3f p0, Vec3f p1, Vec3f& intersection) {
  bool out;
  Matrix4f objFromWorld = obj.modelPose.GetMatrix4().Inverted();
  p0 = objFromWorld * p0;
  p1 = objFromWorld * p1;
  Vec3f l = (p1 - p0).Normalized();
  double R = bigr;
  double r = littler;
  double g = Dot(l, l);
  double h = 2 * Dot(l, p0);
  double k = Dot(p0, p0) + (R * R) - (r * r);
  // Coefficients
  double a = g * g;
  double b = 2 * g * h;
  double c = (2 * g * k) + (h * h);
  double d = 2 * h * k;
  double e = k * k;

  Vec3f l2 = Vec3f(l.x, 0.0f, l.z);
  Vec3f p2 = Vec3f(p0.x, 0.0f, p0.z);

  double m = Dot(l2, l2);
  double n = 2 * Dot(l2, p2);
  double p = Dot(p2, p2);

  c += m * -4 * R * R;
  d += n * -4 * R * R;
  e += p * -4 * R * R;

  double a3 = a * a * a;
  double a2 = a * a;
  double b4 = b * b * b * b;
  double b3 = b * b * b;
  double b2 = b * b;
  // double c4 = c * c * c * c;
  double c3 = c * c * c;
  double c2 = c * c;
  // double d4 = d * d * d * d;
  // double d3 = d * d * d;
  double d2 = d * d;
  // double e3 = e * e * e;
  // double e2 = e * e;

  /*double delta = 256 * a3 * e3 - 192 * a2 * b * d * e2 - 128 * a2 * c2 * e2 + 144 * a2 * c * d2 * e;
  delta += -27 * a2 * d4 + 144 * a * b2 * c * e2 - 6 * a * b2 * d2 * e - 80 * a * b * c2 * d * e;
  delta += 18 * a * b * c * d3 + 16 * a * c4 * e - 4 * a * c3 * d2 - 27 * b4 * e2;
  delta += 18 * b3 * c * d * e - 4 * b3 * d3 - 4 * b2 * c3 * e + b2 * c2 * d2;*/
  double P = 8 * a * c - 3 * b2;
  R = b3 + 8 * d * a2 - 4 * a * b * c;
  double delta0 = c2 - 3.0 * b * d + 12.0 * a * e;
  double delta1 = 2.0 * c3 - 9.0 * b * c * d + 27.0 * b2 * e + 27.0 * a * d2 - 72.0 * a * c * e;
  double D = 64 * a3 * e - 16 * a2 * c2 + 16 * a * b2 * c - 16 * a2 * b * d - 3 * b4;
  p = (P / (8 * a2));
  double q = (R / (8 * a3));
  double y = (delta1 * delta1 - 4 * delta0 * delta0 * delta0);
  double delta = y / -27;
  double qrt = pow(y, 0.5f);
  double Q = cbrt(((delta1 + qrt) / 2.0));
  double S;
  if (delta > 0) {
    double fi = acos((delta1) / (2 * sqrt(delta0 * delta0 * delta0)));
    S = 0.5f * sqrt(-(2.0f / 3.0f) * p + (2.0f / (3.0f * a)) * sqrt(delta0) * cos(fi / 3.0f));
  } else {
    S = 0.5 * pow((-2.0 / 3.0) * p + (1.0 / (3.0 * a)) * (Q + delta0 / Q), 0.5);
  }
  double rt0 = pow(-4 * S * S - 2 * p + (q / S), 0.5);
  double rt1 = pow(-4 * S * S - 2 * p - (q / S), 0.5);
  double z0 = -(b / (4 * a)) - S + 0.5 * rt0;
  double z1 = -(b / (4 * a)) - S - 0.5 * rt0;
  double z2 = -(b / (4 * a)) + S + 0.5 * rt1;
  double z3 = -(b / (4 * a)) + S - 0.5 * rt1;
  double z = 1000.0;
  if (z0 <= z) {
    z = z0;
  }
  if (z1 <= z) {
    z = z1;
  }
  if (z2 <= z) {
    z = z2;
  }
  if (z3 <= z) {
    z = z3;
  }

  // if (z != 1000.0) { printf("Closest root: %lf\n", z); }

  if (delta < 0) {
    intersection = p0 + l * z;
    intersection = obj.modelPose.GetMatrix4() * intersection;
    out = true;
  }
  if (delta > 0) {
    if (P < 0 && D < 0) {
      intersection = p0 + l * z;
      intersection = obj.modelPose.GetMatrix4() * intersection;
      out = true;
    } else {
      out = false;
    }
  }
  intersection = p0 + l * z;
  intersection = obj.modelPose.GetMatrix4() * intersection;
  if (delta == 0) {
    if (P < 0 && D < 0 && delta0 != 0) {
      out = true;
    }
    if (D > 0 || (P > 0 && (D != 0 || R != 0))) {
      out = true;
    }
    if (delta0 == 0 && D != 0) {
      out = true;
    }
    if (D == 0) {
      if (P < 0) {
        out = true;
      }
      if (P > 0 && R == 0) {
        out = true;
      }
      if (delta0 == 0) {
        out = true;
      }
    }
  }

  return out;
}