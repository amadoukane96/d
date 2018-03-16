#include "interface.h"
#include "SMap.h"
#include <stdlib.h>
#include <stdio.h>

void InitGame(unsigned int id, unsigned int nbPlayer, SPlayerInfo *info){
  sprintf(info->name, "basique");
  sprintf(info->members[1],"DIABY");
  sprintf(info->members[0], "MOHAMED-MOUSSA");
  sprintf(info->members[2], "KANE");
  sprintf(info->members[3], "LEBIAN");
}

int PlayTurn(unsigned int id, const SMap *map, STurn *turn){
 
  int id_player = id;
  int i,j,k,diff,draw, nbD;
  k =0;
  j =0;

  diff = 1;
  nbD = 9;
  int totale_des = 0;
  draw = 0;

  SCell *cells = (SCell*) malloc(sizeof(SCell)*map->nbCells);

  int ok = 0;
  for (i=0;i<map->nbCells;i++){
    if (map->cells[i].owner == id_player){
      cells[j]=map->cells[i];
      j++;
      totale_des += map->cells[i].nbDices;
    }
  }
  
  int income = endTurnDices((SMap*)map, id);
  
  if(map->stack[id_player] + income + totale_des >= (8 * j)){
    diff = 0;
  }
  
  while(k<j){

    for (i=0;i < cells[k].nbNeighbors;i++){
	    if(cells[k].neighbors[i]->owner != id_player){

	      if ((cells[k].nbDices - cells[k].neighbors[i]->nbDices) >= diff){

			  if((cells[k].nbDices - cells[k].neighbors[i]->nbDices) == diff){
				   draw = 1;
			  }
			  else{
			       draw = 0;
			  }

			  if((draw && cells[k].nbDices < nbD) || !draw){
				ok = 1;
				if(draw){
					nbD = cells[k].nbDices;
				}
				diff = cells[k].nbDices - cells[k].neighbors[i]->nbDices;
				turn->cellTo = cells[k].neighbors[i]->id;
				turn->cellFrom = cells[k].id;

			  }
	        }
		}
    }
    k++;
  }
  free(cells);

  return ok;
}




void EndGame(unsigned int id, unsigned int idWinner){
}
