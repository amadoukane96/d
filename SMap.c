#include "SMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

SCell* createSCell(int id, int owner, int nbDices, int nbNeighbors){
    // Retourne une cellule initialisée avec les paramètres.
	SCell* cell  = NULL;

	cell = (SCell*) malloc(sizeof(SCell));
	cell->id = id;
	cell->owner = owner;
	cell->nbDices = nbDices;
	cell->nbNeighbors = nbNeighbors;

	cell->neighbors = (SCell**) malloc(cell->nbNeighbors * sizeof(SCell*));

	return cell;
}

int getOwner(SMap* map, int idCell){
    //renvoie l'owner de la cellule
	return map->cells[idCell].owner;
}

SMap* createSMap(){
    // Renvoie une Map vide
	SMap* map = NULL;
	map = (SMap*) malloc(sizeof(SMap));

	map->cells = NULL;
	map->nbCells = 0;
	map->stack = NULL;

	return map;
}

int getDices(SMap* map, int idCell){
    // renvoie le nombre de dès de la cellule d'id idCell
	return map->cells[idCell].nbDices;
}

int valide(SMap* map, int cellFrom, int cellTo){
    // Renvoie vraie si cellFrom peut attaquer cellTo
	return (getOwner(map, cellFrom) != getOwner(map, cellTo) && areNeighbors(&map->cells[cellFrom], &map->cells[cellTo]) != -1 && map->cells[cellFrom].nbDices > 1);
}

void deleteSMap(SMap *map)
{   //Suprimme la map en paramètre
	if(map->cells == NULL){
		map->nbCells = 0;
	}
    // On parcourt les cellules et on libère l'espace mémoire associé.
	for(int i=0; i < map->nbCells; i++)
	{
		free(map->cells[i].neighbors);
		map->cells[i].neighbors = NULL;
	}

	free(map->cells);
	free(map->stack);
	map->cells = NULL;
	map->stack = NULL;

	free(map);
}

SMap* enleverVide(SMap* map, unsigned int nbJouers){

	SMap* cloneMap = NULL;

	int nbPasVide = 0;

	int* nonVides = (int*) malloc(sizeof(int) * map->nbCells);
	int* correspond = (int*) malloc(sizeof(int) * map->nbCells);

	int nbVoisinNonVide = 0;
	//au maximum une cellule à n voisins
	int* voisinsNonVide = (int*) malloc(sizeof(int) * map->nbCells);

	cloneMap = (SMap*) malloc(sizeof(SMap));
	cloneMap->stack = (unsigned int *) malloc(nbJouers * sizeof(unsigned int));
	cloneMap->stack = (unsigned int *) memmove(cloneMap->stack, map->stack, nbJouers * sizeof(unsigned int));

	for(int i = 0; i < map->nbCells; i++){

		correspond[i] = -1;
		if(map->cells[i].owner != -1){

			nonVides[nbPasVide] = i;
			correspond[i] = nbPasVide;

			nbPasVide++;
		}
	}

	cloneMap->nbCells = nbPasVide;
	cloneMap->cells = (SCell*) malloc(nbPasVide * sizeof(SCell));

	//Pour chaque cellule de la map à copier
	for(int i=0; i< cloneMap->nbCells; i++)
	{
		//On les informations dans la cellule clone
		cloneMap->cells[i].id = i;
		cloneMap->cells[i].owner = map->cells[nonVides[i]].owner;
		cloneMap->cells[i].nbDices = map->cells[nonVides[i]].nbDices;

		nbVoisinNonVide = 0;

		for(int j = 0; j < map->cells[nonVides[i]].nbNeighbors; j++){
			if(map->cells[nonVides[i]].neighbors[j]->owner != -1){
				voisinsNonVide[nbVoisinNonVide] = correspond[map->cells[nonVides[i]].neighbors[j]->id];
				nbVoisinNonVide++;
			}
		}

		cloneMap->cells[i].nbNeighbors = nbVoisinNonVide;
		cloneMap->cells[i].neighbors = (SCell**) malloc(nbVoisinNonVide * sizeof(SCell*));

		for(int j = 0; j < cloneMap->cells[i].nbNeighbors; j++){
			cloneMap->cells[i].neighbors[j] = &cloneMap->cells[voisinsNonVide[j]];
		}
	}

	free(nonVides);
	free(voisinsNonVide);
	free(correspond);
	
	return cloneMap;
}

