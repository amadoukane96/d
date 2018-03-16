#include "game.h"
#include "voronoi.h"
#include <stdio.h>
#include <stdlib.h>
//La structure SGame est déclarée dans le fichier game.h

SGame* createGame(SDL_Renderer* rend){
	char nbDes[30]; //variable stockant le nom du fichier image à importer
	SGame* g = NULL;
	g = (SGame*) malloc(sizeof(SGame)); //allocation de l'espace mémoire pour stocker notre (struct) SGame
	g->logs = fopen("logs.txt", "w");

	//On déclare les différents pointeurs
	g->map = createSMap(); //On crée une map
	g->ecran = NULL;
	g->germes = NULL;
	g->isSelected = NULL;
	g->render = rend;
	g->desPlayers = NULL;
	//initialisation les entiers à 0
	g->nbPlayers = 0;
	g->isFinished = 0;
	g->w = 0;
	g->h = 0;
	g->turn = 0;

	g->diceImages = (SDL_Surface**) malloc(sizeof(SDL_Surface*)*15); //Pour les 15 images qu'on utilise dans le jeu

	//Stockage des pointeurs vers les images des dés dans le tableau diceImages pour ne les charger qu'une fois
	//L'indice du pointeur dans le tableau correspond au nombre de dés
	for(int n = 1; n <= 8; n++) {
		snprintf(nbDes, 29 * sizeof(char), "images/%d.png",n);
		g->diceImages[n] = IMG_Load(nbDes);
		printf("nb des : %s\n", nbDes);

	}
	//On stock donc les images restantes
	g->diceImages[0] = IMG_Load("images/crown.png");
	g->diceImages[9] = IMG_Load("images/att.png");
	g->diceImages[10] = IMG_Load("images/att2.png");
	g->diceImages[11] = IMG_Load("images/def.png");
	g->diceImages[12] = IMG_Load("images/def2.png");
	g->diceImages[13] = IMG_Load("images/end.png");
	g->diceImages[14] = IMG_Load("images/turnbox.png");

	//On définit les couleurs qu'on associera au différents joueurs, elles sont bien distinctes
	g->couleurs[0].r = 255; g->couleurs[0].g = 255; g->couleurs[0].b = 0;
	g->couleurs[1].r = 0; g->couleurs[1].g = 204; g->couleurs[1].b = 0;
	g->couleurs[2].r = 0; g->couleurs[2].g = 102; g->couleurs[2].b = 255;
	g->couleurs[3].r = 255; g->couleurs[3].g = 150; g->couleurs[3].b = 150;
	g->couleurs[4].r = 153; g->couleurs[4].g = 51; g->couleurs[4].b = 153;
	g->couleurs[5].r = 255; g->couleurs[5].g = 153; g->couleurs[5].b = 0;
	g->couleurs[6].r = 255; g->couleurs[6].g = 0; g->couleurs[6].b = 255;
	g->couleurs[7].r = 102; g->couleurs[7].g = 51; g->couleurs[7].b = 0;

	return g;
}
//test si le jeu est terminée c-a-d, tous le jouers ont 0 des sauf un.
void testFinished(SGame* game){
	int i = 0;
	int cpt = 0;
	int idWinner = 0;

	while(i < game->nbPlayers && cpt < 2){
		if(game->desPlayers[i] == 0){
			idWinner = i;
		}
		else{
			cpt++;
		}
		i++;
	}

	game->isFinished = (cpt == 1);
	if(game->isFinished){
		fprintf(game->logs, "%d WON\n", idWinner);
	}
}
//initialise un game
void setGame(SGame* g, int nbPlayers ,int nbTerritoire, int w, int h){
	g->map->nbCells = nbTerritoire;
	g->nbPlayers = nbPlayers;
	g->nbTerritoire = nbTerritoire;
	g->w = w;
	g->h = h;
	g->turn = 0;
	g->isFinished = 0;
	g->ecran = (pixel**) malloc(sizeof(pixel*) * (g->w+1));
	g->germes = (SDL_Point*) malloc(sizeof(SDL_Point) * g->map->nbCells);
	g->isSelected = (int*) malloc(sizeof(int) * g->map->nbCells);

	for(int j = 0; j < g->map->nbCells; j++){
		g->isSelected[j] = 0;
	}

	for(int i = 0; i <= w; i++){
		g->ecran[i] = (pixel*) malloc(sizeof(pixel) * (g->h+1));
	}

	for(int x = 0; x <= g->w; x++){
		for(int y = 0; y <= g->h; y++){
			g->ecran[x][y].isBorder = 0;
		}
	}

	g->map->stack = (unsigned int*) malloc(sizeof(int) * g->nbPlayers);
	g->desPlayers = (unsigned int*) malloc(sizeof(int) * g->nbPlayers);
	g->ordrePlayers = (int*) malloc(sizeof(int) * g->nbPlayers);

	for(int k = 0; k < g->nbPlayers; k++){
		g->map->stack[k] = 0;
		g->desPlayers[k] = 0;
		g->ordrePlayers[k] = k;
	}

	shufleCouleur(g->couleurs, 8);
}
//change l'ordre dans le quel les jouers jouennt.
void changerOrdreJoueurs(SGame* g){
	SDL_Color tmp;
	fprintf(g->logs, "nouveau ordre de jeu : ");
	shuffle(g->ordrePlayers, g->nbPlayers);

	for(int i = 0; i < g->nbPlayers; i++){

		fprintf(g->logs, "%d ", g->ordrePlayers[i]);

		tmp.r = g->couleurs[i].r;
		tmp.g = g->couleurs[i].g;
		tmp.b = g->couleurs[i].b;

		g->couleurs[i].r = g->couleurs[g->ordrePlayers[i]].r;
		g->couleurs[i].g = g->couleurs[g->ordrePlayers[i]].g;
		g->couleurs[i].b = g->couleurs[g->ordrePlayers[i]].b;
		g->couleurs[g->ordrePlayers[i]].r = tmp.r;
		g->couleurs[g->ordrePlayers[i]].g = tmp.g;
		g->couleurs[g->ordrePlayers[i]].b = tmp.b;
	}
	fprintf(g->logs, "\n");
}
//
void resetGame(SGame* g){
	
	int nbCells = g->map->nbCells;
	deleteSMap(g->map);
	g->map = createSMap();
	g->map->nbCells = g->nbTerritoire;
	g->turn = 0;
	g->isFinished = 0;
	g->map->stack = (unsigned int*) malloc(sizeof(int) * g->nbPlayers);

	for(int k = 0; k < g->nbPlayers; k++){
		g->map->stack[k] = 0;
		g->desPlayers[k] = 0;
	}

	for(int j = 0; j < nbCells; j++){
		g->isSelected[j] = 0;
	}

	changerOrdreJoueurs(g);
}

