#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

float lToRadians(float degrees) { return degrees * (M_PI / 180.0f); }
float lToDegrees(float radians) { return radians * (180.0f / M_PI); }
float Mat2Det(float a, float b, float c, float d) { return (a * d) - (b * c); }

class lVec3f {
public:
  union {
    float v[3];
    struct {
      float x, y, z;
    };
  };

  lVec3f() {
    v[0] = 0.0f;
    v[1] = 0.0f;
    v[2] = 0.0f;
  }

  lVec3f(float v0, float v1, float v2) {
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
  }

  lVec3f(float *v0) {
    v[0] = v0[0];
    v[1] = v0[1];
    v[2] = v0[2];
  }

  void Normalize() {
    float l = sqrt((x * x) + (y * y) + (z * z));
    x /= l;
    y /= l;
    z /= l;
  }

  lVec3f Normalized() {
    float l = sqrt((x * x) + (y * y) + (z * z));
    return lVec3f((x / l), (y / l), (z / l));
  }

  float &operator[](int i) { return v[i]; }
  const float &operator[](int i) const { return v[i]; }

  lVec3f &operator*=(float d) {
    for (int i = 0; i < 3; i++) {
      v[i] *= d;
    }
    return *this;
  }
  lVec3f &operator*=(const lVec3f &u) {
    for (int i = 0; i < 3; i++) {
      v[i] *= u[i];
    }
    return *this;
  }
  lVec3f &operator/=(float d) { return *this *= (1.0f / d); }

  lVec3f &operator+=(float d) {
    for (int i = 0; i < 3; i++) {
      v[i] += d;
    }
    return *this;
  }
  lVec3f &operator+=(const lVec3f &u) {
    for (int i = 0; i < 3; i++) {
      v[i] += u.v[i];
    }
    return *this;
  }
  lVec3f &operator-=(float d) {
    for (int i = 0; i < 3; i++) {
      v[i] -= d;
    }
    return *this;
  }
  lVec3f &operator-=(const lVec3f &u) {
    for (int i = 0; i < 3; i++) {
      v[i] -= u.v[i];
    }
    return *this;
  }
};

lVec3f operator*(const lVec3f &v, const float &f) { return lVec3f(v) *= f; }
lVec3f operator*(const float &f, const lVec3f &v) { return lVec3f(v) *= f; }
lVec3f operator*(const lVec3f &v0, const lVec3f &v1) {
  return lVec3f(v0) *= v1;
}
lVec3f operator/(const lVec3f &v, const float &f) { return lVec3f(v) /= f; }

lVec3f operator+(const lVec3f &v0, const lVec3f &v1) {
  return lVec3f(v0) += v1;
}
lVec3f operator-(const lVec3f &v0, const lVec3f &v1) {
  return lVec3f(v0) -= v1;
}

bool operator==(const lVec3f &v0, const lVec3f &v1) {
  for (int i = 0; i < 3; i++) {
    if (v0.v[i] != v1.v[i]) {
      return false;
    }
  }
  return true;
}
bool operator!=(const lVec3f &v0, const lVec3f &v1) { return !(v0 == v1); }

lVec3f Min(const lVec3f &v0, const lVec3f &v1) {
  lVec3f out;
  out.x = fmin(v0.x, v1.x);
  out.y = fmin(v0.y, v1.y);
  out.z = fmin(v0.z, v1.z);
  return out;
}
lVec3f Max(const lVec3f &v0, const lVec3f &v1) {
  lVec3f out;
  out.x = fmax(v0.x, v1.x);
  out.y = fmax(v0.y, v1.y);
  out.z = fmax(v0.z, v1.z);
  return out;
}

float Dot(lVec3f v0, lVec3f v1) {
  return (v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z);
}
lVec3f Cross(lVec3f v0, lVec3f v1) {
  return lVec3f((v0.y * v1.z - v0.z * v1.y), (v0.z * v1.x - v0.x * v1.z),
                (v0.x * v1.y - v0.y * v1.x));
}

class lVec4f {
public:
  union {
    float v[4];
    struct {
      float x, y, z, w;
    };
  };

  lVec4f() {
    v[0] = 0.0f;
    v[1] = 0.0f;
    v[2] = 0.0f;
    v[3] = 1.0f;
  }

  lVec4f(float a, float b, float c) {
    v[0] = a;
    v[1] = b;
    v[2] = c;
    v[3] = 1.0f;
  }

