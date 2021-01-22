#pragma once
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>

GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth, GLint* ImHeight, GLint* ImComponents, GLenum* ImFormat);