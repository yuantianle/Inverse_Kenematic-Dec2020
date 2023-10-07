#pragma once
#include <cstdio>
#include <ctime>
#include <iostream>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#pragma warning(disable:4996)
#include "glew.h"
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "glut.h"
#include "glm/gtc/matrix_transform.hpp"

#define  OBJDELIMS		" \t"

struct bmfh
{
	short bfType;
	int bfSize;
	short bfReserved1;
	short bfReserved2;
	int bfOffBits;
};

struct bmih
{
	int biSize;
	int biWidth;
	int biHeight;
	short biPlanes;
	short biBitCount;
	int biCompression;
	int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	int biClrUsed;
	int biClrImportant;
};

struct Vertex
{
	float x, y, z;
};

struct Normal
{
	float nx, ny, nz;
};

struct TextureCoord
{
	float s, t, p;
};

struct face
{
	int v, n, t;
};

unsigned char* BmpToTexture(char* filename, int* width, int* height);
void	Cross(float[3], float[3], float[3]);
float	Unit(float[3]);
float	Unit(float[3], float[3]);
char*	ReadRestOfLine(FILE*);
void	ReadObjVTN(char*, int*, int*, int*);
int		ReadInt(FILE*);
short	ReadShort(FILE*);
int		LoadObjFile(char* name);