//Supprime tous les champs de la SGame passée en paramètre
void destroyGame(SGame* g){
	deleteSMap(g->map);
	for(int i = 0; i <= g->w; i++){
		free(g->ecran[i]);
	}

	free(g->ecran);
	for(int i = 0; i < 15; i++){
		SDL_FreeSurface(g->diceImages[i]);
	}

	//SDL_DestroyRenderer(g->render);
	free(g->diceImages);
	free(g->desPlayers);
	free(g->isSelected);
	free(g->germes);
	free(g->ordrePlayers);
	fclose(g->logs);

	g->map = NULL;
	g->ecran = NULL;
	g->diceImages = NULL;
	//g->render = NULL;
	g->desPlayers = NULL;
	g->isSelected = NULL;
	g->germes = NULL;
	g->ordrePlayers = NULL;

	free(g);
}
//ajoute des creux dans le jeu
void genererVide(SGame* g){

		SMap* mapPropre = NULL;
		SDL_Point* germesPropre = NULL;
		int j = 0;
		int nbCellvide = ajouterVide(g->map);

		int nbCellNonVide = g->map->nbCells - nbCellvide;
		int* correspond = (int*) malloc(sizeof(int) * g->map->nbCells);
		germesPropre = (SDL_Point*) malloc(sizeof(SDL_Point) * (nbCellNonVide));
		free(g->isSelected);
		g->isSelected = (int*) malloc(sizeof(int) * nbCellNonVide);

 		for(int i = 0; i < g->map->nbCells; i++){
			if(g->map->cells[i].owner != -1){
				correspond[i] = j;
				g->isSelected[j] = 0;
				germesPropre[j].x = g->germes[i].x;
				germesPropre[j].y = g->germes[i].y;
				j++;
			}
		}

		for(int x = 0; x <= g->w; x++){
			for(int y = 0; y <= g->h; y++){

				if(g->map->cells[g->ecran[x][y].idCell].owner == -1){
					g->ecran[x][y].isEmpty = 1;
				}
				else{
					g->ecran[x][y].idCell = correspond[g->ecran[x][y].idCell];
					g->ecran[x][y].isEmpty = 0;
				}
			}
		}

		free(g->germes);
		g->germes = germesPropre;
		mapPropre = enleverVide(g->map, g->nbPlayers);
		afficher(g->map);
		deleteSMap(g->map);
		g->map = mapPropre;
		g->map->nbCells = nbCellNonVide;
}

