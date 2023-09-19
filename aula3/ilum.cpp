/*
A esfera "A" � s�lida, e os valores das suas constantes de reflex�o difusa e 
especular pode ser modificados atrav�s de sele��o de op��es em um menu. 
A esfera "B" � transl�cida, e sua translucidez pode ser ajustada atrav�s do canal alfa.

At� o momento, o canal alfa tem sido ignorado (alfa � o A em RGBA). Os valores do 
canal alfa variam de 0 a 1, e s�o especificados com as fun��es glColor(). 
Quando os efeitos de composi��o (blending) s�o utilizados, o valor de alfa � 
utilizado para combinar a cor do fragmento que est� sendo processado com a cor 
j� presente no framebuffer. De modo geral, a composi��o de imagens utilando o 
canal alfa � dada pela seguinte express�o:
IT = IP * alfa + (1-alfa)* IFB
- IT � a imagem total resultante, 
- IP � a imagem que est� sendo processada,
- IFB � a imagem anteriormente presente no framebuffer.

Manual de uso

teclas LEFT e RIGHT: servem para rotacionar a posi��o do observador em torno do 
                     eixo y contra e a favor do sentido dos ponteiros do rel�gio, respectivamente
r, R: altera a dist�ncia entre o observador e o centro de rota��o (raio de observa��o)  
teclas UP e DOWN: controlam a altitude do observador (no eixo y). 
Bot�o direito do mouse (menu): � poss�vel modificar os graus de reflex�o difusa e especular da esfera maci�a 

http://www.ceset.unicamp.br/~magic/opengl/modelos-iluminacao.html
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#define PI 3.1415

GLint WIDTH =320;
GLint HEIGHT=240;

// Define o n�mero de fatias (longitude), o n�mero de pilhas (latitude) e o raio das esferas da cena.
GLint fatias=30;
GLint pilhas=30;
GLint raioEsfera=1.5;

GLfloat obs[3]={0.0,7.0,0.0};
GLfloat olho[3]={0.0,3.0,0.0};

//Define os coeficientes de reflex�o difusa (*_difusa) e especular (*_especular) para os tr�s objetos da cena.
/*Obs: O brilho do material, que pode assumir valores entre 1 e 128, � um expoente 
que modela a fun��o de distribui��o espacial da componente de luz refletida 
especularmente. � medida em que o valor do brilho aumenta, diminui o espalhamento 
da luz refletida.*/
// Cores do plano
GLfloat plano_difusa[]    = { 0.5, 0.5, 0.0, 1.0 };
GLfloat plano_especular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat plano_brilho[]    = { 50.0 };

//Cores da esfera A
GLfloat mat_a_difusa[]    = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_a_especular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_a_brilho[]    = { 50.0 };

//Cores da esfera B
GLfloat mat_b_difusa[]    = { 0.7, 0.7, 0.7, 0.5 };   // o valor de alfa=0.5 d� a apar�ncia transl�cida
GLfloat mat_b_especular[] = { 1.0, 1.0, 1.0, 0.5 };
GLfloat mat_b_brilho[]    = { 50.0 };


GLfloat posicao_luz0[]    = { 0.0, 10.0, 0.0, 1.0};  // posi��o luz
GLfloat cor_luz0[]        = { 1.0, 1.0, 1.0, 1.0};  // componentes de luz para reflex�o difusa e especular
GLfloat cor_luz0_amb[]    = { 0.3, 0.3, 0.3, 1.0};  // componente de luz para reflex�o ambiente

GLfloat posicao_luz1[]    = { 0.0, 10.0, 5.0, 1.0};
GLfloat cor_luz1[]        = { 0.0, 0.0, 1.0, 1.0};
GLfloat direcao_luz1[]    = { 0.0, -10.0, -5.0, 1.0};
GLint   spot_luz1         = 30;

GLfloat sem_cor[]         = { 0.0, 0.0, 0.0, 1.0};
  
GLint gouraud=0;

GLfloat tetaxz=0;
GLfloat raioxz=6;

