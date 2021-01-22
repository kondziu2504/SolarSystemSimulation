#include "Cameras.h"
#include "MathHelper.h"
#include <cmath>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include "OpenGLCallbackFunctions.h"
#include "Point.h"

const float CAMERA_MOVE_SENSITIVITY = 1000;
const float CAMERA_ZOOM_SENSITIVITY = 30;

TargetCamera::TargetCamera(float radius, float minRadius)
{
	this->radius = radius;
	this->minRadius = minRadius;
	
	localVelocity = Point3{ 0,0,0 };
	cameraDamping = 10;
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

void TargetCamera::Update(float deltaTime)
{
	AppendAzimuth(localVelocity.x * deltaTime);
	AppendElevation(localVelocity.y * deltaTime);
	AppendRadius(-localVelocity.z * deltaTime);

	if (status == 1) {
		localVelocity.x = delta_x * deltaTime * CAMERA_MOVE_SENSITIVITY;
		localVelocity.y = delta_y * deltaTime * CAMERA_MOVE_SENSITIVITY;
	}
	else if (status == 2)
	{
		localVelocity.z += delta_y * deltaTime * CAMERA_ZOOM_SENSITIVITY;
	}

	localVelocity.x *= (1 / (1 + cameraDamping * deltaTime));
	localVelocity.y *= (1 / (1 + cameraDamping * deltaTime));
	localVelocity.z *= (1 / (1 + cameraDamping * deltaTime));
}