//Génère un map aléatoire et initialise ses champs en tenant compte du nombre de joueurs et de cellules
void makeMap(SGame* g){
	
	int nbCellvide = 0;
	g->map->nbCells = voronoi(g->ecran, g->germes, g->map->nbCells, g->w, g->h);
	arrayToMap(g->ecran, g->w,  g->h, g->map->nbCells, g->map);
	centrerGermes(g);
	genererVide(g); //Crée des cellules vide dans la map
	distributeCells(g->map, g->nbPlayers, g->map->nbCells - nbCellvide); //Distribue les cellules aléatoirement
	for(int i = 0; i < g->nbPlayers; i++){
		g->desPlayers[i] = ((g->map->nbCells/g->nbPlayers) * 4);
		distributeDices(g->map, i, g->desPlayers[i], 0); //Distribue les dés aléatoirement
	}

	afficher(g->map);
}
//centre les germes (ils sont place au centre de gravite de leur territoire)
void centrerGermes(SGame* game){
	int nbCells = game->map->nbCells;

	SDL_Point* centreGravites = (SDL_Point*) malloc(sizeof(SDL_Point) * nbCells);
	int* nbPoints = (int*) malloc(sizeof(int) * nbCells);

	for(int i = 0; i < nbCells; i++){
		nbPoints[i] = 0;
		centreGravites[i].x = 0;
		centreGravites[i].y = 0;
	}

	for(int x = 0; x <= game->w; x++){
		for(int y = 0; y <= game->h; y++){
			if(game->ecran[x][y].isBorder){

				//printf("%d\n", game->ecran[x][y].idCell);
				centreGravites[game->ecran[x][y].idCell].x += x;
				centreGravites[game->ecran[x][y].idCell].y += y;
				nbPoints[game->ecran[x][y].idCell]++;
			}

		}
	}

	for(int i = 0; i < nbCells; i++){
		centreGravites[i].x /= nbPoints[i];
		centreGravites[i].y /= nbPoints[i];

		game->germes[i].x = centreGravites[i].x;
		game->germes[i].y = centreGravites[i].y;
	}
	
	free(centreGravites);
	free(nbPoints);
}

//Met à 1 la valeur correspondant à la zone donnée en paramètre dans le tableau isSelected
void selectZone(SGame* game, SDL_Point* zone){
	if(zone != NULL){
		//On retrouve l'indice dans le tableau isSelected grâce au tableau double dimensions ecran qui stocke l'id de la cellule pour chaque point
		game->isSelected[game->ecran[zone->x][zone->y].idCell] = 1;
	}
}
//Met à 1 la valeur correspondant à la cellule donnée en paramètre dans le tableau  isSelected
void selectCell(SGame* game, unsigned int idCell){
	game->isSelected[idCell] = 1;
}

//Met à 0 la valeur correspondant à la zone donné en paramètre dans le tableau isSelected
void deselectZone(SGame* game, SDL_Point* zone){
	if(zone != NULL){
		//On retrouve l'indice dans le tableau isSelected grâce au tableau double dimensions ecran qui stocke l'id de la cellule pour chaque point
		game->isSelected[game->ecran[zone->x][zone->y].idCell] = 0;
	}
}

//Met à 0 la valeur correspondant à la cellule donné en paramètre dans le tableau isSelected
void deselectCell(SGame* game, unsigned int idCell){
	game->isSelected[idCell] = 0;
}

