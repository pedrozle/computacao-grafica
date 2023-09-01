/*
1) Faça um programa que apresenta as  transformações geométricas aplicadas a uma primitiva.
    - Acrescente interações com o mouse e teclado.
*/

#include <windows.h>
#include <GL/glut.h>

GLfloat taxa;
GLfloat sX, sY;
GLfloat tX, tY;
GLfloat rotaciona;

void resetVariables() {
    tX = tY = rotaciona = 0;
    sX = sY = 1;
}

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    taxa = 0.2f;
    resetVariables();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

void draw(void) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT);

    glTranslatef(-tX, -tY, 0);
    glScalef(sX, sY, 1.0);
    glRotatef(rotaciona, 0, 0, 1);


    /* desenha um simples ret�ngulo ou outro objeto*/
    glBegin(GL_QUADS);
    glColor3f(0.0, 0.0, 1.0);
    glVertex2f(-0.15, 0.15);
    glVertex2f(0.15, 0.15);
    glVertex2f(0.15, -0.15);
    glVertex2f(-0.15, -0.15);
    glEnd();

    glFlush();
}

void display(void) {
    draw();
    glFlush();
}

void keyboard(unsigned char tecla, int x, int y)
{
    switch (tecla) {

        // Configura a translação
    case 'a':
        tX = tX + taxa;
        break;

    case 'd':
        tX = tX - taxa;
        break;

    case 'w':
        tY = tY - taxa;
        break;

    case 's':
        tY = tY + taxa;
        break;

        // Configura a escala
    case 'E':
        sX = sX + taxa;
        break;

    case 'e':
        sX = sX - taxa;
        break;

    case 'Q':
        sY = sY + taxa;
        break;

    case 'q':
        sY = sY - taxa;
        break;

        
        // Configura a rotação
    case 'f':
        rotaciona = rotaciona + 10 * taxa;
        break;

    case 'F':
        rotaciona = rotaciona - 10 * taxa;
        break;

    case 'o':
        resetVariables();
        break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutCreateWindow("Transformações Geométricas");
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    init();
    glutKeyboardFunc(keyboard);
    // glutMouseFunc(mouse);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}