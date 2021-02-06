#define _CRT_SECURE_NO_WARNINGS
#include "PngReading.h"
#include <iostream>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <png.h>
#include <exception>

using namespace std;

GLbyte* readPng(const char* filename, GLint& imWidth, GLint& imHeight, GLint& imComponents, GLenum& imFormat)
{
    UCHAR* image_data = NULL;
    png_bytep* row_pointers = NULL;

    FILE* fp = fopen(filename, "rb");

    if (fp == NULL)
        throw exception(((string)("Couldn't open file ") + filename).c_str());


    UCHAR sig[8];

    fread(sig, 1, 8, fp);
    if (!png_check_sig(sig, 8))
        throw exception("Bad signature");

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
        NULL);
    if (!png_ptr)
        throw std::bad_alloc();

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        throw std::bad_alloc();
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        if(image_data != NULL)
            free(image_data);
        if (row_pointers != NULL)
            delete[] row_pointers;
        throw exception("Reading error");
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    png_uint_32 width, height, rowbytes, channels;
    int bit_depth, color_type;

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
        &color_type, NULL, NULL, NULL);

    rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    channels = (int)png_get_channels(png_ptr, info_ptr);


    imWidth = width;
    imHeight = height;

    switch (channels)
    {
    case 3:
        imFormat = GL_RGB;
        imComponents = GL_RGB8;
        break;
    case 4:
        imFormat = GL_RGBA;
        imComponents = GL_RGBA8;
        break;
    case 1:
        imFormat = GL_LUMINANCE;
        imComponents = GL_LUMINANCE8;
        break;
    };


    if ((image_data = (UCHAR*)malloc(rowbytes * height)) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        throw std::bad_alloc();
    }

    row_pointers = new png_bytep[height];

    for (png_uint_32 i = 0; i < height; ++i)
        row_pointers[i] = image_data + i * rowbytes;

    png_read_image(png_ptr, row_pointers);

    for (png_uint_32 i = 0; i < height/2; i++)
        std::_Swap_ranges_unchecked(row_pointers[i], row_pointers[i] + rowbytes, row_pointers[height - 1 - i]);
    
    png_read_end(png_ptr, NULL);

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    delete[] row_pointers;
    return (GLbyte*)image_data;
}