//Défini les actions à réaliser pour les clic dans la zone de la map
int gererClick(SGame* g, SDL_Point** firstClick, SDL_Point* click){

	int played = 0;
	int owner_frClicked = -1;
	int owner_clicked;

	int idCell_frClicked = -1;
	int idCell_clicked;

	int* results = (int*) malloc(2*sizeof(int));

	//Si une case a bien été sélectionné avant
	if(*firstClick != NULL){
		idCell_frClicked = g->ecran[(*firstClick)->x][(*firstClick)->y].idCell;
		owner_frClicked = getOwner(g->map, idCell_frClicked);
	}

	//Avec les coordonnées du clic on récupère la cellule et le joueur à qui elle appartient
	idCell_clicked = g->ecran[click->x][click->y].idCell; 
	owner_clicked = getOwner(g->map, idCell_clicked);

	//S'il y a plus d'un dé dans la case sélectionnée et que c'est une case du joueur qui doit jouer
	if(g->ordrePlayers[g->turn] == owner_clicked && g->map->cells[idCell_clicked].nbDices > 1){

			deselectZone(g, *firstClick); //On déselectionne l'ancienne cellule sélectionnée
			selectZone(g, click); //On sélectionne la cellule cliqué

			if(*firstClick == NULL){
				*firstClick = (SDL_Point*) malloc(sizeof(SDL_Point));
			}

			(*firstClick)->x = click->x;
			(*firstClick)->y = click->y;
	}

	else{
		//Si le joueur a déja sélectionné une de ces cellules
		if(*firstClick != NULL){
			//Si c'est une case ennemi ET adjacente sur laquelle le joueur a cliqué
			if(owner_clicked != owner_frClicked && areNeighbors(&g->map->cells[idCell_clicked], &g->map->cells[idCell_frClicked]) != -1){
				selectZone(g, click);

				coup(g->map, idCell_frClicked, idCell_clicked, results); //On calcule le résultat du lancé des dés
				rollDices(g, idCell_frClicked, idCell_clicked, results); //On lance l'animation
				moveDices(g, idCell_frClicked, idCell_clicked, results); //On applique les déplacement de dés qui ont eu lieu
				//Desélection des deux zone
				deselectZone(g, click);
				deselectZone(g, *firstClick);

				free(*firstClick);
				*firstClick = NULL;
				played = 1;

			}
		}
	}
	
	free(results);
	return played;
}

int gererIA(SGame* game, f_PlayTurn ia_play){

	STurn coups;
	SMap* clone = NULL;
	int continu = 0;
	int resRollDices[2];
	clone = clonerSMap(game->map, game->nbPlayers);
	//clone = game->map;
	continu = ia_play(game->ordrePlayers[game->turn], clone, &coups);
	if(continu && valide(game->map, coups.cellFrom, coups.cellTo) && getOwner(game->map, coups.cellFrom) == game->ordrePlayers[game->turn]){

		selectCell(game, coups.cellFrom);
		draw(game);
		selectCell(game, coups.cellTo);
		draw(game);
		coup(game->map, coups.cellFrom, coups.cellTo, resRollDices);
		moveDices(game, coups.cellFrom, coups.cellTo, resRollDices);
		deselectCell(game, coups.cellFrom);
		draw(game);
		deselectCell(game, coups.cellTo);
		draw(game);
	}
	deleteSMap(clone);
	return !continu;
}

//Met à jour le nombre de dés dans les cases après une attaque
int moveDices(SGame* g, int idFrom, int idTo, int* results) {
	int won = 0;
	SMap* map = g->map;

	SCell* cellFrom = &(map->cells[idFrom]);
	SCell* cellTo = &(map->cells[idTo]);
	
	int idFromOwner = cellFrom->owner;
	int idToOwner = cellTo->owner;

	//Si l'attaquant gagne
	if(results[0] > results[1])
	{
		//L'attaquant deplace tous ces dés sauf 1 sur la case conquise
		//La case attaquée appartient desormais à l'attaquant
		g->desPlayers[cellTo->owner] -= cellTo->nbDices;
		cellTo->nbDices = cellFrom->nbDices - 1;
		cellTo->owner = cellFrom->owner;
		won = 1;
	}
	else{
		g->desPlayers[cellFrom->owner] -= (cellFrom->nbDices - 1);
	}
	cellFrom->nbDices = 1;

	fprintf(g->logs, "%d %d %d %d %d\n", idFromOwner, idFrom, idToOwner, idTo, won);

	return won;
}

//Passe le tour et met à jour le champ turn de la structure SGame game
void nextTurn(SGame* g){
	int i = g->turn;
	do
	{
		i++;
		if(i >= g->nbPlayers){
			i = 0;
		}
		else{
		}
	} while(i != g->turn && g->desPlayers[i] == 0);

	if(i == g->turn){
		g->isFinished = 1;
	}
	g->turn = i;
}
//termine le tour du joueurs et distribue au joeurs sont revenue de des
void PlayerEndTurn(SGame* game, TTF_Font* font){
	int nbDesRestant = 0;
	int income = endTurnDices(game->map, game->ordrePlayers[game->turn]);
	income += game->map->stack[game->ordrePlayers[game->turn]];
	nbDesRestant = distributeDices(game->map, game->ordrePlayers[game->turn], income, 1);
	game->desPlayers[game->ordrePlayers[game->turn]] += (income - nbDesRestant);
	game->map->stack[game->ordrePlayers[game->turn]] = nbDesRestant;
	game->map->stack[game->ordrePlayers[game->turn]] = min(40, game->map->stack[game->ordrePlayers[game->turn]]);
	nextTurn(game);
	updateTurnImage(game, font);
	draw(game);
}

