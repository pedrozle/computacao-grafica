#include <windows.h>
#include <GL/glut.h>

void init(void) {
    // Limpa a tela e preenche com uma cor sólida
    glClearColor(0.0, 1.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void draw(void) {
    // Desenha varios losangos
    glBegin(GL_QUADS);
    glColor3f(0.0, 0.0, 1.0);
    glVertex2f(0, 0.25);
    glVertex2f(0.25, 0.5);
    glVertex2f(0, 0.75);
    glVertex2f(-0.25, 0.5);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(0.25, 0);
    glVertex2f(0.5, -0.25);
    glVertex2f(0.75, 0);
    glVertex2f(0.5, 0.25);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.0, 0.0, 0.0);
    glVertex2f(0, -0.25);
    glVertex2f(0.25, -0.5);
    glVertex2f(0, -0.75);
    glVertex2f(-0.25, -0.5);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 0.0);
    glVertex2f(-0.25, 0);
    glVertex2f(-0.5, -0.25);
    glVertex2f(-0.75, 0);
    glVertex2f(-0.5, 0.25);
    glEnd();
}

void display(void) {
    draw();
    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutCreateWindow("Losangulos");
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(50, 50);
    init();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}