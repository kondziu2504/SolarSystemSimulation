#include "Orbit.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include "glm-master/glm/mat4x4.hpp"
#include "glm-master/glm/vec4.hpp"
#include "glm-master/glm/vec3.hpp"
#include "glm-master/glm/gtc/matrix_transform.hpp"
#include <vector>

Orbit::Orbit(float aspectRatio, float scale, float period, Point3 orbitAngles, Point3 orbitColor)
{
	this->aEllipse = scale;
	this->bEllipse = scale / aspectRatio;
	this->period = period;
	this->orbitAngles = orbitAngles;
	this->orbitColor = orbitColor;
}

Orbit::Orbit()
{
    aEllipse = 1;
    bEllipse = 1;
    period = 1;
    orbitAngles = { 0,0,0 };
    orbitColor = { 1,1,1 };
}

//Wyliczanie punktu na orbicie.
//Punkt wyliczany jest dla elipsy dwuwymiarowej,
//przekszta³cany na punkt trójwymiarowy, a nastêpnie
//poddany rotacji, aby odwzorowaæ jego opisane za pomoc¹ k¹tów orbity
//miejsce w przestrzeni
Point3 Orbit::GetPointOnOrbit(float progress)
{
    float c = sqrt(aEllipse * aEllipse - bEllipse * bEllipse);

    glm::vec4 position
    {
        aEllipse * cosf(progress / period * 2 * M_PI) + c,
        0,
        bEllipse * sinf(progress / period * 2 * M_PI),
        0
    };

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, orbitAngles.y, glm::vec3(0, 1, 0));
    model = glm::rotate(model, orbitAngles.z, glm::vec3(0, 0, 1));
    model = glm::rotate(model, orbitAngles.x, glm::vec3(1, 0, 0));

    position = model * position;

    return Point3{ position.x, position.y, position.z };
}

Point3 Orbit::GetOrbitColor()
{
    return orbitColor;
}

float Orbit::GetPeriod()
{
    return period;
}

//Generowanie zbioru punktów na orbicie
std::vector<Point3> Orbit::GetPointsOnOrbit()
{
    std::vector<Point3> orbitPoints;
    int resolution = 100;
    for (int i = 0; i < resolution; i++)
    {
        orbitPoints.push_back(GetPointOnOrbit((float)i / resolution * period));
    }
    return orbitPoints;
}

void Orbit::Draw()
{
    std::vector<Point3> orbitPoints = GetPointsOnOrbit();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    //Obni¿enie przeŸroczystoœci w celu, aby kolory orbit nie dominowa³y na ekranie
    glColor4f(orbitColor.x, orbitColor.y, orbitColor.z, 0.5f);

    glBegin(GL_LINE_LOOP);
    for (Point3& point : orbitPoints)
    {
        glVertex3f(point.x, point.y, point.z);
    }
    glEnd();

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}
