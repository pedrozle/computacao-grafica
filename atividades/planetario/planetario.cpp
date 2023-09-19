#include <windows.h>
#include <stdio.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "tgaload.h"
#include <memory.h>
#include <stdlib.h>
#include <cmath>
#include "planetario.h"


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







#define MAX_NO_TEXTURES 6
GLuint texture_id[MAX_NO_TEXTURES];



void initTexture(void)
{

    // Habilita o uso de textura 
    glEnable(GL_TEXTURE_2D);

    // Define a forma de armazenamento dos pixels na textura (1= alihamento por byte)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Define quantas texturas ser�o usadas no programa 
    glGenTextures(MAX_NO_TEXTURES, texture_id);  // 1 = uma textura;
    // texture_id = vetor que guardas os n�meros das texturas

   //  Scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //  Scale linearly when image smaller than texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Define o n�mero da textura do cubo.
    texture_id[0] = 1001;
    texture_id[1] = 1002;
    texture_id[2] = 1003;
    texture_id[3] = 1004;
    texture_id[4] = 1005;

    // Define que tipo de textura ser� usada
    // GL_TEXTURE_2D ==> define que ser� usada uma textura 2D (bitmaps)
    // texture_id[CUBE_TEXTURE]  ==> define o n�mero da textura 

    // // // carrega a uma imagem TGA 
    image_t background, sun, mercury, venus, earth, mars, jupiter, saturn, uranus, neptune;
    glBindTexture(GL_TEXTURE_2D, texture_id[0]);
    tgaLoad("D:\\computacao-grafica\\atividades\\planetario\\milky.tga", &background, TGA_FREE | TGA_LOW_QUALITY);
    glBindTexture(GL_TEXTURE_2D, texture_id[1]);
    tgaLoad("D:\\computacao-grafica\\atividades\\planetario\\sun.tga", &sun, TGA_FREE | TGA_LOW_QUALITY);
    glBindTexture(GL_TEXTURE_2D, texture_id[2]);
    tgaLoad("D:\\computacao-grafica\\atividades\\planetario\\mercury.tga", &mercury, TGA_FREE | TGA_LOW_QUALITY);
    glBindTexture(GL_TEXTURE_2D, texture_id[3]);
    tgaLoad("D:\\computacao-grafica\\atividades\\planetario\\venus.tga", &venus, TGA_FREE | TGA_LOW_QUALITY);
    glBindTexture(GL_TEXTURE_2D, texture_id[4]);
    tgaLoad("D:\\computacao-grafica\\atividades\\planetario\\earth.tga", &earth, TGA_FREE | TGA_LOW_QUALITY);
    glBindTexture(GL_TEXTURE_2D, texture_id[5]);
    tgaLoad("D:\\computacao-grafica\\atividades\\planetario\\mars.tga", &mars, TGA_FREE | TGA_LOW_QUALITY);
    glBindTexture(GL_TEXTURE_2D, texture_id[6]);
    tgaLoad("D:\\computacao-grafica\\atividades\\planetario\\juspter.tga", &jupiter, TGA_FREE | TGA_LOW_QUALITY);
    glBindTexture(GL_TEXTURE_2D, texture_id[7]);
    tgaLoad("D:\\computacao-grafica\\atividades\\planetario\\saturn.tga", &saturn, TGA_FREE | TGA_LOW_QUALITY);
    glBindTexture(GL_TEXTURE_2D, texture_id[8]);
    tgaLoad("D:\\computacao-grafica\\atividades\\planetario\\uranus.tga", &uranus, TGA_FREE | TGA_LOW_QUALITY);
    glBindTexture(GL_TEXTURE_2D, texture_id[9]);
    tgaLoad("D:\\computacao-grafica\\atividades\\planetario\\nepture.tga", &neptune, TGA_FREE | TGA_LOW_QUALITY);

}

