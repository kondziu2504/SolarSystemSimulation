#include "Planet.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include "Orbit.h"
using namespace std;

Planet::Planet(float radius, int textureIndex, Orbit orbit)
{
    this->textureIndex = textureIndex;
    this->radius = radius;
    this->orbit = orbit;
}

int Planet::GetTexIndex()
{
    return textureIndex;
}


Point3 Planet::GetPointOnOrbit(float progress)
{
    return orbit.GetPointOnOrbit(progress);
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

