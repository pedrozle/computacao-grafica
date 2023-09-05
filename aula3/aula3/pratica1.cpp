#include <gl/glut.h>
#include <cmath>

/* Distância da câmera ao centro do mundo */
float cameraDistanceBase = 100.0f;
float cameraDistance = cameraDistanceBase;

/* Ângulo a ser adicionado na rotação da câmera */
float angleCamera = 5.0f;

/* Ângulo da câmera na coordenada X */
float cameraAngleXBase = 20.0f;
float cameraAngleX = 20.0f;
/* Ângulo da câmera na coordenada Y */
float cameraAngleYBase = 0.0f;
float cameraAngleY = 0.0f;
GLfloat angle, fAspect;

// Constantes
#define QUADRADO 1
#define TRIANGULO 2
#define LOSANGO   3
#define TEAPOT 4

// Variáveis
char texto[30];
GLfloat win, r = 1, g = 1, b = 1, a = 1;
GLint view_w, view_h, primitiva;
GLint gouraud = 0;

//Cores da esfera B
GLfloat mat_b_difusa[] = { r, g, b, a };   // o valor de alfa=0.5 d� a apar�ncia transl�cida
GLfloat mat_b_especular[] = { 1.0, 1.0, 1.0, 0.5 };
GLfloat mat_b_brilho[] = { 50.0 };

void changePaint(float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
}

void DesenhaQuadrado(void)
{
    glBegin(GL_QUADS);
    glVertex2f(-25.0f, -25.0f);
    glVertex2f(-25.0f, 25.0f);
    glVertex2f(25.0f, 25.0f);
    glVertex2f(25.0f, -25.0f);
    glEnd();
}

void DesenhaTriangulo(void)
{
    glBegin(GL_TRIANGLES);
    glVertex2f(-25.0f, -25.0f);
    glVertex2f(0.0f, 25.0f);
    glVertex2f(25.0f, -25.0f);
    glEnd();
}

void DesenhaLosango(void)
{
    glBegin(GL_POLYGON);
    glVertex2f(-25.0f, 0.0f);
    glVertex2f(0.0f, 25.0f);
    glVertex2f(25.0f, 0.0f);
    glVertex2f(0.0f, -25.0f);
    glEnd();
}

void Desenha(void)
{
    // Limpa a janela e o depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    // Calcula a posição da câmera em coordenadas esféricas
    float cameraX = cameraDistance * sin(cameraAngleY * 3.14159265f / 180.0f) * cos(cameraAngleX * 3.14159265f / 180.0f);
    float cameraY = cameraDistance * sin(cameraAngleX * 3.14159265f / 180.0f);
    float cameraZ = cameraDistance * cos(cameraAngleY * 3.14159265f / 180.0f) * cos(cameraAngleX * 3.14159265f / 180.0f);

    gluLookAt(cameraX, cameraY, cameraZ, 0, 0, 0, 0, 1, 0);
    if (gouraud) {
        glShadeModel(GL_SMOOTH);
    }
    else {
        glShadeModel(GL_FLAT);
    }
    glEnable(GL_DEPTH_TEST);   // teste de profundidade - habilita z-buffer
    glEnable(GL_LIGHTING);
    changePaint(1, 1, 1, 1);
    glPushMatrix();
    glTranslatef(0, -15, 0);
    glScalef(100, 0.5, 100);
    glutSolidCube(1);
    glPopMatrix();

    changePaint(r, g, b, a);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_b_difusa);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_b_especular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_b_brilho);
    switch (primitiva) {
    case QUADRADO:
        DesenhaQuadrado();
        break;
    case TRIANGULO:
        DesenhaTriangulo();
        break;
    case LOSANGO:
        DesenhaLosango();
        break;
    case TEAPOT:
        glutSolidTeapot(10.0f);
        break;
    }

    glutSwapBuffers();
}

void Inicializa(void)
{
    GLfloat luzAmbiente[4] = { 0.2,0.2,0.2,1.0 };
    GLfloat luzDifusa[4] = { 0.7,0.7,0.7,1.0 };		 // "cor" 
    GLfloat luzEspecular[4] = { 1.0, 1.0, 1.0, 1.0 };// "brilho" 
    GLfloat posicaoLuz[4] = { 0.0, 50.0, 50.0, 1.0 };

    // Capacidade de brilho do material
    GLfloat especularidade[4] = { 1.0,1.0,1.0,1.0 };
    GLint especMaterial = 60;

    // Especifica que a cor de fundo da janela será preta
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Habilita o modelo de colorização de Gouraud
    glShadeModel(GL_SMOOTH);

    // Define a refletância do material 
    glMaterialfv(GL_FRONT, GL_SPECULAR, especularidade);
    // Define a concentração do brilho
    glMateriali(GL_FRONT, GL_SHININESS, especMaterial);

    // Ativa o uso da luz ambiente 
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

    // Define os parâmetros da luz de número 0
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);

    // Habilita a definição da cor do material a partir da cor corrente
    glEnable(GL_COLOR_MATERIAL);
    //Habilita o uso de iluminação
    glEnable(GL_LIGHTING);
    // Habilita a luz de número 0
    glEnable(GL_LIGHT0);
    // Habilita o depth-buffering
    glEnable(GL_DEPTH_TEST);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    angle = 45;
}

