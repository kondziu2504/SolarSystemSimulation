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
#include "PngReading.h"
#include <chrono>
#include "ViewParameters.h"

using namespace std;

const Point3 RED{ 1, 0, 0 };
const Point3 GREEN{ 0, 1, 0 };
const Point3 BLUE{ 0, 0, 1 };
const Point3 YELLOW{ 1, 1, 0 };
const Point3 VIOLET{ 0.5, 0, 1 };
const Point3 ORANGE{ 1, 0.5, 0 };
const Point3 WHITE{ 1, 1, 1 };
const Point3 CYAN{ 0, 1, 1 };

const string TEXTURES_DIRECTORY = "Tekstury";
const LPCWSTR AUDIO_PATH = L"Muzyka/spaceAmbient.wav";

//Parametry do modyfikacji
const float TIME_SCALE = 0.5;
const float ORBITS_SCALE = 5;
const float PLANETS_SCALE = 0.1;
const float SUN_RADIUS = 8;
const float SKYBOX_RADIUS = 500;
const float SUN_ROTATION_PERIOD = 150;
//---------------------------


vector<Planet> planets;

//Zmienne do przechowywania informacji na temat skybox'a - gwiazdy w tle
unsigned int skyboxTextureInd;
GLUquadric* skyboxSphere;

//ród³o œwiat³a w centrum w ramach symulowania s³oñca
LightSource* light1;
unsigned int sunTextureInd;
GLUquadric* sunSphere;

//Aktualny czas symulacji
float currentTime;
//Czy aktualizowaæ czas symulacji
bool updateTime = true;

bool audioPlaying = false;

TargetCamera* currentCamera;
TargetCamera* sunCamera;
TargetCamera** planetCameras;


const string INSTRUCTION =
"Klawisze 1-8 przelaczaja kamere miedzy planetami w kolejnosci od Merkurego\n"
"Klawisz 0 przelacza kamere na slonce\n"
"Poruszaj mysza z wcisnietym LPM, aby zmieniac pozycje kamery wokol obiektu\n"
"Poruszaj mysza z wcisnietym PPM, aby przyblizac/oddalac kamere\n"
"Nacisnij spacje, aby wstrzymac/wznowic symulacje\n"
"Nacisnij 'm', aby wlaczyc/wylaczyc muzyke\n";


//// Funkcja rysuj¹ca osie uk³adu wspó³rzêdnych
void Axes(void)
{
    float axisLength = SKYBOX_RADIUS;

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_LINES);

    //Oœ X - czerwona
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-axisLength, 0, 0);
    glVertex3f(axisLength, 0, 0);

    //Oœ Y - zielona
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0, -axisLength, 0);
    glVertex3f(0, axisLength, 0);

    //Oœ Z - niebieska
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0, 0, -axisLength);
    glVertex3f(0, 0, axisLength);

    glEnd();

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}

//Aktualizowania pozycji œwiat³a wzglêdem aktualnej macierzy widoku
void UpdateLight()
{
    Point4p lightPos = light1->GetPosition();
    glLightfv(light1->GetIndex(), GL_POSITION, lightPos);
    delete lightPos;
}

//£adowanie tekstury z pliku do danego slotu
void LoadTextureAtInd(string filename, int textureIndex)
{
    GLint ImWidth, ImHeight, ImComponents;
    GLenum ImFormat;
    GLbyte* pBytes = (GLbyte*)readPng(filename.c_str(), ImWidth, ImHeight, ImComponents, ImFormat);
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
    LoadTextureAtInd(TEXTURES_DIRECTORY + "/" + "sun.png", sunTextureInd);

    sunSphere = gluNewQuadric();
    
    gluQuadricDrawStyle(sunSphere, GLU_FILL);
    gluQuadricTexture(sunSphere, GL_TRUE);
    gluQuadricNormals(sunSphere, GLU_SMOOTH);
}

