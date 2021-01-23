#pragma once
#include "OpenGLCallbackFunctions.h"
#include "ViewParameters.h"

std::vector<void (*)(GLsizei, GLsizei)> MotionCallbacks;
std::vector<void (*)(unsigned char key, int x, int y)> KeysCallbacks;

//Stan myszy: 0 - brak wciœniêtych przycisków myszy, 1 - wciœniêty LPM, 2 - wciœniêty PPM
int status = 0;
//Przesuniêcie myszy
int delta_x = 0;
int delta_y = 0;

//Zmienne pomocnicze do okreœlenia przesuniêcia myszy
int x_pos_old = 0;
int y_pos_old = 0;

// Funkcja zwrotna wywo³ywana przy u¿yciu dowolnego przycisku myszy.
// Zapisuje informacje o stanie myszy.
void Mouse(int btn, int state, int x, int y)
{
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        x_pos_old = x; 
        y_pos_old = y; 
        status = 1;
    }
    else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        x_pos_old = x;
        y_pos_old = y;
        status = 2;
    }
    else
        status = 0;
}

//Zapisuje informacje o stanie myszy, gdy nie jest wciœniêty ¿aden przycisk
void PassiveMotion(GLsizei x, GLsizei y)
{
    delta_x = x - x_pos_old;
    x_pos_old = x;
    delta_y = y - y_pos_old;
    y_pos_old = y;

    //Wywo³anie funkcji zwrotnych zdefiniowanych na zewn¹trz
    for (void (*funcPointer)(GLsizei, GLsizei) : MotionCallbacks)
    {
        (*funcPointer)(x, y);
    }
}

//Zapisuje informacje o stanie myszy, gdy jest wciœniêty przycisk
void Motion(GLsizei x, GLsizei y)
{
    delta_x = x - x_pos_old;
    x_pos_old = x;
    delta_y = y - y_pos_old;
    y_pos_old = y;

     //Wywo³anie funkcji zwrotnych zdefiniowanych na zewn¹trz
    for (void (*funcPointer)(GLsizei, GLsizei) : MotionCallbacks)
    {
        (*funcPointer)(x, y);
    }
}

//Funkcja zwrotna zmieniaj¹ca aktualnie wyœwietlane zadanie
void keys(unsigned char key, int x, int y)
{
    for (void (*funcPointer)(unsigned char key, int x, int y) : KeysCallbacks)
    {
        (*funcPointer)(key, x, y);
    }

    glutPostRedisplay();     // przerysowanie obrazu sceny
}


//Funkcja zwrotna zachowuj¹ca odpowiednie proporcje na ekranie
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(fovY, (GLdouble)horizontal / vertical, zNear, zFar);
    glViewport(0, 0, horizontal, vertical);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Idle()
{

}