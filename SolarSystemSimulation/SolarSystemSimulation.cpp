#define _USE_MATH_DEFINES

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
#include "OpenGLCallbackFunctions.h"
#include "TGAReading.h"
#include <chrono>

using namespace std;

const Point3 RED{ 1, 0, 0 };
const Point3 GREEN{ 0, 1, 0 };
const Point3 BLUE{ 0, 0, 1 };
const Point3 YELLOW{ 1, 1, 0 };
const Point3 VIOLET{ 0.5, 0, 1 };
const Point3 ORANGE{ 1, 0.5, 0 };
const Point3 WHITE{ 1, 1, 1 };
const Point3 CYAN{ 0, 1, 1 };

//Do zrobienia
//Kamery

float timeScale = 1;

float orbitsScale = 3;
vector<Planet> planets;

unsigned int skyboxTextureInd;
GLUquadric* skyboxSphere;
float skyboxRadius = 390;

LightSource* light1;
unsigned int sunTextureInd;
GLUquadric* sunSphere;
float sunRadius = 2;
float sunRotationPeriod = 150;

float currentTime;
bool updateTime = true;


//// Funkcja rysuj¹ca osie uk³adu wspó?rz?dnych
void Axes(void)
{
    float axisLength = 100;

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_LINES);

    glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
    glVertex3f(-axisLength, 0, 0);
    glVertex3f(axisLength, 0, 0);

    glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
    glVertex3f(0, -axisLength, 0);
    glVertex3f(0, axisLength, 0);

    glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
    glVertex3f(0, 0, -axisLength);
    glVertex3f(0, 0, axisLength);

    glEnd();

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}

TargetCamera* currentCamera;
TargetCamera* sunCamera;
TargetCamera** planetCameras;

void CameraMotion(GLsizei x, GLsizei y)
{
    if (status == 1)
    {
        currentCamera->AppendAzimuth(delta_x);
        currentCamera->AppendElevation(delta_y);
    }
    if (status == 2)
    {
        currentCamera->AppendRadius(-delta_y * 0.25);
    }
}

void UpdateLight()
{
    Point4p lightPos = light1->GetPosition();
    glLightfv(light1->GetIndex(), GL_POSITION, lightPos);
    delete lightPos;
}

void LoadTextureAtInd(string filename, int textureIndex)
{
    GLint ImWidth, ImHeight, ImComponents;
    GLenum ImFormat;
    GLbyte* pBytes = LoadTGAImage(filename.c_str(), &ImWidth, &ImHeight, &ImComponents, &ImFormat);
    glBindTexture(GL_TEXTURE_2D, textureIndex);
    glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
    free(pBytes);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void InitializeSun()
{
    glGenTextures(1, &sunTextureInd);
    LoadTextureAtInd("sun.tga", sunTextureInd);

    sunSphere = gluNewQuadric();
    
    gluQuadricDrawStyle(sunSphere, GLU_FILL);
    gluQuadricTexture(sunSphere, GL_TRUE);
    gluQuadricNormals(sunSphere, GLU_SMOOTH);
}

void InitializeSkybox()
{
    glGenTextures(1, &skyboxTextureInd);
    LoadTextureAtInd("stars.tga", skyboxTextureInd);

    skyboxSphere = gluNewQuadric();

    gluQuadricDrawStyle(skyboxSphere, GLU_FILL);
    gluQuadricTexture(skyboxSphere, GL_TRUE);
    gluQuadricNormals(skyboxSphere, GLU_SMOOTH);
}

void DrawSun(float time)
{
    glPushMatrix();

    glColor3f(1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, sunTextureInd);
    glDisable(GL_LIGHTING);
    glRotatef(time / sunRotationPeriod * 360, 0, 1, 0);
    gluSphere(sunSphere, sunRadius, 32, 16);
    glEnable(GL_LIGHTING);

    glPopMatrix();
}


void DrawSkybox()
{
    Point3 cameraPosition = currentCamera->GetPosition();

    glPushMatrix();

    glColor3f(1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, skyboxTextureInd);
    glDisable(GL_LIGHTING);
    glCullFace(GL_FRONT);
    glTranslatef(cameraPosition.x, cameraPosition.y, cameraPosition.z);
    glRotatef(90, 1, 0, 0);
    gluSphere(skyboxSphere, skyboxRadius, 32, 16);
    glCullFace(GL_BACK);
    glEnable(GL_LIGHTING);

    glPopMatrix();
}


std::chrono::steady_clock::time_point frameStart;
std::chrono::steady_clock::time_point frameEnd;

void RenderScene(void)
{
    frameStart = std::chrono::steady_clock::now();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < 8; i++)
        planetCameras[i]->UpdateTarget(planets[i].GetPointOnOrbit(currentTime));

    glLoadIdentity();
    currentCamera->UseView();
    UpdateLight();

    DrawSkybox();
    Axes();
    DrawSun(currentTime);

    for (Planet planet : planets)
        planet.Draw(currentTime);

    glFlush(); // Przekazanie poleceñ rysuj¹cych do wykonania
    glutSwapBuffers();
    glutPostRedisplay();

    frameEnd = std::chrono::steady_clock::now();
    if(updateTime)
        currentTime += timeScale * 0.001 * std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart).count();
}


