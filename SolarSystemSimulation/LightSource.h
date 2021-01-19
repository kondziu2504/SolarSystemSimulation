#pragma once
#include "Point.h"

class LightSource
{
	static int lightsCreated;

	Point4 position;
	int lightIndex;

public:
	LightSource(Point4 ambient, Point4 diffuse, Point4 specular);
	void SetPosition(Point4 position);
	Point4p GetPosition();
	int GetIndex();
	void Enable(bool enable);

	LightSource(const LightSource&) = delete;
	LightSource& operator=(const LightSource&) = delete;
};