  lVec4f(float a, float b, float c, float d) {
    v[0] = a;
    v[1] = b;
    v[2] = c;
    v[3] = d;
  }

  lVec4f(float *v0) {
    v[0] = v0[0];
    v[1] = v0[1];
    v[2] = v0[2];
    v[3] = v0[3];
  }

  void Negate() {
    for (int i = 0; i < 4; i++) {
      v[i] = -v[i];
    }
  }

  lVec4f Negated() {
    lVec4f out = lVec4f(&v[0]);
    out.Negate();
    return out;
  }

  void Normalize() {
    float l = sqrt((x * x) + (y * y) + (z * z));
    for (int i = 0; i < 4; i++) {
      v[i] /= l;
    }
  }

  lVec4f Normalized() {
    lVec4f out(*this);
    out.Normalize();
    return out;
  }

  float &operator[](int i) { return v[i]; }
  const float &operator[](int i) const { return v[i]; }

  lVec4f &operator*=(float f) {
    for (int i = 0; i < 4; i++) {
      v[i] *= f;
    }
    return *this;
  }
  lVec4f &operator*=(const lVec4f &u) {
    for (int i = 0; i < 4; i++) {
      v[i] *= u[i];
    }
    return *this;
  }
  lVec4f &operator/=(float f) { return *this *= (1.0f / f); }

  lVec4f &operator+=(const lVec4f &u) {
    for (int i = 0; i < 4; i++) {
      v[i] += u.v[i];
    }
    return *this;
  }
  lVec4f &operator-=(const lVec4f &u) {
    for (int i = 0; i < 4; i++) {
      v[i] -= u.v[i];
    }
    return *this;
  }
  lVec4f operator-() const { return lVec4f(*this).Negated(); }
};

lVec4f operator*(const lVec4f &v, const float &f) { return lVec4f(v) *= f; }
lVec4f operator*(const float &f, const lVec4f &v) { return lVec4f(v) *= f; }
lVec4f operator*(const lVec4f &v0, const lVec4f &v1) {
  return lVec4f(v0) *= v1;
}
lVec4f operator/(const lVec4f &v, const float &f) { return lVec4f(v) /= f; }

lVec4f operator+(const lVec4f &v0, const lVec4f &v1) {
  return lVec4f(v0) += v1;
}
lVec4f operator-(const lVec4f &v0, const lVec4f &v1) {
  return lVec4f(v0) -= v1;
}

bool operator==(const lVec4f &v0, const lVec4f &v1) {
  for (int i = 0; i < 4; i++) {
    if (v0.v[i] != v1.v[i]) {
      return false;
    }
  }
  return true;
}
bool operator!=(const lVec4f &v0, const lVec4f &v1) { return !(v0 == v1); }

lVec4f Min(const lVec4f &v0, const lVec4f &v1) {
  lVec4f out;
  out.x = fmin(v0.x, v1.x);
  out.y = fmin(v0.y, v1.y);
  out.z = fmin(v0.z, v1.z);
  out.w = fmin(v0.w, v1.w);
  return out;
}
lVec4f Max(const lVec4f &v0, const lVec4f &v1) {
  lVec4f out;
  out.x = fmax(v0.x, v1.x);
  out.y = fmax(v0.y, v1.y);
  out.z = fmax(v0.z, v1.z);
  out.w = fmax(v0.w, v1.w);
  return out;
}

float Dot(lVec4f v0, lVec4f v1) {
  return (v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z) + (v0.w * v1.w);
}

enum ElAxis { AXIS_X, AXIS_Y, AXIS_Z };

void printOp(float *m0, float *m1) {
  printf("%.3f, %.3f, %.3f, %.3f\t%.3f, %.3f, %.3f, %.3f\n%.3f, %.3f, %.3f, "
         "%.3f\t%.3f, %.3f, %.3f, %.3f\n%.3f, %.3f, %.3f, %.3f\t%.3f, %.3f, "
         "%.3f, %.3f\n%.3f, %.3f, %.3f, %.3f\t%.3f, %.3f, %.3f, %.3f\n\n",
         m0[0], m0[1], m0[2], m0[3], m1[0], m1[1], m1[2], m1[3], m0[4], m0[5],
         m0[6], m0[7], m1[4], m1[5], m1[6], m1[7], m0[8], m0[9], m0[10], m0[11],
         m1[8], m1[9], m1[10], m1[11], m0[12], m0[13], m0[14], m0[15], m1[12],
         m1[13], m1[14], m1[15]);
}
class lMatrix3f;