SMap* clonerSMap(SMap* map, unsigned int nbJouers)
{   //renvoie une copie de la map passée en paramètre
		SMap* cloneMap = NULL;

		cloneMap = (SMap*) malloc(sizeof(SMap));
		cloneMap->stack = (unsigned int *) malloc(nbJouers * sizeof(unsigned int));
		cloneMap->stack = (unsigned int *) memmove(cloneMap->stack, map->stack, nbJouers * sizeof(unsigned int));

		cloneMap->nbCells = map->nbCells;
		cloneMap->cells = (SCell*) malloc(cloneMap->nbCells * sizeof(SCell));

		//Pour chaque cellule de la map à copier
		for(int i=0; i< cloneMap->nbCells; i++)
		{
			//On copie les informations dans la cellule clone
			cloneMap->cells[i].id = map->cells[i].id;
			cloneMap->cells[i].owner = map->cells[i].owner;
			cloneMap->cells[i].nbDices = map->cells[i].nbDices;
			cloneMap->cells[i].nbNeighbors = map->cells[i].nbNeighbors;
			cloneMap->cells[i].neighbors = (SCell**) malloc(cloneMap->cells[i].nbNeighbors * sizeof(SCell*));

			for(int j = 0; j < cloneMap->cells[i].nbNeighbors; j++){
				cloneMap->cells[i].neighbors[j] = &cloneMap->cells[map->cells[i].neighbors[j]->id];
			}
		}

		return cloneMap;
}

int areNeighbors(SCell* A, SCell* B){
    // renvoie vraie si les cellules A et B sont voisines
	int i = B->nbNeighbors - 1;

	while(i >= 0 && (A->id != B->neighbors[i]->id)){
		i--;
	}

	return i;

}
int inScreen(int x, int y, int w, int h){
    //renvoie vraie si le point (x,y) est continu dans la fenetre de paramètre(w,h)
	return (x >= 0 && x <= w) && (y >= 0 && y <= h);
}
void afficher(const SMap* map){
    // affiche les informations pour chaque cellule de map
	printf("-------------affichage de la map---------------\n");
	for(int i = 0; i < map->nbCells; i++){
		for(int j = 0; j < map->cells[i].nbNeighbors; j++){
		
		}
		
	}
}

//Retourne le nombre de dés à donner au joueur à la fin de son tour
//Calcul les tailles de toutes les composantes connexes appartenant au joueur et retourne la plus grande
int endTurnDices(SMap* map, int idPlayer) {

	int* tab = (int*)malloc(map->nbCells * sizeof(int));
	int increment = 1; //numero de la composante connexe actuelle
	int count;
	int max = 0;
	int i;
	//On commence par initialiser la table avec 0 pour les cases du joueur passé en paramètre et 100 pour les autres
	for(i=0; i<map->nbCells; i++) {
		if(map->cells[i].owner == idPlayer){
			tab[i] = 0;
		}
		else {
			tab[i]= 100;
		}
	}
	//Pour chaque cellule qui n'a pas encore été assignée à un joueur
	for(i = 0; i < map->nbCells; i++) {
		if(map->cells[i].owner != -1){
			if (tab[i] != 100) {
				if (tab[i] == 0) { 
					count = num(i, increment, map, tab, 1);// on calcule la taille de la composante connexe de chaque cellule appartenant à idPlayer
					if (count > max) { // Si cette taille est superieur à celle sauvegardée
						max = count; // on met à jour la taille max des composantes connexe
					}
					increment++;
				}
			}
		}
	}

	free(tab);
	return max;
}