//Dessine la map est tenant compte de la situation(cellule sélectionné, nombre de dés...)
//Appelle aussi la fonction de dessin de la zone des stacks
void draw(SGame* g){
	int w = g->w;
	int h = g->h;

	pixel** screen = g->ecran;
	int* isSelected = g->isSelected;
	SMap* map = g->map;
	SDL_Color* couleurs = g->couleurs;
	SDL_Renderer* rend = g->render;
	//pour tous les pixels du tableau double dimensions screen
	for(int x = 0; x <= w; x++){
		for(int y = 0; y <= h; y++){
				if(screen[x][y].isBorder || screen[x][y].isEmpty || isSelected[screen[x][y].idCell]){

					SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
					if(!screen[x][y].isEmpty && isSelected[screen[x][y].idCell] && screen[x][y].isBorder){
						SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
					}
				}
				else{
					SDL_SetRenderDrawColor(rend, couleurs[map->cells[screen[x][y].idCell].owner].r, couleurs[map->cells[screen[x][y].idCell].owner].g, couleurs[map->cells[screen[x][y].idCell].owner].b, 255);
				}

				SDL_RenderDrawPoint(rend, x, y);
		}
	}
	//Dessin de tous les dés en tenant 
	for(int k = 0; k < g->map->nbCells; k++){
		drawDices(g, g->germes[k], &g->map->cells[k], g->w, g->h);
	}
	updateStack(g, g->nbPlayers);
	SDL_RenderPresent(rend);
}

//Dessine les dés de la cellule dont l'id est passée en paramètre
void drawDices(SGame* g, SDL_Point center, SCell* cell, int w, int h)
{
	//On commence par récupérer le nombre de dés à afficher grace à la cellule passée en paramètre
	int idCell = cell->id;
	int nbDices = g->map->cells[idCell].nbDices;
	SDL_Texture *texture = NULL;
	SDL_Rect dest;
	int largeur = 0, hauteur = 0;
	int a,b,x,y;
	//On peut crée la texture avec la surface (pointeur) qu'on a préalablement stocké dans le champ diceImages de la SGame
	texture = SDL_CreateTextureFromSurface(g->render, g->diceImages[nbDices]);
	if( texture != NULL)
	{
		//Avec les fonctions de SDL on affiche l'image à la position souhaitée
		SDL_QueryTexture(texture,NULL,NULL, &largeur, &hauteur);
		a=(int)(largeur/2);
		b=(int)(hauteur/2);
		x=center.x - a;
		y=center.y - b;
		if( x<=0 ) { x = 0; }
		if( x>g->w ) { x = g->h-largeur; }
		if ( y<=0 ) { y = 0; }
		if ( y>g->h )	{ y=g->w-hauteur ;}
		dest.x = x;
		dest.y = y;
		dest.w = largeur;
		dest.h = hauteur;
		SDL_RenderCopy(g->render,texture,NULL,&dest);
		SDL_DestroyTexture(texture); // Libération de la mémoire associée à la texture
	}
	else {printf("pb creation texture");}
}

//Fonction graphique qui affiche aux endroits définit les image du tour actuel et celle du boutton pour passer son tour
void endTurnButton(SGame* game) {
    SDL_Texture * texture = SDL_CreateTextureFromSurface(game->render, game->diceImages[13]);
    SDL_Rect dsrect;
    dsrect.x = 660;
    dsrect.y = 610;
    dsrect.h = 140;
    dsrect.w = 140;
    SDL_RenderCopy(game->render, texture, NULL, &dsrect);
    SDL_RenderPresent(game->render);
    texture = SDL_CreateTextureFromSurface(game->render, game->diceImages[14]);
    dsrect.x = 6;
    dsrect.y = 606;
    dsrect.w = 188;
    dsrect.h = 47;
    SDL_RenderCopy(game->render, texture, NULL, &dsrect);
    SDL_RenderPresent(game->render);
    SDL_DestroyTexture(texture);
 
}