lMatrix3f operator*(const lMatrix3f &m0, const lMatrix3f &m1);

class lMatrix3f {
public:
  float lM[9];

  lMatrix3f() { MakeIdentity(); }

  lMatrix3f(float f00, float f01, float f02, float f10, float f11, float f12,
            float f20, float f21, float f22) {
    Set(f00, f01, f02, f10, f11, f12, f20, f21, f22);
  }

  lMatrix3f(const lVec3f &r0, const lVec3f &r1, const lVec3f &r2) {
    Row(0, r0);
    Row(1, r1);
    Row(2, r2);
  }

  void MakeIdentity() {
    Set(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
  }

  lVec3f Row(int i) const { return lVec3f(el(i, 0), el(i, 1), el(i, 2)); }

  void Row(int i, const lVec3f &r) {
    el(i, 0) = r.x;
    el(i, 1) = r.y;
    el(i, 2) = r.z;
  }

  void SwapRow(int i, int j) {
    lVec3f stowRow = Row(i);
    Row(i, Row(j));
    Row(j, stowRow);
  }

  lVec3f Col(int i) const { return lVec3f(el(0, i), el(1, i), el(2, i)); }

  void Col(int i, const lVec3f &c) {
    el(0, i) = c.x;
    el(1, i) = c.y;
    el(2, i) = c.z;
  }

  float Determinant() {
    return (lM[0] * Mat2Det(lM[4], lM[5], lM[8], lM[7])) -
           (lM[1] * Mat2Det(lM[3], lM[5], lM[6], lM[7])) +
           (lM[2] * Mat2Det(lM[3], lM[4], lM[6], lM[8]));
  }

  lMatrix3f Inverted() {
    lMatrix3f mat = *this;
    mat.Invert();
    return mat;
  }

  static lMatrix3f Rotate(ElAxis a, double angle) {
    lMatrix3f mat;
    mat.SetRotation(a, angle);
    return mat;
  }

  static lMatrix3f Rotate(lVec3f a, double angle) {
    lMatrix3f mat;
    mat.SetRotation(a, angle);
    return mat;
  }

  static lMatrix3f Scale(float s) {
    lMatrix3f mat;
    mat.SetScale(s);
    return mat;
  }

  void Invert() {
    lMatrix3f out;
    for (int r = 0; r < 3; r++) { // Pivots
      float rowEl = fabs(Row(r).v[r]);
      for (int j = r + 1; j < 3; j++) {
        if (fabs(Row(j).v[r]) > rowEl) {
          out.SwapRow(r, j);
          SwapRow(r, j);
        }
      }
      float pivotEl = Row(r).v[r];
      for (int i = 0; i < 3; i++) {
        if (i == r) {
          continue;
        }
        float rowEl = Col(r).v[i];
        out.Row(i, ((out.Row(i) * pivotEl) - (out.Row(r) * rowEl)));
        Row(i, ((Row(i) * pivotEl) - (Row(r) * rowEl)));
      }
    }
    for (int r = 0; r < 3; r++) { // Divides diagonals
      float element = Row(r).v[r];
      out.Row(r, (out.Row(r) / element));
      Row(r, (Row(r) / element));
    }
    *this = out;
  }

  void Set(float f00, float f01, float f02, float f10, float f11, float f12,
           float f20, float f21, float f22) {
    Row(0, lVec3f(f00, f01, f02));
    Row(1, lVec3f(f10, f11, f12));
    Row(2, lVec3f(f20, f21, f22));
  }

  void SetRotation(ElAxis a, double angle) { // Rotates about x, y, or z axis
    MakeIdentity();
    switch (a) {
    case AXIS_X: { // x-axis
      el(1, 1) = cos(angle);
      el(1, 2) = -sin(angle);
      el(2, 1) = sin(angle);
      el(2, 2) = cos(angle);
      break;
    }
    case AXIS_Y: { // y-axis
      el(0, 0) = cos(angle);
      el(0, 2) = sin(angle);
      el(2, 0) = -sin(angle);
      el(2, 2) = cos(angle);
      break;
    }
    case AXIS_Z: { // z-axis
      el(0, 0) = cos(angle);
      el(0, 1) = -sin(angle);
      el(1, 0) = sin(angle);
      el(1, 1) = cos(angle);
      break;
    }
    default: { break; }
    }
  }

  void SetRotation(lVec3f a, double angle) { // Rotates about any abitrary axis
    MakeIdentity();
    a.Normalize();
    lMatrix3f toXyPlane, fromXyPlane, toXAxis, fromXAxis;
    float xRot = 0.0f;
    float zRot = 0.0f;
    if (a.z != 0) {
      xRot = atan2(a.z, a.y);
      toXyPlane = lMatrix3f::Rotate(AXIS_X, -xRot);
      fromXyPlane = lMatrix3f::Rotate(AXIS_X, xRot);
    }
    if (a.y != 0) {
      zRot = atan2(sqrt(1.0f - a.x * a.x), a.x);
      toXAxis = lMatrix3f::Rotate(AXIS_Z, -zRot);
      fromXAxis = lMatrix3f::Rotate(AXIS_Z, zRot);
    }
    *this = fromXyPlane * fromXAxis * lMatrix3f::Rotate(AXIS_X, angle) *
            toXAxis * toXyPlane;
  }

  void SetScale(float s) {
    el(0, 0) = s;
    el(1, 1) = s;
    el(2, 2) = s;
  }

  float &el(int row, int col) { return lM[(row * 3) + col]; }
  float el(int row, int col) const { return lM[(row * 3) + col]; }
};

lVec3f operator*(const lMatrix3f &m, const lVec3f &v) {
  return lVec3f(m.el(0, 0) * v.x + m.el(0, 1) * v.y + m.el(0, 2) * v.z,
                m.el(1, 0) * v.x + m.el(1, 1) * v.y + m.el(1, 2) * v.z,
                m.el(2, 0) * v.x + m.el(2, 1) * v.y + m.el(2, 2) * v.z);
}
lVec3f operator*(const lVec3f &v, const lMatrix3f &m) {
  return lVec3f(Dot(m.Col(0), v), Dot(m.Col(1), v), Dot(m.Col(2), v));
}
lMatrix3f operator*(const lMatrix3f &m0, const lMatrix3f &m1) {
  return lMatrix3f((m0.Row(0) * m1), (m0.Row(1) * m1), (m0.Row(2) * m1));
}

bool operator==(const lMatrix3f &m0, const lMatrix3f &m1) {
  for (int i = 0; i < 9; i++) {
    if (m0.lM[i] != m1.lM[i]) {
      return false;
    }
  }
  return true;
}
bool operator!=(const lMatrix3f &m0, const lMatrix3f &m1) {
  return !(m0 == m1);
}

class lMatrix4f;

lMatrix4f operator*(const lMatrix4f &m0, const lMatrix4f &m1);

class lMatrix4f {
public:
  float lM[16];

