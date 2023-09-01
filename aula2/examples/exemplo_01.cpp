//piramide

#include <GL/glut.h>
#include <stdlib.h>

// Vari�veis para controles de navega��o
GLfloat angle, fAspect;
GLfloat rotX, rotY, rotX_ini, rotY_ini;
GLfloat obsX, obsY, obsZ, obsX_ini, obsY_ini, obsZ_ini;
int x_ini,y_ini,bot;

// Define um v�rtice
typedef struct {
	float x,y,z;	// posi��o no espa�o
} VERT;

// Define uma face
typedef struct {
	int total;	// total de v�rtices
	int ind[4];	// �ndices para o vetor de v�rtices
} FACE;

// Define um objeto 3D
typedef struct {
	VERT *vertices;		// aponta para os v�rtices
	FACE *faces;		// aponta para as faces
	int total_faces;	// total de faces no objeto
} OBJ;

// Defini��o dos v�rtices
VERT vertices[] = {
	{ -1, 0, -1 },	// 0 canto inf esquerdo tras.
	{  1, 0, -1 },	// 1 canfo inf direito  tras.
	{  1, 0,  1 },	// 2 canto inf direito  diant.
	{ -1, 0,  1 },  // 3 canto inf esquerdo diant.
	{  0, 2,  0 },  // 4 topo
};

// Defini��o das faces
FACE faces[] = {
	{ 4, { 0,1,2,3 }},	// base
	{ 3, { 0,1,4,-1 }},	// lado traseiro
	{ 3, { 0,3,4,-1 }},	// lado esquerdo
	{ 3, { 1,2,4,-1 }},	// lado direito
	{ 3, { 3,2,4,-1 }}	// lado dianteiro
};


OBJ piramide = {
	vertices, faces, 5 };


void DesenhaObjetoWireframe(OBJ* objeto)
{
    OBJ* obj = objeto;
    
    for (int f = 0; f < obj->total_faces; ++f)
	{
        glBegin(GL_LINE_LOOP);
        
        for (int v = 0; v < obj->faces[f].total; ++v)
			glVertex3f(obj->vertices[faces[f].ind[v]].x,
				obj->vertices[faces[f].ind[v]].y,
				obj->vertices[faces[f].ind[v]].z);
	}
	glEnd();
}

void Desenha(void)
{
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glColor3f(0.0f, 0.0f, 0.0f);
    
    DesenhaObjetoWireframe(&piramide);
    
    glFlush();
}

void PosicionaObservador(void)
{
    
    glMatrixMode(GL_MODELVIEW);
    
    glLoadIdentity();
    
	glRotatef(rotX,1,0,0);
	glRotatef(rotY,0,1,0);
}

void EspecificaParametrosVisualizacao(void)
{
    
    glMatrixMode(GL_PROJECTION);
    
    glLoadIdentity();
    
    gluPerspective(angle, fAspect, 0.1, 1200);

	PosicionaObservador();
}

void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
    
    if (h == 0) h = 1;
    
    glViewport(0, 0, w, h);
    
    fAspect = (GLfloat)w / (GLfloat)h;

	EspecificaParametrosVisualizacao();
}

void Teclado(unsigned char tecla, int x, int y)
{
	if(tecla==27) // ESC ?
    {
        exit(0);
	}
}

void TeclasEspeciais(int tecla, int x, int y)
{
	switch (tecla)
	{
		case GLUT_KEY_HOME:	if(angle>=10)  angle -=5;
							break;
		case GLUT_KEY_END:	if(angle<=150) angle +=5;
							break;
	}
	EspecificaParametrosVisualizacao();
	glutPostRedisplay();
}

void GerenciaMouse(int button, int state, int x, int y)
{
	if(state==GLUT_DOWN)
    {
        
        x_ini = x;
		y_ini = y;
		obsX_ini = obsX;
		obsY_ini = obsY;
		obsZ_ini = obsZ;
		rotX_ini = rotX;
		rotY_ini = rotY;
		bot = button;
	}
	else bot = -1;
}

#define SENS_ROT	5.0
#define SENS_OBS	15.0
#define SENS_TRANSL	30.0
void GerenciaMovim(int x, int y)
{
    
    if (bot == GLUT_LEFT_BUTTON)
    {
        
        int deltax = x_ini - x;
        int deltay = y_ini - y;
        
        rotY = rotY_ini - deltax / SENS_ROT;
		rotX = rotX_ini - deltay/SENS_ROT;
    }
    
    else if (bot == GLUT_RIGHT_BUTTON)
    {
        
        int deltaz = y_ini - y;
        
        obsZ = obsZ_ini + deltaz / SENS_OBS;
    }
    
    else if (bot == GLUT_MIDDLE_BUTTON)
    {
        
        int deltax = x_ini - x;
        int deltay = y_ini - y;
        
        obsX = obsX_ini + deltax / SENS_TRANSL;
		obsY = obsY_ini - deltay/SENS_TRANSL;
	}
	PosicionaObservador();
	glutPostRedisplay();
}

void Inicializa (void)
{	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
 
	angle=60;

	rotX = 0;
	rotY = 0;
	obsX = obsY = 0;
	obsZ = 5;
}

// Programa Principal 
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); 
	glutInitWindowPosition(5,5);
	glutInitWindowSize(450,450);
	glutCreateWindow("Desenho do wireframe de uma pir�mide");
	glutDisplayFunc(Desenha);
	glutReshapeFunc(AlteraTamanhoJanela);
	glutMouseFunc(GerenciaMouse);
	glutMotionFunc(GerenciaMovim);
	glutKeyboardFunc (Teclado);
	glutSpecialFunc (TeclasEspeciais);
	Inicializa();
	glutMainLoop();
 
	return 0;
}
