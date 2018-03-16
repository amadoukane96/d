#pragma once

#include <SDL2/SDL.h>

#define max(a,b) (a > b) ? a : b
#define min(a,b) (a < b) ? a : b

typedef struct{
	int idCell, isBorder, isEmpty;
} pixel;

//prend une list d'entier de taille n et la shuflle (permutation aleatoire des elements de la liste)
void shuffle(int *array, int n);
//retourne une liste d'entier entre a, et b inclus avec un pas de pas, et la shuffle ensuite
int* sequence(int a, int b, int pas);
//donne un tableau de de point compris dans (0, w) et (0, h) avec un des espacements minimaux
void randomPoints(SDL_Point* tab, int n, int w, int h, int x_spacement, int y_spacement);
//donne une list de n elements de couleurs aleatoire different
void randomColor(SDL_Color* tab, int n);
//shuffle une liste de couleurs
void shufleCouleur(SDL_Color* couleurs, int n);
