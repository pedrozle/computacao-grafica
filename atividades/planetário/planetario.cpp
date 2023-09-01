/*
2) Hierarquia do Sistema Solar: modelar o sistema solar com o Sol, os 8 planetas e 2 ou 3 luas de Júpiter e a lua da Terra.
*/

#include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include "planetario.h"

void init()
{
    glClearColor(0, 0, 0, 0);
    glClearDepth(1.0); // Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS); // The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST); // Enables Depth Testing
    glShadeModel(GL_SMOOTH); // Enables Smooth Color Shading
}

void drawPlanet(GLfloat red, GLfloat green, GLfloat blue, GLfloat radius, GLfloat slices, GLfloat stacks) {

    glColor3f(red, green, blue);
    glutSolidSphere(radius, slices, stacks);

}

void drawRing(int num_segments, float inner_radius, float outer_radius) {

    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= num_segments; i++) {
        // Calcula o ângulo em radianos
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);

        // Calcula as coordenadas x e y para o ponto atual do círculo
        float x = outer_radius * cosf(theta);
        float y = outer_radius * sinf(theta);

        // Define o vértice externo do anel
        glVertex2f(x, y);

        // Calcula as coordenadas x e y para o ponto atual do círculo interno
        x = inner_radius * cosf(theta);
        y = inner_radius * sinf(theta);

        // Define o vértice interno do anel
        glVertex2f(x, y);
    }
    glEnd();
}

