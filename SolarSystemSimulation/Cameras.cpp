#include "Cameras.h"
#include "MathHelper.h"
#include <cmath>

TargetCamera::TargetCamera(Point3 position, float radius)
{
	this->position = position;

	this->radius = radius;

	azimuth = 0;
	elevation = 0;
}

Point3 TargetCamera::GetPosition()
{
	Point3 position
	{
		radius * cosf((azimuth + 90.f) / 360.0f * 2.f * M_PI) * cosf(elevation / 360.0f * 2.f * M_PI),
		radius * sinf(elevation / 360.0 * 2 * M_PI),
		radius * sinf((azimuth + 90.f) / 360.0f * 2.f * M_PI) * cosf(elevation / 360.0f * 2.f * M_PI)
	};
	return position;
}


void TargetCamera::AppendAzimuth(float deltaAzimuth)
{
	azimuth += deltaAzimuth;
}

void TargetCamera::AppendElevation(float deltaElevation)
{
	elevation += deltaElevation;
	if (elevation >= 90)
		elevation = 90 - 0.01;
	else if (elevation <= -90)
		elevation = -90 + 0.01;
}

void TargetCamera::AppendRadius(float deltaRadius)
{
	radius += deltaRadius;
	if (radius < 0)
		radius = 0;
}
