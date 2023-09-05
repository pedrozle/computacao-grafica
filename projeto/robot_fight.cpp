#include <windows.h>
#include <GL/glut.h>
#include <cmath>

#include "robot.h"

void changePaint(float red, float green, float blue) {
    glColor3f(red, green, blue);
}

void drawPlane() {
    changePaint(0.1, 0.1, 0.1);
    glPushMatrix();
    glScalef(50, 1, 50);
    glutSolidCube(1);
    glPopMatrix();
}

void drawRingue() {
    glPushMatrix(); // inicio ringue

    changePaint(0.5, 0.5, 0.5);
    glPushMatrix(); // inicia chão
    glTranslatef(0.0, -100, 0.0);
    glScalef(500, 1, 500);
    glutSolidCube(1);
    glPopMatrix(); // fim chão


    changePaint(1, 1, 1);
    glPushMatrix(); // inicia poste da barreira 1
    glTranslatef(-245, 0, -245);
    glScalef(0.5, 10, 0.5);
    glutSolidCube(20);
    glPopMatrix(); // fim poste da barreira 1

    glPushMatrix(); // inicia poste da barreira 2
    glTranslatef(245, 0, -245);
    glScalef(0.5, 10, 0.5);
    glutSolidCube(20);
    glPopMatrix(); // fim poste da barreira 2

    glPushMatrix(); // inicia poste da barreira 3
    glTranslatef(-245, 0, 245);
    glScalef(0.5, 10, 0.5);
    glutSolidCube(20);
    glPopMatrix(); // fim poste da barreira 3

    glPushMatrix(); // inicia poste da barreira 4
    glTranslatef(245, 0, 245);
    glScalef(0.5, 10, 0.5);
    glutSolidCube(20);
    glPopMatrix(); // fim poste da barreira 4

    glPopMatrix(); // fim ringue
}

void drawBraco(int tipo) {

    glPushMatrix(); // inicia ombro

    if (tipo == 0) // esq
    {
        glRotatef(angleOmbroEsqX, 1, 0, 0);
        glRotatef(angleOmbroEsqY, 0, 1, 0);
        glRotatef(angleOmbroEsqZ, 0, 0, 1);
    }
    else // dir
    {
        glRotatef(angleOmbroDirX, 1, 0, 0);
        glRotatef(angleOmbroDirY, 0, 1, 0);
        glRotatef(angleOmbroDirZ, 0, 0, 1);
    }

    glPushMatrix(); // junta
    changePaint(0.9, 0.8, 0.8);
    glutSolidSphere(juntaTam, 16, 16);
    glPopMatrix(); // fim junta

    glPushMatrix(); // ombro
    changePaint(redMembros, greenMembros, blueMembros);
    glTranslatef(0, -18, 0);
    glScalef(bracoSx, bracoSy, bracoSz);
    glutWireCube(10);
    glPopMatrix(); // fim ombro

    glTranslatef(0, -35, 0); // cotovelo
    if (tipo == 0)
    {
        glRotatef(angleCotoveloEsqX, 1, 0, 0);
        glRotatef(angleCotoveloEsqY, 0, 1, 0);
        glRotatef(angleCotoveloEsqZ, 0, 0, 1);
    }
    else {
        glRotatef(angleCotoveloDirX, 1, 0, 0);
        glRotatef(angleCotoveloDirY, 0, 1, 0);
        glRotatef(angleCotoveloDirZ, 0, 0, 1);
    }
    glPushMatrix(); // junta
    changePaint(0.9, 0.8, 0.8);
    glutSolidSphere(juntaTam, 16, 16);
    glPopMatrix(); // fim junta

    glPushMatrix(); // inicia antebraco
    changePaint(redMembros, greenMembros, blueMembros);
    glTranslatef(0, -18, 0);
    glScalef(bracoSx, bracoSy, bracoSz);
    glutWireCube(10);

    glTranslatef(0, -6, 0);
    glPushMatrix(); // inicia criação do mão
    changePaint(redExtremidades, greenExtremidades, blueExtremidades);
    glScalef(1, 0.2, 1);
    glutWireSphere(10, 16, 16);
    glPopMatrix(); // fim mão

    glPopMatrix(); // fim antebraco

    glPopMatrix(); // fim  ombro

}