void EspecificaParametrosVisualizacao(void)
{
    // Especifica sistema de coordenadas de projeção
    glMatrixMode(GL_PROJECTION);
    // Inicializa sistema de coordenadas de projeção
    glLoadIdentity();

    // Especifica a projeção perspectiva
    gluPerspective(angle, fAspect, 0.4, 500);

    // Especifica sistema de coordenadas do modelo
    glMatrixMode(GL_MODELVIEW);
    // Inicializa sistema de coordenadas do modelo
    glLoadIdentity();

    // Especifica posição do observador e do alvo
    gluLookAt(0, 80, 200, 0, 0, 0, 0, 1, 0);
}

void MenuCor(int op)
{
    switch (op) {
    case 0:
        r = 1.0f;
        g = 0.0f;
        b = 0.0f;
        break;
    case 1:
        r = 0.0f;
        g = 1.0f;
        b = 0.0f;
        break;
    case 2:
        r = 0.0f;
        g = 0.0f;
        b = 1.0f;
        break;
    case 3:
        r = 1.0f;
        g = 1.0f;
        b = 0.0f;
        break;
    }
    glutPostRedisplay();
}

void MenuPrimitiva(int op)
{
    switch (op) {
    case 0:
        primitiva = QUADRADO;
        break;
    case 1:
        primitiva = TRIANGULO;
        break;
    case 2:
        primitiva = LOSANGO;
        break;
    case 3:
        primitiva = TEAPOT;
        break;
    }
    glutPostRedisplay();
}

void MenuPrincipal(int op) {}

void CriaMenu()
{
    int menu, submenu1, submenu2;

    submenu1 = glutCreateMenu(MenuCor);
    glutAddMenuEntry("Vermelho", 0);
    glutAddMenuEntry("Verde", 1);
    glutAddMenuEntry("Azul", 2);
    glutAddMenuEntry("Amarelo", 3);

    submenu2 = glutCreateMenu(MenuPrimitiva);
    glutAddMenuEntry("Quadrado", 0);
    glutAddMenuEntry("Triângulo", 1);
    glutAddMenuEntry("Losango", 2);
    glutAddMenuEntry("Teapot", 3);

    menu = glutCreateMenu(MenuPrincipal);
    glutAddSubMenu("Cor", submenu1);
    glutAddSubMenu("Primitivas", submenu2);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void GerenciaMouse(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON)
        if (state == GLUT_DOWN)
            CriaMenu();
    EspecificaParametrosVisualizacao();
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{

    switch (key) {

        /* Controles da câmera */
    case 'a':
        cameraAngleY += angleCamera;
        break;

    case 'd':
        cameraAngleY -= angleCamera;
        break;

    case 'w':
        cameraAngleX += angleCamera;
        break;

    case 's':
        cameraAngleX -= angleCamera;
        break;

    case 'q':
        cameraDistance += 20;
        break;

    case 'e':
        cameraDistance -= 20;
        break;

    case 'o':
        cameraDistance = cameraDistanceBase;
        cameraAngleX = cameraAngleXBase;
        cameraAngleY = cameraAngleYBase;
        break;
        /* Fim controles da câmera */

    case 'g':
        gouraud = !gouraud;
        glutPostRedisplay();
        break;

    case 'z':
        a -= 0.15f;
        if (a < 0.0f)
            a = 0.0f;
        glutPostRedisplay();
        break;

    case 'x':
        a += 0.15f;
        if (a > 1.0f)
            a = 1.0f;
        glutPostRedisplay();
        break;

    }
}

void reshape(int w, int h)
{
    if (h == 0) h = 1;

    // Especifica o tamanho da viewport
    glViewport(0, 0, w, h);

    // Calcula a correção de aspecto
    fAspect = (GLfloat)w / (GLfloat)h;

    // Calculate The Aspect Ratio Of The Window
    gluPerspective(30.0f, (float)640 / (float)480, 0.1f, 5000.0f);
    // Always keeps the same aspect as a 640 wide and 480 high window

    glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
    glLoadIdentity(); // Reset The Modelview Matrix
    EspecificaParametrosVisualizacao();
}

void TimerCamera(int value) {
    glutPostRedisplay();
    glutTimerFunc(30, TimerCamera, value);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(400, 350);
    glutCreateWindow("Visualizacao 3D");
    Inicializa();
    glutDisplayFunc(Desenha);
    glutReshapeFunc(reshape);
    glutMouseFunc(GerenciaMouse);
    glutKeyboardFunc(keyboard);
    TimerCamera(1);
    glutMainLoop();
    return 0;
}
