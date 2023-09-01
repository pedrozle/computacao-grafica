#include <stdlib.h>
#include <GL/glut.h>

GLfloat fAspect = 1;

void Desenha() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, fAspect, 0.5, 500);
    glFrustum(0.4, 0.4, 0.4, 0.4, 0.3, 0.8);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(40, 60, 100, 0, 0, 0, 0, 1, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0, 0, 1);
    // glutWireCube(50);
    // glutWireTeapot(30);
    // glutWireSphere(30,10,10);
    // glutWireCone(30, 50, 10, 10);
    glRotatef(30, 0, 1, 0);
    glutSolidTorus(10, 20, 100, 100);
    glFlush();
}

void Teclado(unsigned char key, int x, int y)
{
    if (key == 27)
        exit(0);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glLineWidth(2.0);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(5, 5);
    glutInitWindowSize(450, 450);
    glutCreateWindow("Desenho de um cubo");
    glutDisplayFunc(Desenha);
    glutKeyboardFunc(Teclado);
    glutMainLoop();
    return 0;
}

