#include <windows.h>
#include <stdio.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "tgaload.h"


// Qtd m�xima de texturas a serem usadas no programa
#define MAX_NO_TEXTURES 1
#define CUBE_TEXTURE 0
// vetor com os n�meros das texturas
GLuint texture_id[MAX_NO_TEXTURES];

void init(void) {
    // Limpa a tela e preenche com uma cor sólida
    glClearColor(1.0, 1.0, 0.5, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void draw(void) {
    // Moldura quadro
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0);
    glVertex2f(-0.45f, 0.75f);
    glVertex2f(0.45f, 0.75f);
    glVertex2f(0.45f, 0.10f);
    glVertex2f(-0.45f, 0.10f);
    glEnd();

    // quadro
    glBegin(GL_QUADS);
    glColor3f(0, 0.8, 1);
    glVertex2f(-0.35f, 0.70f);
    glVertex2f(0.35f, 0.70f);
    glVertex2f(0.35f, 0.15f);
    glVertex2f(-0.35f, 0.15f);
    glEnd();

    // tampo bancada
    glBegin(GL_QUADS);
    glColor3f(0, 0.3, 0);
    glVertex2f(-0.80f, 0);
    glVertex2f(0.80f, 0);
    glVertex2f(0.80f, -0.05f);
    glVertex2f(-0.80f, -0.05f);
    glEnd();

    // porta esq
    glBegin(GL_QUADS);
    glColor3f(0, 0.3, 0);
    glVertex2f(-0.75f, -0.08);
    glVertex2f(-0.02, -0.08);
    glVertex2f(-0.02, -0.8);
    glVertex2f(-0.75f, -0.8);
    glEnd();

    // porta dir
    glBegin(GL_QUADS);
    glColor3f(0, 0.3, 0);
    glVertex2f(0.02, -0.08);
    glVertex2f(0.75, -0.08);
    glVertex2f(0.75, -0.8);
    glVertex2f(0.02, -0.8);
    glEnd();

    // pé esq
    glBegin(GL_QUADS);
    glColor3f(0, 0.3, 0);
    glVertex2f(-0.7, -0.82);
    glVertex2f(-0.5, -0.82);
    glVertex2f(-0.5, -0.98);
    glVertex2f(-0.7, -0.98);
    glEnd();

    // pé dir
    glBegin(GL_QUADS);
    glColor3f(0, 0.3, 0);
    glVertex2f(0.7, -0.82);
    glVertex2f(0.5, -0.82);
    glVertex2f(0.5, -0.98);
    glVertex2f(0.7, -0.98);
    glEnd();

}

void display(void) {
    draw();
    glFlush();
}

void initTexture(void)
{

	// Habilita o uso de textura 
	glEnable ( GL_TEXTURE_2D );

	// Define a forma de armazenamento dos pixels na textura (1= alihamento por byte)
	glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

	// Define quantas texturas ser�o usadas no programa 
	glGenTextures (1, texture_id);  // 1 = uma textura;
									// texture_id = vetor que guardas os n�meros das texturas

	// Define o n�mero da textura do cubo.
	texture_id[CUBE_TEXTURE] = 1001;

	// Define que tipo de textura ser� usada
	// GL_TEXTURE_2D ==> define que ser� usada uma textura 2D (bitmaps)
	// texture_id[CUBE_TEXTURE]  ==> define o n�mero da textura 
	glBindTexture ( GL_TEXTURE_2D, texture_id[CUBE_TEXTURE] );

	// carrega a uma imagem TGA 
	image_t temp_image;
	tgaLoad  ( "tartaruga.tga", &temp_image, TGA_FREE | TGA_LOW_QUALITY );

}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(400, 600);
    glutCreateWindow("Aplicando");
    glutInitWindowPosition(50, 50);
    init();
    initTexture();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}