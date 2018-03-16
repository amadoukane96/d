/*
 * SMap.h: fichier définissant plusieurs méthode manupilation des structures SMap (graphe).
 * Voire le fichier interface.h pour la définision de cette structure
 */

#pragma once

#include "interface.h"
#include "utile.h"

//n'hésitez pas à ajouter une fonctions qui vous semble importante.

//cree un SCell vide
SCell* createSCell(int id, int owner, int nbDices, int nbNeighbors);
//cree un map vide.
SMap* createSMap();
//duplique une map, et retourne un pointer vers le clone.
SMap* clonerSMap(SMap* map, unsigned int nbJouers);

SMap* enleverVide(SMap* map, unsigned int nbJouers);

//ajoute le sommet cell à la map
void addSCell(SCell* cell, SMap* map);
//suprimme un sommet d'une map
void deleteSCell(int idCell, SMap* map);
//suprimme une map
void deleteSMap(SMap* map);
//renvoie l'owner de la cellule d'indice idCell
int getOwner(SMap* map, int idCell);
//renvoie le nombre de dès de la cellule d'indice idCell
int getDices(SMap* map, int idCell);
// renvoie vrai si le coup de la cellule d'indice cellFrom vers celle d'indice cellTo est valide
int valide(SMap* map, int cellFrom, int cellTo);
//retourne vrai ssi a et b sont des sommets adjacents
int areNeighbors(SCell* A, SCell* B);
//repartie de maniere aleatoire nbDes dés aux territoires du jouer idPlayer
int distributeDices(SMap* map, int idPlayer, int nbDes, int mode);
//genere une map aleatoire
SMap* randomSMap(unsigned int nbCells, unsigned int nbJouers, unsigned int des);
//lance les des et mets les resultats dans results
void coup(SMap *map, int idFrom, int idTo, int* results);
//renvoie vraie si (x,y) est dans l'ecran de parametre(w,h)
int inScreen(int x, int y, int w, int h);

int indexOf_int(int* tab, int n, int val);
//renvoie la taille de la plus grande composante connexe de idPlayer 
int endTurnDices(SMap* map, int idPlayer);
// affiche la map
void afficher(const SMap* map);

int findMapRoot(SMap* map);
//renvoie la taille de la composante à laquelle appartient cell d'indice idCell
int num(int idCell, int numZone, SMap* map, int* tab, int prevCount);
void arrayToMap(pixel** screen, int w, int h, int n, SMap* map);
void distributeCells(SMap* map, int nbPlayers, int nbCellNonVide);
int findRoot(SCell* cell, int* num);
int getCutVertex(SMap* map, int* art);
//renvoie l'id de d'un sommet(cellule) d'articulation
int findCutVertex(SCell* cell, int* num, int cpt, int* low, int* parents, int* art);
//ajoute un vide dans la map
int ajouterVide(SMap* map);