  lMatrix4f() { MakeIdentity(); }

  lMatrix4f(float f00, float f01, float f02, float f03, float f10, float f11,
            float f12, float f13, float f20, float f21, float f22, float f23,
            float f30, float f31, float f32, float f33) {
    Set(f00, f01, f02, f03, f10, f11, f12, f13, f20, f21, f22, f23, f30, f31,
        f32, f33);
  }

  lMatrix4f(const lVec4f &r0, const lVec4f &r1, const lVec4f &r2,
            const lVec4f &r3) {
    Row(0, r0);
    Row(1, r1);
    Row(2, r2);
    Row(3, r3);
  }

  void MakeIdentity() {
    Set(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
  }

  lVec4f Row(int i) const {
    return lVec4f(el(i, 0), el(i, 1), el(i, 2), el(i, 3));
  }

  void Row(int i, const lVec4f &r) {
    el(i, 0) = r.x;
    el(i, 1) = r.y;
    el(i, 2) = r.z;
    el(i, 3) = r.w;
  }

  void SwapRow(int i, int j) {
    lVec4f stowRow = Row(i);
    Row(i, Row(j));
    Row(j, stowRow);
  }

  lVec4f Col(int i) const {
    return lVec4f(el(0, i), el(1, i), el(2, i), el(3, i));
  }

  void Col(int i, const lVec4f &c) {
    el(0, i) = c.x;
    el(1, i) = c.y;
    el(2, i) = c.z;
    el(3, i) = c.w;
  }

  float Determinant() {
    lMatrix3f a(lM[5], lM[6], lM[7], lM[9], lM[10], lM[11], lM[13], lM[14],
                lM[15]);
    lMatrix3f b(lM[4], lM[6], lM[7], lM[8], lM[10], lM[11], lM[12], lM[14],
                lM[15]);
    lMatrix3f c(lM[4], lM[5], lM[7], lM[8], lM[9], lM[11], lM[12], lM[13],
                lM[15]);
    lMatrix3f d(lM[4], lM[5], lM[6], lM[8], lM[9], lM[10], lM[12], lM[13],
                lM[14]);
    return (lM[0] * a.Determinant()) - (lM[1] * b.Determinant()) +
           (lM[2] * c.Determinant()) - (lM[3] * d.Determinant());
  }

  lMatrix4f Inverted(bool print = false) {
    lMatrix4f mat = *this;
    mat.Invert(print);
    return mat;
  }

  static lMatrix4f Rotate(ElAxis a, double angle) {
    lMatrix4f mat;
    mat.SetRotation(a, angle);
    return mat;
  }

  static lMatrix4f Rotate(lVec3f a, double angle) {
    lMatrix4f mat;
    mat.SetRotation(a, angle);
    return mat;
  }

  static lMatrix4f Scale(float s) {
    lMatrix4f mat;
    mat.SetScale(s);
    return mat;
  }

  static lMatrix4f Translate(float x, float y, float z) {
    lMatrix4f mat;
    mat.SetTranslate(x, y, z);
    return mat;
  }

  void Invert(bool print = false) {
    lMatrix4f out;
    if (print) {
      printOp(&el(0, 0), &out.el(0, 0));
    }
    for (int r = 0; r < 4; r++) { // Pivots
      float rowEl = fabs(Row(r).v[r]);
      for (int j = r + 1; j < 4; j++) {
        if (fabs(Row(j).v[r]) > rowEl) {
          out.SwapRow(r, j);
          SwapRow(r, j);
        }
      }
      float pivotEl = Row(r).v[r];
      for (int i = 0; i < 4; i++) {
        if (i == r) {
          continue;
        }
        float rowEl = Col(r).v[i];
        if (print) {
          printf("R[%i] = %.3f*R[%i] - %.3f*R[%i]\n\n", i, pivotEl, i, rowEl,
                 r);
        }
        out.Row(i, ((out.Row(i) * pivotEl) - (out.Row(r) * rowEl)));
        Row(i, ((Row(i) * pivotEl) - (Row(r) * rowEl)));
        if (print) {
          printOp(&el(0, 0), &out.el(0, 0));
        }
      }
    }
    for (int r = 0; r < 4; r++) { // Divides diagonals
      float element = Row(r).v[r];
      if (print) {
        printf("R[%i] = R[%i] / %f\n\n", r, r, element);
      }
      out.Row(r, (out.Row(r) / element));
      Row(r, (Row(r) / element));
      if (print) {
        printOp(&el(0, 0), &out.el(0, 0));
      }
    }
    *this = out;
  }

  void Set(float f00, float f01, float f02, float f03, float f10, float f11,
           float f12, float f13, float f20, float f21, float f22, float f23,
           float f30, float f31, float f32, float f33) {
    Row(0, lVec4f(f00, f01, f02, f03));
    Row(1, lVec4f(f10, f11, f12, f13));
    Row(2, lVec4f(f20, f21, f22, f23));
    Row(3, lVec4f(f30, f31, f32, f33));
  }

  void SetRotation(ElAxis a, double angle) { // Rotates about x, y, or z axis
    MakeIdentity();
    switch (a) {
    case AXIS_X: { // x-axis
      el(1, 1) = cos(angle);
      el(1, 2) = -sin(angle);

      el(2, 1) = sin(angle);
      el(2, 2) = cos(angle);
      break;
    }
    case AXIS_Y: { // y-axis
      el(0, 0) = cos(angle);
      el(0, 2) = sin(angle);

      el(2, 0) = -sin(angle);
      el(2, 2) = cos(angle);
      break;
    }
    case AXIS_Z: { // z-axis
      el(0, 0) = cos(angle);
      el(0, 1) = -sin(angle);

      el(1, 0) = sin(angle);
      el(1, 1) = cos(angle);
      break;
    }
    default: { break; }
    }
  }

  void SetRotation(lVec3f a, double angle) { // Rotates about any abitrary axis
    MakeIdentity();
    a.Normalize();
    lMatrix4f toXyPlane, fromXyPlane, toXAxis, fromXAxis;
    float xRot = 0.0f;
    float zRot = 0.0f;

    if (a.z != 0) {
      xRot = atan2(a.z, a.y);
      toXyPlane = lMatrix4f::Rotate(AXIS_X, -xRot);
      fromXyPlane = lMatrix4f::Rotate(AXIS_X, xRot);
    }
    if (a.y != 0) {
      zRot = atan2(sqrt(1.0f - a.x * a.x), a.x);
      toXAxis = lMatrix4f::Rotate(AXIS_Z, -zRot);
      fromXAxis = lMatrix4f::Rotate(AXIS_Z, zRot);
    }

    *this = fromXyPlane * fromXAxis * lMatrix4f::Rotate(AXIS_X, angle) *
            toXAxis * toXyPlane;
  }

  void SetScale(float s) {
    el(0, 0) = s;
    el(1, 1) = s;
    el(2, 2) = s;
  }

  void SetTranslate(float x, float y, float z) {
    el(0, 3) = x;
    el(1, 3) = y;
    el(2, 3) = z;
  }

  float &el(int row, int col) { return lM[(row * 4) + col]; }
  float el(int row, int col) const { return lM[(row * 4) + col]; }
};

lVec4f operator*(const lMatrix4f &m, const lVec4f &v) {
  return lVec4f(Dot(m.Row(0), v), Dot(m.Row(1), v), Dot(m.Row(2), v),
                Dot(m.Row(3), v));
}
lVec4f operator*(const lVec4f &v, const lMatrix4f &m) {
  return lVec4f(Dot(m.Col(0), v), Dot(m.Col(1), v), Dot(m.Col(2), v),
                Dot(m.Col(3), v));
}
lMatrix4f operator*(const lMatrix4f &m0, const lMatrix4f &m1) {
  return lMatrix4f((m0.Row(0) * m1), (m0.Row(1) * m1), (m0.Row(2) * m1),
                   (m0.Row(3) * m1));
}

bool operator==(const lMatrix4f &m0, const lMatrix4f &m1) {
  for (int i = 0; i < 16; i++) {
    if (m0.lM[i] != m1.lM[i]) {
      return false;
    }
  }
  return true;
}
bool operator!=(const lMatrix4f &m0, const lMatrix4f &m1) {
  return !(m0 == m1);
}

lMatrix4f ToMatrix4(lMatrix3f &mat3) {
  lMatrix4f mat4;
  mat4.Row(0, lVec4f(mat3.el(0, 0), mat3.el(0, 1), mat3.el(0, 2), 0.0f));
  mat4.Row(1, lVec4f(mat3.el(1, 0), mat3.el(1, 1), mat3.el(1, 2), 0.0f));
  mat4.Row(2, lVec4f(mat3.el(2, 0), mat3.el(2, 1), mat3.el(2, 2), 0.0f));
  mat4.Row(3, lVec4f(0.0f, 0.0f, 0.0f, 1.0f));
  return mat4;
}

lMatrix3f ToMatrix3(lMatrix4f &mat4) {
  lMatrix3f mat3;
  mat3.Row(0, lVec3f(mat4.el(0, 0), mat4.el(0, 1), mat4.el(0, 2)));
  mat3.Row(1, lVec3f(mat4.el(1, 0), mat4.el(1, 1), mat4.el(1, 2)));
  mat3.Row(2, lVec3f(mat4.el(2, 0), mat4.el(2, 1), mat4.el(2, 2)));
  return mat3;
}

class lQuaternionf {
public:
  union {
    float q[4];
    struct {
      float w, x, y, z;
    };
  };

