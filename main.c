#include "interface.h"
#include "game.h"
#include "utile.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <dlfcn.h>

int main(int argv, char* argc[])
{
	srand(time(NULL));

	int w = 800; //largeur de la map
	int h = 600; //hauteur de la map
	int wWindow = 800; //largeur de la fenêtre
	int hWindow = 750; //hauteur de la fenêtre

	if(argv < 3){
		exit(EXIT_FAILURE);
	}

	int nbPlayers = atoi(argc[2]);
	int nbPartieRestante = atoi(argc[1]);
	int nbIA = argv - 3;
	int nbHumains = nbPlayers - nbIA;
	int played = 0;

	void **libs = NULL;
	f_InitGame *initGames = NULL;
	f_EndGame *endGames = NULL;
	f_PlayTurn *playTurns = NULL;
	SPlayerInfo *infos = NULL;

	libs = (void**) malloc(sizeof(void*) * nbIA);
	initGames = (f_InitGame*) malloc(sizeof(f_InitGame) * nbIA);
	endGames = (f_EndGame*) malloc(sizeof(f_EndGame) * nbIA);
	playTurns = (f_PlayTurn*) malloc(sizeof(f_PlayTurn) * nbIA);
	infos = (SPlayerInfo*) malloc(sizeof(SPlayerInfo) * nbIA);

	for(int i = 0; i < nbIA; i++){
		libs[i] = dlopen(argc[3 + i], RTLD_LAZY);
		initGames[i] = (f_InitGame) dlsym(libs[i], "InitGame");
		endGames[i] = (f_EndGame) dlsym(libs[i], "EndGame");
		playTurns[i] = (f_PlayTurn) dlsym(libs[i], "PlayTurn");
	}
	SDL_Window* screen = NULL;
	SDL_Renderer* render = NULL;
	SDL_Event e;
	TTF_Font * font = NULL;
	SGame* game = NULL;
	SDL_Point *firstClick, *click;

	int endTurn = 0;
	int stop;
	int nbCells = 52;
	int gameStarted = 0;

	if(SDL_Init(SDL_INIT_VIDEO) == 0 && TTF_Init() != -1){
		font = TTF_OpenFont("arial.ttf", 15);
		screen = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, wWindow, hWindow, SDL_WINDOW_SHOWN);;
		render = SDL_CreateRenderer(screen, -1, 0);
		SDL_SetRenderDrawColor( render, 217, 207, 217, 255 );
    	SDL_RenderClear( render );
		stop = 0;


		//Boucle principale du jeu
		while(nbPartieRestante > 0 && !stop){
			
			game = createGame(render);
			setGame(game, nbPlayers, nbCells, w, h);
			makeMap(game);
			
			for(int i = 0; i < nbIA; i++){
				initGames[i](nbHumains + i, nbPlayers, &infos[i]);
			}

			//On appelle les différentes fonctions graphiques qui afficher les éléments de l'interface
			endTurnButton(game); //Affiche le bouton pour passer son tour
			updateTurnImage(game,font); //Colorie la case du tour avec la couleur du premier joueur
			drawStack(game, nbPlayers, font); //Dessine la zone des piles
			setDiceSquare(game); //Dessine la zone 
			draw(game); //Dessine la map

			endTurn = 0; //variable binaire déterminant le passage d'un tour à un autre
			firstClick = NULL; //Pointeur vers les coordonnées du clic qui s'est fait avant le dernier clic
			click = NULL; //Pointeur vers les coordonnées du clic qui vient de se faire
			gameStarted = 0;
			played = 0; //variable binaire valant 1 quand les deux derniers click permettent de jouer un coup et 0 sinon
			
			//Boucle principale d'une partie
			while(!game->isFinished){
				SDL_PollEvent(&e);

				switch (e.type) {
					case SDL_QUIT:
						printf("bouton  X clicker, fermeture de la fenetre\n");
						game->isFinished = 1;
						stop = 1;
					break;

					case SDL_MOUSEBUTTONDOWN:
						if(game->ordrePlayers[game->turn] < nbHumains){
							// si clic sur le bouton de fin de tour
							if(e.button.y > 630 && e.button.y <= 630+120 && e.button.x > 680 && e.button.x <= 660+120){
									endTurn = 1;
							}
							if(!endTurn){
								if(click == NULL){
									click = (SDL_Point*) malloc(sizeof(SDL_Point));
								}
								click->x = e.button.x;
								click->y = e.button.y;

								played = gererClick(game, &firstClick, click);
								if(played) {
									SDL_Delay(850);
									setDiceSquare(game);
								}
								gameStarted = 1;
								draw(game);
							}

						}
					break;

					case SDL_KEYDOWN:
						//On peut aussi terminer la partie en appuyant sur la touche entrée
						if(game->ordrePlayers[game->turn] < nbHumains){
							if(e.key.keysym.sym == SDLK_RETURN){
								endTurn = 1;
							}
						}
						if(e.key.keysym.sym == SDLK_m && !gameStarted && nbHumains > 0){
							game = createGame(render);
							setGame(game, nbPlayers, nbCells, w, h);
							makeMap(game);
						}
					break;
					}

					//Si c'est au tour de l'IA
					if(game->ordrePlayers[game->turn] >= nbHumains && !stop){
							endTurn = gererIA(game, playTurns[game->turn - nbHumains]);
					}

					if(endTurn && !stop){
						testFinished(game);
						deselectZone(game, click);
						deselectZone(game, firstClick);
						PlayerEndTurn(game, font);
						endTurn = 0;
					}

				}
				nbPartieRestante--;
				for(int i = 0; i < nbIA; i++){
					endGames[i](game->ordrePlayers[game->turn], game->ordrePlayers[game->turn]);
				}
				if(!game->isFinished){
					destroyGame(game);
					changerOrdreJoueurs(game);
				}
				printf("endGames fini\n");
			}
	}
	printf("lets destoy\n");
	destroyGame(game);
	for(int i = 0; i < nbIA; i++){
		dlclose(libs[i]);
	}
	free(libs);
	free(initGames);
	free(endGames);
	free(playTurns);
	free(infos);
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
	SDL_DestroyWindow(screen);
	SDL_DestroyRenderer(render);
	exit(EXIT_SUCCESS);
}
