#include <iostream>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

using namespace std;

struct vertex {
  vertex(float x, float y, float z, float w)
    :_x(x), _y(y), _z(z), _w(w) {}
  vertex(float x, float y, float z)
    :_x(x), _y(y), _z(z), _w(1.f) {}
  vertex(float x, float y)
    :_x(x), _y(y), _z(0), _w(1.f) {}

  vertex operator/(const float n) {
    return(vertex(_x/n, _y/n, _z/n, _w));
  }

  vertex operator+(const vertex &v) {
    return(vertex(_x+v._x, _y+v._y, _z+v._z, _w));
  }

  void gl() const { glVertex4f(_x, _y, _z, _w); }
  float _x, _y, _z, _w;
};

void triangle(vertex v1, vertex v2, vertex v3) {
  v1.gl(); v2.gl(); v3.gl();
}

void sierpinski_gasket(vertex v1, vertex v2, vertex v3, int k) {
  if(k > 0) {
    vertex v12 = (v1+v2)/2.f;
    vertex v23 = (v2+v3)/2.f;
    vertex v31 = (v3+v1)/2.f;
    sierpinski_gasket(v1, v12, v31, k-1);
    sierpinski_gasket(v2, v23, v12, k-1);
    sierpinski_gasket(v3, v23, v31, k-1);
  }
  else triangle(v1, v2, v3);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);

  stack<vertex> st;
  vertex v1(0.0, 0.0), v2(50.0, 0.0), v3(25.0, 50.0);

  glBegin(GL_TRIANGLES);
  sierpinski_gasket(v1, v2, v3, 5);
  glEnd();

  glFlush();
}

void init() {
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glColor3f(1.0, 0.0, 0.0);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, 50.0, 0.0, 50.0);
  glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

  glutInitWindowSize(500, 500);
  glutInitWindowPosition(0,0);
  glutCreateWindow("Sierpinski gasket program");

  glutDisplayFunc(display);
  init();
  glutMainLoop();
  
  return 0;
}
