#include <windows.h>
#include <GL/glut.h>

void init(void) {
    // Limpa a tela e preenche com uma cor sólida
    glClearColor(1.0, 1.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void draw(void) {
    // Desenha linha de cima
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(-0.05, 0.1);
    glVertex2f(0.05, 0.1);
    glVertex2f(0.05, 1);
    glVertex2f(-0.05, 1);
    glEnd();

    // desenha linha de cima no meio
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(-0.5, 0.5);
    glVertex2f(-0.5, 0.4);
    glVertex2f(0.5, 0.4);
    glVertex2f(0.5, 0.5);
    glEnd();

    // desenha linha da direita
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(0.07, 0.05);
    glVertex2f(1, 0.05);
    glVertex2f(1, -0.05);
    glVertex2f(0.07, -0.05);
    glEnd();

    // desenha linha da direita no meio
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(0.5, 0.5);
    glVertex2f(0.4, 0.5);
    glVertex2f(0.4, -0.5);
    glVertex2f(0.5, -0.5);
    glEnd();

    // desenha linha de baixo
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(-0.05, -0.1);
    glVertex2f(0.05, -0.1);
    glVertex2f(0.05, -1);
    glVertex2f(-0.05, -1);
    glEnd();

    // desenha linha de baixo no meio
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(-0.5, -0.4);
    glVertex2f(-0.5, -0.5);
    glVertex2f(0.5, -0.5);
    glVertex2f(0.5, -0.4);
    glEnd();

    // desenha linha da esquerda
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(-0.07, 0.05);
    glVertex2f(-0.07, -0.05);
    glVertex2f(-1, -0.05);
    glVertex2f(-1, 0.05);
    glEnd();

    // desenha linha da esquerda no meio
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(-0.5, 0.5);
    glVertex2f(-0.4, 0.5);
    glVertex2f(-0.4, -0.5);
    glVertex2f(-0.5, -0.5);
    glEnd();
}

void display(void) {
    draw();
    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutCreateWindow("Alvo");
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(50, 50);
    init();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}