  lQuaternionf() { Identity(); }
  lQuaternionf(float v[4]) { SetValue(v); }
  lQuaternionf(float q0, float q1, float q2, float q3) {
    SetValue(q0, q1, q2, q3);
  }
  lQuaternionf(lMatrix4f &m) { SetValue(m); }
  lQuaternionf(lVec3f axis, float radians) { SetValue(axis, radians); }
  lQuaternionf(lVec3f &rotateFrom, lVec3f &rotateTo) {
    SetValue(rotateFrom, rotateTo);
  }

  /*
  lQuaternionf(lVec3f &fromLook, lVec3f &fromUp, lVec3f &toLook, lVec3f &toUp) {
    SetValue(fromLook, fromUp, toLook, toUp);
  }
  */

  void Identity() {
    q[1] = q[1] = q[3] = 0.0f;
    q[0] = 1.0f;
  }

  void SetValue(float q0, float q1, float q2, float q3) {
    q[0] = q0;
    q[1] = q1;
    q[2] = q2;
    q[3] = q3;
  }

  void SetValue(float *ql) {
    q[0] = ql[0];
    q[1] = ql[1];
    q[2] = ql[2];
    q[3] = ql[3];
  }

  void SetValue(lMatrix4f m) {
    /*
    float t, k;
    if (m.el(2, 2) < 0) {
      if (m.el(0, 0) > m.el(1, 1)) { // X-form
        t = 1 + m.el(0, 0) - m.el(1, 1) - m.el(2, 2);
        k = 0.5 / sqrt(t);
        SetValue(t * k, (m.el(0, 1) + m.el(1, 0)) * k, (m.el(2, 0) + m.el(0, 2))
    * k, (m.el(1, 2) - m.el(2, 1)) * k); } else { // Y-form t = 1 - m.el(0, 0) +
    m.el(1, 1) - m.el(2, 2); k = 0.5 / sqrt(t); SetValue((m.el(0, 1) + m.el(1,
    0)) * k, t * k, (m.el(1, 2) + m.el(2, 1)) * k, (m.el(2, 0) - m.el(0, 2)) *
    k);
      }
    } else {
      if (m.el(0, 0) < -m.el(1, 1)) { // Z-form
        t = 1 - m.el(0, 0) - m.el(1, 1) + m.el(2, 2);
        k = 0.5 / sqrt(t);
        SetValue((m.el(2, 0) + m.el(0, 2)) * k, (m.el(1, 2) + m.el(2, 1)) * k, t
    * k, (m.el(0, 1) - m.el(1, 0)) * k); } else { // W-form t = 1 + m.el(0, 0) +
    m.el(1, 1) + m.el(2, 2); k = 0.5 / sqrt(t); SetValue((m.el(1, 2) - m.el(2,
    1)) * k, (m.el(2, 0) - m.el(0, 2)) * k, (m.el(0, 1) - m.el(1, 0)) * k, t *
    k);
      }
    }
    */

    float tr, S;

    tr = m.el(0, 0) + m.el(1, 1) + m.el(2, 2);

    if (tr > 0) {
      S = sqrt(tr + 1.0) * 2; // S=4*qw
      q[0] = 0.25 * S;
      q[1] = (m.el(2, 1) - m.el(1, 2)) / S;
      q[2] = (m.el(0, 2) - m.el(2, 0)) / S;
      q[3] = (m.el(1, 0) - m.el(0, 1)) / S;
    } else if ((m.el(0, 0) > m.el(1, 1)) && (m.el(0, 0) > m.el(2, 2))) {
      S = sqrt(1.0 + m.el(0, 0) - m.el(1, 1) - m.el(2, 2)) * 2; // S=4*qx
      q[0] = (m.el(2, 1) - m.el(1, 2)) / S;
      q[1] = 0.25 * S;
      q[2] = (m.el(0, 1) + m.el(1, 0)) / S;
      q[3] = (m.el(0, 2) + m.el(2, 0)) / S;
    } else if (m.el(1, 1) > m.el(2, 2)) {
      S = sqrt(1.0 + m.el(1, 1) - m.el(0, 0) - m.el(2, 2)) * 2; // S=4*qy
      q[0] = (m.el(0, 2) - m.el(2, 0)) / S;
      q[1] = (m.el(0, 1) + m.el(1, 0)) / S;
      q[2] = 0.25 * S;
      q[3] = (m.el(1, 2) + m.el(2, 1)) / S;
    } else {
      S = sqrt(1.0 + m.el(2, 2) - m.el(0, 0) - m.el(1, 1)) * 2; // S=4*qz
      q[0] = (m.el(1, 0) - m.el(0, 1)) / S;
      q[1] = (m.el(0, 2) + m.el(2, 0)) / S;
      q[2] = (m.el(1, 2) + m.el(2, 1)) / S;
      q[3] = 0.25 * S;
    }
    printf("%.3f, %.3f, %.3f : %.3f\n", x, y, z, w);
  }