void drawBackground() {

    // glMatrixMode(GL_PROJECTION);
    // glPushMatrix();
    // glLoadIdentity();
    // gluPerspective(95.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);
    // glMatrixMode(GL_MODELVIEW);
    // glPushMatrix();


    // glLoadIdentity();
    // glDisable(GL_LIGHTING);


    // glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_id[0]);

    // Draw a textured quad
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(0, 0, -100);
    glTexCoord2f(0, 1); glVertex3f(0, glutGet(GLUT_WINDOW_HEIGHT), -100);
    glTexCoord2f(1, 1); glVertex3f(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), -100);
    glTexCoord2f(1, 0); glVertex3f(glutGet(GLUT_WINDOW_WIDTH), 0, -100);
    glEnd();


    // glDisable(GL_TEXTURE_2D);
    // glPopMatrix();


    // glMatrixMode(GL_PROJECTION);
    // glPopMatrix();

}

void init()
{
    glClearColor(1, 1, 1, 1);
    glClearDepth(1.0); // Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS); // The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST); // Enables Depth Testing
    glShadeModel(GL_SMOOTH); // Enables Smooth Color Shading
    initTexture();
}

void drawPlanet(GLfloat red, GLfloat green, GLfloat blue, GLfloat radius, GLfloat slices, GLfloat stacks) {

    glEnable(GL_TEXTURE_2D);
    GLUquadric* sphere;
    sphere = gluNewQuadric();             //Instatiate our sphere
    gluQuadricDrawStyle(sphere, GLU_FILL);//Following three method calls instantiate the texturing style
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, radius, slices, stacks);
    glDisable(GL_TEXTURE_2D);


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
    glBindTexture(GL_TEXTURE_2D, texture_id[1]);
    drawPlanet(1, 1, 0, 12.0f, 16, 16); // Sol

    glPushMatrix(); // Push pai mercurio
    glRotatef(angleTransMercurio, 0, 1, 0);
    glTranslatef(distMercurio, 0, 0);
    glPushMatrix(); // Push filhos mercurio
    glPopMatrix(); // Pop filhos mercurio
    glRotatef(angleRotMercurio, 0, 1, 0);
    glBindTexture(GL_TEXTURE_2D, texture_id[2]);
    drawPlanet(0, 0, 1, tamMercurio, 16, 16); // Mercurio
    glPopMatrix(); // Pop pai mercurio

    glPushMatrix(); // Push pai Venus
    glRotatef(angleTransVenus, 0, 1, 0);
    glTranslatef(distVenus, 0, 0);
    glPushMatrix(); // Push filhos Venus
    glPopMatrix(); // Pop filhos Venus
    glRotatef(angleRotVenus, 0, 1, 0);
    glBindTexture(GL_TEXTURE_2D, texture_id[3]);
    drawPlanet(1, 0.6, 0, tamVenus, 16, 16); // Venus
    glPopMatrix(); // Pop pai Venus

    glPushMatrix(); // Push pai terra
    glRotatef(angleTransTerra, 0, 1, 0);
    glTranslatef(distTerra, 0, 0);
    glPushMatrix(); // Push filhos da terra
    glRotatef(angleRotTerra, 0, 1, 0);
    glBindTexture(GL_TEXTURE_2D, texture_id[4]);
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
    glBindTexture(GL_TEXTURE_2D, texture_id[5]);
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
    glBindTexture(GL_TEXTURE_2D, texture_id[6]);
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
    glBindTexture(GL_TEXTURE_2D, texture_id[7]);
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
    glBindTexture(GL_TEXTURE_2D, texture_id[8]);
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
    glBindTexture(GL_TEXTURE_2D, texture_id[9]);
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

    // drawBackground();
    drawPlanetary();

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height); // Reset The Current Viewport

    glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
    glLoadIdentity(); // Reset The Projection Matrix

    // Calculate The Aspect Ratio Of The Window
    gluPerspective(95.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);
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
