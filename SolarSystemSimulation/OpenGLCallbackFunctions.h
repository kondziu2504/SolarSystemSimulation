#pragma once
#include<vector>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>

extern std::vector<void (*)(GLsizei, GLsizei)> MotionCallbacks;
extern std::vector<void (*)(unsigned char key, int x, int y)> KeysCallbacks;

//Wciœniêty przycisk myszy: 0 - ¿aden, 1 - lewy przycisk myszy, 2 - prawy przycisk myszy
extern GLint status;

extern int delta_x;
extern int delta_y;

void Motion(GLsizei x, GLsizei y);
void Mouse(int btn, int state, int x, int y);
void keys(unsigned char key, int x, int y);
void ChangeSize(GLsizei horizontal, GLsizei vertical);