  void SetValue(lVec3f axis, float theta) {
    axis.Normalize();
    theta *= 0.5;
    float sinTheta = sin(theta);
    w = cos(theta);
    x = sinTheta * axis.x;
    y = sinTheta * axis.y;
    z = sinTheta * axis.z;
  }

  void SetValue(lVec3f rotFrom, lVec3f rotTo) {
    lVec3f p1 = rotFrom.Normalized();
    lVec3f p2 = rotTo.Normalized();
    float alpha = Dot(p1, p2);

    if (alpha > 1.0f) {
      Identity();
    }
    if (alpha < -1.0f) {
      lVec3f v;
      if (p1.x != p1.y) {
        v = lVec3f(p1.y, p1.x, p1.z);
      } else {
        v = lVec3f(p1.z, p1.y, p1.x);
      }
      v = v - (p1 * Dot(p1, v));
      v.Normalize();
      SetValue(v, M_PI);
    } else {
      p1 = Cross(p1, p2);
      p1.Normalize();
      SetValue(p1, acos(alpha));
    }
  }

  /*
  void SetValue(lVec3f fromLook, lVec3f fromUp, lVec3f toLook, lVec3f toUp) {
    lVec3f fL = fromLook.Normalized();
    lVec3f fU = fromUp.Normalized();
    lVec3f tL = toLook.Normalized();
    lVec3f tU = toUp.Normalized();
    lQuaternionf r(fL, tL);

    lVec3f rfU = r * fU;
    // lVec3f rfL = r * fL;

    lVec3f tUo = tU.Orthonormalized(tL);

    float d = max(-1.0f, std::min(1.0f, Dot(rfU, tUo)));

    float twist = acosf(d);
    lVec3f ux = Cross(rfU, tUo);
    if (Dot(ux, tL) < 0) {
      twist = -twist;
    }

    lQuaternionf rTwist = Quaternion(tL, twist);

    *this = rTwist * r;
  }

  float *GetValue() {
    return &q[0];
  }
  */

