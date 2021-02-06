#pragma once
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>

GLbyte* readPng(const char* filename, GLint& imWidth, GLint& imHeight, GLint& imComponents, GLenum& imFormat);