void InitializeSkybox()
{
    glGenTextures(1, &skyboxTextureInd);
    LoadTextureAtInd(TEXTURES_DIRECTORY + "/" + "stars.png", skyboxTextureInd);

    skyboxSphere = gluNewQuadric();

    //Rysowanie wnêtrza, bo kula skybox'a widoczna od œrodka
    gluQuadricOrientation(skyboxSphere, GLU_INSIDE);
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
    glRotatef(time / SUN_ROTATION_PERIOD * 360, 0, 1, 0);
    gluSphere(sunSphere, SUN_RADIUS, 32, 32);
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
    
    glTranslatef(cameraPosition.x, cameraPosition.y, cameraPosition.z);
    glRotatef(90, 1, 0, 0);
    gluSphere(skyboxSphere, SKYBOX_RADIUS, 16, 16);
    
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

//Korekcja informacji na temat ruchu myszy.
//Z powodu braku funkcji zwrotnej dla myszy, gdy jest w spoczynku,
//po jej zatrzymaniu zapamiêtywana by³a informacja o prêdkoœci ostatniego ruchu.
//Ta funkcja stopniowo redukuje zapamiêtan¹ prêdkoœæ, aby zasymulowaæ zatrzymanie.
void MouseMotionCorrection()
{
    if (delta_x >= -8 && delta_x <= 8)
        delta_x /= 2;
    if (delta_y >= -8 && delta_y <= 8)
        delta_y /= 2;
}



std::chrono::steady_clock::time_point lastFrame = std::chrono::steady_clock::now();

float deltaTime = 0.01;
float unscaledDeltaTime = 0.01;

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Aktualizacja pozycji kamery
    currentCamera->Update(unscaledDeltaTime);

    //Aktualizacja celów kamer - ka¿da z kamer patrzy na jedn¹ z planet
    for (int i = 0; i < planets.size(); i++)
        planetCameras[i]->UpdateTarget(planets[i].GetPointOnOrbit(currentTime));

    glLoadIdentity();
    //Aktualizacja macierzy widoku na podstawie obecnie wybranej kamery
    currentCamera->UseView();
    UpdateLight();

    DrawSkybox();
    //Axes();
    DrawSun(currentTime);

    for (Planet planet : planets)
        planet.Draw(currentTime);

    MouseMotionCorrection();

    glFlush();
    glutSwapBuffers();
    //Zkolejkowanie kolejnego wyœwietlenia sceny, aby zachowaæ aktualnoœæ symulacji na ekranie
    glutPostRedisplay();

    if (updateTime)
    {
        unscaledDeltaTime = 0.001 * std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastFrame).count();
        deltaTime = TIME_SCALE * unscaledDeltaTime;
        currentTime += deltaTime;
    }

    lastFrame = std::chrono::steady_clock::now();
}



//Funkcja zwrotna do wstrzymania/wznowienia symulacji po nacisnieciu klawisza
void KeysToggleTimeUpdate(unsigned char key, int x, int y)
{
    if (key == ' ')
        updateTime = !updateTime;
}

//Funkcja zwrotna do wyboru kamery za pomoc¹ klawiszy
void KeysCameras(unsigned char key, int x, int y)
{
    if (key == '0')
        currentCamera = sunCamera;
    else if (key >= '1' && key <= '9')
    {
        int index = key - '0' - 1;
        if(index < planets.size())
            currentCamera = planetCameras[key - '0' - 1];
    }
}

//Funkcja zwrotna do w³¹czania/wy³¹czania muzyki za pomoc¹ klawisza
void KeysToggleAudio(unsigned char key, int x, int y)
{
    if (key == 'm')
    {
        if (audioPlaying)
        {
            PlaySound(NULL, NULL, SND_ASYNC);
        }
        else
            PlaySound(AUDIO_PATH, NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);

        audioPlaying = !audioPlaying;
    }    
}

void InitializeTexturing()
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
}

void InitializeLighting()
{
    glShadeModel(GL_SMOOTH); // w³aczenie ³agodnego cieniowania
    glEnable(GL_LIGHTING);   // w³aczenie systemu oœwietlenia sceny

    float ambient = 3.f;
    float diffuse = 10.f;
    float specular = 5.f;

    //Ustawienie œwiat³a bia³ego dla ¿ród³a œwiat³a w s³oñcu
    light1 = new LightSource(
        Point4{ ambient, ambient, ambient, 1.0 },
        Point4{ diffuse, diffuse, diffuse, 1.0 },
        Point4{ specular, specular, specular, 1.0 });

    light1->Enable(true);
    //Œwiat³o w centrum uk³adu, poniewa¿ na tej pozycji znajduje siê model s³oñca
    light1->SetPosition(Point4{ 0,0,0,1 });
}