void drawPerna(int tipo) {
    if (tipo == 0) // esq
    {
        glRotatef(anglePernaEsqX, 1, 0, 0);
        glRotatef(anglePernaEsqY, 0, 1, 0);
        glRotatef(anglePernaEsqZ, 0, 0, 1);
    }
    else // dir
    {
        glRotatef(anglePernaDirX, 1, 0, 0);
        glRotatef(anglePernaDirY, 0, 1, 0);
        glRotatef(anglePernaDirZ, 0, 0, 1);
    }
    glPushMatrix();
    if (tipo == 0) // esq
    {
        glRotatef(angleCoxaEsqX, 1, 0, 0);
        glRotatef(angleCoxaEsqY, 0, 1, 0);
        glRotatef(angleCoxaEsqZ, 0, 0, 1);
    }
    else // dir
    {
        glRotatef(angleCoxaDirX, 1, 0, 0);
        glRotatef(angleCoxaDirY, 0, 1, 0);
        glRotatef(angleCoxaDirZ, 0, 0, 1);
    }
    glPushMatrix();
    glPushMatrix(); // junta
    changePaint(0.9, 0.8, 0.8);
    glutSolidSphere(juntaTam, 16, 16);
    glPopMatrix();

    glPushMatrix(); // coxa
    changePaint(redMembros, greenMembros, blueMembros);
    glTranslatef(0, -18, 0);
    glScalef(pernaSx, pernaSy, pernaSz);
    glutWireCube(10);
    glPopMatrix();

    glTranslatef(0, -35, 0); // joelho

    if (tipo == 0) // esq
    {
        glRotatef(angleJoelhoEsqX, 1, 0, 0);
        glRotatef(angleJoelhoEsqY, 0, 1, 0);
        glRotatef(angleJoelhoEsqZ, 0, 0, 1);
    }
    else {
        glRotatef(angleJoelhoDirX, 1, 0, 0);
        glRotatef(angleJoelhoDirY, 0, 1, 0);
        glRotatef(angleJoelhoDirZ, 0, 0, 1);
    }

    glPushMatrix(); // junta
    changePaint(0.9, 0.8, 0.8);
    glutSolidSphere(juntaTam, 16, 16);
    glPopMatrix();

    glPushMatrix(); // inicia canela
    changePaint(redMembros, greenMembros, blueMembros);
    glTranslatef(0, -18, 0);
    glScalef(pernaSx, pernaSy, pernaSz);
    glutWireCube(10);

    glTranslatef(0, -5.5, 3);
    glPushMatrix(); // inicia pé
    changePaint(redExtremidades, greenExtremidades, blueExtremidades);
    glScalef(1, 0.1, 1.5);
    glutWireCube(10);
    glPopMatrix(); // fim pé

    glPopMatrix(); // fim canela

    glPopMatrix();
    glPopMatrix();
}

void drawCabeca() {
    glPushMatrix(); // inicio cabeca
    changePaint(redExtremidades, greenExtremidades, blueExtremidades);
    glTranslatef(0, 31, 0);
    glScalef(1, 1.5, 1);
    glutSolidCube(10);

    glPushMatrix(); // inicio olhos
    changePaint(0, 0, 0);
    glTranslatef(-2, 0, 4);
    glutSolidSphere(2, 16, 16);
    glTranslatef(4, 0, 0);
    glutSolidSphere(2, 16, 16);
    glPopMatrix(); // fim olhos

    glPopMatrix(); // fim cabeca
}