void reshape(int width, int height){
  WIDTH=width;
  HEIGHT=height;
  glViewport(0,0,(GLint)width,(GLint)height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(70.0,width/(float)height,0.1,30.0);
  glMatrixMode(GL_MODELVIEW);
}

void display(void){
/*o depth-buffer (tamb�m conhecido por z-buffer) serve para guardar o valor de 
profundidade de cada pixel desenhado. Assim, � f�cil imp�r a condi��o de que cada 
novo pixel s� ser� desenhado se tiver um valor de profundidade superior (valores 
negativos) ao que j� l� estava, evitando sobrep�r pixels mais pr�ximos.
*/
  glEnable(GL_DEPTH_TEST);   // teste de profundidade - habilita z-buffer
  glEnable(GL_LIGHTING);

  glDepthMask(GL_TRUE);
  glClearColor(1.0,1.0,1.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  if(gouraud){
    glShadeModel(GL_SMOOTH);
  }
  else{
    glShadeModel(GL_FLAT);
  }    
  
  glPushMatrix();

  /* calcula a posicao do observador */
  obs[0]=raioxz*cos(2*PI*tetaxz/360);
  obs[2]=raioxz*sin(2*PI*tetaxz/360);
  gluLookAt(obs[0],obs[1],obs[2],olho[0],olho[1],olho[2],0.0,1.0,0.0);
  
  
/*A fun��o glMaterialfv() define as propriedades de reflex�o difusa e especular, 
e brilho do material que ser� utilizado para compor o objeto imediatamente desenhado, 
neste caso um plano, via GL_QUADS. 

Observe a chamada � fun��o glNormal3f() antes 
do plano ser desenhado. Isto � necess�rio porque, por default, o vetor normal 
encontra-se na dire��o (x,y,z)=(0,0,1) e para que a normal � superf�cie encontra-se 
orientada com eixo y, ou seja, na dire��o (x,y,z)=(0,1,0).
*/
  /* propriedades do material do plano */
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, plano_difusa);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, plano_especular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, plano_brilho);
  
  /* desenha o plano */
  glNormal3f(0,1,0); 
  glBegin(GL_QUADS);
  glVertex3f(-10,0,10);
  glVertex3f(10,0,10);
  glVertex3f(10,0,-10);
  glVertex3f(-10,0,-10);
  glEnd();

/* desenha esferas */

/*Para facilitar a identifica��o da fonte de luz, � desenhada uma esfera na posi��o 
desta fonte. 
Na chamada � fun��o glMaterialfv(), o par�metro GL_EMISSION define a intensidade 
luminosa emitida pelo material como sendo a pr�pria cor da fonte de luz, dando a 
apar�ncia de que a esfera brilha, assim como brilharia uma l�mpada.
*/
  glPushMatrix();
  glTranslatef(posicao_luz0[0],posicao_luz0[1],posicao_luz0[2]);
  glMaterialfv(GL_FRONT, GL_EMISSION, cor_luz0);
  glutSolidSphere(0.3,5,5);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(posicao_luz1[0],posicao_luz1[1],posicao_luz1[2]);
  glMaterialfv(GL_FRONT, GL_EMISSION, cor_luz1);
  glutSolidSphere(0.3,5,5);
  glPopMatrix();

  glMaterialfv(GL_FRONT, GL_EMISSION, sem_cor);

  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_a_difusa);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_a_especular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_a_brilho);

  glPushMatrix();
  glTranslatef(0.0,3.0,-3.0);
  glutSolidSphere(raioEsfera,fatias,pilhas);
  glPopMatrix();

  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_b_difusa);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_b_especular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_b_brilho);
  
  glTranslatef(0.0,+3.0,+3.0);

  glutSolidSphere(raioEsfera,fatias,pilhas);

  glPopMatrix();
  glutSwapBuffers();
}

// Rotaciona a cena - � poss�vel ver a posi��o das luzes
void special(int key, int x, int y){
  switch (key) {
  case GLUT_KEY_UP:
    obs[1]=obs[1]+1;
    glutPostRedisplay();
    break;
  case GLUT_KEY_DOWN:
    obs[1] =obs[1]-1;
    glutPostRedisplay();
    break;
  case GLUT_KEY_LEFT:
    tetaxz=tetaxz+2;
    glutPostRedisplay();
    break;
  case GLUT_KEY_RIGHT:
    tetaxz=tetaxz-2;
    glutPostRedisplay();
    break;
  }
}

