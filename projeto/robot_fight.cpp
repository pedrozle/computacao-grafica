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
#pragma comment(lib, "winmm.lib") // Add the Windows library that contains the code for the function PlaySound()

#include "robot.h"

using namespace std;

// ###################################################################################################

/* Extension Management */
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC  glCompressedTexImage2DARB = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEARBPROC glGetCompressedTexImageARB = NULL;

GLfloat win, r = 1, g = 1, b = 1, a = 1;
GLint view_w, view_h, primitiva;
GLint gouraud = 0;

//Cores da esfera B
GLfloat mat_b_difusa[] = { r, g, b, a };   // o valor de alfa=0.5 d� a apar�ncia transl�cida
GLfloat mat_b_especular[] = { 1.0, 1.0, 1.0, 0.5 };
GLfloat mat_b_brilho[] = { 50.0 };

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

// ###################################################################################################


// vetor com os n�meros das texturas
GLuint texture_id[4];

void initTexture(void)
{
    // Habilita o uso de textura 
    glEnable(GL_TEXTURE_2D);
    // Define a forma de armazenamento dos pixels na textura (1= alihamento por byte)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Define quantas texturas ser�o usadas no programa 
    glGenTextures(3, texture_id);  // 1 = uma textura;
    // texture_id = vetor que guardas os n�meros das texturas

// Define o n�mero da textura do cubo.
    texture_id[0] = 1001;
    texture_id[1] = 1002;
    texture_id[2] = 1003;
    texture_id[3] = 1004;

    // Define que tipo de textura ser� usada
    // GL_TEXTURE_2D ==> define que ser� usada uma textura 2D (bitmaps)
    // texture_id[CUBE_TEXTURE]  ==> define o n�mero da textura 
    image_t ringue_chao, camisa_roboto;
    glBindTexture(GL_TEXTURE_2D, texture_id[0]);
    tgaLoad("C:\\Users\\pedro\\Documents\\repos\\static\\computacao-grafica\\projeto\\ringue.tga", &ringue_chao, TGA_FREE | TGA_LOW_QUALITY);

    glBindTexture(GL_TEXTURE_2D, texture_id[1]);
    tgaLoad("C:\\Users\\pedro\\Documents\\repos\\static\\computacao-grafica\\projeto\\textures\\camisa.tga", &camisa_roboto, TGA_FREE | TGA_LOW_QUALITY);

    glBindTexture(GL_TEXTURE_2D, texture_id[2]);
    tgaLoad("C:\\Users\\pedro\\Documents\\repos\\static\\computacao-grafica\\projeto\\textures\\cao.tga", &camisa_roboto, TGA_FREE | TGA_LOW_QUALITY);

    glBindTexture(GL_TEXTURE_2D, texture_id[3]);
    tgaLoad("C:\\Users\\pedro\\Documents\\repos\\static\\computacao-grafica\\projeto\\textures\\dojo.tga", &camisa_roboto, TGA_FREE | TGA_LOW_QUALITY);
}

void cubo(int drawTop) {
    glScalef(5, 5, 5);
    // glRotatef(180, 1, 0, 0);
    // glRotatef(180, 0, 0, 1);
    glBegin(GL_QUADS);
    // Front Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
    // Back Face
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);

    if (drawTop == 1) {
        // Top Face
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
    }
    else {
        // Top Face
        glVertex3f(-1.0f, 1.0f, -1.0f);
        glVertex3f(-1.0f, 1.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, -1.0f);
    }

    // Bottom Face
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
    // Right face
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
    // Left Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
    glEnd();
}

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

    // glNormal3f(0, 1, 0);
    glBindTexture(GL_TEXTURE_2D, texture_id[0]);

    glTranslatef(0.0, -100, 0.0);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(10, 10, 1);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0); glVertex2f(-25.0f, -25.0f);
    glTexCoord2f(0, 0); glVertex2f(-25.0f, 25.0f);
    glTexCoord2f(1.0, 0); glVertex2f(25.0f, 25.0f);
    glTexCoord2f(1.0, 1.0); glVertex2f(25.0f, -25.0f);
    glEnd();

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
    changePaint(0.5, 0.5, 0.5);
    glutSolidSphere(juntaTam, 16, 16);
    glPopMatrix(); // fim junta

    glPushMatrix(); // ombro
    changePaint(redMembros, greenMembros, blueMembros);
    glTranslatef(0, -18, 0);
    glScalef(bracoSx, bracoSy, bracoSz);
    glutSolidCube(10);
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
    glutSolidCube(10);

    glTranslatef(0, -6, 0);
    glPushMatrix(); // inicia criação do mão
    changePaint(redExtremidades, greenExtremidades, blueExtremidades);
    glScalef(1, 0.2, 1);
    glutSolidSphere(10, 16, 16);
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
    glutSolidCube(10);
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
    glutSolidCube(10);

    glTranslatef(0, -5.5, 3);
    glPushMatrix(); // inicia pé
    changePaint(redExtremidades, greenExtremidades, blueExtremidades);
    glScalef(1, 0.1, 1.5);
    glutSolidCube(10);
    glPopMatrix(); // fim pé

    glPopMatrix(); // fim canela

    glPopMatrix();
    glPopMatrix();
}

