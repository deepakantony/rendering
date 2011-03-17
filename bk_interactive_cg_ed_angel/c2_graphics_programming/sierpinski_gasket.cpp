#include <iostream>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

using namespace std;

void display() {
}

void init() {
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