void keyboard(unsigned char key, int x, int y){
  switch (key) {
  case 27:
    exit(0);
    break;
  case 'g':
    gouraud = !gouraud;
    glutPostRedisplay();
    break;
  case 'r':
    raioxz=raioxz+1;
    glutPostRedisplay();
    break;
  case 'R':
    if(raioxz>1){
      raioxz=raioxz-1;
      glutPostRedisplay();
    }
    break;
  }
}


void init(){
  gouraud=1;
  glEnable(GL_DEPTH_TEST);
  
/*Habilita a composi��o (blending) de imagens dos valores RGBA correntes com aqueles 
presentes no framebuffer. */
  glEnable(GL_BLEND);
  
/*A fun��o glBlendFunc() define os pesos para a imagem que est� sendo processada (IP)
e a presente no framebuffer. Neste caso, os pesos s�o o pr�prio valor do canal 
alfa (GL_SRC_ALPHA) e 1-alfa (GL_ONE_MINUS_SRC_ALPHA). */  
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

// Define a posi��o e as componentes de cor da fonte de luz GL_LIGHT0.
  glLightfv(GL_LIGHT0, GL_DIFFUSE, cor_luz0);
  glLightfv(GL_LIGHT0, GL_SPECULAR, cor_luz0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, cor_luz0_amb);
  glLightfv(GL_LIGHT0, GL_POSITION, posicao_luz0);

  glLightfv(GL_LIGHT1, GL_DIFFUSE, cor_luz1);
  glLightfv(GL_LIGHT1, GL_SPECULAR, cor_luz1);
  glLightf (GL_LIGHT1, GL_SPOT_CUTOFF, spot_luz1);
  glLightfv(GL_LIGHT1, GL_POSITION, posicao_luz1);
  glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direcao_luz1);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

  glEnable(GL_AUTO_NORMAL);
  glEnable(GL_NORMALIZE);
}

/*A fun��o menu � ativada quando o bot�o direito do mouse � pressionado. De acordo 
com o valor da vari�vel value, determinadas propriedades do material que comp�e 
a esfera A s�o modificadas. */

void menu(int value){
  switch (value) {
  case 0:
    mat_a_especular[0]=mat_a_especular[1]=mat_a_especular[2]=0.0;
    break;
  case 1: 
    mat_a_especular[0]=mat_a_especular[1]=mat_a_especular[2]=0.5;
    break;
  case 2:
    mat_a_especular[0]=mat_a_especular[1]=mat_a_especular[2]=1.0;
    break;
  case 3:
    mat_a_difusa[0]=mat_a_difusa[1]=mat_a_difusa[2]=0.0;
    break;
  case 4: 
    mat_a_difusa[0]=mat_a_difusa[1]=mat_a_difusa[2]=0.5;
    break;
  case 5:
    mat_a_difusa[0]=mat_a_difusa[1]=mat_a_difusa[2]=1.0;
    break;
  }
  glutPostRedisplay();
}


int main(int argc,char **argv){
  glutInitWindowPosition(0,0);
  glutInitWindowSize(WIDTH,HEIGHT);
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE);

  if(!glutCreateWindow("Modelos de iluminacao")) {
    fprintf(stderr,"Error opening a window.\n");
    exit(-1);
  }

  init();
  
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);

/*A fun��o glutCreateMenu(), em conjunto com a fun��o glutAddMenuEntry(), 
habilita um menu popup com seis op��es que permitir�o ativar entradas presentes 
na fun��o menu, de acordo com os valores passados nos segundos argumentos das 
fun��es glutAddMenuEntry(). 
A fun��o glutAttachMenu() associa o aparecimento deste menu ao pressionamento do 
bot�o direito do mouse.
*/
  glutCreateMenu(menu);
  glutAddMenuEntry("-sem spec", 0);
  glutAddMenuEntry("-spec m�dia", 1);
  glutAddMenuEntry("-spec alta", 2);
  glutAddMenuEntry("-sem difusa", 3);
  glutAddMenuEntry("-difusa m�dia", 4);
  glutAddMenuEntry("-difusa alta", 5);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glutMainLoop();
  return(0);
}