void drawPlanetary() {

    // Draw planetary system
    glPushMatrix();
    drawPlanet(1, 1, 0, 12.0f, 16, 16); // Sol

    glPushMatrix(); // Push pai mercurio
    glRotatef(angleTransMercurio, 0, 1, 0);
    glTranslatef(distMercurio, 0, 0);
    glPushMatrix(); // Push filhos mercurio
    glPopMatrix(); // Pop filhos mercurio
    glRotatef(angleRotMercurio, 0, 1, 0);
    drawPlanet(0.65, 0.17, 0.17, tamMercurio, 16, 16); // Mercurio
    glPopMatrix(); // Pop pai mercurio

    glPushMatrix(); // Push pai Venus
    glRotatef(angleTransVenus, 0, 1, 0);
    glTranslatef(distVenus, 0, 0);
    glPushMatrix(); // Push filhos Venus
    glPopMatrix(); // Pop filhos Venus
    glRotatef(angleRotVenus, 0, 1, 0);
    drawPlanet(1, 0.6, 0, tamVenus, 16, 16); // Venus
    glPopMatrix(); // Pop pai Venus

    glPushMatrix(); // Push pai terra
    glRotatef(angleTransTerra, 0, 1, 0);
    glTranslatef(distTerra, 0, 0);
    glPushMatrix(); // Push filhos da terra
    glRotatef(angleRotTerra, 0, 1, 0);
    drawPlanet(0, 0, 1, tamTerra, 16, 16); // Terra
    glPushMatrix(); // Push  pai Lua
    glTranslatef(distLua, 0, 0);
    glPushMatrix(); // Push filhos da lua
    drawPlanet(0.5, 0.5, 0.5, tamLua, 16, 16); // Lua
    glPopMatrix(); // pop filhos lua
    glPopMatrix(); // pop pai lua
    glPopMatrix(); // pop filhos terra
    glPopMatrix(); // pop pai terra

    glPushMatrix(); // Push pai marte
    glRotatef(angleTransMarte, 0, 1, 0);
    glTranslatef(distMarte, 0, 0);
    glPushMatrix(); // Push filhos de marte
    glRotatef(angleRotMarte, 0, 1, 0);
    drawPlanet(1, 0.2, 0, tamMarte, 16, 16); // Marte
    glPushMatrix(); // Push pai lua Fobos
    glTranslatef(-distLuaFobos, distLuaFobos, distLuaFobos);
    glPushMatrix(); // Push filhos da lua
    drawPlanet(1, 0.78, 0.5, tamLuaFobos, 16, 16); // Lua
    glPopMatrix(); // pop filhos lua
    glPopMatrix(); // Pop pai lua Fobos

    glPushMatrix(); // Push pai lua Deimos
    glTranslatef(distLuaDeimos, -distLuaDeimos, distLuaDeimos);
    glPushMatrix(); // Push filhos da lua
    drawPlanet(1, 1, 1, tamLuaDeimos, 16, 16); // Lua
    glPopMatrix(); // pop filhos lua
    glPopMatrix(); // Pop pai lua Deimos

    glPopMatrix(); // pop filhos de marte
    glPopMatrix(); // pop pai marte

    glPushMatrix(); // Push pai Jupiter
    glRotatef(angleTransJupiter, 0, 1, 0);
    glTranslatef(distJupiter, 0, 0);
    glPushMatrix(); // Push filhos de Jupiter
    glRotatef(45, 1, 0, 1);
    glRotatef(angleRotJupiter, 0, 0, 1);
    drawPlanet(1, 0.2, 0, tamJupiter, 16, 16); // Jupiter
    glPushMatrix(); // Push pai lua Io
    glTranslatef(distLuaIo, -distLuaIo, distLuaIo);
    glPushMatrix(); // Push filhos da lua
    drawPlanet(1, 0.78, 0.5, tamLuaIo, 16, 16); // Lua
    glPopMatrix(); // pop filhos lua
    glPopMatrix(); // Pop pai lua Io

    glPushMatrix(); // Push pai lua Europa
    glTranslatef(distLuaEuropa, distLuaEuropa, -distLuaEuropa);
    glPushMatrix(); // Push filhos da lua
    drawPlanet(1, 1, 1, tamLuaEuropa, 16, 16); // Lua
    glPopMatrix(); // pop filhos lua
    glPopMatrix(); // Pop pai lua Europa

    glPushMatrix(); // Push pai lua Ganimedes
    glTranslatef(distLuaGanimedes, distLuaGanimedes, distLuaGanimedes);
    glPushMatrix(); // Push filhos da lua
    drawPlanet(0.78, 0.78, 0.78, tamLuaGanimedes, 16, 16); // Lua
    glPopMatrix(); // pop filhos lua
    glPopMatrix(); // Pop pai lua Ganimedes

    glPushMatrix(); // Push pai lua Calisto
    glTranslatef(distLuaCalisto, -distLuaCalisto, distLuaCalisto);
    glPushMatrix(); // Push filhos da lua
    drawPlanet(0.5, 0.5, 0.5, tamLuaCalisto, 16, 16); // Lua
    glPopMatrix(); // pop filhos lua
    glPopMatrix(); // Pop pai lua Calisto


    glPopMatrix(); // pop filhos de Jupiter
    glPopMatrix(); // pop pai Jupiter

    glPushMatrix(); // Push pai Saturno
    glRotatef(angleTransSaturno, 0, 1, 0);
    glTranslatef(distSaturno, 0, 0);
    glPushMatrix(); // Push filhos de Saturno
    glRotatef(-70, 0, 1, 0);
    glRotatef(angleRotSaturno, 0, 0, 1);
    drawPlanet(0.58, 0.39, 0.19, tamSaturno, 16, 16); // Saturno
    drawRing(30, distInterAnelSaturnoA, distExterAnelSaturnoA);
    drawRing(30, distInterAnelSaturnoB, distExterAnelSaturnoB);
    drawRing(30, distInterAnelSaturnoC, distExterAnelSaturnoC);
    drawRing(30, distInterAnelSaturnoD, distExterAnelSaturnoD);

    glPushMatrix(); // Push pai lua Titã
    glTranslatef(-distLuaTita, distLuaTita, distLuaTita);
    glPushMatrix(); // Push filhos da lua
    drawPlanet(1, 0.78, 0.5, tamLuaTita, 16, 16); // Lua
    glPopMatrix(); // pop filhos lua
    glPopMatrix(); // Pop pai lua Titã

    glPushMatrix(); // Push pai lua Encelado
    glTranslatef(distLuaEncelado, -distLuaEncelado, distLuaEncelado);
    glPushMatrix(); // Push filhos da lua
    drawPlanet(1, 1, 1, tamLuaEncelado, 16, 16); // Lua
    glPopMatrix(); // pop filhos lua
    glPopMatrix(); // Pop pai lua Encelado

    glPopMatrix(); // pop filhos de Saturno
    glPopMatrix(); // pop pai Saturno

    glPushMatrix(); // Push pai Urano
    glRotatef(angleTransUrano, 0, 1, 0);
    glTranslatef(distUrano, 0, 0);
    glPushMatrix(); // Push filhos de Urano
    glRotatef(-97, 0, 1, 0);
    glRotatef(angleRotUrano, 0, 0, 1);
    drawPlanet(0, 0.39, 1, tamUrano, 16, 16); // Urano
    drawRing(30, distInterAnelUrano, distExterAnelUrano);
    glPushMatrix(); // Push pai lua Miranda
    glTranslatef(-distLuaMiranda, distLuaMiranda, distLuaMiranda);
    glPushMatrix(); // Push filhos da lua
    drawPlanet(1, 0.78, 0.5, tamLuaMiranda, 16, 16); // Lua
    glPopMatrix(); // pop filhos lua
    glPopMatrix(); // Pop pai lua Miranda

    glPushMatrix(); // Push pai lua Titania
    glTranslatef(distLuaTitania, -distLuaTitania, distLuaTitania);
    glPushMatrix(); // Push filhos da lua
    drawPlanet(1, 1, 1, tamLuaTitania, 16, 16); // Lua
    glPopMatrix(); // pop filhos lua
    glPopMatrix(); // Pop pai lua Titania

    glPopMatrix(); // pop filhos de Urano
    glPopMatrix(); // pop pai Urano

    glPushMatrix(); // Push pai Netuno
    glRotatef(angleTransNetuno, 0, 1, 0);
    glTranslatef(distNetuno, 0, 0);
    glPushMatrix(); // Push filhos de Netuno
    glRotatef(28, 1, 1, 0);
    glRotatef(angleRotNetuno, 0, 0, 1);
    drawPlanet(0, 0, 1, tamNetuno, 16, 16); // Netuno
    drawRing(30, distInterAnelNetuno, distExterAnelNetuno);
    glPushMatrix(); // Push pai lua Tritao
    glTranslatef(distLuaTritao, -distLuaTritao, -distLuaTritao);
    glPushMatrix(); // Push filhos da lua
    drawPlanet(1, 0.78, 0.5, tamLuaTritao, 16, 16); // Lua
    glPopMatrix(); // pop filhos lua
    glPopMatrix(); // Pop pai lua Tritao

    glPopMatrix(); // pop filhos de Netuno
    glPopMatrix(); // pop pai Netuno

    glPopMatrix(); // pop filhos de sol
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

    drawPlanetary();

    glutSwapBuffers();
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height); // Reset The Current Viewport

    glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
    glLoadIdentity(); // Reset The Projection Matrix

    // Calculate The Aspect Ratio Of The Window
    gluPerspective(60, (float)640 / (float)480, 0.1f, 2000.0f);
    // Always keeps the same aspect as a 640 wide and 480 high window

    glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
    glLoadIdentity(); // Reset The Modelview Matrix
}

