#pragma once
#include "Point.h"
#include <cmath>
#include <vector>
#include "Orbit.h"

class Planet
{
	float radius;
	int textureIndex;	
	Orbit orbit;

public:
	Planet(float radius, int textureIndex, Orbit orbit);
	int GetTexIndex();

	Point3 GetPointOnOrbit(float progress);
	std::vector<Point3> GetPointsOnOrbit();
	float GetRadius();
	Point3 GetOrbitColor();
};

