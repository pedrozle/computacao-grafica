#include <windows.h>
#include <stdio.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "tgaload.h"
#include <memory.h>
#include <stdlib.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace std;

/* Extension Management */
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC  glCompressedTexImage2DARB = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEARBPROC glGetCompressedTexImageARB = NULL;

/* Default support - lets be optimistic! */
bool tgaCompressedTexSupport = true;

void tgaGetExtensions(void)
{
    glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)
        wglGetProcAddress("glCompressedTexImage2DARB");
    glGetCompressedTexImageARB = (PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)
        wglGetProcAddress("glGetCompressedTexImageARB");

    if (glCompressedTexImage2DARB == NULL || glGetCompressedTexImageARB == NULL)
        tgaCompressedTexSupport = false;
}

void tgaSetTexParams(unsigned int min_filter, unsigned int mag_filter, unsigned int application)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, application);
}

unsigned char* tgaAllocMem(tgaHeader_t info)
{
    unsigned char* block;

    block = (unsigned char*)malloc(info.bytes);

    if (block == NULL)
        return 0;

    memset(block, 0x00, info.bytes);

    return block;
}

void tgaPutPacketTuples(image_t* p, unsigned char* temp_colour, int& current_byte)
{
    if (p->info.components == 3)
    {
        p->data[current_byte] = temp_colour[2];
        p->data[current_byte + 1] = temp_colour[1];
        p->data[current_byte + 2] = temp_colour[0];
        current_byte += 3;
    }

    if (p->info.components == 4)    // Because its BGR(A) not (A)BGR :(
    {
        p->data[current_byte] = temp_colour[2];
        p->data[current_byte + 1] = temp_colour[1];
        p->data[current_byte + 2] = temp_colour[0];
        p->data[current_byte + 3] = temp_colour[3];
        current_byte += 4;
    }
}

void tgaGetAPacket(int& current_byte, image_t* p, FILE* file)
{
    unsigned char  packet_header;
    int            run_length;
    unsigned char  temp_colour[4] = { 0x00, 0x00, 0x00, 0x00 };

    fread(&packet_header, (sizeof(unsigned char)), 1, file);
    run_length = (packet_header & 0x7F) + 1;

    if (packet_header & 0x80)  // RLE packet
    {
        fread(temp_colour, (sizeof(unsigned char) * p->info.components), 1, file);

        if (p->info.components == 1)  // Special optimised case :)
        {
            memset(p->data + current_byte, temp_colour[0], run_length);
            current_byte += run_length;
        }
        else
            for (int i = 0; i < run_length; i++)
                tgaPutPacketTuples(p, temp_colour, current_byte);
    }

    if (!(packet_header & 0x80))  // RAW packet
    {
        for (int i = 0; i < run_length; i++)
        {
            fread(temp_colour, (sizeof(unsigned char) * p->info.components), 1, file);

            if (p->info.components == 1)
            {
                memset(p->data + current_byte, temp_colour[0], run_length);
                current_byte += run_length;
            }
            else
                tgaPutPacketTuples(p, temp_colour, current_byte);
        }
    }
}

void tgaGetPackets(image_t* p, FILE* file)
{
    int current_byte = 0;

    while (current_byte < p->info.bytes)
        tgaGetAPacket(current_byte, p, file);
}

void tgaGetImageData(image_t* p, FILE* file)
{
    unsigned char  temp;

    p->data = tgaAllocMem(p->info);

    /* Easy unRLE image */
    if (p->info.image_type == 1 || p->info.image_type == 2 || p->info.image_type == 3)
    {
        fread(p->data, sizeof(unsigned char), p->info.bytes, file);

        /* Image is stored as BGR(A), make it RGB(A)     */
        for (int i = 0; i < p->info.bytes; i += p->info.components)
        {
            temp = p->data[i];
            p->data[i] = p->data[i + 2];
            p->data[i + 2] = temp;
        }
    }

    /* RLE compressed image */
    if (p->info.image_type == 9 || p->info.image_type == 10)
        tgaGetPackets(p, file);
}

