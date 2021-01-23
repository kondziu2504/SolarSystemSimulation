#pragma once
#include "Point.h"

class TargetCamera
{
	float radius;
	float minRadius;
	float elevation, azimuth;
	Point3 target;
	Point3 localVelocity;
	
public:
	TargetCamera(float radius, float minRadius);
	Point3 GetPosition();
	void AppendAzimuth(float deltaAzimuth);
	void AppendElevation(float deltaElevation);
	void AppendRadius(float deltaRadius);
	void UpdateTarget(Point3 target);
	void UseView();
	void Update(float deltaTime);
};

