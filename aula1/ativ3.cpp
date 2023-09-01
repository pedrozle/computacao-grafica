#include <windows.h>
#include <GL/glut.h>

void init(void) {
    // Limpa a tela e preenche com uma cor sólida
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glLineWidth(3);
    glClear(GL_COLOR_BUFFER_BIT);
}

void draw(void) {

    // Desenha linha de cima
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0, 1.0, 0.0);
    glVertex2f(0, 0.75);
    glVertex2f(0.25, 0.25);
    glColor3f(0.0, 0.0, 1.0);
    glVertex2f(0.75, -0.5);
    glVertex2f(0, -0.25);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(-0.75, -0.5);
    glVertex2f(-0.25, 0.25);
    glColor3f(0.0, 1.0, 0.0);
    glVertex2f(0, 0.75);
    glEnd();
}

void display(void) {
    draw();
    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutCreateWindow("Seta LGBTQIA+");
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(50, 50);
    init();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}