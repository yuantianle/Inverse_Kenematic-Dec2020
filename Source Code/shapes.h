#pragma once
#include <cstdio>
#include <ctime>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#pragma warning(disable:4996)
#include "glew.h"
#endif

#include <GL/gl.h>
#include <GL/glu.h>

void	DrawPoint(struct point* p);
void	MjbSphere(float radius, int slices, int stacks);

struct point
{
	float x, y, z;		// coordinates
	float nx, ny, nz;	// surface normal
	float s, t;		// texture coords
};
