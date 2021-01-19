#pragma once
#include "Point.h"
#include <vector>

class Orbit
{
	float aEllipse, bEllipse;
	float period;
	Point3 orbitAngles;
	Point3 orbitColor;

public:
	Orbit(float aspectRatio, float scale, float period, Point3 orbitAngles, Point3 orbitColor);
	Orbit();
	Point3 GetPointOnOrbit(float progress);
	Point3 GetOrbitColor();
	float GetPeriod();
	std::vector<Point3> GetPointsOnOrbit();
};

