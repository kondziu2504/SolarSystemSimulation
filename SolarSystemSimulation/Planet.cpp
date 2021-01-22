#include "Planet.h"
#define _USE_MATH_DEFINES
#include <iostream>
#include <math.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>

#include "Orbit.h"
using namespace std;

Planet::Planet(float radius, float rotationPeriod, int textureIndex, Orbit orbit)
{
    this->textureIndex = textureIndex;
    this->rotationPeriod = rotationPeriod;
    this->radius = radius;
    this->orbit = orbit;

    timeOffset = rand();

    GLuint ind;
    glGenTextures(1, &ind);

    sphere = gluNewQuadric();
}

int Planet::GetTexIndex()
{
    return textureIndex;
}


Point3 Planet::GetPointOnOrbit(float progress)
{
    return orbit.GetPointOnOrbit(progress + timeOffset);
}

std::vector<Point3> Planet::GetPointsOnOrbit()
{
    return orbit.GetPointsOnOrbit();
}

float Planet::GetRadius()
{
    return radius;
}

Point3 Planet::GetOrbitColor()
{
    return orbit.GetOrbitColor();
}

void Planet::Draw(float time)
{
    glPushMatrix();
    glPushMatrix();

    Point3 position = GetPointOnOrbit(time);

    float rotationProgress = time;
    while (rotationProgress >= rotationPeriod)
        rotationProgress -= rotationPeriod;
    rotationProgress /= rotationPeriod;

    glTranslatef(position.x, position.y, position.z);
    glRotatef(rotationProgress * 360, 0, 1, 0);
    glRotatef(90, 1, 0, 0);

    glBindTexture(GL_TEXTURE_2D, textureIndex);
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, radius, 32, 16);

    glPopMatrix();

    orbit.Draw();    

    glPopMatrix();
}

