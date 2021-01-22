#include "Cameras.h"
#include "MathHelper.h"
#include <cmath>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>

TargetCamera::TargetCamera(float radius, float minRadius)
{
	this->radius = radius;
	this->minRadius = minRadius;
	target = Point3{ 0,0,0 };

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

	position.x += target.x;
	position.y += target.y;
	position.z += target.z;

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
	if (radius < minRadius)
		radius = minRadius;
}

void TargetCamera::UpdateTarget(Point3 target)
{
	this->target = target;
}

void TargetCamera::UseView()
{
	Point3 position = GetPosition();

	gluLookAt(
		position.x, position.y, position.z,
		target.x, target.y, target.z,
		0, 1, 0
	);
}
