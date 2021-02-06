#include "LightSource.h"
#include <iostream>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>

int LightSource::lightsCreated = 0;

LightSource::LightSource(Point4 ambient, Point4 diffuse, Point4 specular)
{
	lightIndex = lightsCreated;
	lightsCreated++;

	glLightfv(GL_LIGHT0 + lightIndex, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0 + lightIndex, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0 + lightIndex, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0 + lightIndex, GL_POSITION, Point4{ 0.0, 0.0, 0.0, 1.0 });

	GLfloat att_constant = { 0.0 };
	// sk³adowa sta³a ds dla modelu zmian oœwietlenia w funkcji
	// odleg³oœci od Ÿród³a

	GLfloat att_linear = { 0.0 };
	// sk³adowa liniowa dl dla modelu zmian oœwietlenia w funkcji
	// odleg³oœci od Ÿród³a

	GLfloat att_quadratic = { 0.001 };
	// sk³adowa kwadratowa dq dla modelu zmian oœwietlenia w funkcji
	// odleg³oœci od Ÿród³a

	glLightf(GL_LIGHT0 + lightIndex, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0 + lightIndex, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0 + lightIndex, GL_QUADRATIC_ATTENUATION, att_quadratic);

	glEnable(GL_LIGHT0 + lightIndex);
}

void LightSource::SetPosition(Point4 position)
{
	for (int i = 0; i < 4; i++)
		this->position[i] = position[i];
}

Point4p LightSource::GetPosition()
{
	Point4p position = new Point4
	{
		this->position[0], 
		this->position[1], 
		this->position[2], 
		this->position[3]
	};

	return position;
}

int LightSource::GetIndex()
{
	return GL_LIGHT0 + lightIndex;
}

void LightSource::Enable(bool enable)
{
	if (enable)
		glEnable(GL_LIGHT0 + lightIndex);
	else
		glDisable(GL_LIGHT0 + lightIndex);
}