void ToggleTimeUpdate(unsigned char key, int x, int y)
{
    if (key == ' ')
        updateTime = !updateTime;
}

void InitializeTexturing()
{
    glEnable(GL_CULL_FACE);    
    glEnable(GL_TEXTURE_2D);
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

void InitializePlanets()
{
    unsigned int textures[8];
    glGenTextures(8, textures);
    planets =
    {
        Planet(0.2, 3, textures[0], Orbit(1.1, 2 * orbitsScale, 5, Point3{ 1,3,2 }, RED)),  //mercury
        Planet(0.3, 7, textures[1], Orbit(1.02, 3 * orbitsScale, 10, Point3{ 6,5,-1 }, GREEN)),    //wenus
        Planet(0.4, 4, textures[2], Orbit(1.04, 5 * orbitsScale, 20, Point3{ 0,-3,0 }, WHITE)),    //ziemia
        Planet(0.5, 5, textures[3], Orbit(1.03, 7 * orbitsScale, 40, Point3{ 4,0,1 }, BLUE)),    //Mars
        Planet(0.6, 9, textures[4], Orbit(1.04, 9 * orbitsScale, 80, Point3{ -5,2,0 }, YELLOW)),    //Jowisz
        Planet(0.7, 4, textures[5], Orbit(1.05, 11 * orbitsScale, 160, Point3{ 1,0,-3 }, VIOLET)),    //Saturn
        Planet(0.8, 10, textures[6], Orbit(1.05, 13 * orbitsScale, 320, Point3{ 0,1,0 }, CYAN)),    //Uran
        Planet(0.9, 11, textures[7], Orbit(1.02, 15 * orbitsScale, 640, Point3{ -3,-2,2 }, ORANGE))    //Neptun
    };

    string filenames[]
    {
        "mercury.tga",
        "venus.tga",
        "earth.tga",
        "mars.tga",
        "jupiter.tga",
        "saturn.tga",
        "uranus.tga",
        "neptune.tga"
    };

    for (int i = 0; i < 8; i++)
        LoadTextureAtInd(filenames[i], textures[i]);
}

void KeysCameras(unsigned char key, int x, int y)
{
    if (key == '0')
        currentCamera = sunCamera;
    else if (key >= '1' && key <= '8')
        currentCamera = planetCameras[key - '0' - 1];
}

void InitializeCameras()
{
    float cameraGap = 0.7;
    sunCamera = new TargetCamera(10, sunRadius + cameraGap);
    planetCameras = new TargetCamera * [8];
    for (int i = 0; i < 8; i++)
        planetCameras[i] = new TargetCamera(5, planets[i].GetRadius() + cameraGap);

    currentCamera = sunCamera;
}

// Funkcja ustalaj¹ca stan renderowania
void MyInit(void)
{
    InitializeLighting();
    InitializeTexturing();

    InitializeSkybox();
    InitializeSun();
    InitializePlanets();
    InitializeCameras();

    MotionCallbacks.push_back(&CameraMotion);
    KeysCallbacks.push_back(&ToggleTimeUpdate);
    KeysCallbacks.push_back(&KeysCameras);
}


// G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli
void main(void)
{
    srand(time(NULL));

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Uk³ad s³oneczny");

    //Przypisanie funkcji zwrotnych
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutKeyboardFunc(keys);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    MyInit();

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutMainLoop();
}

/*************************************************************************************/