//Attribue un numero donné (numZone) à toutes les cellules de la composante connexe à laquelle appartient la cellule dont l'id est donné en paramètre
//Retourne la taille de la composant connexe à laquelle idCell appartient
int num(int idCell, int numZone, SMap* map, int* tab, int prevCount) {
	int neighbor;
	int count = prevCount; //variable contenant le nombre de cellules dans la composante connexe
	int j;

	tab[map->cells[idCell].id] = numZone;
	for (j=0; j<map->cells[idCell].nbNeighbors; j++) {
		neighbor = tab[map->cells[idCell].neighbors[j]->id];
		if (neighbor != 100) {   //Si ce n'est pas la cellule d'un autre joueur
			if(neighbor == 0 ){   //Si la cellule n'a pas encore été numérotée
				count ++;
				count = num(map->cells[idCell].neighbors[j]->id, numZone, map, tab, count);
			}
		}
	}

	return count;
}

//Distribue les cellules aléatoirement entre les joueurs de telle sorte qu'il aient tous le même nombre de cellule plus ou moins 1
//Cette fonction initialise aussi le nombre de dés de la cellule à 0
void distributeCells(SMap* map, int nbPlayers, int nbCellNonVide){

	int* ordre_remplissage = sequence(0, map->nbCells - 1, 1); // on calcule un ordre de distribution aléatoire aux joueurs

	int i = 0;
	int owner = 0;
	int remplit = 0;
	int nbCellParPlyr = floor(nbCellNonVide/nbPlayers);

	do{
		if(map->cells[ordre_remplissage[i]].owner != -1){
			map->cells[ordre_remplissage[i]].owner = owner;
			remplit++;
			if(remplit % nbCellParPlyr == 0){
				owner++;
			}
		}

		i++;
	}

	while(i < map->nbCells && owner < nbPlayers);

	owner = 0;

	for(int j = i; j < map->nbCells;j++){
		if(map->cells[ordre_remplissage[j]].owner != -1){
			map->cells[ordre_remplissage[j]].owner = owner;
			owner++;
		}
	}

	free(ordre_remplissage);
}