void tgaUploadImage(image_t* p, tgaFLAG mode)
{
    /*  Determine TGA_LOWQUALITY  internal format
        This directs OpenGL to upload the textures at half the bit
        precision - saving memory
     */
    GLenum internal_format = p->info.tgaColourType;

    if (mode & TGA_LOW_QUALITY)
    {
        switch (p->info.tgaColourType)
        {
        case GL_RGB: internal_format = GL_RGB4; break;
        case GL_RGBA: internal_format = GL_RGBA4; break;
        case GL_LUMINANCE: internal_format = GL_LUMINANCE4; break;
        case GL_ALPHA: internal_format = GL_ALPHA4; break;
        }
    }

    /*  Let OpenGL decide what the best compressed format is each case. */
    if (mode & TGA_COMPRESS && tgaCompressedTexSupport)
    {
        switch (p->info.tgaColourType)
        {
        case GL_RGB: internal_format = GL_COMPRESSED_RGB_ARB; break;
        case GL_RGBA: internal_format = GL_COMPRESSED_RGBA_ARB; break;
        case GL_LUMINANCE: internal_format = GL_COMPRESSED_LUMINANCE_ARB; break;
        case GL_ALPHA: internal_format = GL_COMPRESSED_ALPHA_ARB; break;
        }
    }

    /*  Pass OpenGL Texture Image */
    if (!(mode & TGA_NO_MIPMAPS))
        gluBuild2DMipmaps(GL_TEXTURE_2D, internal_format, p->info.width,
            p->info.height, p->info.tgaColourType, GL_UNSIGNED_BYTE, p->data);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, p->info.width,
            p->info.height, 0, p->info.tgaColourType, GL_UNSIGNED_BYTE, p->data);
}

void tgaFree(image_t* p)
{
    if (p->data != NULL)
        free(p->data);
}

void tgaChecker(image_t* p)
{
    unsigned char TGA_CHECKER[16384];
    unsigned char* pointer;

    // 8bit image
    p->info.image_type = 3;

    p->info.width = 128;
    p->info.height = 128;

    p->info.pixel_depth = 8;

    // Set some stats
    p->info.components = 1;
    p->info.bytes = p->info.width * p->info.height * p->info.components;

    pointer = TGA_CHECKER;

    for (int j = 0; j < 128; j++)
    {
        for (int i = 0; i < 128; i++)
        {
            if ((i ^ j) & 0x10)
                pointer[0] = 0x00;
            else
                pointer[0] = 0xff;
            pointer++;
        }
    }

    p->data = TGA_CHECKER;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE4, p->info.width,
        p->info.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, p->data);

    /*  Should we free?  I dunno.  The scope of TGA_CHECKER _should_ be local, so it
        probably gets destroyed automatically when the function completes... */
        //  tgaFree ( p );
}

void tgaError(char* error_string, char* file_name, FILE* file, image_t* p)
{
    printf("%s - %s\n", error_string, file_name);
    tgaFree(p);

    fclose(file);

    tgaChecker(p);
}

void tgaGetImageHeader(FILE* file, tgaHeader_t* info)
{
    /*   Stupid byte alignment means that we have to fread each field
         individually.  I tried splitting tgaHeader into 3 structures, no matter
         how you arrange them, colour_map_entry_size comes out as 2 bytes instead
         1 as it should be.  Grrr.  Gotta love optimising compilers - theres a pragma
         for Borland, but I dunno the number for MSVC or GCC :(
     */
    fread(&info->id_length, (sizeof(unsigned char)), 1, file);
    fread(&info->colour_map_type, (sizeof(unsigned char)), 1, file);
    fread(&info->image_type, (sizeof(unsigned char)), 1, file);

    fread(&info->colour_map_first_entry, (sizeof(short int)), 1, file);
    fread(&info->colour_map_length, (sizeof(short int)), 1, file);
    fread(&info->colour_map_entry_size, (sizeof(unsigned char)), 1, file);

    fread(&info->x_origin, (sizeof(short int)), 1, file);
    fread(&info->y_origin, (sizeof(short int)), 1, file);
    fread(&info->width, (sizeof(short int)), 1, file);
    fread(&info->height, (sizeof(short int)), 1, file);

    fread(&info->pixel_depth, (sizeof(unsigned char)), 1, file);
    fread(&info->image_descriptor, (sizeof(unsigned char)), 1, file);

    // Set some stats
    info->components = info->pixel_depth / 8;
    info->bytes = info->width * info->height * info->components;
}

