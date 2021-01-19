#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#define GL_GLEXT_PROTOTYPES
#include <iostream>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cmath>
#include <math.h>
#include "Point.h"
#include "Cameras.h"
#include "LightSource.h"
#include "Planet.h"
#include <vector>
#include "glm-master/glm/mat4x4.hpp"
#include "glm-master/glm/vec4.hpp"
#include "glm-master/glm/vec3.hpp"
#include "glm-master/glm/gtc/matrix_transform.hpp"

using namespace std;

const Point3 RED{ 1, 0, 0 };
const Point3 GREEN{ 0, 1, 0 };
const Point3 BLUE{ 0, 0, 1 };
const Point3 YELLOW{ 1, 1, 0 };
const Point3 VIOLET{ 0.5, 0, 1 };
const Point3 ORANGE{ 1, 0.5, 0 };
const Point3 WHITE{ 1, 1, 1 };
const Point3 CYAN{ 0, 1, 1 };

unsigned int textures[9];
LightSource * light1;
float orbitsScale = 3;
vector<Planet> planets
{
    Planet(0.2, 1, Orbit(1.1, 2 * orbitsScale, 5, Point3{1,3,2}, RED)),  //mercury
    Planet(0.3, 2, Orbit(1.02, 3 * orbitsScale, 10, Point3{6,5,-1}, GREEN)),    //wenus
    Planet(0.4, 3, Orbit(1.04, 5 * orbitsScale, 20,  Point3{0,-3,0}, WHITE)),    //ziemia
    Planet(0.5, 4, Orbit(1.03, 7 * orbitsScale, 40, Point3{4,0,1}, BLUE)),    //Mars
    Planet(0.6, 5, Orbit(1.04, 9 * orbitsScale, 80,  Point3{-5,2,0}, YELLOW)),    //Jowisz
    Planet(0.7, 6, Orbit(1.05, 11 * orbitsScale, 160, Point3{1,0,-3}, VIOLET)),    //Saturn
    Planet(0.8, 7, Orbit(1.05, 13 * orbitsScale, 320,  Point3{0,1,0}, CYAN)),    //Uran
    Planet(0.9, 8, Orbit(1.02, 15 * orbitsScale, 640,  Point3{-3,-2,2}, ORANGE))    //Neptun
}; 

GLUquadric* sphere;

GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth, GLint* ImHeight, GLint* ImComponents, GLenum* ImFormat)
{
    // Struktura dla nag³ówka pliku  TGA
    
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

    // Wartoœci domyœlne zwracane w przypadku b³êdu

    *ImWidth = 0;
    *ImHeight = 0;
    *ImFormat = GL_BGR_EXT;
    *ImComponents = GL_RGB8;

    pFile = fopen(FileName, "rb");
    if (pFile == NULL)
        return NULL;

    // Przeczytanie nag³ówka pliku 
    fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

    // Odczytanie szerokoœci, wysokoœci i g³êbi obrazu
    *ImWidth = tgaHeader.width;
    *ImHeight = tgaHeader.height;
    sDepth = tgaHeader.bitsperpixel / 8;

    /*************************************************************************************/
    // Sprawdzenie, czy g³êbia spe³nia za³o¿one warunki (8, 24, lub 32 bity)

    if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
        return NULL;

    // Obliczenie rozmiaru bufora w pamiêci
    lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

    // Alokacja pamiêci dla danych obrazu
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


//// Funkcja rysuj¹ca osie uk³adu wspó?rz?dnych
void Axes(void)
{
    float axisLength = 100;

    Point3 x_min = { -axisLength, 0.0, 0.0 };
    Point3 x_max = { axisLength, 0.0, 0.0 };
    // pocz?tek i koniec obrazu osi x

    Point3 y_min = { 0.0, -axisLength, 0.0 };
    Point3 y_max = { 0.0,  axisLength, 0.0 };
    // pocz?tek i koniec obrazu osi y

    Point3 z_min = { 0.0, 0.0, -axisLength };
    Point3 z_max = { 0.0, 0.0,  axisLength };
    //  pocz?tek i koniec obrazu osi y

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
    glBegin(GL_LINES); // rysowanie osi x
    glVertex3f(x_min.x, x_min.y, x_min.z);
    glVertex3f(x_max.x, x_max.y, x_max.z);

    glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
    glBegin(GL_LINES);  // rysowanie osi y

    glVertex3f(y_min.x, y_min.y, y_min.z);
    glVertex3f(y_max.x, y_max.y, y_max.z);
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
    glBegin(GL_LINES); // rysowanie osi z

    glVertex3f(z_min.x, z_min.y, z_min.z);
    glVertex3f(z_max.x, z_max.y, z_max.z);
    glEnd();

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}

TargetCamera mainCamera(Point3{ 0, 0, -10 }, 10);

void LookWithCamera(TargetCamera camera) 
{
    Point3 position = camera.GetPosition();
  
    gluLookAt(
        position.x, position.y, position.z,
        0, 0, 0,
        0, 1, 0
    );
}



//Wciœniêty przycisk myszy: 0 - ¿aden, 1 - lewy przycisk myszy, 2 - prawy przycisk myszy
GLint status = 0;


//Zmienne pomocnicze do okreœlenia przesuniêcia myszy
int x_pos_old = 0;
int delta_x = 0;
int y_pos_old = 0;
int delta_y = 0;


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
void Motion(GLsizei x, GLsizei y)
{
    delta_x = x - x_pos_old;    // obliczenie ró¿nicy po³o¿enia kursora myszy
    x_pos_old = x;              // podstawienie bie¿¹cego po³o¿enia jako poprzednie
    delta_y = y - y_pos_old;    // obliczenie ró¿nicy po³o¿enia kursora myszy
    y_pos_old = y;              // podstawienie bie¿¹cego po³o¿enia jako poprzednie

    if (status == 1)
    {
        mainCamera.AppendAzimuth(delta_x);
        mainCamera.AppendElevation(delta_y);
    }
    if (status == 2)
    {
        mainCamera.AppendRadius(-delta_y * 0.25);
    }

    glutPostRedisplay();     // przerysowanie obrazu sceny
}


void UpdateLight()
{
    Point4p lightPos = light1->GetPosition();
    glLightfv(light1->GetIndex(), GL_POSITION, lightPos);
    delete lightPos;
}


float time;

float sunRadius = 2;

void DrawSun()
{
    glLoadIdentity();
    LookWithCamera(mainCamera);

    glDisable(GL_LIGHTING);
    glColor3f(1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, sunRadius, 32, 16);

    glEnable(GL_LIGHTING);
}

void DrawPlanet(Planet planet)
{
    Point3 position = planet.GetPointOnOrbit(time);

    glLoadIdentity();

    LookWithCamera(mainCamera);
    UpdateLight();
    glTranslatef(position.x, position.y, position.z);

    glBindTexture(GL_TEXTURE_2D, textures[planet.GetTexIndex()]);
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, planet.GetRadius(), 32, 16);

    vector<Point3> orbitPoints = planet.GetPointsOnOrbit();
    glLoadIdentity();
    LookWithCamera(mainCamera);
    
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    Point3 color = planet.GetOrbitColor();
    float colorIntensity = 0.5f;
    glColor3f(color.x * colorIntensity, color.y * colorIntensity, color.z * colorIntensity);
    glBegin(GL_LINE_LOOP);
    for (Point3 & point : orbitPoints)
    {    
        glVertex3f(point.x, point.y, point.z);
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}


// Funkcja okreœlaj¹ca co ma byæ rysowane (zawsze wywo³ywana, gdy trzeba
// przerysowaæ scenê)
void RenderScene(void)
{
    time += 0.01;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();



    glLoadIdentity();
    LookWithCamera(mainCamera);
    Axes();

    DrawSun();

    for (Planet planet : planets)
    {
        DrawPlanet(planet);
    }



    glFlush();
    // Przekazanie poleceñ rysuj¹cych do wykonania
    glutSwapBuffers();

    glutPostRedisplay();
}
/*************************************************************************************/



//Funkcja zwrotna zmieniaj¹ca aktualnie wyœwietlane zadanie
void keys(unsigned char key, int x, int y)
{

    glutPostRedisplay();     // przerysowanie obrazu sceny
}


void InitializeTexturing()
{
    glEnable(GL_CULL_FACE);  
    
    glGenTextures(9, textures);
    
    glEnable(GL_TEXTURE_2D);
}

void LoadTextureAtInd(string filename, int index)
{
    GLint ImWidth, ImHeight, ImComponents;
    GLenum ImFormat;
    GLbyte * pBytes = LoadTGAImage(filename.c_str(), &ImWidth, &ImHeight, &ImComponents, &ImFormat);
    glBindTexture(GL_TEXTURE_2D, textures[index]);
    glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
    free(pBytes);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// Ustawienie opcji systemu oœwietlania sceny i inicjalizacja Ÿróde³ œwiat³a
void InitializeLighting()
{
    glShadeModel(GL_SMOOTH); // w³aczenie ³agodnego cieniowania
    glEnable(GL_LIGHTING);   // w³aczenie systemu oœwietlenia sceny
    glEnable(GL_DEPTH_TEST); // w³¹czenie mechanizmu z-bufora

    float ambient = 3.f;
    float diffuse = 10.f;
    float specular = 5.f;

    light1 = new LightSource(
        Point4{ ambient, ambient, ambient, 1.0 },
        Point4{ diffuse, diffuse, diffuse, 1.0 },
        Point4{ specular, specular, specular, 1.0 });

    light1->Enable(true);
    light1->SetPosition(Point4{ 0,0,0,1 });
}

// Funkcja ustalaj¹ca stan renderowania
void MyInit(void)
{
    InitializeLighting();
    InitializeTexturing();

    string filenames[]
    { 
        "sun.tga",
        "mercury.tga", 
        "venus.tga",
        "earth.tga", 
        "mars.tga", 
        "jupiter.tga", 
        "saturn.tga", 
        "uranus.tga", 
        "neptune.tga" 
    };
    
    for (int i = 0; i < 9; i++)
        LoadTextureAtInd(filenames[i], i);

    sphere = gluNewQuadric();
}


// Funkcja ma za zadanie utrzymanie sta³ych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokoœæ i szerokoœæ okna) s¹ 
// przekazywane do funkcji za ka¿dym razem gdy zmieni siê rozmiar okna.
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
    glMatrixMode(GL_PROJECTION);
    // Prze³¹czenie macierzy bie¿¹cej na macierz projekcji

    glLoadIdentity();
    // Czyszcznie macierzy bie¿¹cej

    gluPerspective(70, 1.0, 1.0, 400.0);
    // Ustawienie parametrów dla rzutu perspektywicznego

    
    if (horizontal <= vertical)
        glViewport(-(vertical - horizontal) / 2, 0, vertical, vertical);
    else
        glViewport(0, -(horizontal - vertical) / 2, horizontal, horizontal);
    // Ustawienie wielkoœci okna okna widoku (viewport) w zale¿noœci
    // relacji pomiêdzy wysokoœci¹ i szerokoœci¹ okna

    glMatrixMode(GL_MODELVIEW);
    // Prze³¹czenie macierzy bie¿¹cej na macierz widoku modelu  

    glLoadIdentity();
    // Czyszczenie macierzy bie¿¹cej

}


void Idle()
{

}

// G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli
void main(void)
{

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(600, 600);

    glutCreateWindow("Rzutowanie perspektywiczne");

    //Przypisanie funkcji zwrotnych
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutKeyboardFunc(keys);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutIdleFunc(Idle);
    MyInit();
    // Funkcja MyInit() (zdefiniowana powy¿ej) wykonuje wszelkie
    // inicjalizacje konieczne  przed przyst¹pieniem do renderowania
    glEnable(GL_DEPTH_TEST);
    // W³¹czenie mechanizmu usuwania niewidocznych elementów sceny

    glutMainLoop();
    // Funkcja uruchamia szkielet biblioteki GLUT
}

/*************************************************************************************/