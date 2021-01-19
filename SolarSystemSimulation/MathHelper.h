#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

float ToRadians(float angle)
{
	return angle / 180. * M_PI;
}