int tgaLoadTheImage(char* file_name, image_t* p, tgaFLAG mode)
{
    FILE* file;

    tgaGetExtensions();

    p->data = NULL;

    if ((file = fopen(file_name, "rb")) == NULL)
    {
        tgaError("File not found", file_name, file, p);
        cout << "File not found " << file_name << endl;
        return 0;
    }

    tgaGetImageHeader(file, &p->info);

    switch (p->info.image_type)
    {
    case 1:
        tgaError("8-bit colour no longer supported", file_name, file, p);
        return 0;

    case 2:
        if (p->info.pixel_depth == 24)
            p->info.tgaColourType = GL_RGB;
        else if (p->info.pixel_depth == 32)
            p->info.tgaColourType = GL_RGBA;
        else
        {
            tgaError("Unsupported RGB format", file_name, file, p);
            return 0;
        }
        break;

    case 3:
        if (mode & TGA_LUMINANCE)
            p->info.tgaColourType = GL_LUMINANCE;
        else if (mode & TGA_ALPHA)
            p->info.tgaColourType = GL_ALPHA;
        else
        {
            tgaError("Must be LUMINANCE or ALPHA greyscale", file_name, file, p);
            return 0;
        }
        break;

    case 9:
        tgaError("8-bit colour no longer supported", file_name, file, p);
        return 0;

    case 10:
        if (p->info.pixel_depth == 24)
            p->info.tgaColourType = GL_RGB;
        else if (p->info.pixel_depth == 32)
            p->info.tgaColourType = GL_RGBA;
        else
        {
            tgaError("Unsupported compressed RGB format", file_name, file, p);
            return 0;
        }
    }

    tgaGetImageData(p, file);

    fclose(file);

    return 1;
}

void tgaLoad(char* file_name, image_t* p, tgaFLAG mode)
{
    if (tgaLoadTheImage(file_name, p, mode))
    {
        if (!(mode & TGA_NO_PASS))
            tgaUploadImage(p, mode);

        if (mode & TGA_FREE)
            tgaFree(p);
    }

}

GLuint tgaLoadAndBind(char* file_name, tgaFLAG mode)
{
    GLuint   texture_id;
    image_t* p;

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    if (tgaLoadTheImage(file_name, p, mode))
    {
        tgaUploadImage(p, mode);
        tgaFree(p);
    }

    return texture_id;
}

// Qtd m�xima de texturas a serem usadas no programa
#define MAX_NO_TEXTURES 1
#define CUBE_TEXTURE 0
// vetor com os n�meros das texturas
GLuint texture_id[2];

void initTexture(void)
{
    // Habilita o uso de textura 
    glEnable(GL_TEXTURE_2D);
    // Define a forma de armazenamento dos pixels na textura (1= alihamento por byte)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Define quantas texturas ser�o usadas no programa 
    glGenTextures(2, texture_id);  // 1 = uma textura;
    // texture_id = vetor que guardas os n�meros das texturas

// Define o n�mero da textura do cubo.
    texture_id[0] = 1001;

    // Define que tipo de textura ser� usada
    // GL_TEXTURE_2D ==> define que ser� usada uma textura 2D (bitmaps)
    // texture_id[CUBE_TEXTURE]  ==> define o n�mero da textura 
    image_t temp_image;
    glBindTexture(GL_TEXTURE_2D, texture_id[0]);
    tgaLoad("C:/Users/pedro/Documents/repos/static/computacao-grafica/aula3/aula3/cao.tga", &temp_image, TGA_FREE | TGA_LOW_QUALITY);
}

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
    glutSolidSphere(10.0f, 50.0f, 50.0f);
}

void DesenhaLosango(void)
{
    glBegin(GL_QUADS);
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
    
    GLfloat arr[] = { 0,0,0,1 };

    glPushMatrix();
    switch (primitiva) {

    case QUADRADO:
        glMaterialfv(GL_FRONT, GL_EMISSION, arr);
        glBindTexture(GL_TEXTURE_2D, texture_id[0]);
        glutSolidCube(10);
        break;
    case TRIANGULO:
        glBindTexture(GL_TEXTURE_2D, texture_id[0]);
        DesenhaTriangulo();
        break;
    case LOSANGO:
        glBindTexture(GL_TEXTURE_2D, texture_id[0]);
        DesenhaLosango();
        break;
    case TEAPOT:
        glBindTexture(GL_TEXTURE_2D, texture_id[0]);
        glutSolidTeapot(10.0f);
        break;
    }
    glPopMatrix();



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
    initTexture();
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