  void GetValue(lVec3f &axis, float &radians) {
    radians = acos(q[0]) * 2.0f;
    if (radians == 0.0f)
      axis = lVec3f(0.0, 0.0, 1.0);
    else {
      axis.x = q[1];
      axis.y = q[2];
      axis.z = q[3];
      axis.Normalize();
    }
  }

  void GetValue(lMatrix3f &m) {
    float s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;
    float norm = q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
    s = (norm == 0.0f) ? 0.0f : (2.0f / norm);

    xs = q[0] * s;
    ys = q[1] * s;
    zs = q[2] * s;
    wx = q[3] * xs;
    wy = q[3] * s;
    wz = q[3] * zs;
    xx = q[0] * xs;
    xy = q[0] * ys;
    xz = q[0] * zs;
    yy = q[1] * ys;
    yz = q[1] * zs;
    zz = q[2] * zs;

    m.el(0, 0) = 1.0f - (yy + zz);
    m.el(1, 0) = xy + wz;
    m.el(2, 0) = xz - wy;
    m.el(0, 1) = xy - wz;
    m.el(1, 1) = 1.0f - (xx + zz);
    m.el(2, 1) = yz + wx;
    m.el(0, 2) = xz + wy;
    m.el(1, 2) = yz - wx;
    m.el(2, 2) = 1.0f - (xx + yy);
  }

