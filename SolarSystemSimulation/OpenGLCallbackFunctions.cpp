#pragma once
#include "OpenGLCallbackFunctions.h"

std::vector<void (*)(GLsizei, GLsizei)> MotionCallbacks;
std::vector<void (*)(unsigned char key, int x, int y)> KeysCallbacks;

int status = 0;
int delta_x = 0;
int delta_y = 0;

//Zmienne pomocnicze do okre�lenia przesuni�cia myszy
int x_pos_old = 0;
int y_pos_old = 0;

// Funkcja zwrotna wywo�ywana przy u�yciu dowolnego przycisku myszy.
// Zapisuje informacje o stanie myszy w odpowiednich zmiennych globalnych
void Mouse(int btn, int state, int x, int y)
{
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        //Zapisanie obecnej pozycji myszy, aby p�niej okre�li� zmian� pozycji.
        x_pos_old = x;  // podstawienie bie��cego po�o�enia jako poprzednie
        y_pos_old = y;  // podstawienie bie��cego po�o�enia jako poprzednie
        status = 1;     //zosta� wci�ni�ty lewy przycisk myszy
    }
    else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        //Zapisanie obecnej pozycji myszy, aby p�niej okre�li� zmian� pozycji.
        x_pos_old = x;  // podstawienie bie��cego po�o�enia jako poprzednie
        y_pos_old = y;  // podstawienie bie��cego po�o�enia jako poprzednie
        status = 2;     //zosta� wci�ni�ty prawy przycisk myszy
    }
    else
        status = 0;// nie zosta� wcisni�ty �aden przycisk
}

// Funkcja zwrotna wywo�ywana przy ruchu mysz� i 
//jednoczesnym naci�ni�ciu dowolnego przycisku myszy.
//Zapisuje informacje o stanie myszy w odpowiednich zmiennych globalnych.
void PassiveMotion(GLsizei x, GLsizei y)
{
    delta_x = x - x_pos_old;    // obliczenie r�nicy po�o�enia kursora myszy
    x_pos_old = x;              // podstawienie bie��cego po�o�enia jako poprzednie
    delta_y = y - y_pos_old;    // obliczenie r�nicy po�o�enia kursora myszy
    y_pos_old = y;              // podstawienie bie��cego po�o�enia jako poprzednie

    //if (delta_x == 1)
    //    delta_x = 0;
    //if (delta_y == 1)
    //    delta_y = 0;

    for (void (*funcPointer)(GLsizei, GLsizei) : MotionCallbacks)
    {
        (*funcPointer)(x, y);
    }
}

void Motion(GLsizei x, GLsizei y)
{
    delta_x = x - x_pos_old;    // obliczenie r�nicy po�o�enia kursora myszy
    x_pos_old = x;              // podstawienie bie��cego po�o�enia jako poprzednie
    delta_y = y - y_pos_old;    // obliczenie r�nicy po�o�enia kursora myszy
    y_pos_old = y;              // podstawienie bie��cego po�o�enia jako poprzednie

    for (void (*funcPointer)(GLsizei, GLsizei) : MotionCallbacks)
    {
        (*funcPointer)(x, y);
    }
}

//Funkcja zwrotna zmieniaj�ca aktualnie wy�wietlane zadanie
void keys(unsigned char key, int x, int y)
{
    for (void (*funcPointer)(unsigned char key, int x, int y) : KeysCallbacks)
    {
        (*funcPointer)(key, x, y);
    }

    glutPostRedisplay();     // przerysowanie obrazu sceny
}


// Funkcja ma za zadanie utrzymanie sta�ych proporcji rysowanych
// w przypadku zmiany rozmiar�w okna.
// Parametry vertical i horizontal (wysoko�� i szeroko�� okna) s� 
// przekazywane do funkcji za ka�dym razem gdy zmieni si� rozmiar okna.
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(70, 1.0, 0.2, 400.0);
    // Ustawienie parametr�w dla rzutu perspektywicznego

    if (horizontal <= vertical)
        glViewport(-(vertical - horizontal) / 2, 0, vertical, vertical);
    else
        glViewport(0, -(horizontal - vertical) / 2, horizontal, horizontal);
    // Ustawienie wielko�ci okna okna widoku (viewport) w zale�no�ci
    // relacji pomi�dzy wysoko�ci� i szeroko�ci� okna

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Idle()
{

}