void timerFunction(int value) {

    // //---------- MERCURIO -------------------
    angleTransMercurio += velTransMercurio;
    if (angleTransMercurio >= 360) angleTransMercurio = 0;

    angleRotMercurio += velRotMercurio;
    if (angleRotMercurio >= 360) angleRotMercurio = 0;

    //---------- VENUS -------------------
    angleTransVenus += velTransVenus;
    if (angleTransVenus >= 360) angleTransVenus = 0;

    angleRotVenus += velRotVenus;
    if (angleRotVenus >= 360) angleRotVenus = 0;

    //---------- TERRRA -------------------
    angleTransTerra += velTransTerra;
    if (angleTransTerra >= 360) angleTransTerra = 0;

    angleRotTerra += velRotTerra;
    if (angleRotTerra >= 360) angleRotTerra = 0;

    //---------- MARTE -------------------
    angleTransMarte += velTransMarte;
    if (angleTransMarte >= 360) angleTransMarte = 0;

    angleRotMarte += velRotMarte;
    if (angleRotMarte >= 360) angleRotMarte = 0;

    //---------- JUPITER -------------------
    angleTransJupiter += velTransJupiter;
    if (angleTransJupiter >= 360) angleTransJupiter = 0;

    angleRotJupiter += velRotJupiter;
    if (angleRotJupiter >= 360) angleRotJupiter = 0;

    //---------- SATURNO -------------------
    angleTransSaturno += velTransSaturno;
    if (angleTransSaturno >= 360) angleTransSaturno = 0;

    angleRotSaturno += velRotSaturno;
    if (angleRotSaturno >= 360) angleRotSaturno = 0;

    //---------- URANO -------------------
    angleTransUrano += velTransUrano;
    if (angleTransUrano >= 360) angleTransUrano = 0;

    angleRotUrano += velRotUrano;
    if (angleRotUrano >= 360) angleRotUrano = 0;

    //---------- NETUNO -------------------
    angleTransNetuno += velTransNetuno;
    if (angleTransNetuno >= 360) angleTransNetuno = 0;

    angleRotNetuno += velRotNetuno;
    if (angleRotNetuno >= 360) angleRotNetuno = 0;


    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timerFunction, 0);
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 'a')
        cameraAngleY += angleCamera;

    if (key == 'd')
        cameraAngleY -= angleCamera;

    if (key == 'w')
        cameraAngleX += angleCamera;

    if (key == 's')
        cameraAngleX -= angleCamera;

    if (key == 'e')
        cameraDistance -= angleCamera;

    if (key == 'q')
        cameraDistance += 20;

    if (key == 'o') {
        cameraDistance = cameraDistanceBase;
        cameraAngleX = cameraAngleXBase;
        cameraAngleY = cameraAngleYBase;
    }

}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Planetario");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    timerFunction(1);
    glutMainLoop();
    return 0;
}