void drawTroncoSuperior() {

    glPushMatrix(); // inicia tronco articulado

    glRotatef(troncoAngleTopX, 1, 0, 0);
    glRotatef(troncoAngleTopY, 0, 1, 0);
    glRotatef(troncoAngleTopZ, 0, 0, 1);
    glPushMatrix(); // junta
    changePaint(0.9, 0.8, 0.8);
    glutSolidSphere(juntaTam * 1.5, 16, 16);
    glPopMatrix(); // fim junta

    drawCabeca();

    glPushMatrix(); // tronco superior
    glTranslatef(0, 13, 0);
    changePaint(redCorpo, greenCorpo, blueCorpo);
    glScalef(2, 2.5, 1.5);
    glutSolidCube(10);
    glPopMatrix(); // fim tronco superior

    glPushMatrix(); // início braco esq
    glTranslatef(14, 20, 0);
    drawBraco(0);
    glPopMatrix(); // fim braco esq

    glPushMatrix(); // início braco dir
    glTranslatef(-14, 20, 0);
    drawBraco(1);
    glPopMatrix(); // fim braco dir

    glPopMatrix(); // fim tronco

}

void drawTroncoInferior() {

    glPushMatrix(); // tronco
    
    glRotatef(troncoAngleBotX, 1, 0, 0);
    glRotatef(troncoAngleBotY, 0, 1, 0);
    glRotatef(troncoAngleBotZ, 0, 0, 1);

    glPushMatrix(); // tronco model
    glTranslatef(0, -10, 0);
    // changePaint(redCorpo, greenCorpo, blueCorpo);
    changePaint(0.5, 0.5, 0.5);
    glScalef(2, 2.5, 1.5);
    glutSolidCube(10);
    glPopMatrix(); // fim tronco model

    glPushMatrix(); // início perna esq
    glTranslatef(5, -25, 0);
    drawPerna(0);
    glPopMatrix(); // fim perna esq

    glPushMatrix(); // início perna dir
    glTranslatef(-5, -25, 0);
    drawPerna(1);
    glPopMatrix(); // fim perna dir
    
    glPopMatrix(); // fim tronco

}

void drawCorpo(int tipo) {

    if (tipo == 1) {
        // troca a  cor das extremidades
        redExtremidades = 1;
        greenExtremidades = blueExtremidades = 0;

        // troca a cor dos membros
        redMembros = greenMembros = blueMembros = 1;

        // troca a cor do corpo
        redCorpo = greenCorpo = 0;
        blueCorpo = 1;
    }
    else {
        // troca a  cor das extremidades
        redExtremidades = 1;
        greenExtremidades = 0.4;
        blueExtremidades = 0;

        // troca a cor dos membros
        redMembros = greenMembros = blueMembros = 1;

        // troca a cor do corpo
        redCorpo = 0.1;
        greenCorpo = 0.6;
        blueCorpo = 0;
    }

    glRotatef(angleCorpoX, 1, 0, 0);
    glRotatef(angleCorpoY, 0, 1, 0);
    glRotatef(angleCorpoZ, 0, 0, 1);
    glTranslatef(transCorpoX, transCorpoY, transCorpoZ);
    glPushMatrix();// inicia criação do corpo do caba

    drawTroncoSuperior();
    drawTroncoInferior();

    glPopMatrix(); // fim corpo  do caba
}