void drawCabeca() {
    glPushMatrix(); // inicio cabeca
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_b_difusa);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_b_especular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_b_brilho);
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
    glBindTexture(GL_TEXTURE_2D, texture_id[1]);
    cubo(0);

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
    glBindTexture(GL_TEXTURE_2D, texture_id[2]);
    cubo(0);
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

void resetIdle() {
    baseAnim = 0.0f;
    deltaTimeBaseAnim = deltaTime;

    angleCorpoX = 0.0f;
    angleCorpoY = 0.0f;
    angleCorpoZ = 0.0f;

    deltaTimeAngleCorpoX = deltaTime;

    // Tronco
    deltaTimeTroncoBase = 1.5;
    troncoAngleTopX = 0;
    troncoAngleTopY = 0;
    troncoAngleTopZ = 0;
    troncoAngleBotX = 0;
    troncoAngleBotY = 0;
    troncoAngleBotZ = 0;
    deltaTimeTroncoX = 1.5;
    deltaTimeTroncoZ = 0.5;
    deltaTimeTroncoY = 1.5;

    timerAnim = 0.0f;

    /* Ombro */

    angleOmbroIdle = -45.0f;

    angleOmbroDirX = 0.0f;
    angleOmbroDirY = 0.0f;
    angleOmbroDirZ = 0.0f;
    angleOmbroEsqX = 0.0f;
    angleOmbroEsqY = 0.0f;
    angleOmbroEsqZ = 0.0f;

    deltaTimeOmbroBase = 2;
    deltaTimeOmbroX = deltaTimeOmbroBase;
    deltaTimeOmbroY = deltaTimeOmbroBase;
    deltaTimeOmbroZ = 0.5;

    /* Cotovelo */
    cotoveloAngleIdle = -110;

    angleCotoveloDirX = 0.0f;
    angleCotoveloDirY = 0.0f;
    angleCotoveloDirZ = 0.0f;
    angleCotoveloEsqX = 0.0f;
    angleCotoveloEsqY = 0.0f;
    angleCotoveloEsqZ = 0.0f;

    deltaTimeCotoveloBase = 1.5;
    deltaTimeCotoveloX = deltaTimeCotoveloBase;
    deltaTimeCotoveloY = deltaTimeCotoveloBase;
    deltaTimeCotoveloZ = deltaTimeCotoveloBase;

    // Perna
    anglePernaDirX = 0.0f;
    anglePernaDirY = 0.0f;
    anglePernaDirZ = 0.0f;
    anglePernaEsqX = 0.0f;
    anglePernaEsqY = 0.0f;
    anglePernaEsqZ = 0.0f;

    // Coxa
    angleCoxaDirX = 0.0f;
    angleCoxaDirY = 0.0f;
    angleCoxaDirZ = 0.0f;
    angleCoxaEsqX = 0.0f;
    angleCoxaEsqY = 0.0f;
    angleCoxaEsqZ = 0.0f;


    deltaTimeCoxaBase = 1.5;
    deltaTimeCoxaX = deltaTimeCoxaBase;
    deltaTimeCoxaY = deltaTimeCoxaBase;
    deltaTimeCoxaZ = deltaTimeCoxaBase;

    // Joelho
    angleJoelhoDirX = 0.0f;
    angleJoelhoDirY = 0.0f;
    angleJoelhoDirZ = 0.0f;
    angleJoelhoEsqX = 0.0f;
    angleJoelhoEsqY = 0.0f;
    angleJoelhoEsqZ = 0.0f;

    deltaTimeJoelho = deltaTime;

    transCorpoX = 0.0f;
    transCorpoY = 0.0f;
    transCorpoZ = 0;
}

