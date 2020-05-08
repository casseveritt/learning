// cube generator

static void appendVert(Object & obj, const Matrix4f& m, const Vec3f& v) {
  Vec3f xv = m * v;
  obj.position( xv.x, xv.y, xv.z );
}

static void appendSquare( Object & obj, Matrix4f & m ) {
  appendVert( obj, m, Vec3f(-1, -1, 1));
  appendVert( obj, m, Vec3f(1, -1, 1));
  appendVert( obj, m, Vec3f(-1, 1, 1));

  appendVert( obj, m, Vec3f(1, -1, 1));
  appendVert( obj, m, Vec3f(1, 1, 1));
  appendVert( obj, m, Vec3f(-1, 1, 1));
}

static void makeCube( Object& obj, const Matrix4f& mbase ) {

  Matrix4f m = mbase;
  Posef p;

  obj.begin(GL_TRIANGLES);
  obj.color(0.1f, 0.05f, 0.95f);

  obj.normal(0.0,0.0,1.0);
  appendSquare( obj, m);
  p.r = Quaternionf( Vec3f( 1, 0, 0 ), ToRadians(90.0f));
  m *= p.GetMatrix4();

  obj.normal(0.0,-1.0,0.0);
  obj.color(0.1f, 0.95f, 0.05f);
  appendSquare( obj, m);
  m *= p.GetMatrix4();

  obj.normal(0.0,0.0,-1.0);
  obj.color(0.95f, 0.05f, 0.05f);
  appendSquare( obj, m);
  m *= p.GetMatrix4();

  obj.normal(0.0,1.0,0.0);
  obj.color(0.1f, 0.95f, 0.95f);
  appendSquare( obj, m);
  m *= p.GetMatrix4();
  p.r = Quaternionf( Vec3f( 0, 1, 0 ), ToRadians(90.0f));
  m *= p.GetMatrix4();

  obj.normal(1.0,0.0,0.0);
  obj.color(0.95f, 0.05f, 0.95f);
  appendSquare( obj, m);
  m *= p.GetMatrix4();
  m *= p.GetMatrix4();

  obj.normal(-1.0,0.0,0.0);
  obj.color(0.95f, 0.95f, 0.05f);
  appendSquare( obj, m);

  obj.end();
}