//Fonction graphique mettant à jour la zone indiquant le tour en actualisant le numero du joueur actuel et la couleur affichées
void updateTurnImage(SGame* game, TTF_Font* font){
	SDL_Color color = { 255, 255, 255 };
	SDL_Surface *surface = NULL;
	SDL_Texture *texture = NULL;
	int texW = 0;
	int texH = 0;
	SDL_Rect turnRect = { 147, game->h + 13, 30, 30};
	SDL_Rect rect;
	char* str = (char*) malloc(sizeof(char) * 20);
	SDL_Color currentColor = game->couleurs[game->ordrePlayers[game->turn]];
	printf("%s\n", TTF_GetError());
	snprintf(str, 20, "TOUR : JOUEUR    %d", game->ordrePlayers[game->turn]);
	surface = TTF_RenderText_Solid(font, str, color);
	texture = SDL_CreateTextureFromSurface(game->render, surface);
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	rect.w = texW;
	rect.h = texH;
	rect.x = 27;
	rect.y = game->h + 20;
	SDL_SetRenderDrawColor( game->render, currentColor.r, currentColor.g, currentColor.b, 255 );
	SDL_RenderFillRect( game->render, &turnRect );
	SDL_RenderCopy(game->render, texture, NULL, &rect);
	
	free(str);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
	surface = NULL;
	texture = NULL;
}

//Fonction graphique qui remplit les carrés des lancés de dés avec les couleurs correspondant à la situation actuelle
void setDiceSquare(SGame* g) {
	SDL_Renderer* rend = g->render;
	SDL_Rect rect12, rect22;
	SDL_Color player1Color = g->couleurs[g->ordrePlayers[g->turn]];
	rect12.w = 75;
	rect12.h = 105;
	rect12.x = 291;
	rect12.y = g->h + 1;
	rect22.w = 75;
	rect22.h = 105;
	rect22.x = 431;
	rect22.y = g->h + 1;
	SDL_SetRenderDrawColor( rend, 217, 207, 217, 255 );
	SDL_RenderFillRect( rend, &rect22 );
	rect22.h = 75;
	rect22.y = g->h + 40;
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
	SDL_RenderFillRect( rend, &rect22 );
	SDL_RenderPresent(rend);
	SDL_SetRenderDrawColor( rend, 217, 207, 217, 255 );
	SDL_RenderFillRect( rend, &rect12 );
	rect12.h = 75;
	rect12.y = g->h + 40;
	SDL_SetRenderDrawColor(rend, player1Color.r, player1Color.g, player1Color.b, 255);
	SDL_RenderFillRect( rend, &rect12 );
	SDL_RenderPresent(rend);
}

//Fonction graphique qui lance l'animation des dés et afficher de manière dynamique le résultat de chaque joueur
void rollDices(SGame* game, int idFrom, int idTo, int* results){
	SDL_Color color = { 255, 255, 255 };
	SDL_Surface *surface = NULL;
	SDL_Texture *texture = NULL;
	TTF_Font * font = NULL;
	int i;
	int p1Score = 0; //incrémenteur du score de la case qui attaque
	int p2Score = 0; //incrémenteur du score de la case qui est attaquée
	int texW = 0;
	int texH = 0;
	SDL_Rect rect1, rect12, rect2, rect22; //On déclare les rectangle du texte et des couleurs associés aux joueurs
	SDL_Color player1Color, player2Color;
	char* str = (char*) malloc(sizeof(char) * 2);
	int maxRes = max(results[0],results[1]);

	font = TTF_OpenFont("arial.ttf", 50);
	player1Color = game->couleurs[getOwner(game->map,idFrom)];
	player2Color = game->couleurs[getOwner(game->map,idTo)];
	rect1.x = 300;
	rect1.y = game->h + 50;
	rect2.x = 440;
	rect2.y = game->h + 50;
	rect12.w = 75;
	rect12.h = 75;
	rect12.x = 291;
	rect12.y = game->h + 40;
	rect22.w = 75;
	rect22.h = 75;
	rect22.x = 431;
	rect22.y = game->h + 40;

	printf("%s\n", TTF_GetError());
	shakeDices(game, texture, player1Color, player2Color);
	//Pour les deux cellules concernées on simule le lancer des dés
	for(i=0; i<=maxRes; i++)
	{
		//Player who is attacking
		if(i<results[0]){
			p1Score += 1;
			snprintf(str, 16, "%d", p1Score);
			surface = TTF_RenderText_Solid(font, str, color);
			texture = SDL_CreateTextureFromSurface(game->render, surface);
			SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
			rect1.w = texW;
			rect1.h = texH;
			SDL_SetRenderDrawColor(game->render, player1Color.r, player1Color.g, player1Color.b, 255);
			SDL_RenderFillRect( game->render, &rect12 );
			SDL_RenderCopy(game->render, texture, NULL, &rect1);
			SDL_RenderPresent(game->render);
		}
		//Player who is defending
		if(i<results[1]) {
			p2Score += 1;
			snprintf(str, 16, "%d", p2Score);
			surface = TTF_RenderText_Solid(font, str, color);
			texture = SDL_CreateTextureFromSurface(game->render, surface);
			SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
			rect2.w = texW;
			rect2.h = texH;
			SDL_SetRenderDrawColor(game->render, player2Color.r, player2Color.g, player2Color.b, 255);
			SDL_RenderFillRect( game->render, &rect22 );
			SDL_RenderCopy(game->render, texture, NULL, &rect2);
			SDL_RenderPresent(game->render);
		}
		SDL_Delay(30);
	}
	//On affiche la couronne pour montrer qui a obtenu plus de dés
	SDL_Surface* crown = game->diceImages[0];
	if(p1Score > p2Score) { //Si le joueur 1 a gagné
		rect1.y = game->h + 2;
		rect1.h = 38;
		rect1.w = 60;
	    texture = SDL_CreateTextureFromSurface(game->render, crown);
	    SDL_RenderCopy(game->render, texture, NULL, &rect1);
	}
	else {
		rect2.y = game->h + 2;
		rect2.h = 38;
		rect2.w = 60;
	    texture = SDL_CreateTextureFromSurface(game->render, crown);
	    SDL_RenderCopy(game->render, texture, NULL, &rect2);
	}
	SDL_Delay(180);
	SDL_RenderPresent(game->render);

	free(str);
	SDL_DestroyTexture(texture);
	
	SDL_FreeSurface(surface);
	SDL_FreeSurface(crown);
	
	surface = NULL;
	texture = NULL;
}

