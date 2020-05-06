// cube generator

static void appendVert(Object & obj, const Matrix4f& m, const Vec3f& v) {
  Vec3f xv = m * v;
  obj.position( xv.x, xv.y, xv.z );
}

static void appendSquare( Object & obj, Matrix4f & m ) {
  appendVert( obj, m, Vec3f(-1, -1, 0));
  appendVert( obj, m, Vec3f(1, -1, 0));
  appendVert( obj, m, Vec3f(-1, 1, 0));

  appendVert( obj, m, Vec3f(1, -1, 0));
  appendVert( obj, m, Vec3f(1, 1, 0));
  appendVert( obj, m, Vec3f(-1, 1, 0));
}

static void makeCube( Object& obj, const Matrix4f& mbase ) {

  Matrix4f m;
  Matrix4f mtrans;
  {
    Posef p;
    p.t.z = 1;
    mtrans = p.GetMatrix4();
  }

  Posef p;

  obj.begin(GL_TRIANGLES);
  obj.color(0.1f, 0.05f, 0.95f);

  m = mbase * p.GetMatrix4() * mtrans;
  appendSquare( obj, m);

  obj.color(0.1f, 0.95f, 0.05f);
  p.r = Quaternionf( Vec3f( 1, 0, 0 ), ToRadians(90.0f));
  m = mbase * p.GetMatrix4() * mtrans;
  appendSquare( obj, m);

  obj.color(0.95f, 0.05f, 0.05f);
  p.r = Quaternionf( Vec3f( 1, 0, 0 ), ToRadians(180.0f));
  m = mbase * p.GetMatrix4() * mtrans;
  appendSquare( obj, m);

  obj.color(0.1f, 0.95f, 0.95f);
  p.r = Quaternionf( Vec3f( 1, 0, 0 ), ToRadians(270.0f));
  m = mbase * p.GetMatrix4() * mtrans;
  appendSquare( obj, m);

  obj.color(0.95f, 0.05f, 0.95f);
  p.r = Quaternionf( Vec3f( 0, 1, 0 ), ToRadians(90.0f));
  m = mbase * p.GetMatrix4() * mtrans;
  appendSquare( obj, m);

  obj.color(0.95f, 0.95f, 0.05f);
  p.r = Quaternionf( Vec3f( 0, 1, 0 ), ToRadians(-90.0f));
  m = mbase * p.GetMatrix4() * mtrans;
  appendSquare( obj, m);

  obj.end();
}