int distributeDices(SMap* map, int idPlayer, int nbDes, int mode)
{

    /*fonction distribuant nbDes aux cases appartenant à idPlayer */
    /* mode permet de savoir si nous distribuons au debut(0) ou apres un tour(1) */

	int *tab = (int*) malloc( map->nbCells * sizeof(int) );/*on declare un tableau pour sauvegarder les indices des cellules de idPlayer */
	int n=0; /*nb de cellules correspondant à idPlayer*/
	int i;
	int cell_full=0;
	int idCell = 0;
	int indice_aleatoire = 0;
	int add = 0;
	int* ordre_remplissage = NULL;
	int nbDesJouer = 0;

	/* on commence par chercher les cellules dont l'owner est idPlayer */
	for( i=0 ; i<map->nbCells ; i++ ) /* Pour ca on parcourt toutes les cellules de la map */
	{
		if (map->cells[i].owner == idPlayer) /* si le numero owner correspond*/
		{
			tab[n] = i; /* on sauvegarde l'indice de la cellule dans le tableau qu'on a declaré*/
			n++; /* on met à jour le nombre de cellules appartenant à idPlayer */
			nbDesJouer += map->cells[i].nbDices;
		}
	}
	/* Ensuite on passe à la repartition des dès dans les cellules trouvées */

	/*Si  c'est pour l'initialisation de la map on met un dès dans chaque cellule de idPlayer pour commencer */
	if((int) nbDesJouer < n * 8){

		if (mode == 0)/* mais on verifie dabord qu'on a assez de dès */
		{
			idCell = 0;
			for( i=0 ; i<n ; i++ )
			{
				idCell = tab[i]; /* on recupere graces aux indices sauvegardés la cellule appartenant à idPlayer*/
				map->cells[idCell].nbDices++;
			}

			nbDes -= n; /* on actualise le nombre de dès total restant */
	
			//map->stack[idPlayer] += n;
			nbDesJouer += n;
		}

		/* ensuite on repartie alèatoirement les dès*/

		while ( nbDes > 0 && (int) nbDesJouer < n * 8)  /* tant qu'il reste des dès à distribuer */
		{

			ordre_remplissage = sequence(0, n - 1, 1);
			cell_full = 0;

			while(nbDes > 0 && cell_full < n){

				/*on choisit une cellule au hasard*/
		
				indice_aleatoire = ordre_remplissage[cell_full];

				idCell = tab[indice_aleatoire];/* indice de la cellule dans le tableau de cellule de la Map */
			

				add = rand() % (min(nbDes+1, 9)); /* on calcule aleatoirement par rapport au nombre de dès restant le nombre de dès à ajouter dans la cellule */


				if(map->cells[idCell].nbDices < 8){
					if(add + map->cells[idCell].nbDices > 8){
						add = 8 - map->cells[idCell].nbDices;
					}

					map->cells[idCell].nbDices += add; /* on ajoute le nombre calculé de dès à la cellule */
					nbDesJouer += add;
					nbDes -= add ; /* on actualise le nombre de dès restant à distribuer */

				}
				cell_full++;
			}

			free(ordre_remplissage);
			ordre_remplissage = NULL;

		}



		for( i=0 ; i<n ; i++ )
		{
			idCell = tab[i]; //on recupere graces aux indices sauvegardés la cellule appartenant à idPlayer
		}

		free(tab); /* on libère l'espace alloué au tableau de sauvegarde */
	}

	return nbDes;
}


//Attaquer la cellule d'id idTo avec une cellule d'id idFrom
void coup(SMap *map, int idFrom, int idTo, int* results )
{
	int i;
	int p1Score = 0; //score de la case qui attaque
	int p2Score = 0; //score de la case qui est attaquée

	SCell* cellFrom = &(map->cells[idFrom]);
	SCell* cellTo = &(map->cells[idTo]);

	//Si l'attaque est possible (nombre de dés correct et cases voisines)

	//Pour les deux cellules concernées on simule le lancer des dés
	for(i=0; i<cellFrom->nbDices; i++)
	{
		p1Score += rand() % 6 + 1;
	}
	for(i=0; i<cellTo->nbDices; i++)
	{
		p2Score += rand() % 6 + 1;
	}

	results[0] = p1Score;
	results[1] = p2Score;

}

void arrayToMap(pixel** screen, int w, int h, int n, SMap* map){


	int** voisins = (int**) malloc(sizeof(int*) * n);
	int tmp_x = 0;
	int tmp_y = 0;
	int index1 = 0;
	int index2 = 0;

	for(int i = 0; i < n; i++){
		voisins[i] = (int*) malloc(sizeof(int) * (n + 1));
		voisins[i][n] = 0;
	}


	for(int x = 0; x <= w; x++){
		for(int y = 0; y <= h; y++){
			index1 = screen[x][y].idCell;

			for(int i = -1; i <= 1; i++){
				for(int j = -1; j <= 1; j++){

					if(i != 0 || j != 0){

						tmp_x = x + i;
						tmp_y = y + j;

						if(inScreen(tmp_x, tmp_y, w, h)){

							index2 = screen[tmp_x][tmp_y].idCell;
							if(screen[x][y].idCell != screen[tmp_x][tmp_y].idCell){

								screen[x][y].isBorder = 1;
								if(indexOf_int(voisins[index1], voisins[index1][n], index2) == -1){

									voisins[index1][voisins[index1][n]] = index2;
									voisins[index1][n]++;
								}
							}
						}
						else{
							screen[x][y].isBorder = 1;
						}
					}
				}
			}
		}
	}
	map->nbCells = n;
	map->cells = (SCell*) malloc(sizeof(SCell) * map->nbCells);

	for(int i = 0; i < n; i++){
		map->cells[i].id = i;
		map->cells[i].owner = 0;
		map->cells[i].nbNeighbors = voisins[i][n];
		map->cells[i].neighbors = (SCell**) malloc(sizeof(SCell*) * map->cells[i].nbNeighbors);
		map->cells[i].nbDices = 0;

		for(int j = 0; j < voisins[i][n]; j++){
			map->cells[i].neighbors[j] = &map->cells[voisins[i][j]];
		}
		free(voisins[i]);
	}

	free(voisins);
	voisins = NULL;
}

