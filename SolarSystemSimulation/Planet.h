#pragma once
#include "Point.h"
#include <cmath>
#include <vector>
#include "Orbit.h"
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>

class Planet
{
	float radius;
	float rotationPeriod;
	int textureIndex;	
	Orbit orbit;
	GLUquadric * sphere;
	float timeOffset;


public:
	Planet(float radius, float rotationPeriod, int textureIndex, Orbit orbit);
	int GetTexIndex();

	Point3 GetPointOnOrbit(float progress);
	float GetRadius();
	Point3 GetOrbitColor();
	void Draw(float progress);
};