//Fonction graphique qui sera utilisisée dans shakeDices
//Elle efface les images des dés
void fillSquares(SGame* game, SDL_Color player1Color, SDL_Color player2Color, SDL_Rect* rect12, SDL_Rect* rect22) {
	
	SDL_SetRenderDrawColor(game->render, player1Color.r, player1Color.g, player1Color.b, 255);
	SDL_RenderFillRect( game->render, rect12 );
	SDL_SetRenderDrawColor(game->render, player2Color.r, player2Color.g, player2Color.b, 255);
	SDL_RenderFillRect( game->render, rect22 );
	SDL_RenderPresent(game->render);
}

//Produit une animation simple à chaque lancé de dés
void shakeDices(SGame* game, SDL_Texture* texture, SDL_Color player1Color, SDL_Color player2Color) {
	SDL_Rect* rect12 = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	SDL_Rect* rect22 = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	rect12->w = 75;
	rect12->h = 75;
	rect12->x = 291;
	rect12->y = game->h + 40;
	rect22->w = 75;
	rect22->h = 75;
	rect22->x = 431;
	rect22->y = game->h + 40;
	//On efface les images des dés et on change l'image puis on reproduit ces actions plusieurs fois pour crée un effet de mouvement
    fillSquares(game, player1Color, player2Color, rect12, rect22);
    texture = SDL_CreateTextureFromSurface(game->render, game->diceImages[9]);
    SDL_RenderCopy(game->render, texture, NULL, rect12);
    texture = SDL_CreateTextureFromSurface(game->render, game->diceImages[11]);
    SDL_RenderCopy(game->render, texture, NULL, rect22);
    SDL_RenderPresent(game->render);
	SDL_Delay(160);
	fillSquares(game, player1Color, player2Color, rect12, rect22);
    texture = SDL_CreateTextureFromSurface(game->render, game->diceImages[10]);
    SDL_RenderCopy(game->render, texture, NULL, rect12);
    texture = SDL_CreateTextureFromSurface(game->render, game->diceImages[12]);
    SDL_RenderCopy(game->render, texture, NULL, rect22);
    SDL_RenderPresent(game->render);
	SDL_Delay(160);
	fillSquares(game, player1Color, player2Color, rect12, rect22);
    texture = SDL_CreateTextureFromSurface(game->render, game->diceImages[9]);
    SDL_RenderCopy(game->render, texture, NULL, rect12);
    texture = SDL_CreateTextureFromSurface(game->render, game->diceImages[11]);
    SDL_RenderCopy(game->render, texture, NULL, rect22);
    SDL_RenderPresent(game->render);
	SDL_Delay(160);
	fillSquares(game, player1Color, player2Color, rect12, rect22);
    texture = SDL_CreateTextureFromSurface(game->render, game->diceImages[10]) ;
    SDL_RenderCopy(game->render, texture, NULL, rect12);
    texture = SDL_CreateTextureFromSurface(game->render, game->diceImages[12]);
    SDL_RenderCopy(game->render, texture, NULL, rect22);
    SDL_RenderPresent(game->render);
	SDL_Delay(160);

	free(rect12);
	free(rect22);
}