void resetAnim() {
    deltaTimeAngleCorpoY = 10;
    switch (animacaoAtual) {
    case 2: // anim idle
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

        transCorpoZ = 0;

        break;
    case  3:
        // tronco
        troncoAngleTopZ = 0;

        // reseta timer
        timerAnim = 0;
        break;
    case 4:
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
    case 5:
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
    case  6:
        deltaTimeCoxaZ = deltaTimeCoxaBase;
        break;
    case 9:
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

    if (animacaoAtual == 0) { // animação idle
        // anima o tronco
        troncoAngleTopZ += deltaTimeTroncoZ;
        if (troncoAngleTopZ > 5)
            deltaTimeTroncoZ *= -1;
        else if (troncoAngleTopZ < -5)
            deltaTimeTroncoZ *= -1;
    }

    if (animacaoAtual == 1) {

        troncoAngleTopZ += deltaTimeTroncoZ;
        if (troncoAngleTopZ > 5)
            deltaTimeTroncoZ *= -1;
        else if (troncoAngleTopZ < -5)
            deltaTimeTroncoZ *= -1;

        transCorpoZ += deltaTransCorpoZ;

        angleCoxaDirX += deltaTimeCoxaBase;
        angleCoxaEsqX = angleCoxaDirX * -1;

        if (angleCoxaDirX > 30) {
            deltaTimeCoxaBase *= -1;
        }

        if (angleCoxaDirX < -30) {
            deltaTimeCoxaBase *= -1;
        }

        if (transCorpoZ > 0) {
            animacaoAtual = 2;
            PlaySound(TEXT("C:\\Users\\pedro\\Documents\\repos\\static\\computacao-grafica\\projeto\\naruto.wav"), NULL, SND_FILENAME | SND_ASYNC);
            resetAnim();
        }

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

        angleCotoveloDirX = (-1 * ((angleOmbroDirX / -90) - 1)) * -90;

        angleOmbroEsqX = (-1 * ((angleOmbroDirX / -90) - 1)) * -30;
        angleCotoveloEsqX = (angleOmbroDirX / -90) * -90;

        if (angleOmbroDirX < -90) {
            deltaTimeOmbroX = 0;
            animacaoAtual = 6;
            resetAnim();
        }

    }

    if (animacaoAtual == 6) {
        troncoAngleTopY -= deltaTimeTroncoBase;

        transCorpoX = (troncoAngleTopY / 40) * -10;

        angleCoxaDirZ += deltaTimeCoxaZ;

        anglePernaEsqY = (angleCoxaDirZ / -30) * 90;

        if (troncoAngleTopY < 0) {
            deltaTimeTroncoBase = 0;
            animacaoAtual = 7;
        }

        if (angleCoxaDirZ > 0) {
            deltaTimeCoxaZ = 0;
        }

    }

    if (animacaoAtual == 7) {
        angleCoxaEsqX -= deltaTimeCoxaX * 2;

        angleOmbroDirX = (-1 * ((angleCoxaEsqX / -90) - 1)) * -90;

        angleJoelhoEsqX = (angleCoxaDirX / -110) * 1;

        if (angleCoxaEsqX < -110) {
            deltaTimeCoxaX = deltaTimeCoxaX * -1;
        }

        if (angleCoxaEsqX > 0) {
            animacaoAtual = 8;
        }

    }

    if (animacaoAtual == 8) {
        resetIdle();
        animacaoAtual = 9;
    }

    if (animacaoAtual == 9) { // cambalhota

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
            resetIdle();
            animacaoAtual = 0;
        }

    }

}

void init()
{
    initTexture();
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

    drawRingue();
    drawRobots();
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
    glutInitWindowSize(900, 600);
    glutInitWindowPosition(150, 100);
    glutCreateWindow("Luta de Robotos?");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    TimerCamera(1);
    glutMainLoop();
    return 0;
}
