#pragma once
#include "Point.h"

class TargetCamera
{
	Point3 position;
	float radius;
	float elevation, azimuth;

public:
	TargetCamera(Point3 position, float radius);
	Point3 GetPosition();
	void AppendAzimuth(float deltaAzimuth);
	void AppendElevation(float deltaElevation);
	void AppendRadius(float deltaRadius);
};