//Dessine la zone graphique servant à afficher les dés des stack
void drawStack(SGame* game, int nbPlayers, TTF_Font* font) {
	SDL_Rect rect;
	SDL_Color currentColor;
	SDL_Color color = { 0, 0, 0 };
	SDL_Surface *surface = NULL;
	SDL_Texture *texture = NULL;
	int texW = 0;
	int texH = 0;
	char* str = (char*) malloc(sizeof(char) * 20);
	printf("%s\n", TTF_GetError());
	//Partie graphique on dessine les rectangle de la zone où on affichera les nombre de dés dans les piles
	//On écrit STACK pour que le joueur sache à quoi ces nombres correspondent
	rect.w = 170;
	rect.h = 83;
	rect.x = 14;
	rect.y = game->h + 54;
	SDL_SetRenderDrawColor( game->render, 0, 0, 0, 255 );
	SDL_RenderFillRect( game->render, &rect );
	rect.w = 168;
	rect.h = 81;
	rect.x = 15;
	rect.y = game->h + 55;
	SDL_SetRenderDrawColor( game->render, 204, 221, 255, 255 );
	SDL_RenderFillRect( game->render, &rect );
	snprintf(str, 15, "STACKS :");
	surface = TTF_RenderText_Solid(font, str, color);
	texture = SDL_CreateTextureFromSurface(game->render, surface);
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	rect.w = texW;
	rect.h = texH;
	rect.x = 20;
	rect.y = game->h + 56;
	SDL_RenderCopy(game->render, texture, NULL, &rect);
	int i;
	int j;
	//Pour chaque joueur on dessine un rectangle à coté de sa couleur pour indiiquer sa pile de dés
		for(j = 0; i*3+j < nbPlayers; j++) {
		//Après 3 rectangles on passe à une nouvelle colonne
			if(j>2) {
				j = 0;
				i++;
			}
			currentColor = game->couleurs[i*3+j];
			rect.w = 8;
			rect.h = 15;
			rect.x = 20 + i * 60;
			rect.y = game->h + 80 + j*18;
			SDL_SetRenderDrawColor( game->render, currentColor.r, currentColor.g, currentColor.b, 255 );
			SDL_RenderFillRect( game->render, &rect );
		}
	free(str);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
}

//Met à jour sur l'interface le nombre de dé dans chaque pile
void updateStack(SGame* game, int nbPlayers) {
	TTF_Font * font = NULL;
	SDL_Rect rect;
	SDL_Color color = { 0, 0, 0 };
	SDL_Surface *surface = NULL;
	SDL_Texture *texture = NULL;
	char* str = (char*) malloc(sizeof(char) * 3);
	printf("%s\n", TTF_GetError());
	font = TTF_OpenFont("arial.ttf", 12);
	rect.w = 20;
	rect.h = 15;

	int i = 0;
	int j;
	//Pour chaque joueur on dessine un rectangle de la couleur du fond pour effacer le nombre affiché
	for(j = 0 ; i*3+j < nbPlayers; j++) {
		if(j>2) {
			j = 0;
			i++;
		}
		rect.x = 30 + i * 60;
		rect.y = game->h + 80 + j*18;
		SDL_SetRenderDrawColor( game->render, 204, 221, 255, 255 );
		SDL_RenderFillRect( game->render, &rect );
	}

	//Pour chaque joueur on écrit le nombre de dés dans sa pile
	i = 0;
	for(j = 0; i*3+j < nbPlayers; j++) {
	//Après 3 rectangles on passe à une nouvelle colonne
		if(j>2) {
			j = 0;
			i++;
		}
		snprintf(str, 3, "%d", game->map->stack[i*3+j]);
		surface = TTF_RenderText_Solid(font, str, color);
		texture = SDL_CreateTextureFromSurface(game->render, surface);
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
		rect.x = 30 + i * 60;
		rect.y = game->h + 80 + j*18;
		SDL_RenderCopy(game->render, texture, NULL, &rect);
		}
		
	free(str);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
		
}