int ajouterVide(SMap* map){

		int ok = 0;
		int n = map->nbCells;
		int nbVide = n/3;
		int *v = (int*) malloc(sizeof(int) * n);

		//-------------------ajouter le vide--------------------------//
		int nbArt = getCutVertex(map, v);

		int occ = 0;
		int j = 0;
		int stop = 0;
		int cpt = 0;

		while(cpt < nbVide && !stop){
			ok = (rand()%((n) - nbArt)) + 1;
			j = 0;
			occ = 0;

			while(j < map->nbCells && occ < ok){
				if(map->cells[j].owner != -1){
					if(v[j] == 0){
						occ++;
					}
					if(occ < ok)
					{
						j++;
					}
				}
				else{
					j++;
				}
			}
			if(occ <= ok && j < map->nbCells){
				map->cells[j].owner = -1;
				cpt++;
				n--;
				nbArt = getCutVertex(map, v);

			}
			else{
				stop = 1;
			}
		}
		free(v);
		return cpt;
}

int indexOf_int(int* tab, int n, int val){
	int i = n - 1;
	while(i >= 0 && tab[i] != val){
		i--;
	}
	return i;
}

int getCutVertex(SMap* map, int* art){
	int *num = NULL;
	int *low = NULL;
	int *parents = NULL;
	int nbArt = 0;
	int root = 0;

	num = (int*) malloc(sizeof(int) * map->nbCells);
	low = (int*) malloc(sizeof(int) * map->nbCells);
	parents = (int*) malloc(sizeof(int) * map->nbCells);

	for(int i = 0; i < map->nbCells; i++){
		num[i] = -1;
		low[i] = -1;
		art[i] = 0;
		parents[i] = -1;
		if(map->cells[i].owner != -1){
			root = i;
		}
	}
	nbArt = findCutVertex(&map->cells[root], num, 1, low, parents, art);

	art[root] = 1;
	nbArt++;
	free(num);
	free(low);
	free(parents);
	return nbArt;
}
//inspiré fortement de http://www.eecs.wsu.edu/~holder/courses/CptS223/spr08/slides/graphapps.pdf

int findCutVertex(SCell* cell, int* num, int cpt, int* low, int* parents, int* art){

	int totale = 0;

	int w = 0;
	int v = cell->id;

	if(cell->owner != -1){
		num[v] = cpt;
		low[v] = cpt;

		cpt++;

		for(int i = 0; i < cell->nbNeighbors; i++){

			w = cell->neighbors[i]->id;
			if(cell->neighbors[i]->owner != -1){

				if(num[w] == -1){
					parents[w] = v;
					totale += findCutVertex(cell->neighbors[i], num, cpt, low, parents, art);
					low[v] = min(low[v], low[w]);

					if(low[w] >= num[v] && num[v] != 1){
						if(art != NULL){
							art[v] = 1;
						}
					
						totale++;
					}
				}
				else{
					if(parents[v] != w){
						low[v] = min(low[v], num[w]);
					}
				}
			}
		}
	}


	return totale;
}