  lMatrix3f GetMatrix3() {
    lMatrix3f mat3;
    GetValue(mat3);
    return mat3;
  }

  lMatrix4f GetMatrix4() {
    lMatrix3f mat4;
    GetValue(mat4);
    return ToMatrix4(mat4);
  }
};

lQuaternionf operator*(lQuaternionf q0, lQuaternionf q1) {
  lQuaternionf out;
  out.w = (q0.w * q1.w) - (q0.x * q1.x) - (q0.y * q1.y) - (q0.z * q1.z);
  out.x = (q0.w * q1.x) + (q0.x * q1.w) + (q0.y * q1.z) - (q0.z * q1.y);
  out.y = (q0.w * q1.y) + (q0.y * q1.w) + (q0.z * q1.x) - (q0.x * q1.z);
  out.z = (q0.w * q1.z) + (q0.z * q1.w) + (q0.x * q1.y) - (q0.y * q1.x);
  return out;
}
lVec3f operator*(lQuaternionf q, lVec3f v) {
  float vCo = (q.w * q.w) - (q.x * q.x) - (q.y * q.y) - (q.z * q.z);
  float uCo = 2.0f * ((v.x * q.x) + (v.y * q.y) + (v.z * q.z));
  float cCo = 2.0f * q.w;
  return lVec3f(
      ((vCo * v.x) + (uCo * q.x) + cCo * ((q.y * v.z) - (q.z * v.y))),
      ((vCo * v.y) + (uCo * q.y) + cCo * ((q.z * v.x) - (q.x * v.z))),
      ((vCo * v.z) + (uCo * q.z) + cCo * ((q.x * v.y) - (q.y * v.x))));
}