void drawRobots() {

    // drawCabeca();

    glPushMatrix();
    glTranslatef(-90, 0, 0);
    // glRotatef(90, 0, 1, 0);
    drawCorpo(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(90, 0, 0);
    // glRotatef(-90, 0, 1, 0);
    drawCorpo(2);
    glPopMatrix();

}

void resetAnim() {
    switch (animacaoAtual) {
    case 1: // anim idle
        // Ombros
        angleOmbroDirX = angleOmbroDirY = angleOmbroDirZ = 0;
        angleOmbroEsqX = angleOmbroEsqY = angleOmbroEsqZ = 0;

        // Cotovelos
        angleCotoveloDirX = angleCotoveloDirY = angleCotoveloDirZ = 0;
        angleCotoveloEsqX = angleCotoveloEsqY = angleCotoveloEsqZ = 0;

        // Coxas
        angleCoxaDirX = angleCoxaDirY = angleCoxaDirZ = 0;
        angleCoxaEsqX = angleCoxaEsqY = angleCoxaEsqZ = 0;

        // Joelhos
        angleJoelhoDirX = angleJoelhoDirY = angleJoelhoDirZ = 0;
        angleJoelhoEsqX = angleJoelhoEsqY = angleJoelhoEsqZ = 0;

        break;
    case  2:
        // tronco
        troncoAngleTopZ = 0;

        // reseta timer
        timerAnim = 0;
        break;
    case 3:
        // reseta o timer da animação da reverencia
        deltaTimeTroncoX = deltaTimeTroncoBase;

        // reseta animação do tronco
        troncoAngleTopX = troncoAngleTopZ = 0;

        // Cotovelos
        angleCotoveloDirX = angleCotoveloDirY = angleCotoveloDirZ = 0;
        angleCotoveloEsqX = angleCotoveloEsqY = angleCotoveloEsqZ = 0;

        // delta time cotovelo
        deltaTimeCotoveloX = 6;
        break;
    case 4:
        // reseta o timer da animação da reverencia
        deltaTimeTroncoX = deltaTimeTroncoBase;

        // reseta animação do tronco
        troncoAngleTopX = troncoAngleTopZ = 0;

        // Ombros
        angleOmbroDirX = angleOmbroDirY = angleOmbroDirZ = 0;
        angleOmbroEsqX = angleOmbroEsqY = angleOmbroEsqZ = 0;

        // Cotovelos
        angleCotoveloDirX = angleCotoveloDirY = angleCotoveloDirZ = 0;
        angleCotoveloEsqX = angleCotoveloEsqY = angleCotoveloEsqZ = 0;

        // reset Delta times
        deltaTimeOmbroX = deltaTimeOmbroZ = deltaTimeOmbroBase;
        deltaTimeCotoveloX = deltaTimeCotoveloZ = deltaTimeCotoveloBase;
        break;
    case  5:
        deltaTimeCoxaZ = deltaTimeCoxaBase;
        break;
    case 10:
        // reseta o timer da animação da reverencia
        deltaTimeTroncoX = deltaTimeTroncoBase;

        // reseta animação do tronco
        troncoAngleTopX = troncoAngleTopZ = 0;

        // reseta animação dos bracos
        angleOmbroDirY = angleOmbroEsqY = 0;
        angleCotoveloDirX = angleCotoveloEsqX = -160;

        // reseta animação do corpo
        angleCorpoY = angleCorpoZ = 0;

        // reseta animação das pernas
        angleCoxaEsqZ = angleCoxaDirZ = 0;
        angleJoelhoDirX = angleJoelhoEsqX = 0;
        break;
    }
}

void animate() {

    if (animacaoAtual == 1) { // animação idle
        // anima o tronco
        troncoAngleTopZ += deltaTimeTroncoZ;
        if (troncoAngleTopZ > 5)
            deltaTimeTroncoZ *= -1;
        else if (troncoAngleTopZ < -5)
            deltaTimeTroncoZ *= -1;
    }

    if (animacaoAtual == 2) { // animação de reverência
        troncoAngleTopX += deltaTimeTroncoX;

        angleCotoveloDirX = angleCotoveloEsqX = troncoAngleTopX * -1;
        if (troncoAngleTopX > 45) {
            deltaTimeTroncoX *= -1;
        }
        if (troncoAngleTopX == 0) {
            animacaoAtual = 3;
            resetAnim();
        }
    }

    if (animacaoAtual == 3) { // animação inicio cruza bracos
        angleOmbroDirX = angleOmbroEsqX -= deltaTimeOmbroX;

        angleCotoveloDirX = angleOmbroDirX;
        angleCotoveloEsqX = angleOmbroDirX;

        angleCotoveloDirZ = (angleCotoveloDirX / -90) * 30;
        angleCotoveloEsqZ = (angleCotoveloDirX / -90) * -30;

        if (angleOmbroDirX < -90) {
            deltaTimeOmbroX *= -1;
            deltaTimeOmbroZ *= -1;
        }

        if (angleOmbroDirX > 0) {
            animacaoAtual = 4;
            resetAnim();
        }
    }

    if (animacaoAtual == 4) { // animação primeira pose

        angleCoxaDirZ -= deltaTimeCoxaZ;

        transCorpoX = (angleCoxaDirZ / -30) * 10;

        anglePernaEsqY = (angleCoxaDirZ / -30) * 90;
        angleCoxaEsqX -= deltaTimeCoxaZ;

        angleJoelhoEsqX = (angleCoxaDirZ / -30) * 30;

        troncoAngleTopY = (angleCoxaDirZ / -30) * 40;

        angleCotoveloDirX = (angleCoxaDirZ / -30) * -90;
        
        angleOmbroEsqX = (angleCoxaDirZ / -30) * -40;

        if (angleCoxaDirZ < -30) {
            animacaoAtual = 5;
            resetAnim();
        }

    }

    if (animacaoAtual == 5) {

        angleOmbroDirX -= deltaTimeOmbroX;

        angleCotoveloDirX = (angleOmbroDirX / -90) * -90;

        angleCotoveloEsqX = (angleOmbroDirX / -90) * -90;

        if (angleOmbroDirX < -90) {
            deltaTimeOmbroX = 0;
        }

    }

    if (animacaoAtual == 10) { // cambalhota

        angleCorpoX -= deltaTimeAngleCorpoX;
        angleCoxaDirX = angleCoxaEsqX = (angleCorpoX / -30) * -140;
        angleJoelhoDirX = angleJoelhoEsqX = (angleCorpoX / -30) * 140;

        if (angleCorpoX < -30) {
            deltaTimeAngleCorpoX = deltaTime + 5;
            angleCoxaDirX = angleCoxaEsqX = -140;
            angleJoelhoDirX = angleJoelhoEsqX = 140;
        }
        if (angleCorpoX < -300) {
            deltaTimeAngleCorpoX = deltaTime;
            angleCoxaDirX = angleCoxaEsqX = ((((angleCorpoX + 300) / -60) - 1) / -1) * -140;
            angleJoelhoDirX = angleJoelhoEsqX = ((((angleCorpoX + 300) / -60) - 1) / -1) * 140;

        }
        if (angleCorpoX < -360) {
            angleJoelhoDirX = angleJoelhoEsqX = angleCoxaDirX = angleCoxaEsqX = angleCorpoX = 0;
        }

    }

}

void init()
{
    glClearColor(0, 0, 0, 0);
    glClearDepth(1.0); // Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS); // The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST); // Enables Depth Testing
    glShadeModel(GL_SMOOTH); // Enables Smooth Color Shading
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    // Calcula a posição da câmera em coordenadas esféricas
    float cameraX = cameraDistance * sin(cameraAngleY * 3.14159265f / 180.0f) * cos(cameraAngleX * 3.14159265f / 180.0f);
    float cameraY = cameraDistance * sin(cameraAngleX * 3.14159265f / 180.0f);
    float cameraZ = cameraDistance * cos(cameraAngleY * 3.14159265f / 180.0f) * cos(cameraAngleX * 3.14159265f / 180.0f);

    gluLookAt(cameraX, cameraY, cameraZ, 0, 0, 0, 0, 1, 0);

    // drawPlane();
    drawRingue();
    drawRobots();
    // drawPerna(0);

    glutSwapBuffers();
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

        /* Controles da animação */
    case '1':
        animacaoAtual = 1;
        // resetam posição para idle
        resetAnim();
        break;

    case '2':
        animacaoAtual = 2;
        // resetam posição para socos
        resetAnim();
        break;

    case '3':
        animacaoAtual = 3;
        // resetam posição para chute
        resetAnim();
        break;

    case '4':
        animacaoAtual = 4;
        // resetam posição para cambalhota
        resetAnim();
        break;

        /* Fim controles da animação */

    }

}

void TimerCamera(int value) {

    animate();
    glutPostRedisplay();
    glutTimerFunc(30, TimerCamera, value);
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height); // Reset The Current Viewport

    glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
    glLoadIdentity(); // Reset The Projection Matrix

    // Calculate The Aspect Ratio Of The Window
    gluPerspective(30.0f, (float)640 / (float)480, 0.1f, 5000.0f);
    // Always keeps the same aspect as a 640 wide and 480 high window

    glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
    glLoadIdentity(); // Reset The Modelview Matrix
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Luta de Robotos?");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    TimerCamera(1);
    glutMainLoop();
    return 0;
}
