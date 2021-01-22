#define _CRT_SECURE_NO_WARNINGS
#include "TGAReading.h"
#include <iostream>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>


GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth, GLint* ImHeight, GLint* ImComponents, GLenum* ImFormat)
{
    // Struktura dla nag��wka pliku  TGA

#pragma pack(1)           
    typedef struct
    {
        GLbyte    idlength;
        GLbyte    colormaptype;
        GLbyte    datatypecode;
        unsigned short    colormapstart;
        unsigned short    colormaplength;
        unsigned char     colormapdepth;
        unsigned short    x_orgin;
        unsigned short    y_orgin;
        unsigned short    width;
        unsigned short    height;
        GLbyte    bitsperpixel;
        GLbyte    descriptor;
    }TGAHEADER;
#pragma pack(8)

    FILE* pFile;
    TGAHEADER tgaHeader;
    unsigned long lImageSize;
    short sDepth;
    GLbyte* pbitsperpixel = NULL;

    // Warto�ci domy�lne zwracane w przypadku b��du

    *ImWidth = 0;
    *ImHeight = 0;
    *ImFormat = GL_BGR_EXT;
    *ImComponents = GL_RGB8;

    pFile = fopen(FileName, "rb");
    if (pFile == NULL)
        return NULL;

    // Przeczytanie nag��wka pliku 
    fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

    // Odczytanie szeroko�ci, wysoko�ci i g��bi obrazu
    *ImWidth = tgaHeader.width;
    *ImHeight = tgaHeader.height;
    sDepth = tgaHeader.bitsperpixel / 8;

    /*************************************************************************************/
    // Sprawdzenie, czy g��bia spe�nia za�o�one warunki (8, 24, lub 32 bity)

    if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
        return NULL;

    // Obliczenie rozmiaru bufora w pami�ci
    lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

    // Alokacja pami�ci dla danych obrazu
    pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));

    if (pbitsperpixel == NULL)
        return NULL;

    if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
    {
        free(pbitsperpixel);
        return NULL;
    }

    // Ustawienie formatu OpenGL
    switch (sDepth)
    {
    case 3:
        *ImFormat = GL_BGR_EXT;
        *ImComponents = GL_RGB8;
        break;
    case 4:
        *ImFormat = GL_BGRA_EXT;
        *ImComponents = GL_RGBA8;
        break;
    case 1:
        *ImFormat = GL_LUMINANCE;
        *ImComponents = GL_LUMINANCE8;
        break;
    };

    fclose(pFile);

    return pbitsperpixel;
}
