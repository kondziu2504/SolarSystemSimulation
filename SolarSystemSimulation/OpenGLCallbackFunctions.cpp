#pragma once
#include "OpenGLCallbackFunctions.h"

std::vector<void (*)(GLsizei, GLsizei)> MotionCallbacks;
std::vector<void (*)(unsigned char key, int x, int y)> KeysCallbacks;

int status = 0;
int delta_x = 0;
int delta_y = 0;

//Zmienne pomocnicze do okreœlenia przesuniêcia myszy
int x_pos_old = 0;
int y_pos_old = 0;

// Funkcja zwrotna wywo³ywana przy u¿yciu dowolnego przycisku myszy.
// Zapisuje informacje o stanie myszy w odpowiednich zmiennych globalnych
void Mouse(int btn, int state, int x, int y)
{
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        //Zapisanie obecnej pozycji myszy, aby póŸniej okreœliæ zmianê pozycji.
        x_pos_old = x;  // podstawienie bie¿¹cego po³o¿enia jako poprzednie
        y_pos_old = y;  // podstawienie bie¿¹cego po³o¿enia jako poprzednie
        status = 1;     //zosta³ wciêniêty lewy przycisk myszy
    }
    else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        //Zapisanie obecnej pozycji myszy, aby póŸniej okreœliæ zmianê pozycji.
        x_pos_old = x;  // podstawienie bie¿¹cego po³o¿enia jako poprzednie
        y_pos_old = y;  // podstawienie bie¿¹cego po³o¿enia jako poprzednie
        status = 2;     //zosta³ wciêniêty prawy przycisk myszy
    }
    else
        status = 0;// nie zosta³ wcisniêty ¿aden przycisk
}

// Funkcja zwrotna wywo³ywana przy ruchu mysz¹ i 
//jednoczesnym naciœniêciu dowolnego przycisku myszy.
//Zapisuje informacje o stanie myszy w odpowiednich zmiennych globalnych.
void PassiveMotion(GLsizei x, GLsizei y)
{
    delta_x = x - x_pos_old;    // obliczenie ró¿nicy po³o¿enia kursora myszy
    x_pos_old = x;              // podstawienie bie¿¹cego po³o¿enia jako poprzednie
    delta_y = y - y_pos_old;    // obliczenie ró¿nicy po³o¿enia kursora myszy
    y_pos_old = y;              // podstawienie bie¿¹cego po³o¿enia jako poprzednie

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
    delta_x = x - x_pos_old;    // obliczenie ró¿nicy po³o¿enia kursora myszy
    x_pos_old = x;              // podstawienie bie¿¹cego po³o¿enia jako poprzednie
    delta_y = y - y_pos_old;    // obliczenie ró¿nicy po³o¿enia kursora myszy
    y_pos_old = y;              // podstawienie bie¿¹cego po³o¿enia jako poprzednie

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


// Funkcja ma za zadanie utrzymanie sta³ych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokoœæ i szerokoœæ okna) s¹ 
// przekazywane do funkcji za ka¿dym razem gdy zmieni siê rozmiar okna.
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(70, 1.0, 0.2, 400.0);
    // Ustawienie parametrów dla rzutu perspektywicznego

    if (horizontal <= vertical)
        glViewport(-(vertical - horizontal) / 2, 0, vertical, vertical);
    else
        glViewport(0, -(horizontal - vertical) / 2, horizontal, horizontal);
    // Ustawienie wielkoœci okna okna widoku (viewport) w zale¿noœci
    // relacji pomiêdzy wysokoœci¹ i szerokoœci¹ okna

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Idle()
{

}