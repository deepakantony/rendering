#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

struct vertex {
  vertex(float x, float y, float z) : _x(x), _y(y), _z(z) {}
  vertex operator+(const vertex &v) const { return vertex(_x+v._x, _y+v._y, _z+v._z); }
  vertex operator/(float n) const { return vertex(_x/n, _y/n, _z/n); }
  void gl() { glVertex3f(_x, _y, _z); }
  float _x, _y, _z;
};

void triangle(vertex v1, vertex v2, vertex v3) {
  v1.gl(); v2.gl(); v3.gl();
}

void tetrahedron(vertex v1, vertex v2, vertex v3, vertex v4) {
  glColor3f(1.f,1.f,0.f);
  triangle(v1, v2, v3);
  glColor3f(1.f,0.2,0.2);
  triangle(v1, v3, v4);
  glColor3f(0.f,1.f,0.f);
  triangle(v1, v2, v4);
  glColor3f(0.f,1.f,1.f);
  triangle(v2, v3, v4);
}

void sierpinski_gasket_3D(vertex a, vertex b, vertex c, vertex d, int k) {
  if(k>0) {
    vertex mab = (a+b)/2;
    vertex mac = (a+c)/2;
    vertex mbc = (b+c)/2;
    vertex mad = (a+d)/2;
    vertex mbd = (b+d)/2;
    vertex mcd = (c+d)/2;
    sierpinski_gasket_3D(a, mab, mac, mad, k-1);
    sierpinski_gasket_3D(mab, b, mbc, mbd, k-1);
    sierpinski_gasket_3D(mac, mbc, c, mcd, k-1);
    sierpinski_gasket_3D(mad, mbd, mcd, d, k-1);
  }
  else
    tetrahedron(a,b,c,d);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  vertex v1(0.f,0.f,50.f), v2(50.f,0.f,50.f), v3(25.f, 50.f, 50.f), v4(25.f, 25.f, 0.f);
  glBegin(GL_TRIANGLES);
  sierpinski_gasket_3D(v1, v2, v3, v4, 5);
  glEnd();
  glFlush();
}

void init() {
  glClearColor(1.f, 1.f, 1.f, 1.f);
  glColor3f(1.f, 0.f, 0.f);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 50.0, 0.0, 50.0, 50.f, -50.0);
  glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE | GLUT_DEPTH);

  glutInitWindowSize(500, 500);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Sierpinski gasket 3d");
  init();
  glutDisplayFunc(display);
  glutMainLoop();
  return 0;
}
