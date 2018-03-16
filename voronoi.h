#pragma once
#include "utile.h"
#include <SDL2/SDL.h>
#include <stdio.h>

#define max(a,b) (a > b) ? a : b
#define min(a,b) (a < b) ? a : b

float maxmin(int ox, int oy, int x, int y);
float generale(int ox, int oy, int x, int y, int p);

int voronoi(pixel** ecran, SDL_Point* germes, int nbpoints, int w, int h);
