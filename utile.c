#include "utile.h"
#include "time.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void shufleCouleur(SDL_Color* couleurs, int n) {
	int* rand_i = sequence(0, n - 1, 1);
	SDL_Color tmp;

	for(int i = 0; i < n; i++){

		tmp.r = couleurs[i].r;
		tmp.g = couleurs[i].g;
		tmp.b = couleurs[i].b;

		couleurs[i].r = couleurs[rand_i[i]].r;
		couleurs[i].g = couleurs[rand_i[i]].g;
		couleurs[i].b = couleurs[rand_i[i]].b;
		couleurs[rand_i[i]].r = tmp.r;
		couleurs[rand_i[i]].g = tmp.g;
		couleurs[rand_i[i]].b = tmp.b;
	}
}

void randomColor(SDL_Color* tab, int n) {
	int* r = sequence(0, 255, n);
	int* g = sequence(0, 255, n);
	int* b = sequence(0, 255, n);
	int* r_i = sequence(0, n - 1, 1);
	int* g_i = sequence(0, n - 1, 1);
	int* b_i = sequence(0, n - 1, 1);

	SDL_Color tmp;
	int i = 0;

	while(i < n){
		tmp.r = r[r_i[i]];
		tmp.g = g[g_i[i]];
		tmp.b = b[b_i[i]];
		tab[i] = tmp;
		i++;
	}

	free(r);
	free(g);
	free(b);
	free(r_i);
	free(g_i);
	free(b_i);
}

void shuffle(int *array, int n) {
	int i;
	int j;
	int t;
    if(n > 1)
    {
        for (i = 0; i < n - 1; i++)
        {
          j = i + rand() / (RAND_MAX / (n - i) + 1);
          t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

int* sequence(int a, int b, int pas){
	int size = ((b-a+1)/pas);
	int* tab = (int*) malloc(sizeof(int) * size);
	int i = a;
	int len = 0;
	while(len < size){
		tab[len] = i;
		len++;
		i += pas;
		if(i > b){
			i = a;
		}
	}
	shuffle(tab, size);
	return tab;
}

void randomPoints(SDL_Point* tab, int n, int w, int h, int x_spacement, int y_spacement){
	int* points_x = sequence(0, w, x_spacement);
	int* points_y = sequence(0, h, y_spacement);
	int* x = sequence(0, n, 1);
	int* y = sequence(0, n, 1);
	int i = 0;
	while(i < n){
		tab[i].x = points_x[x[i]];
		tab[i].y = points_y[y[i]];
		i++;
	}

	free(points_x);
	free(points_y);
	free(x);
	free(y);
}
