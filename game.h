#pragma once

#include "utile.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "SMap.h"

typedef void (*f_InitGame)(unsigned int, unsigned int, SPlayerInfo*);
typedef void (*f_EndGame)(unsigned int, unsigned int);
typedef int (*f_PlayTurn)(unsigned int, const SMap*, STurn*);


typedef struct{
  SMap* map;
  pixel** ecran;
  SDL_Point* germes;
  SDL_Renderer* render;
  SDL_Color couleurs[8];
  SDL_Surface** diceImages;
  FILE* logs;

  int* isSelected;
  int* ordrePlayers;
  unsigned int* desPlayers;
  int nbTerritoire;
  int isFinished;
  int turn;
  int nbPlayers;
  int w;
  int h;

} SGame;

//Crée la structure SGame
//Dans cette fonction on charge toutes les images et on stocke les pointeur dans un tableau qui sera un attribut de la structure SGame
SGame* createGame(SDL_Renderer* rend);
void testFinished(SGame* game);
//Initialise la structure SGame avec les éléments passés en paramètre
void setGame(SGame* g, int nbPlayers, int nbTerritoire, int w, int h);

void changerOrdreJoueurs(SGame *game);

void resetGame(SGame* g);

//Supprime tous les champs de la SGame passée en paramètre
void destroyGame(SGame* game);

//ajoute des creux dans le jeu
void genererVide(SGame* g);

//Génère un map aléatoire et initialise ses champs en tenant compte du nombre de joueurs et de cellules
void makeMap(SGame* game);

//centre les germes (ils sont place au centre de gravite de leur territoire)
void centrerGermes(SGame* game);

//Met à 1 la valeur correspondant à la cellule donnée en paramètre dans le tableau  isSelected
void selectCell(SGame* game, unsigned int idCell);
//Met à 1 la valeur correspondant à la zone donnée en paramètre dans le tableau isSelected
void selectZone(SGame* game, SDL_Point* zone);

//Met à 0 la valeur correspondant à la zone donné en paramètre dans le tableau isSelected
void deselectZone(SGame* game, SDL_Point* zone);
//Met à 0 la valeur correspondant à la cellule donné en paramètre dans le tableau isSelected
void deselectCell(SGame* game, unsigned int idCell);

//Défini les actions à réaliser pour les click dans la zone de la map
int gererClick(SGame* g, SDL_Point** firstClick, SDL_Point* click);
int gererIA(SGame* g, f_PlayTurn ia_play);

//Met à jour le nombre de dés dans les case après une attaque
int moveDices(SGame* g, int idFrom, int idTo, int* results);

//Passe le tour et met à jour le champ turn de la structure SGame game
void nextTurn(SGame* game);

//termine le tour du joueurs et distribue au joeurs sont revenue de des
void PlayerEndTurn(SGame* game, TTF_Font* font);

//Dessine la map est tenant compte de la situation(cellule sélectionné, nombre de dés...)
//Appelle aussi la fonction de dessin de la zone des stacks
void draw(SGame* g);
//Dessine les dés de la cellule dont l'id est passée en paramètre
void drawDices(SGame *game, SDL_Point center, SCell* cell, int w,int h);
//Fonction graphique qui affiche aux endroits définit les images du tour actuel et celle du boutton pour passer son tour
void endTurnButton(SGame* g);
void updateTurnImage(SGame* game, TTF_Font* font);
//Fonction graphique qui remplit les carrés des lancés de dés avec les couleurs correspondant à la situation actuelle
void setDiceSquare(SGame* g);

//Fonctions graphique qui lance l'animation des dés et afficher de manière dynamique le résultat de chaque joueur
void rollDices(SGame* game, int attackCell, int defCell, int* results);
//Fonctions de dessin des dés et de carré
void fillSquares(SGame* game, SDL_Color player1Color, SDL_Color player2Color, SDL_Rect* rect12, SDL_Rect* rect22);
void shakeDices(SGame* game, SDL_Texture* texture, SDL_Color player1Color, SDL_Color player2Color);

//Dessine la zone graphique servant à afficher les dés des stack
void drawStack(SGame* game, int nbPlayers, TTF_Font* font);
//Met à jour sur l'interface le nombre de dé dans chaque pile
void updateStack(SGame* game, int nbPlayers);
