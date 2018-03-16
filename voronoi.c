#include "voronoi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float maxmin(int ox, int oy, int x, int y){
	return max(max(max(abs(ox), abs(oy)), abs(x)), abs(y));
}

float generale(int ox, int oy, int x, int y, int p){
	return pow(pow(abs(ox-x), p) + pow(abs(oy-y), p), 1.0/p);
}

int voronoi(pixel** ecran, SDL_Point* germes, int nbpoints, int w, int h){

	int x_spacement = 1;
	int y_spacement = 1;
	int max_x = floor(w/x_spacement);
	int max_y = floor(h/y_spacement);
	int p = 1;
	float (*norme)(int, int, int, int, int) = generale;
	float dist = 0;
	int min_i = 0;
	nbpoints = min(max_x * max_y, nbpoints);
	randomPoints(germes, nbpoints, w, h, x_spacement, y_spacement);
	for(int i = 0; i < nbpoints; i++){
		printf("(%d, %d) ", germes[i].x, germes[i].y);
	}
	for(int x = 0; x <= w; x++){
		for(int y = 0; y <= h; y++){
			dist = norme(0, 0, w, h, p);

			for(int i = 0; i < nbpoints; i++){

				if(norme(x, y, germes[i].x, germes[i].y, p) < dist){
					dist = norme(x, y, germes[i].x, germes[i].y, p);
					min_i = i;
				}
			}
			ecran[x][y].idCell = min_i;
			//on en profite pour initialiser l'autre champs du pixel
			ecran[x][y].isBorder = 0;
		}
	}
	return nbpoints;
}
