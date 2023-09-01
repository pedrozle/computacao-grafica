//*****************************************************
//
// CasaComEscala.cpp 
// Um programa OpenGL simples que abre uma janela GLUT, 
// altera a escala e desenha uma casa.
//
// Marcelo Cohen e Isabel H. Manssour
// Este c�digo acompanha o livro 
// "OpenGL - Uma Abordagem Pr�tica e Objetiva"
// 
//*****************************************************

#include <stdlib.h>
#include <GL/glut.h>

void Desenha(void)
{

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT);

	//glScalef(0.4f, 2.0f, 1.0f);
	//glRotatef(30.0f, 1.0f, 1.0f, 1.0f);
	glTranslatef(18.0f, 12.0f, 0.0f);

	glColor3f(0.0f, 0.0f, 1.0f);

	glBegin(GL_QUADS);
	glVertex2f(-15.0f, -15.0f);
	glVertex2f(-15.0f, 5.0f);
	glVertex2f(15.0f, 5.0f);
	glVertex2f(15.0f, -15.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
	glVertex2f(-4.0f, -14.5f);
	glVertex2f(-4.0f, 0.0f);
	glVertex2f(4.0f, 0.0f);
	glVertex2f(4.0f, -14.5f);
	glVertex2f(7.0f, -5.0f);
	glVertex2f(7.0f, -1.0f);
	glVertex2f(13.0f, -1.0f);
	glVertex2f(13.0f, -5.0f);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);

	glBegin(GL_LINES);
	glVertex2f(7.0f, -3.0f);
	glVertex2f(13.0f, -3.0f);
	glVertex2f(10.0f, -1.0f);
	glVertex2f(10.0f, -5.0f);
	glEnd();


	glColor3f(1.0f, 0.0f, 0.0f);

	glBegin(GL_TRIANGLES);
	glVertex2f(-15.0f, 5.0f);
	glVertex2f(0.0f, 17.0f);
	glVertex2f(15.0f, 5.0f);
	glEnd();
	glFlush();
}

void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
	GLsizei largura, altura;

	if (h == 0) h = 1;

	largura = w;
	altura = h;

	glViewport(0, 0, largura, altura);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (largura <= altura)
		gluOrtho2D(-40.0f, 40.0f, -40.0f * altura / largura, 40.0f * altura / largura);
	else
		gluOrtho2D(-40.0f * largura / altura, 40.0f * largura / altura, -40.0f, 40.0f);
}

void Teclado(unsigned char key, int x, int y)
{
	if (key == 27)
		exit(0);
}

void Inicializa(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

// Programa Principal 
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(5, 5);
	glutInitWindowSize(450, 450);
	glutCreateWindow("Desenho de uma casa com escala");
	glutDisplayFunc(Desenha);
	glutReshapeFunc(AlteraTamanhoJanela);
	glutKeyboardFunc(Teclado);
	Inicializa();
	glutMainLoop();
	return 0;
}