void InitializePlanets()
{
    const int planetsCount = 8;

    unsigned int textureIndices[planetsCount];
    //Wygenerowanie indeksów, na których przechowywane bêd¹ tekstury
    glGenTextures(planetsCount, textureIndices);

    string textureFilenames[planetsCount]
    {
        TEXTURES_DIRECTORY + "/" + "mercury.png",
        TEXTURES_DIRECTORY + "/" + "venus.png",
        TEXTURES_DIRECTORY + "/" + "earth.png",
        TEXTURES_DIRECTORY + "/" + "mars.png",
        TEXTURES_DIRECTORY + "/" + "jupiter.png",
        TEXTURES_DIRECTORY + "/" + "saturn.png",
        TEXTURES_DIRECTORY + "/" + "uranus.png",
        TEXTURES_DIRECTORY + "/" + "neptune.png"
    };

    //Za³adowanie tekstur
    for (int i = 0; i < planetsCount; i++)
    {
        try
        {
            LoadTextureAtInd(textureFilenames[i], textureIndices[i]);
        }
        catch (exception e)
        {
            cout << e.what() << endl;
        }
    }

    planets =
    {
        Planet(2.4 * PLANETS_SCALE, 3, textureIndices[0], Orbit(1.08, 9 * ORBITS_SCALE, 5, Point3{ 1, 3, 2 }, RED)),  //mercury
        Planet(6.1 * PLANETS_SCALE, 7, textureIndices[1], Orbit(1.01, 12 * ORBITS_SCALE, 10, Point3{ 6, 5, -1 }, GREEN)),    //wenus
        Planet(6.4 * PLANETS_SCALE, 4, textureIndices[2], Orbit(1.03, 15 * ORBITS_SCALE, 20, Point3{ 0, -3, 0 }, WHITE)),    //ziemia
        Planet(3.4 * PLANETS_SCALE, 5, textureIndices[3], Orbit(1.02, 18 * ORBITS_SCALE, 40, Point3{ 4, 0, 1 }, BLUE)),    //Mars
        Planet(69.9/3 * PLANETS_SCALE, 9, textureIndices[4], Orbit(1.03, 21 * ORBITS_SCALE, 80, Point3{ -5, 2, 0 }, YELLOW)),    //Jowisz
        Planet(58.2/3 * PLANETS_SCALE, 4, textureIndices[5], Orbit(1.04, 24 * ORBITS_SCALE, 160, Point3{ 5, 0, -3 }, VIOLET)),    //Saturn
        Planet(25.4/3 * PLANETS_SCALE, 10, textureIndices[6], Orbit(1.04, 27 * ORBITS_SCALE, 320, Point3{ 0, 20, 3 }, CYAN)),    //Uran
        Planet(24.6/3 * PLANETS_SCALE, 11, textureIndices[7], Orbit(1.02, 30 * ORBITS_SCALE, 640, Point3{ -3, -40, 2 }, ORANGE))    //Neptun
    };
}

//Utworzenie kamery dla ka¿dej planety
void InitializeCameras()
{
    float cameraGap = 0.7;
    sunCamera = new TargetCamera(15, SUN_RADIUS + cameraGap);
    planetCameras = new TargetCamera * [planets.size()];
    for (int i = 0; i < planets.size(); i++)
        planetCameras[i] = new TargetCamera(planets[i].GetRadius() + 5, planets[i].GetRadius() + cameraGap);

    currentCamera = sunCamera;
}

void MyInitialize(void)
{
    zFar = SKYBOX_RADIUS;

    InitializeLighting();
    InitializeTexturing();

    InitializeSkybox();
    InitializeSun();
    InitializePlanets();
    InitializeCameras();

    KeysCallbacks.push_back(&KeysToggleTimeUpdate);
    KeysCallbacks.push_back(&KeysCameras);
    KeysCallbacks.push_back(&KeysToggleAudio);
}


void InitializeWindowsSizeAndPos()
{
    HWND consoleWindow = GetConsoleWindow();
    HWND desktopWindow = GetDesktopWindow();

    RECT desktopRect;
    GetWindowRect(desktopWindow, &desktopRect);
    float screenWidth = desktopRect.right;
    float screenHeight = desktopRect.bottom;

    RECT windowRect = { screenWidth / 2 + 10, 0, screenWidth, screenHeight / 2 };
    MoveWindow(consoleWindow, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, TRUE);


    glutInitWindowSize(screenWidth / 2, screenHeight / 2);
}

void EnableLineAntialiasing()
{
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli
void main(void)
{
    srand(time(NULL));

    cout << INSTRUCTION << endl;

    InitializeWindowsSizeAndPos();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("Uk³ad s³oneczny");

    //Przypisanie funkcji zwrotnych
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutKeyboardFunc(keys);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutPassiveMotionFunc(PassiveMotion);
    glutIdleFunc(Idle);

    MyInitialize();

    glEnable(GL_DEPTH_TEST);

    EnableLineAntialiasing();

    glutMainLoop();
}

/*************************************************************************************/